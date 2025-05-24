#pragma once
/* ===========================================================
 *  Solver.h
 *  ========================================================= */
#include <vector>
#include "mpreal.h"
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/rounded_transc.hpp>
#include <boost/numeric/interval/policies.hpp>

using mpfr::mpreal;
using namespace boost::numeric;

/* --------- typ przedziału wysokiej precyzji ---------------- */
using IntervalMP = interval<
    mpreal,
    interval_lib::policies<
        interval_lib::save_state<interval_lib::rounded_transc_std<mpreal>>,
        interval_lib::checking_no_nan<mpreal>
    >
>;

/* --------- aliasy macierz / wektor ------------------------- */
template<typename T>
using Matrix = std::vector<std::vector<T>>;

template<typename T>
using Vector = std::vector<T>;

/* --------- wynik solvera trójdiagonalnego ------------------ */
template<typename T>
struct TriResult
{
    Vector<T> x;   // rozwiązanie (jeśli st == 0)
    int       st;  // 0 = OK; k>0 = zerowy pivot w kroku k (1-based)
};

/* --------- klasa z metodami statycznymi -------------------- */
class Solver {
public:
    /* pełna macierz (Crout) */
    template<typename T>
    static Vector<T> solveCrout(const Matrix<T>& A, const Vector<T>& b);

    /* pełna macierz symetryczna (Crout) */
    template<typename T>
    static Vector<T> solveCroutSymmetric(const Matrix<T>& A, const Vector<T>& b);

    /* macierz trójdiagonalna (Crout) + status */
    template<typename T>
    static TriResult<T> solveCroutTridiagonal(const Matrix<T>& A, const Vector<T>& b);
};
