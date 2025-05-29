// Parser.cpp
#include "Parser.h"
#include <sstream>
#include <boost/numeric/interval/rounded_transc.hpp>
#include <boost/numeric/interval/policies.hpp>
#include "interval.h"
#include <algorithm>
using namespace boost::numeric;

// Parsowanie zwykłych typów
template<typename T>
T parseValue(const QString &text) {
    std::stringstream ss(text.toStdString());
    T value;
    ss >> value;
    if (ss.fail()) {
        throw std::runtime_error("Niepoprawna wartość: " + text.toStdString());
    }
    return value;
}

// Specjalizacja dla mpreal
template<>
mpreal parseValue<mpreal>(const QString &text) {
    return mpreal(text.toStdString());
}

// Specjalizacja dla interval<mpreal> w formacie "a,b"
template<>
IntervalMP parseValue<IntervalMP>(const QString &text)
{
    using namespace interval_arithmetic;

    std::string s = text.toStdString();

    /* — policz przecinki — */
    const std::size_t commas = std::count(s.begin(), s.end(), ',');

    /* 1)  format:  2,2   (przecinek dziesiętny PL) */
    if (commas == 1 && s.find('.') == std::string::npos)
    {
        s[s.find(',')] = '.';                 // zamień , → .
        auto I = IntRead<mpreal>(s);          // [lew, pra]
        return IntervalMP(I.a, I.b);
    }

    /* 2)  format:  a,b   (dwa końce przedziału) */
    if (commas == 1 && s.find('.') != std::string::npos)
    {
        const std::size_t sep = s.find(',');
        mpreal a(s.substr(0, sep));
        mpreal b(s.substr(sep + 1));
        if (a > b) std::swap(a, b);
        return IntervalMP(a, b);
    }

    /* 3)  format:  pojedyncza liczba z kropką */
    if (commas == 0)
    {
        auto I = IntRead<mpreal>(s);
        return IntervalMP(I.a, I.b);
    }

    throw std::runtime_error("Niepoprawny format przedziału");
}

template<typename T>
std::vector<std::vector<T>> Parser::parseMatrix(const QVector<QVector<QLineEdit*>> &inputs) {
    std::vector<std::vector<T>> matrix;
    for (const auto& row : inputs) {
        std::vector<T> rowValues;
        for (QLineEdit* cell : row) {
            rowValues.push_back(parseValue<T>(cell->text()));
        }
        matrix.push_back(rowValues);
    }
    return matrix;
}

template<typename T>
std::vector<T> Parser::parseVector(const QVector<QLineEdit*> &inputs) {
    std::vector<T> vec;
    for (QLineEdit* cell : inputs) {
        vec.push_back(parseValue<T>(cell->text()));
    }
    return vec;
}

// Wymuszenie instancji
template std::vector<std::vector<double>> Parser::parseMatrix(const QVector<QVector<QLineEdit*>> &);
template std::vector<std::vector<mpreal>> Parser::parseMatrix(const QVector<QVector<QLineEdit*>> &);
template std::vector<std::vector<IntervalMP>> Parser::parseMatrix(const QVector<QVector<QLineEdit*>> &);

template std::vector<double> Parser::parseVector(const QVector<QLineEdit*> &);
template std::vector<mpreal> Parser::parseVector(const QVector<QLineEdit*> &);
template std::vector<IntervalMP> Parser::parseVector(const QVector<QLineEdit*> &);
