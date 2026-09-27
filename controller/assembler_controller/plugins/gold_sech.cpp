#include <cmath>
#include <iostream>
const double TAU = 0.6180339887498949;  // золотое сечение

template<typename T, typename Func>
T golden_section(Func f, T a, T b) {
    T x1 = a + (1 - TAU) * (b - a);
    T x2 = a + TAU * (b - a);
    T fx1 = f(x1);
    T fx2 = f(x2);

    std::cout << "a=" << a << ", x1=" << x1 << ", x2=" << x2 << ", b=" << b << std::endl;

    while ((b - a) > EPS) {
        if (fx1 < fx2) {
            b = x2;
            x2 = x1;
            fx2 = fx1;
            x1 = a + b - x2;
            fx1 = f(x1);
        } else {
            a = x1;
            x1 = x2;
            fx1 = fx2;
            x2 = a + b - x1;
            fx2 = f(x2);
        }
        std::cout << "a=" << a << ", x1=" << x1 << ", x2=" << x2 << ", b=" << b << std::endl;
    }

    return (a + b) / 2;
}
