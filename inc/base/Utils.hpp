#pragma once

namespace Utils
{
template <size_t N, typename T = float>
void mul(const T mat0[N * N], const T mat1[N * N], T mat[N * N])
{
    T temp;
    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = 0; j < N; ++j)
        {
            temp = 0;
            for (size_t k = 0; k < N; ++k)
            {
                temp += (mat0[N * i + k] * mat1[N * k + j]);
            }
            mat[N * i + j] = temp;
        }
    }
}

template <size_t N, typename T = float>
void mul(T mat[N * N], const T value)
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] *= value;
    }
}

template <size_t N, typename T = float>
void dot(const T mat0[N * N], const T mat1[N * N], T mat[N * N])
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] = mat0[i] * mat1[i];
    }
}

template <size_t N, typename T = float>
void add(const T mat0[N * N], const T mat1[N * N], T mat[N * N])
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] = mat0[i] + mat1[0];
    }
}

template <size_t N, typename T = float>
void add(T mat[N * N], const T value)
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] += value;
    }
}

template <size_t N, typename T = float>
void sub(const T mat0[N * N], const T mat1[N * N], T mat[N * N])
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] = mat0[i] - mat1[0];
    }
}

template <size_t N, typename T = float>
void sub(T mat[N * N], const T value)
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] -= value;
    }
}

template <size_t N, typename T = float>
void div(const T mat0[N * N], const T mat1[N * N], T mat[N * N])
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] = mat0[i] / mat1[0];
    }
}

template <size_t N, typename T = float>
void div(T mat[N * N], const T value)
{
    for (size_t i = 0; i < N * N; ++i)
    {
        mat[i] /= value;
    }
}

};