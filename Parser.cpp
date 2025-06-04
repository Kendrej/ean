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
    std::size_t commas = std::count(s.begin(), s.end(), ',');

    // JEŚLI jest dokładnie 1 przecinek → bierzemy dwa końce przedziału
    if (commas == 1)
    {
        // rozbijamy na lewy i prawy fragment łańcucha
        auto sep = s.find(',');
        std::string sl = s.substr(0, sep);
        std::string sr = s.substr(sep + 1);

        // każdą z tych części parsujemy funkcją IntRead<mpreal>,
        // by otrzymać najwęższy możliwy przedział obejmujący dokładną wartość zapisaną w tekście:
        auto IL = IntRead<mpreal>(sl);
        auto IR = IntRead<mpreal>(sr);

        mpreal leftEnd  = IL.a;   // dolna granica IntRead 
        mpreal rightEnd = IR.b;   // górna granica IntRead

        if (leftEnd > rightEnd)
            std::swap(leftEnd, rightEnd);

        return IntervalMP(leftEnd, rightEnd);
    }

    // JEŚLI nie ma przecinka → to jest pojedyncza liczba
    if (commas == 0)
    {
        auto I = IntRead<mpreal>(s);
        return IntervalMP(I.a, I.b);
    }

    // w każdym innym przypadku uznajemy format za niepoprawny
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
