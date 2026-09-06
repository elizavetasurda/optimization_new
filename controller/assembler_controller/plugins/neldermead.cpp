#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace neldermead_name{
using Point = vector<double>;

class NelderMead {
private:
    double alpha;
    double gamma;
    double rho;
    double sigma;

public:
    NelderMead(
        double alpha = 1.0,
        double gamma = 2.0,
        double rho = 0.5,
        double sigma = 0.5
    ) : alpha(alpha),
        gamma(gamma),
        rho(rho),
        sigma(sigma)
    {}

    pair<Point, double> minimize(
        function<double(const Point&)> f,
        Point start,
        double step = 1.0,
        double tol = 1e-8,
        int maxIter = 1000
    ) {
        Logger* logger = &Logger::getInstance();
        auto consoleSub = std::make_shared<ConsoleLogCommand>();
        logger->subscribe(LogLevel::INFO, consoleSub);

        int n = start.size();
        vector<Point> simplex(n + 1, start);
        for (int i = 0; i < n; i++) {
            simplex[i + 1][i] += step;
        }

        for (int iter = 0; iter < maxIter; iter++) {
            sort(simplex.begin(), simplex.end(),
                [&](const Point& a, const Point& b) {
                    return f(a) < f(b);
                });

            vector<double> values(n + 1);
            for (int i = 0; i <= n; i++) {
                values[i] = f(simplex[i]);
            }

            double current_value = values[0];
            logger->info("Iteration " + std::to_string(iter) + ": value = " + std::to_string(current_value));

            double diff = 0;
            for (int i = 1; i <= n; i++) {
                diff = max(diff, fabs(values[i] - values[0]));
            }
            if (diff < tol)
                break;

            Point best = simplex[0];
            Point worst = simplex[n];
            Point centroid(n, 0.0);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    centroid[j] += simplex[i][j];
                }
            }
            for (double& x : centroid) {
                x /= n;
            }

            Point xr(n);
            for (int i = 0; i < n; i++) {
                xr[i] = centroid[i] + alpha * (centroid[i] - worst[i]);
            }

            double fr = f(xr);

            if (fr < values[0]) {
                Point xe(n);
                for (int i = 0; i < n; i++) {
                    xe[i] = centroid[i] + gamma * (xr[i] - centroid[i]);
                }
                double fe = f(xe);
                if (fe < fr) {
                    simplex[n] = xe;
                }
                else {
                    simplex[n] = xr;
                }
            }
            else if (fr < values[n - 1]) {
                simplex[n] = xr;
            }
            else {
                Point xc(n);
                for (int i = 0; i < n; i++) {
                    xc[i] = centroid[i] + rho * (worst[i] - centroid[i]);
                }
                double fc = f(xc);
                if (fc < values[n]) {
                    simplex[n] = xc;
                }
                else {
                    for (int i = 1; i <= n; i++) {
                        for (int j = 0; j < n; j++) {
                            simplex[i][j] = best[j] + sigma * (simplex[i][j] - best[j]);
                        }
                    }
                }
            }
        }

        sort(simplex.begin(), simplex.end(),
            [&](const Point& a, const Point& b) {
                return f(a) < f(b);
            });

        return {simplex[0], f(simplex[0])};
    }
};

double parabola_target(const Point& x) {
    return pow(x[0] - 2, 2) + pow(x[1] - 3, 2);
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b) {
    (void)target_f;
    NelderMead nm;
    Point start = {(double)a, (double)b};
    auto result = nm.minimize(parabola_target, start, 1.0);
    return (T)result.second;
}
}

template<typename T>
auto neldermead(){
    return &neldermead_name::f<T>;
}
