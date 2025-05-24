#pragma once
/* ============================================================
 *  Parser.h  – parsowanie danych z QLineEdit-ów
 * ============================================================ */
#include <QVector>
#include <QLineEdit>
#include <vector>
#include <QString>
#include <stdexcept>

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

class Parser
{
public:
    /* --------- oryginalne wersje (QVector) ------------------ */
    template<typename T>
    static std::vector<std::vector<T>>
    parseMatrix(const QVector<QVector<QLineEdit *>> &inputs);

    template<typename T>
    static std::vector<T>
    parseVector(const QVector<QLineEdit *> &inputs);


    /* =========================================================
     *  NOWE ADAPTERY  std::vector  →  QVector
     *  (muszą być *zdefiniowane* tutaj, w nagłówku!)
     * ========================================================= */
    template<typename T>
    static std::vector<std::vector<T>>
    parseMatrix(const std::vector<std::vector<QLineEdit *>> &inputs)
    {
        QVector<QVector<QLineEdit *>> tmp;
        tmp.reserve(static_cast<int>(inputs.size()));

        for (const auto &row : inputs)
            tmp.push_back(QVector<QLineEdit *>(row.begin(), row.end()));

        return parseMatrix<T>(tmp);          // wywołanie wersji Qt-owej
    }

    template<typename T>
    static std::vector<T>
    parseVector(const std::vector<QLineEdit *> &inputs)
    {
        QVector<QLineEdit *> tmp(inputs.begin(), inputs.end());
        return parseVector<T>(tmp);          // wywołanie wersji Qt-owej
    }
};
