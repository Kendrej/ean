#include "Solver.h"
#include <stdexcept>

template<typename T>
Vector<T> Solver::solveCrout(const Matrix<T> &A, const Vector<T> &b) {
    int n = A.size();
    Matrix<T> L(n, Vector<T>(n, T(0)));
    Matrix<T> U(n, Vector<T>(n, T(0)));

    for (int i = 0; i < n; ++i)
        U[i][i] = T(1);

    for (int j = 0; j < n; ++j) {
        for (int i = j; i < n; ++i) {
            T sum = T(0);
            for (int k = 0; k < j; ++k)
                sum += L[i][k] * U[k][j];
            L[i][j] = A[i][j] - sum;
        }
        for (int i = j + 1; i < n; ++i) {
            T sum = T(0);
            for (int k = 0; k < j; ++k)
                sum += L[j][k] * U[k][i];
            if (L[j][j] == T(0))
                throw std::runtime_error("Dzielenie przez zero w metodzie Crouta.");
            U[j][i] = (A[j][i] - sum) / L[j][j];
        }
    }

    Vector<T> y(n);
    for (int i = 0; i < n; ++i) {
        T sum = T(0);
        for (int k = 0; k < i; ++k)
            sum += L[i][k] * y[k];
        y[i] = (b[i] - sum) / L[i][i];
    }

    Vector<T> x(n);
    for (int i = n - 1; i >= 0; --i) {
        T sum = T(0);
        for (int k = i + 1; k < n; ++k)
            sum += U[i][k] * x[k];
        x[i] = y[i] - sum;
    }

    return x;
}

template<typename T>
Vector<T> Solver::solveCroutSymmetric(const Matrix<T> &A, const Vector<T> &b) {
    int n = A.size();
    Matrix<T> L(n, Vector<T>(n, T(0)));
    Matrix<T> U(n, Vector<T>(n, T(0)));

    for (int i = 0; i < n; ++i)
        U[i][i] = T(1);

    for (int j = 0; j < n; ++j) {
        for (int i = j; i < n; ++i) {
            T sum = T(0);
            for (int k = 0; k < j; ++k)
                sum += L[i][k] * U[k][j];
            L[i][j] = A[i][j] - sum;
        }
        for (int i = j + 1; i < n; ++i) {
            T sum = T(0);
            for (int k = 0; k < j; ++k)
                sum += L[j][k] * U[k][i];
            if (L[j][j] == T(0))
                throw std::runtime_error("Dzielenie przez zero w metodzie Crouta (sym).");
            U[j][i] = (A[j][i] - sum) / L[j][j];
        }
    }

    Vector<T> y(n);
    for (int i = 0; i < n; ++i) {
        T sum = T(0);
        for (int k = 0; k < i; ++k)
            sum += L[i][k] * y[k];
        y[i] = (b[i] - sum) / L[i][i];
    }

    Vector<T> x(n);
    for (int i = n - 1; i >= 0; --i) {
        T sum = T(0);
        for (int k = i + 1; k < n; ++k)
            sum += U[i][k] * x[k];
        x[i] = y[i] - sum;
    }

    return x;
}

template<typename T>
Vector<T> Solver::solveCroutTridiagonal(const Matrix<T> &A, const Vector<T> &b) {
    int n = A.size();
    std::vector<T> a(n), d(n), c(n);

    for (int i = 0; i < n; ++i) {
        d[i] = A[i][i];
        if (i > 0) a[i] = A[i][i - 1];
        if (i < n - 1) c[i] = A[i][i + 1];
    }

    std::vector<T> l(n), u(n), y(n), x(n);
    l[0] = d[0];
    u[0] = c[0] / l[0];
    y[0] = b[0] / l[0];

    for (int i = 1; i < n; ++i) {
        l[i] = d[i] - a[i] * u[i - 1];
        if (i < n - 1)
            u[i] = c[i] / l[i];
        y[i] = (b[i] - a[i] * y[i - 1]) / l[i];
    }

    x[n - 1] = y[n - 1];
    for (int i = n - 2; i >= 0; --i)
        x[i] = y[i] - u[i] * x[i + 1];

    return x;
}

template Vector<double> Solver::solveCrout(const Matrix<double>&, const Vector<double>&);
template Vector<mpreal> Solver::solveCrout(const Matrix<mpreal>&, const Vector<mpreal>&);
template Vector<IntervalMP> Solver::solveCrout(const Matrix<IntervalMP>&, const Vector<IntervalMP>&);

template Vector<double> Solver::solveCroutSymmetric(const Matrix<double>&, const Vector<double>&);
template Vector<mpreal> Solver::solveCroutSymmetric(const Matrix<mpreal>&, const Vector<mpreal>&);
template Vector<IntervalMP> Solver::solveCroutSymmetric(const Matrix<IntervalMP>&, const Vector<IntervalMP>&);

template Vector<double> Solver::solveCroutTridiagonal(const Matrix<double>&, const Vector<double>&);
template Vector<mpreal> Solver::solveCroutTridiagonal(const Matrix<mpreal>&, const Vector<mpreal>&);
template Vector<IntervalMP> Solver::solveCroutTridiagonal(const Matrix<IntervalMP>&, const Vector<IntervalMP>&);
