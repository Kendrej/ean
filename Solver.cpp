/* ===========================================================
 *  Solver.cpp
 * ========================================================= */
#include "Solver.h"
#include <stdexcept>
#include <cmath>          //  std::abs – dla double/long double

/* ——— na samej górze Solver.cpp (po #include-ach) ——— */
template<typename T>
inline T aabs(const T& x)
{
    using std::abs;           // int, double, long double
    using mpfr::abs;          // mpreal   (ADL)
    using boost::numeric::abs;/* interval (ADL)           */
    return abs(x);            // wybierze właściwą wersję
}

/* -----------------------------------------------------------
   1.  Pełna macierz – klasyczny LU-Crout
   ----------------------------------------------------------- */
template<typename T>
Vector<T> Solver::solveCrout(const Matrix<T>& A, const Vector<T>& b)
{
    const int n = A.size();
    Matrix<T> L(n, Vector<T>(n, T(0)));
    Matrix<T> U(n, Vector<T>(n, T(0)));

    for (int i=0;i<n;++i) U[i][i] = T(1);

    for (int j=0;j<n;++j)
    {
        for (int i=j;i<n;++i)                      /* kolumna L */
        {
            T s = T(0);
            for (int k=0;k<j;++k) s += L[i][k]*U[k][j];
            L[i][j] = A[i][j] - s;
        }
        if (L[j][j] == T(0))
            throw std::runtime_error("Pivot zero – Crout");

        for (int i=j+1;i<n;++i)                   /* wiersz U */
        {
            T s = T(0);
            for (int k=0;k<j;++k) s += L[j][k]*U[k][i];
            U[j][i] = (A[j][i] - s)/L[j][j];
        }
    }

    Vector<T> y(n), x(n);
    for (int i=0;i<n;++i)                         /* Ly = b  */
    {
        T s = T(0);
        for (int k=0;k<i;++k) s += L[i][k]*y[k];
        y[i] = (b[i] - s)/L[i][i];
    }
    for (int i=n-1;i>=0;--i)                      /* Ux = y  */
    {
        T s = T(0);
        for (int k=i+1;k<n;++k) s += U[i][k]*x[k];
        x[i] = y[i] - s;
    }
    return x;
}

/* -----------------------------------------------------------
   2.  Pełna macierz SYMETRYCZNA (SPD) – z kodem st
   ----------------------------------------------------------- */
template<typename T>
TriResult<T> Solver::solveCroutSymmetric(const Matrix<T>& A,
                                         const Vector<T>& b)
{
    const int n   = A.size();
    const T   eps = T(1e-20);

    Matrix<T> L(n, Vector<T>(n, T(0)));
    Matrix<T> U(n, Vector<T>(n, T(0)));     // U – diag = 1
    for (int i=0;i<n;++i) U[i][i] = T(1);

    int st = 0;                             // kod statusu

    for (int j=0;j<n;++j)
    {
        for (int i=j;i<n;++i)               /* kolumna L */
        {
            T s = T(0);
            for (int k=0;k<j;++k) s += L[i][k]*U[k][j];
            L[i][j] = A[i][j] - s;
        }

        /* --- warunek SPD: nie-zerowy i > 0 ---------------- */
        if (aabs(L[j][j]) < eps || L[j][j] <= T(0)) {
            st = j+1;                       // pierwszy „zły” pivot
            break;
        }

        for (int i=j+1;i<n;++i)             /* wiersz U */
        {
            T s = T(0);
            for (int k=0;k<j;++k) s += L[j][k]*U[k][i];
            U[j][i] = (A[j][i] - s)/L[j][j];
        }
    }

    Vector<T> y(n,T(0)), x(n,T(0));
    if (st == 0)                            /* tylko gdy OK */
    {
        for (int i=0;i<n;++i)               /* Ly = b */
        {
            T s = T(0);
            for (int k=0;k<i;++k) s += L[i][k]*y[k];
            y[i] = (b[i] - s)/L[i][i];
        }
        for (int i=n-1;i>=0;--i)            /* Ux = y */
        {
            T s = T(0);
            for (int k=i+1;k<n;++k) s += U[i][k]*x[k];
            x[i] = y[i] - s;
        }
    }
    return { x, st };
}

/* -----------------------------------------------------------
   3.  Macierz trójdiagonalna – z kodem st
   ----------------------------------------------------------- */
template<typename T>
TriResult<T> Solver::solveCroutTridiagonal(const Matrix<T>& A,
                                           const Vector<T>& b)
{
    const int n   = A.size();
    const T   eps = T(1e-20);

    std::vector<T> a(n), d(n), c(n);
    for (int i=0;i<n;++i) {
        d[i] = A[i][i];
        if (i>0)   a[i] = A[i][i-1];
        if (i<n-1) c[i] = A[i][i+1];
    }

    std::vector<T> l(n), u(n), y(n), x(n);
    int st = 0;

    l[0] = d[0];
    if (aabs(l[0]) < eps) st = 1;
    else
    {
        u[0] = c[0]/l[0];
        y[0] = b[0]/l[0];

        for (int i=1;i<n && st==0;++i)
        {
            l[i] = d[i] - a[i]*u[i-1];
            if (aabs(l[i]) < eps) { st = i+1; break; }

            if (i<n-1) u[i] = c[i]/l[i];
            y[i] = (b[i] - a[i]*y[i-1])/l[i];
        }
    }

    if (st==0)
    {
        x[n-1] = y[n-1];
        for (int i=n-2;i>=0;--i) x[i] = y[i] - u[i]*x[i+1];
    }
    return { x, st };
}

/* ---------- eksplicytne instancje szablonów ----------------------------- */
template Vector<double>
        Solver::solveCrout(const Matrix<double>&, const Vector<double>&);
template Vector<mpreal>
        Solver::solveCrout(const Matrix<mpreal>&, const Vector<mpreal>&);
template Vector<IntervalMP>
        Solver::solveCrout(const Matrix<IntervalMP>&, const Vector<IntervalMP>&);

template TriResult<double>
        Solver::solveCroutSymmetric(const Matrix<double>& ,const Vector<double>&);
template TriResult<mpreal>
        Solver::solveCroutSymmetric(const Matrix<mpreal>& ,const Vector<mpreal>&);
template TriResult<IntervalMP>
        Solver::solveCroutSymmetric(const Matrix<IntervalMP>& ,const Vector<IntervalMP>&);

template TriResult<double>
        Solver::solveCroutTridiagonal(const Matrix<double>& ,const Vector<double>&);
template TriResult<mpreal>
        Solver::solveCroutTridiagonal(const Matrix<mpreal>& ,const Vector<mpreal>&);
template TriResult<IntervalMP>
        Solver::solveCroutTridiagonal(const Matrix<IntervalMP>& ,const Vector<IntervalMP>&);
