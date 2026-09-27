#include <functional>
#include <cmath>
#include <iostream>

const float EPS = 1e-6;

float f(float x) {
    return (x - 2) * (x - 2) + std::sin(x) * x;
}

float Metog_Porabola(std::function<float(float)> f, float a, float c) {
    float b = (c + a) / 2;
    float x = b;          
    float b_old = b;   

    float fa = f(a);
    float fb = f(b);
    float fc = f(c);
    float fb_old = fb;

    do {
        b_old = b;       
        fb_old = fb;     

        float ca = c - a;
        float cb = c - b;
        float ab = a + b;

        float znam = cb * fa + (a - c) * fb + (b - a) * fc;
        if (std::abs(znam) < 1e-12) {
            break;        // вырожденная парабола
        }
        x = -0.5f * (((fb - fa) * ca * cb - ab * znam) / znam);
        float fx = f(x);

        std::cout << "x = " << x << ", f(x) = " << fx
                  << ", a = " << a << ", b = " << b
                  << ", f(b) = " << fb << ", c = " << c << "\n";

        if (x <= a || x >= c) {
            break;
        }

        if (fx < fb) {
            if (x < b) {
                c = b;
                fc = fb;  
                b = x;
                fb = fx;
            } else { 
                a = b;
                fa = fb;  
                b = x;
                fb = fx;
            }
        } else {
            if (x < b) {
                a = x;
                fa = fx;
            } else { 
                c = x;
                fc = fx;
            }
        }

    } while ((c - a) > EPS || std::abs(fb - fb_old) > EPS);

    return b;
}

int main() {
    float res_x = Metog_Porabola(f, -10, 2);
    std::cout << "res : x = " << res_x << ", y = " << f(res_x) << std::endl;
    return 0;
}
