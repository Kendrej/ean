// Solver.h
#pragma once

#include <vector>
#include "mpreal.h"
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/rounded_transc.hpp>
#include <boost/numeric/interval/policies.hpp>

using mpfr::mpreal;
using namespace boost::numeric;
using IntervalMP = interval<
    mpreal,
    interval_lib::policies<
        interval_lib::save_state<interval_lib::rounded_transc_std<mpreal>>,
        interval_lib::checking_no_nan<mpreal>
    >
>;

template<typename T>
using Matrix = std::vector<std::vector<T>>;

template<typename T>
using Vector = std::vector<T>;

class Solver {
public:
    template<typename T>
    static Vector<T> solveCrout(const Matrix<T> &A, const Vector<T> &b);

    template<typename T>
    static Vector<T> solveCroutSymmetric(const Matrix<T> &A, const Vector<T> &b);

    template<typename T>
    static Vector<T> solveCroutTridiagonal(const Matrix<T> &A, const Vector<T> &b);
};
