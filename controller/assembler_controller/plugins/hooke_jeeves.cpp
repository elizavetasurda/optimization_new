#include <iostream>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace hooke_jeeves_name{

double f_target(double x, double y)
{
    return pow(x - 2, 2) + pow(y - 3, 2);
}

void hooke_jeeves(double& x, double& y)
{
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    double alpha = 1.0;
    double eps = 0.001;
    double gamma = 0.5;
    int iter = 0;

    while (alpha > eps)
    {
        bool improved = false;
        double bestX = x;
        double bestY = y;
        double bestValue = f_target(x, y);

        if (f_target(x + alpha, y) < bestValue)
        {
            bestX = x + alpha;
            bestValue = f_target(bestX, y);
            improved = true;
        }
        if (f_target(x - alpha, y) < bestValue)
        {
            bestX = x - alpha;
            bestValue = f_target(bestX, y);
            improved = true;
        }

        if (f_target(bestX, y + alpha) < bestValue)
        {
            bestY = y + alpha;
            bestValue = f_target(bestX, bestY);
            improved = true;
        }

        if (f_target(bestX, y - alpha) < bestValue)
        {
            bestY = y - alpha;
            bestValue = f_target(bestX, bestY);
            improved = true;
        }

        x = bestX;
        y = bestY;

        double current_value = f_target(x, y);
        logger->info("Iteration " + std::to_string(iter) + ": value = " + std::to_string(current_value));

        iter++;

        if (!improved)
            alpha *= gamma;
    }
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b)
{
    (void)target_f;
    double x = (double)a;
    double y = (double)b;
    hooke_jeeves(x, y);
    return (T)f_target(x, y);
}
}

template<typename T>
auto hooke_jeeves(){
    return &hooke_jeeves_name::f<T>;
}
