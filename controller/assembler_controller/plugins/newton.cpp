template<typename T>
T derivative1(const std::function<T(T)>& f, T x, T h)
{
    return (f(x + h) - f(x - h)) / (static_cast<T>(2) * h);
}

template<typename T>
T derivative2(const std::function<T(T)>& f, T x, T h)
{
    return (f(x + h) - static_cast<T>(2) * f(x) + f(x - h)) / (h * h);
}

template<typename T>
T Newton(const std::function<T(T)>& f, T x, T eps, T h, int max_iter)
{
    for (int i = 0; i < max_iter; i++)
    {
        T df = derivative1<T>(f, x, h);
        T ddf = derivative2<T>(f, x, h);

        if (std::abs(df) < eps)
        {
            break;
        }

        if (std::abs(ddf) < eps)
        {
            break;
        }

        T x_new = x - df / ddf;

        if (std::abs(x_new - x) < eps)
        {
            x = x_new;
            break;
        }

        x = x_new;
    }

    return x;
}
