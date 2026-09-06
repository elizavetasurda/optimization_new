#include <iostream>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace lagrange_name{

double function_target(double x, double y)
{
    return x*x + y*y;
}

double constraint(double x, double y)
{
    return x + y - 1;
}

void gradient(
    double x,
    double y,
    double lambda,
    double r,
    double& gx,
    double& gy
)
{
    double h = constraint(x,y);
    gx = 2*x + lambda + r*h;
    gy = 2*y + lambda + r*h;
}

void augmented_lagrangian(double& x, double& y)
{
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    double lambda = 0.0;
    double r = 10.0;
    double alpha = 0.01;

    for(int k = 0; k < 10; k++)
    {
        for(int i = 0; i < 500; i++)
        {
            double gx, gy;
            gradient(x, y, lambda, r, gx, gy);
            x -= alpha * gx;
            y -= alpha * gy;
        }

        lambda += r * constraint(x,y);

        double current_value = function_target(x, y);
        logger->info("Iteration " + std::to_string(k) + ": value = " + std::to_string(current_value));

        cout << "iter = " << k << "  x = " << x << "  y = " << y  << "  lambda = " << lambda << endl;
    }
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b)
{
    (void)target_f;
    double x = (double)a;
    double y = (double)b;
    augmented_lagrangian(x, y);
    return (T)function_target(x, y);
}
}

template<typename T>
auto lagrange(){
    return &lagrange_name::f<T>;
}
