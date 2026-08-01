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
        int digit = token[index] - '0';
        result = result * 10 + digit;
    }
    value = negative ? -result : result;
    return true;
}

static inline bool parseQuotedString(const string &token, string &value) {
    if (token.size() < 2 || token.front() != '"' || token.back() != '"') {
        return false;
    }
    value = token.substr(1, token.size() - 2);
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    vector<unordered_map<string, Variable>> scopes(1);
    string line;
    getline(cin, line);

    for (int i = 0; i < n; ++i) {
        getline(cin, line);
        if (line.empty()) {
            --i;
            continue;
        }
        stringstream ss(line);
        string op;
        ss >> op;

        if (op == "Indent") {
            scopes.emplace_back();
            continue;
        }
        if (op == "Dedent") {
            if (scopes.size() > 1) {
                scopes.pop_back();
            } else {
                cout << "Invalid operation\n";
            }
            continue;
        }

        auto findVar = [&](const string &name) -> Variable* {
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                auto found = it->find(name);
                if (found != it->end()) return &found->second;
            }
            return nullptr;
        };

        if (op == "Declare") {
            string type, name;
            ss >> type >> name;
            string valueToken;
            if (type == "string") {
                getline(ss >> ws, valueToken);
            } else {
                ss >> valueToken;
            }
            Variable variable;
            variable.type = type;
            if (type == "int") {
                long long number;
                if (!parseIntStrict(valueToken, number)) {
                    cout << "Invalid operation\n";
                    continue;
                }
                variable.value.isString = false;
                variable.value.intValue = number;
            } else if (type == "string") {
                if (!parseQuotedString(valueToken, variable.value.stringValue)) {
                    cout << "Invalid operation\n";
                    continue;
                }
                variable.value.isString = true;
            } else {
                cout << "Invalid operation\n";
                continue;
            }
            auto &current = scopes.back();
            if (current.find(name) != current.end()) {
                cout << "Invalid operation\n";
                continue;
            }
            current[name] = std::move(variable);
            continue;
        }

        if (op == "Print") {
            string name;
            ss >> name;
            Variable *variable = findVar(name);
            if (!variable) {
                cout << "Invalid operation\n";
                continue;
            }
            cout << name << ":";
            if (variable->type == "int") cout << variable->value.intValue;
            else cout << variable->value.stringValue;
            cout << '\n';
            continue;
        }

        if (op == "Add") {
            string a, b, c;
            ss >> a >> b >> c;
            Variable *result = findVar(a);
            Variable *lhs = findVar(b);
            Variable *rhs = findVar(c);
            if (!result || !lhs || !rhs || result->type != lhs->type || lhs->type != rhs->type) {
                cout << "Invalid operation\n";
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
            ss >> name;
            string valueToken;
            getline(ss >> ws, valueToken);
            Variable *variable = findVar(name);
            if (!variable) {
                cout << "Invalid operation\n";
                continue;
            }
            if (variable->type == "int") {
                long long number;
                if (!parseIntStrict(valueToken, number)) {
                    cout << "Invalid operation\n";
                    continue;
                }
                variable->value.intValue += number;
                variable->value.isString = false;
            } else {
                string appended;
                if (!parseQuotedString(valueToken, appended)) {
                    cout << "Invalid operation\n";
                    continue;
                }
                variable->value.stringValue += appended;
                variable->value.isString = true;
            }
            continue;
        }
    }

    return 0;
}
