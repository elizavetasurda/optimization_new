#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace grad_spysk_name{
using Point = vector<double>;

double norm(const Point& x) {
    double sum = 0.0;
    for (double v : x) {
        sum += v * v;
    }
    return sqrt(sum);
}

double rosenbrock(const Point& x) {
    return pow(1 - x[0], 2)
         + 100 * pow(x[1] - x[0] * x[0], 2);
}

Point rosenbrock_grad(const Point& x) {
    int n = x.size();
    Point g(n, 0.0);
    g[0] = -2 * (1 - x[0]) - 400 * x[0] * (x[1] - x[0] * x[0]);
    g[1] = 200 * (x[1] - x[0] * x[0]);
    return g;
}

double line_search(
    function<double(const Point&)> f,
    function<Point(const Point&)> grad,
    const Point& x,
    const Point& d,
    double alpha = 1.0,
    double beta = 1e-4,
    double rho = 0.5
) {
    int n = x.size();
    double fx = f(x);
    Point g = grad(x);
    double gtd = 0.0;
    for (int i = 0; i < n; i++) {
        gtd += g[i] * d[i];
    }
    Point x_new(n, 0.0);
    while (true) {
        for (int i = 0; i < n; i++) {
            x_new[i] = x[i] + alpha * d[i];
        }
        if (f(x_new) <= fx + beta * alpha * gtd) {
            break;
        }
        alpha *= rho;
        if (alpha < 1e-12) {
            break;
        }
    }
    return alpha;
}

Point gradient_descent(
    function<double(const Point&)> f,
    function<Point(const Point&)> grad,
    Point start,
    double tol = 1e-8,
    int max_iter = 1000
) {
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    Point x = start;
    int n = x.size();

    for (int iter = 0; iter < max_iter; iter++) {
        Point g = grad(x);
        if (norm(g) < tol) {
            logger->info("Сошлись за " + std::to_string(iter) + " итераций");
            break;
        }
        Point d(n, 0.0);
        for (int i = 0; i < n; i++) {
            d[i] = -g[i];
        }
        double alpha = line_search(f, grad, x, d);
        for (int i = 0; i < n; i++) {
            x[i] += alpha * d[i];
        }

        double current_value = f(x);
        logger->info("Iteration " + std::to_string(iter) + ": value = " + std::to_string(current_value));

        if (iter % 10 == 0) {
            cout << "iter = " << iter
                 << ", f = " << current_value
                 << ", alpha = " << alpha << endl;
        }
    }
    return x;
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b) {
    (void)target_f;
    Point start = {(double)a, (double)b};
    Point result = gradient_descent(
        rosenbrock,
        rosenbrock_grad,
        start,
        1e-8,
        1000
    );
    return (T)rosenbrock(result);
}
}

template<typename T>
auto grad_spysk(){
    return &grad_spysk_name::f<T>;
}
