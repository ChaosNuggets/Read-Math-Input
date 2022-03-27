#include <iostream>
#include <cmath>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

typedef double (*func_ptr)(const double, const double);

string input;
string numString;
vector<double> nums;
vector<map<int, func_ptr>> ops(3, map<int, func_ptr>());

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

void throwError(string error) {
    cout << "Error: " << error << '\n';
    cout << "Press enter to exit";
    cin.ignore();
    throw runtime_error(error);
}

class ProcessInput {
    private:
    unordered_map<func_ptr, int> priorities = {
        {add, 0},
        {subtract, 0},
        {multiply, 1},
        {divide, 1},
        {pow, 2}
    };

    int opOrder = 0;
    bool rightAfterOp = false;

    func_ptr getOperation(const char c) {
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
                throwError("invalid operation");
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
        if (priorities[pow] > ops.size()-1) {
            ops.resize(priorities[pow]+1);
        }
    }

    void decreasePriority() {
        if (priorities[add] - 3 < 0) {
            throwError("too many closing parentheses");
        }
        for (auto&[operation, priority] : priorities) {
            priority -= 3;
        }
    }

    void addOperationToVector(func_ptr operation) {
        int priority = priorities[operation];
        ops[priority][opOrder] = operation;
        opOrder++;
        rightAfterOp = true;
    }

    public:
    void processInput() {
        for (int i = 0; i < input.length(); i++) {
            if (isdigit(input[i])) {
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
                throwError("back to back operations");
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
                if (!numString.empty()) {
                    handleEndOfNum();
                    addOperationToVector(multiply);
                }
                continue;
            }
            if (!numString.empty()) handleEndOfNum();
            func_ptr operation = getOperation(input[i]);
            addOperationToVector(operation);
        }
        if (!numString.empty()) handleEndOfNum();
    }
};

class DoCalculations {
    private:
    vector<int> deletedIndexes;

    int calculateShift(int index) {
        int shift = 0;
        bool hasBeenInserted = false;
        for (int k = 0; k < deletedIndexes.size(); k++) {
            if (deletedIndexes[k] <= index) shift++;
            else {
                deletedIndexes.insert(deletedIndexes.begin() + k, index + 1);
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

    public:
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

int main() {
    ProcessInput p;
    DoCalculations d;

    cout << "Enter expression: \n";
    getline(cin, input);
    p.processInput();
    d.doCalculations();
    cout << "answer: " << nums[0] << endl;
    cout << "Press enter to exit";
    cin.ignore();
    return 0;
}