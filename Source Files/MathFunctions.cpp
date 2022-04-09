#include <cmath>

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

double sin(const double x, const double y) {
    return sin(x);
}

double cos(const double x, const double y) {
    return cos(x);
}

double tan(const double x, const double y) {
    return tan(x);
}

double csc(const double x, const double y) {
    return 1/sin(x);
}

double sec(const double x, const double y) {
    return 1/cos(x);
}

double cot(const double x, const double y) {
    return 1/tan(x);
}

double arcsin(const double x, const double y) {
    return asin(x);
}

double arccos(const double x, const double y) {
    return acos(x);
}

double arctan(const double x, const double y) {
    return atan(x);
}

double arccsc(const double x, const double y) {
    return asin(1/x);
}

double arcsec(const double x, const double y) {
    return acos(1/x);
}

double arccot(const double x, const double y) {
    return atan(1/x);
}