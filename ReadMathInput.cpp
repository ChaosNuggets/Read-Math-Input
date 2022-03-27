#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

typedef double (*func_ptr)(const double, const double);

string input;
string numString;
vector<double> nums;
vector<map<int, func_ptr>> ops(2, map<int, func_ptr>());

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

unordered_map<func_ptr, int> priorities = {
    {add, 0},
    {subtract, 0},
    {multiply, 1},
    {divide, 1}
};

class ProcessInput {
    private:
    static func_ptr getOperation(const char c) {
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
            default:
                throw runtime_error("invalid operation");
                break;
        }
    }

    static void handleEndOfNum() {
        nums.push_back(stod(numString));
        numString.clear();
    }

    public:
    static void processInput() {
        int opOrder = 0;
        for (int i = 0; i < input.length(); i++) {
            if (isdigit(input[i])) {
                numString += input[i];
                continue;
            }
            if (input[i] == ' ') {
                continue;
            }
            handleEndOfNum();
            func_ptr operation = getOperation(input[i]);
            int priority = priorities[operation];
            ops[priority][opOrder] = operation;
            opOrder++;
        }
        handleEndOfNum();
    }
};

class DoCalculations {
    public:
    static void doCalculations() {
        vector<int> deletedIndexes;
        bool hasBeenInserted = false;
        for (int i = ops.size() - 1; i >= 0; i--) {
            for (auto j : ops[i]) {
                int shift = 0;
                int index = j.first;
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
                int realIndex = index - shift;
                nums[realIndex] = j.second(nums[realIndex], nums[realIndex + 1]);
                nums.erase(nums.begin() + realIndex + 1);
            }
        }
    }
};

int main() {
    cout << "Enter expression: \n";
    getline(cin, input);
    ProcessInput::processInput();
    DoCalculations::doCalculations();
    cout << "answer: " << nums[0] << endl;
    cout << "Press enter to exit";
    cin.ignore();
    return 0;
}