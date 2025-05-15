#include "MatrixInputWidget.h"
#include "Parser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

MatrixInputWidget::MatrixInputWidget(int n, QWidget *parent) : QWidget(parent), size(n) {
    matrixTable = new QTableWidget(size, size);
    vectorTable = new QTableWidget(size, 1);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(matrixTable);
    layout->addWidget(vectorTable);
    setLayout(layout);
}

std::vector<std::vector<mpfr::mpreal>> MatrixInputWidget::getMatrix(bool) {
    std::vector<std::vector<mpfr::mpreal>> mat(size, std::vector<mpfr::mpreal>(size));
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            mat[i][j] = Parser::parseInput(matrixTable->item(i, j)->text()).singleValue;
    return mat;
}

std::vector<mpfr::mpreal> MatrixInputWidget::getVector(bool) {
    std::vector<mpfr::mpreal> vec(size);
    for (int i = 0; i < size; ++i)
        vec[i] = Parser::parseInput(vectorTable->item(i, 0)->text()).singleValue;
    return vec;
}