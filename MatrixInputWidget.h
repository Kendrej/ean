#ifndef MATRIXINPUTWIDGET_H
#define MATRIXINPUTWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <vector>
#include <mpreal.h>
#include <boost/numeric/interval.hpp>

class MatrixInputWidget : public QWidget {
    Q_OBJECT

public:
    MatrixInputWidget(int size, QWidget *parent = nullptr);
    void setSize(int n);
    std::vector<std::vector<mpfr::mpreal>> getMatrix(bool useInterval);
    std::vector<mpfr::mpreal> getVector(bool useInterval);

private:
    QTableWidget *matrixTable;
    QTableWidget *vectorTable;
    int size;
};

#endif // MATRIXINPUTWIDGET_H