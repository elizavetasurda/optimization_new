#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace conjugate_grad_name{
using Point = vector<double>;

double norm(const Point& x) {
    double sum = 0.0;
    for (double v : x) {
        sum += v * v;
    }
    return sqrt(sum);
}

double dot(const Point& a, const Point& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

double rosenbrock(const Point& x) {
    return pow(1 - x[0], 2)
         + 100 * pow(x[1] - x[0] * x[0], 2);
}

Point rosenbrock_grad(const Point& x) {
    Point g(2);
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
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    double fx = f(x);
    Point g = grad(x);
    double gtd = dot(g, d);
    Point x_new(x.size());
    while (true) {
        for (size_t i = 0; i < x.size(); i++) {
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

Point conjugate_gradient(
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
    Point g = grad(x);
    Point d(x.size());
    for (size_t i = 0; i < x.size(); i++) {
        d[i] = -g[i];
    }

    for (int iter = 0; iter < max_iter; iter++) {
        if (norm(g) < tol) {
            logger->info("Сошлись за " + std::to_string(iter) + " итераций");
            break;
        }

        double alpha = line_search(f, grad, x, d);
        Point x_new = x;
        for (size_t i = 0; i < x.size(); i++) {
            x_new[i] += alpha * d[i];
        }

        Point g_new = grad(x_new);
        Point diff(x.size());
        for (size_t i = 0; i < x.size(); i++) {
            diff[i] = g_new[i] - g[i];
        }

        double beta = dot(g_new, diff) / dot(g, g);
        if (beta < 0) beta = 0;

        for (size_t i = 0; i < x.size(); i++) {
            d[i] = -g_new[i] + beta * d[i];
        }

        x = x_new;
        g = g_new;

        double current_value = f(x);
        logger->info("Iteration " + std::to_string(iter) + ": value = " + std::to_string(current_value));

        if (iter % 10 == 0) {
            cout << "iter = " << iter
                 << ", f = " << current_value
                 << ", alpha = " << alpha
                 << ", beta = " << beta << endl;
        }
    }
    return x;
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b) {
    (void)target_f;
    Point start = {(double)a, (double)b};
    Point result = conjugate_gradient(
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
auto conjugate_grad(){
    return &conjugate_grad_name::f<T>;
}
