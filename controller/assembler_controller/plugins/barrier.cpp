#include <iostream>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace barrier_name{

double function_target(double x, double y)
{
    return x*x + y*y;
}

double barrier_function(double x, double y, double mu)
{
    return function_target(x,y) - mu * log(x + y - 1);
}

void gradient(
    double x,
    double y,
    double mu,
    double& gx,
    double& gy
)
{
    double t = x + y - 1;
    gx = 2*x - mu / t;
    gy = 2*y - mu / t;
}

void interior_point(double& x, double& y)
{
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    double mu = 1.0;
    double beta = 0.5;
    double alpha = 0.01;

    for(int k = 0; k < 10; k++)
    {
        for(int i = 0; i < 500; i++)
        {
            double gx, gy;
            gradient(x, y, mu, gx, gy);
            x -= alpha * gx;
            y -= alpha * gy;
        }

        double current_value = function_target(x, y);
        logger->info("Iteration " + std::to_string(k) + ": value = " + std::to_string(current_value));

        cout << "iter = " << k
             << "  mu = " << mu
             << "  x = " << x
             << "  y = " << y
             << endl;

        mu *= beta;
    }
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b)
{
    (void)target_f;
    double x = (double)a;
    double y = (double)b;
    interior_point(x, y);
    return (T)function_target(x, y);
}
}

template<typename T>
auto barrier(){
    return &barrier_name::f<T>;
}
