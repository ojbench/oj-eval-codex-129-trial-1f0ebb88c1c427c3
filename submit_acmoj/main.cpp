#include <bits/stdc++.h>
using namespace std;

struct Value {
    bool isString = false;
    long long intValue = 0;
    string stringValue;
};

struct Variable {
    string type;
    Value value;
};

class FastScanner {
public:
    FastScanner() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        data.assign(istreambuf_iterator<char>(cin), istreambuf_iterator<char>());
    }

    bool readInt(int &value) {
        skipSpaces();
        if (index >= data.size()) return false;
        bool negative = false;
        if (data[index] == '+' || data[index] == '-') {
            negative = data[index] == '-';
            ++index;
        }
        long long result = 0;
        while (index < data.size() && isdigit(static_cast<unsigned char>(data[index]))) {
            result = result * 10 + (data[index] - '0');
            ++index;
        }
        value = negative ? -static_cast<int>(result) : static_cast<int>(result);
        return true;
    }

    bool readToken(string &token) {
        skipSpaces();
        if (index >= data.size()) return false;
        if (data[index] == '"') {
            token.clear();
            token.push_back(data[index++]);
            while (index < data.size()) {
                char ch = data[index++];
                token.push_back(ch);
                if (ch == '"') break;
            }
            return true;
        }
        size_t start = index;
        while (index < data.size() && !isspace(static_cast<unsigned char>(data[index]))) ++index;
        token.assign(data.data() + start, index - start);
        return true;
    }

private:
    vector<char> data;
    size_t index = 0;

    void skipSpaces() {
        while (index < data.size() && isspace(static_cast<unsigned char>(data[index]))) ++index;
    }
};

static inline bool parseIntStrict(const string &token, long long &value) {
    if (token.empty()) return false;
    size_t index = 0;
    bool negative = false;
    if (token[index] == '+' || token[index] == '-') {
        negative = token[index] == '-';
        ++index;
        if (index == token.size()) return false;
    }
    long long result = 0;
    for (; index < token.size(); ++index) {
        if (!isdigit(static_cast<unsigned char>(token[index]))) return false;
        result = result * 10 + (token[index] - '0');
    }
    value = negative ? -result : result;
    return true;
}

static inline bool parseQuotedString(const string &token, string &value) {
    if (token.size() < 2 || token.front() != '"' || token.back() != '"') return false;
    value = token.substr(1, token.size() - 2);
    return true;
}

int main() {
    FastScanner scanner;

    int n;
    if (!scanner.readInt(n)) return 0;

    vector<unordered_map<string, Variable>> scopes(1);
    string op;
    string token;

    auto findVar = [&](const string &name) -> Variable* {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) return &found->second;
        }
        return nullptr;
    };

    string output;
    output.reserve(static_cast<size_t>(n) * 8);

    for (int i = 0; i < n; ++i) {
        scanner.readToken(op);

        if (op == "Indent") {
            scopes.emplace_back();
            continue;
        }
        if (op == "Dedent") {
            if (scopes.size() > 1) {
                scopes.pop_back();
            } else {
                output += "Invalid operation\n";
            }
            continue;
        }

        if (op == "Declare") {
            string type, name;
            scanner.readToken(type);
            scanner.readToken(name);
            scanner.readToken(token);
            Variable variable;
            variable.type = type;
            if (type == "int") {
                long long number;
                if (!parseIntStrict(token, number)) {
                    output += "Invalid operation\n";
                    continue;
                }
                variable.value.isString = false;
                variable.value.intValue = number;
            } else if (type == "string") {
                if (!parseQuotedString(token, variable.value.stringValue)) {
                    output += "Invalid operation\n";
                    continue;
                }
                variable.value.isString = true;
            } else {
                output += "Invalid operation\n";
                continue;
            }
            auto &current = scopes.back();
            if (current.find(name) != current.end()) {
                output += "Invalid operation\n";
                continue;
            }
            current.emplace(std::move(name), std::move(variable));
            continue;
        }

        if (op == "Print") {
            string name;
            scanner.readToken(name);
            Variable *variable = findVar(name);
            if (!variable) {
                output += "Invalid operation\n";
                continue;
            }
            output += name;
            output.push_back(':');
            if (variable->type == "int") {
                output += to_string(variable->value.intValue);
            } else {
                output += variable->value.stringValue;
            }
            output.push_back('\n');
            continue;
        }

        if (op == "Add") {
            string a, b, c;
            scanner.readToken(a);
            scanner.readToken(b);
            scanner.readToken(c);
            Variable *result = findVar(a);
            Variable *lhs = findVar(b);
            Variable *rhs = findVar(c);
            if (!result || !lhs || !rhs || result->type != lhs->type || lhs->type != rhs->type) {
                output += "Invalid operation\n";
                continue;
            }
            if (result->type == "int") {
                result->value.isString = false;
                result->value.intValue = lhs->value.intValue + rhs->value.intValue;
            } else {
                result->value.isString = true;
                result->value.stringValue = lhs->value.stringValue + rhs->value.stringValue;
            }
            continue;
        }

        if (op == "SelfAdd") {
            string name;
            scanner.readToken(name);
            scanner.readToken(token);
            Variable *variable = findVar(name);
            if (!variable) {
                output += "Invalid operation\n";
                continue;
            }
            if (variable->type == "int") {
                long long number;
                if (!parseIntStrict(token, number)) {
                    output += "Invalid operation\n";
                    continue;
                }
                variable->value.intValue += number;
                variable->value.isString = false;
            } else {
                string appended;
                if (!parseQuotedString(token, appended)) {
                    output += "Invalid operation\n";
                    continue;
                }
                variable->value.stringValue += appended;
                variable->value.isString = true;
            }
            continue;
        }
    }

    cout << output;
    return 0;
}
