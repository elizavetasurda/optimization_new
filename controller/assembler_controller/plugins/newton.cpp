#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace newton_name{
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
    Point g(2);
    g[0] = -2 * (1 - x[0]) - 400 * x[0] * (x[1] - x[0] * x[0]);
    g[1] = 200 * (x[1] - x[0] * x[0]);
    return g;
}

vector<vector<double>> rosenbrock_hessian(const Point& x) {
    vector<vector<double>> H(2, vector<double>(2));
    H[0][0] = 2 - 400 * x[1] + 1200 * x[0] * x[0];
    H[0][1] = -400 * x[0];
    H[1][0] = -400 * x[0];
    H[1][1] = 200;
    return H;
}

Point solve_system(
    vector<vector<double>> H,
    Point g
) {
    Point d(2);
    double a = H[0][0];
    double b = H[0][1];
    double c = H[1][0];
    double e = H[1][1];
    double det = a * e - b * c;
    d[0] = (-g[0] * e + b * g[1]) / det;
    d[1] = (-a * g[1] + c * g[0]) / det;
    return d;
}

Point newton_method(
    function<double(const Point&)> f,
    function<Point(const Point&)> grad,
    function<vector<vector<double>>(const Point&)> hessian,
    Point start,
    double tol = 1e-8,
    int max_iter = 1000
) {
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    Point x = start;
    for (int iter = 0; iter < max_iter; iter++) {
        Point g = grad(x);
        if (norm(g) < tol) {
            logger->info("Сошлись за " + std::to_string(iter) + " итераций");
            break;
        }
        vector<vector<double>> H = hessian(x);
        Point d = solve_system(H, g);
        for (size_t i = 0; i < x.size(); i++) {
            x[i] += d[i];
        }

        double current_value = f(x);
        logger->info("Iteration " + std::to_string(iter) + ": value = " + std::to_string(current_value));

        if (iter % 10 == 0) {
            cout << "iter = " << iter << ", f = " << current_value << endl;
        }
    }
    return x;
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b) {
    (void)target_f;
    Point start = {(double)a, (double)b};
    Point result = newton_method(
        rosenbrock,
        rosenbrock_grad,
        rosenbrock_hessian,
        start,
        1e-8,
        1000
    );
    return (T)rosenbrock(result);
}
}

template<typename T>
auto newton(){
    return &newton_name::f<T>;
}
