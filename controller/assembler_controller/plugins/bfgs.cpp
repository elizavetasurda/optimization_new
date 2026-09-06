#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
#include "../../logger_controller/handler.hpp"
using namespace std;

namespace bfgs_name{
using Point = vector<double>;

double norm(const Point& x)
{
    double sum = 0.0;
    for(double v : x)
    {
        sum += v*v;
    }
    return sqrt(sum);
}

double dot(const Point& a, const Point& b)
{
    double sum = 0.0;
    for(size_t i = 0; i < a.size(); i++)
    {
        sum += a[i]*b[i];
    }
    return sum;
}

double rosenbrock(const Point& x)
{
    return pow(1-x[0],2) + 100*pow(x[1]-x[0]*x[0],2);
}

Point rosenbrock_grad(const Point& x)
{
    Point g(2);
    g[0] = -2*(1-x[0]) - 400*x[0]*(x[1]-x[0]*x[0]);
    g[1] = 200*(x[1]-x[0]*x[0]);
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
)
{
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    double fx = f(x);
    Point g = grad(x);
    double gtd = dot(g,d);
    Point x_new(x.size());
    while(alpha > 1e-12)
    {
        for(size_t i=0;i<x.size();i++)
        {
            x_new[i] = x[i] + alpha*d[i];
        }
        if(f(x_new) <= fx + beta*alpha*gtd)
        {
            break;
        }
        alpha *= rho;
    }
    return alpha;
}

Point matrix_vector(
    vector<vector<double>>& H,
    Point x
)
{
    Point result(x.size(),0.0);
    for(size_t i=0;i<x.size();i++)
    {
        for(size_t j=0;j<x.size();j++)
        {
            result[i]+=H[i][j]*x[j];
        }
    }
    return result;
}

vector<vector<double>> matrix_mult(
    vector<vector<double>>& A,
    vector<vector<double>>& B
)
{
    size_t n=A.size();
    vector<vector<double>> C(n, vector<double>(n,0.0));
    for(size_t i=0;i<n;i++)
    {
        for(size_t j=0;j<n;j++)
        {
            for(size_t k=0;k<n;k++)
            {
                C[i][j]+=A[i][k]*B[k][j];
            }
        }
    }
    return C;
}

Point bfgs_method(
    function<double(const Point&)> f,
    function<Point(const Point&)> grad,
    Point start,
    double tol = 1e-8,
    int max_iter = 1000
)
{
    Logger* logger = &Logger::getInstance();
    auto consoleSub = std::make_shared<ConsoleLogCommand>();
    logger->subscribe(LogLevel::INFO, consoleSub);

    size_t n=start.size();
    Point x=start;
    vector<vector<double>> H(n, vector<double>(n,0.0));
    for(size_t i=0;i<n;i++)
    {
        H[i][i]=1.0;
    }
    Point g=grad(x);

    for(int iter=0;iter<max_iter;iter++)
    {
        if(norm(g)<tol)
        {
            logger->info("Сошлись за " + std::to_string(iter) + " итераций");
            break;
        }

        Point Hg=matrix_vector(H, g);
        Point d(n);
        for(size_t i=0;i<n;i++)
        {
            d[i]=-Hg[i];
        }

        double alpha=line_search(f, grad, x, d);

        Point x_new=x;
        for(size_t i=0;i<n;i++)
        {
            x_new[i]+=alpha*d[i];
        }

        Point g_new=grad(x_new);
        Point s(n);
        Point y(n);
        for(size_t i=0;i<n;i++)
        {
            s[i]=x_new[i]-x[i];
            y[i]=g_new[i]-g[i];
        }

        double ys=dot(y,s);
        if(ys>1e-12)
        {
            double rho=1.0/ys;
            vector<vector<double>> I(n, vector<double>(n,0.0));
            for(size_t i=0;i<n;i++)
            {
                I[i][i]=1.0;
            }

            vector<vector<double>> A(n, vector<double>(n));
            vector<vector<double>> B(n, vector<double>(n));
            for(size_t i=0;i<n;i++)
            {
                for(size_t j=0;j<n;j++)
                {
                    A[i][j]=I[i][j]-rho*s[i]*y[j];
                    B[i][j]=I[i][j]-rho*y[i]*s[j];
                }
            }

            vector<vector<double>> temp = matrix_mult(A,H);
            vector<vector<double>> H_new = matrix_mult(temp,B);
            for(size_t i=0;i<n;i++)
            {
                for(size_t j=0;j<n;j++)
                {
                    H_new[i][j]+=rho*s[i]*s[j];
                }
            }
            H=H_new;
        }

        x=x_new;
        g=g_new;

        double current_value = f(x);
        logger->info("Iteration " + std::to_string(iter) + ": value = " + std::to_string(current_value));

        if(iter%10==0)
        {
            cout<<"iter = "<<iter<<", f = "<<current_value<<", alpha = "<<alpha<<endl;
        }
    }
    return x;
}

template<typename T>
T f(std::function<T(T)> target_f, T a, T b)
{
    (void)target_f;
    Point start = {(double)a, (double)b};
    Point result = bfgs_method(
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
auto bfgs(){
    return &bfgs_name::f<T>;
}
