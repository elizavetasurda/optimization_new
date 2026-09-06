#include <iostream>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace penalty_name{

double function_target(double x, double y)
{
    return x*x + y*y;
}

double penalty(double x, double y)
{
    double violation = 1 - x - y;
    if(violation > 0)
        return violation * violation;
    return 0.0;
}

double penalty_function(double x, double y, double r)
{
    return function_target(x,y) + r * penalty(x,y);
}

void gradient(double x, double y, double r,
              double& gx, double& gy)
{
    gx = 2*x;
    gy = 2*y;
    double violation = 1-x-y;
    if(violation > 0)
    {
        gx -= 2*r*violation;
        gy -= 2*r*violation;
    }
}

void penalty_method(double& x, double& y)
{
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    double r = 1;
    double beta = 10;
    double alpha = 0.01;

    for(int k=0;k<5;k++)
    {
        for(int i=0;i<1000;i++)
        {
            double gx,gy;
            gradient(x, y, r, gx, gy);
            x -= alpha*gx;
            y -= alpha*gy;
        }

        r *= beta;

        double current_value = function_target(x, y);
        logger->info("Iteration " + std::to_string(k) + ": value = " + std::to_string(current_value));

        cout<<"r = "<<r
            <<"  x = "<<x
            <<"  y = "<<y
            <<endl;
    }
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b)
{
    (void)target_f;
    double x = (double)a;
    double y = (double)b;
    penalty_method(x, y);
    return (T)function_target(x, y);
}
}

template<typename T>
auto penalty(){
    return &penalty_name::f<T>;
}
