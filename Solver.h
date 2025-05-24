#pragma once
#include <vector>
#include "mpreal.h"
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/rounded_transc.hpp>
#include <boost/numeric/interval/policies.hpp>

using mpfr::mpreal;
using namespace boost::numeric;

/* --- przedział mp-real --------------------------------------------------- */
using IntervalMP = interval<
    mpreal,
    interval_lib::policies<
        interval_lib::save_state<interval_lib::rounded_transc_std<mpreal>>,
        interval_lib::checking_no_nan<mpreal>
    >
>;

/* --- aliasy -------------------------------------------------------------- */
template<typename T>
using Matrix = std::vector<std::vector<T>>;

template<typename T>
using Vector = std::vector<T>;

/* --- wynik z kodem statusu ---------------------------------------------- */
template<typename T>
struct TriResult
{
    Vector<T> x;   // wektor rozwiązań (albo 0 przy błędzie)
    int       st;  // 0 OK,  k>0 – zerowy / niedodatni pivot w kolumnie k
};

/* ======================================================================== */
class Solver
{
public:
    /* 1) pełna macierz – bez kodu statusu */
    template<typename T>
    static Vector<T>
    solveCrout(const Matrix<T>& A, const Vector<T>& b);

    /* 2) pełna macierz SYMETRYCZNA – z kodem statusu (Ta część zmieniona) */
    template<typename T>
    static TriResult<T>
    solveCroutSymmetric(const Matrix<T>& A, const Vector<T>& b);

    /* 3) macierz trójdiagonalna – z kodem statusu */
    template<typename T>
    static TriResult<T>
    solveCroutTridiagonal(const Matrix<T>& A, const Vector<T>& b);
};
