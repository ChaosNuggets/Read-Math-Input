#include <iostream>
#include <cmath>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

typedef double (*func_ptr)(const double, const double);

int i;

double add(const double x, const double y) {
    return x+y;
}

double subtract(const double x, const double y) {
    return x-y;
}

double multiply(const double x, const double y) {
    return x*y;
}

double divide(const double x, const double y) {
    return x/y;
}

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
    vector<double> nums;
    vector<map<int, func_ptr>> ops;

    private:
    vector<int> deletedIndexes;

    int calculateShift(int index) {
        int shift = 0;
        bool hasBeenInserted = false;
        for (int k = 0; k < deletedIndexes.size(); k++) {
            if (deletedIndexes[k] <= index) shift++;
            else {
                deletedIndexes.insert(deletedIndexes.begin() + k, index + 1); //Insert at index k 1+ the index because the num to the right gets deleted after calcultions
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
        nums.erase(nums.begin() + realIndex + 1);
    }

    protected:
    void doCalculations() {
        for (int priority = ops.size() - 1; priority >= 0; priority--) {
            for (auto&[index, operation] : ops[priority]) {
                int shift = calculateShift(index);
                int realIndex = index - shift;
                doOperation(realIndex, operation);
            }
        }
    }
};

class InputHandler : DoCalculations {
    private:
    string numString;
    string input;

    public:
    InputHandler(string input) {
        this->input = input;
        ops.resize(3);
    }

    private:
    unordered_map<func_ptr, int> priorities = {
        {add, 0},
        {subtract, 0},
        {multiply, 1},
        {divide, 1},
        {pow, 2}
    };

    int opOrder = 0;
    bool rightAfterOp = true;
    bool rightAfterClosingParentheses = false;

    func_ptr getOperation(const char c) const {
        switch (c) {
            case '+':
                return add;
                break;
            case '-':
                return subtract;
                break;
            case '*':
                return multiply;
                break;
            case '/':
                return divide;
                break;
            case '^':
                return pow;
                break;
            default:
                throwError(c + string(" is not a valid operation"));
                break;
        }
    }

    void handleEndOfNum() {
        nums.push_back(stod(numString));
        numString.clear();
    }

    void increasePriority() {
        for (auto&[operation, priority] : priorities) {
            priority += 3;
        }
        if (priorities.at(pow) > ops.size()-1) {
            ops.resize(priorities.at(pow)+1);
        }
    }

    void decreasePriority() {
        if (priorities.at(add) - 3 < 0) {
            throwError(string("too many closing parentheses"));
        }
        for (auto&[operation, priority] : priorities) {
            priority -= 3;
        }
    }

    void addOperationToVector(func_ptr operation) {
        int priority = priorities.at(operation);
        ops[priority][opOrder] = operation;
        opOrder++;
        rightAfterOp = true;
        rightAfterClosingParentheses = false;
    }

    public:
    void processInput() {
        for (i = 0; i < input.length(); i++) {
            if (isdigit(input[i]) || input[i] == '.') {
                if (rightAfterClosingParentheses) {
                    handleEndOfNum();
                    addOperationToVector(multiply);
                }
                numString += input[i];
                rightAfterOp = false;
                continue;
            }
            if (input[i] == ' ') {
                continue;
            }
            if (rightAfterOp) {
                if (input[i] == '-') {
                    numString += input[i];
                    continue;
                }
                if (input[i] == '+') {
                    continue;
                }
                if (input[i] == '(') {
                    increasePriority();
                    continue;
                }
                throwError(string("back to back operations, ") + input[i] + string(" is invalid"));
            }
            if (input[i] == '(') {
                if (!numString.empty()) {
                    handleEndOfNum();
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
            if (!numString.empty()) handleEndOfNum();
            func_ptr operation = getOperation(input[i]);
            addOperationToVector(operation);
        }
        if (!numString.empty()) handleEndOfNum();
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