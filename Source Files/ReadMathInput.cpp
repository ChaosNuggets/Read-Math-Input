#include <iostream>
#include <cmath>
#include <unordered_map>
#include <string>
#include <vector>
#include "MathFunctions.hpp"

using namespace std;

typedef double (*func_ptr)(const double, const double);

int i;

void waitForEnterKey() {
    cout << "Press enter to exit";
    cin.ignore();
}

void throwError(string error) {
    cout << "Error at index " << i << ": " << error << '\n';
    waitForEnterKey();
    throw runtime_error(error);
}

class DoCalculations {
    protected:
    vector<double> nums; //All nums are listed in the order of the input
    vector<vector<pair<int, func_ptr>>> ops; //1st dimension = priority, 2nd dimension = the index of the num and its operation

    private:
    vector<int> deletedIndexes;

    const unordered_map<func_ptr, bool> shouldDelete = {
        {add, true},
        {subtract, true},
        {multiply, true},
        {divide, true},
        {pow, true},
        {sin, false},
        {cos, false},
        {tan, false},
        {csc, false},
        {sec, false},
        {cot, false},
        {arcsin, false},
        {arccos, false},
        {arctan, false},
        {arccsc, false},
        {arcsec, false},
        {arccot, false}
    };

    int calculateShift(int index, func_ptr operation) {
        int shift = 0;
        bool hasBeenInserted = false;
        for (auto it = deletedIndexes.begin(); it != deletedIndexes.end(); it++) {
            if (*it <= index) shift++;
            else {
                if (shouldDelete.at(operation))
                    deletedIndexes.insert(it, index + 1); //Insert at index k 1+ the index because the num to the right gets deleted after calcultions
                hasBeenInserted = true;
                break;
            }
        }
        if (!hasBeenInserted) {
            deletedIndexes.push_back(index + 1);
        }
        return shift;
    }

    void doOperation(int realIndex, func_ptr operation) {
        nums[realIndex] = operation(nums[realIndex], nums[realIndex + 1]);
        if (shouldDelete.at(operation))
            nums.erase(nums.begin() + realIndex + 1);
    }

    protected:
    void doCalculations() {
        for (int priority = ops.size() - 1; priority >= 0; priority--) {
            for (auto& [index, operation] : ops[priority]) {
                // for (double number : nums) cout << number << ' ';
                // cout << '\n';
                int shift = calculateShift(index, operation);
                int realIndex = index - shift;
                doOperation(realIndex, operation);
            }
        }
    }
};

class InputHandler : DoCalculations {
    private:
    string input;

    public:
    InputHandler(string input) {
        this->input = input;
        ops.resize(4);
    }

    private:
    unordered_map<func_ptr, int> priorities = {
        {add, 0},
        {subtract, 0},
        {multiply, 1},
        {divide, 1},
        {pow, 2},
        {sin, 3},
        {cos, 3},
        {tan, 3},
        {csc, 3},
        {sec, 3},
        {cot, 3},
        {arcsin, 3},
        {arccos, 3},
        {arctan, 3},
        {arccsc, 3},
        {arcsec, 3},
        {arccot, 3}
    };

    const unordered_map<char, func_ptr> charToOp = {
        {'+', add},
        {'-', subtract},
        {'*', multiply},
        {'/', divide},
        {'^', pow}
    };

    const unordered_map<string, func_ptr> stringToOp = {
        {"sin", sin},
        {"cos", cos},
        {"tan", tan},
        {"csc", csc},
        {"sec", sec},
        {"cot", cot},
        {"arcsin", arcsin},
        {"arccos", arccos},
        {"arctan", arctan},
        {"arccsc", arccsc},
        {"arcsec", arcsec},
        {"arccot", arccot}
    };

    int numIndex = 0;
    bool rightAfterOp = true;
    bool rightAfterClosingParentheses = false;

    func_ptr getOperation(const char c) const {
        //If not in hashmap, return nullptr, else return the pointer to the function
        if (charToOp.find(c) == charToOp.end()) return nullptr;
        return charToOp.at(c);
    }

    func_ptr getOperation(const string s) const {
        //If not in hashmap, return nullptr, else return the pointer to the function
        if (stringToOp.find(s) == stringToOp.end()) return nullptr;
        return stringToOp.at(s);
    }

    void handleEndOfNum(string& numString) {
        if (numString == "-" || numString == "+") {
            throwError(string("cannot do operation to \"") + numString + string("\""));
        }
        nums.push_back(stod(numString));
        numString.clear();
    }

    void increasePriority() {
        for (auto& [operation, priority] : priorities) {
            priority += 4;
        }
        if (priorities.at(sin) > ops.size()-1) {
            ops.resize(priorities.at(sin)+1);
        }
    }

    void decreasePriority() {
        if (priorities.at(add) - 4 < 0) {
            throwError(string("too many closing parentheses"));
        }
        for (auto& [operation, priority] : priorities) {
            priority -= 4;
        }
    }

    void addFunctionToVector(func_ptr operation) {
        int priority = priorities.at(operation);
        ops[priority].push_back({numIndex, operation});
        rightAfterOp = true;
        rightAfterClosingParentheses = false;
    }

    void addOperationToVector(func_ptr operation) {
        addFunctionToVector(operation);
        numIndex++;
    }

    public:
    void processInput() {
        string numString;
        string funcString;
        for (i = 0; i < input.length(); i++) {
            if (isdigit(input[i]) || input[i] == '.') {
                if (rightAfterClosingParentheses) {
                    handleEndOfNum(numString); //We handle end of num here because the multiplication would be low priority
                    addOperationToVector(multiply);
                }
                if (!funcString.empty()) {
                    throwError("You need to have an opening parentheses after a function");
                }
                numString += input[i];
                rightAfterOp = false;
                continue;
            }
            if (input[i] == ' ') {
                continue;
            }
            if (rightAfterOp) {
                if (input[i] == '-' || input[i] == '+') {
                    numString += input[i];
                    rightAfterOp = false;
                    continue;
                }
                if (input[i] == '(') {
                    if (!funcString.empty()) {
                        func_ptr operation = getOperation(funcString);
                        if (operation != nullptr) {
                            addFunctionToVector(operation);
                        } else {
                            throwError(string("\"") + funcString + string("\" is an invalid function"));
                        }
                        funcString.clear();
                    }
                    increasePriority();
                    continue;
                }
                if (isalpha(input[i])) {
                    funcString += input[i];
                    continue;
                }
                if (getOperation(input[i]) == nullptr)
                    throwError(string("\"") + input[i] + string("\" is an invalid character"));
                else 
                    throwError(string("back to back operations, \"") + input[i] + string("\" is invalid"));
            }
            if (input[i] == '(') {
                if (!numString.empty()) {
                    handleEndOfNum(numString);
                    addOperationToVector(multiply);
                }
                increasePriority();
                continue;
            }
            if (input[i] == ')') {
                decreasePriority();
                rightAfterClosingParentheses = true;
                continue;
            }
            if (isalpha(input[i])) {
                if (!numString.empty()) {
                    handleEndOfNum(numString);
                    addOperationToVector(multiply);
                }
                funcString += input[i];
                continue;
            }
            //The only possibilities left are that input[i] is an operation or input[i] is invalid
            if (!numString.empty()) handleEndOfNum(numString);
            func_ptr operation = getOperation(input[i]);
            if (operation != nullptr) {
                addOperationToVector(operation);
                continue;
            }
            throwError(string("\"") + input[i] + string("\" is an invalid character"));
        }
        if (!numString.empty()) handleEndOfNum(numString);
    }

    double doCalculations() {
        DoCalculations::doCalculations();
        return nums[0];
    }
};

int main() {
    string input;

    cout << "Enter expression: \n";
    getline(cin, input);
    InputHandler p(input);
    p.processInput();
    double answer = p.doCalculations();
    cout.precision(12);
    cout << "answer: " << answer << endl;
    waitForEnterKey();
    return 0;
}