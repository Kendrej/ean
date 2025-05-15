#include "MainWindow.h"
#include "Parser.h"
#include "Solver.h"
#include <stdexcept>
#include <sstream>
#include <mpreal.h>
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/rounded_transc.hpp>
#include <boost/numeric/interval/policies.hpp>

using namespace boost::numeric;
using IntervalMP = interval<
    mpfr::mpreal,
    interval_lib::policies<
        interval_lib::save_state<interval_lib::rounded_transc_std<mpfr::mpreal>>,
        interval_lib::checking_no_nan<mpfr::mpreal>
    >
>;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    createMatrixInputs(3); // domyślny rozmiar

    connect(solveButton, &QPushButton::clicked, this, [this]() {
        int size = matrixSizeSpinBox->value();
        QString dataType = typeSelector->currentText();
        QString matrixType = matrixSelector->currentText();

        try {
            if (dataType.contains("double")) {
                auto A = Parser::parseMatrix<double>(matrixAInputs);
                auto b = Parser::parseVector<double>(vectorBInputs);
                std::vector<double> x;
                if (matrixType.contains("Symetryczna")) {
                    x = Solver::solveCroutSymmetric(A, b);
                } else {
                    x = Solver::solveCroutTridiagonal(A, b);
                }

                QString result;
                for (size_t i = 0; i < x.size(); ++i)
                    result += QString("x%1 = %2\n").arg(i + 1).arg(x[i]);
                resultDisplay->setText(result);

            } else if (dataType.contains("mpreal") && !dataType.contains("interval")) {
                auto A = Parser::parseMatrix<mpfr::mpreal>(matrixAInputs);
                auto b = Parser::parseVector<mpfr::mpreal>(vectorBInputs);
                std::vector<mpfr::mpreal> x;
                if (matrixType.contains("Symetryczna")) {
                    x = Solver::solveCroutSymmetric(A, b);
                } else {
                    x = Solver::solveCroutTridiagonal(A, b);
                }

                QString result;
                for (size_t i = 0; i < x.size(); ++i) {
                    std::stringstream ss;
                    ss.precision(50);
                    ss << std::fixed << x[i];
                    result += QString("x%1 = %2\n").arg(i + 1).arg(QString::fromStdString(ss.str()));
                }
                resultDisplay->setText(result);

            } else if (dataType.contains("interval")) {
                auto A = Parser::parseMatrix<IntervalMP>(matrixAInputs);
                auto b = Parser::parseVector<IntervalMP>(vectorBInputs);
                std::vector<IntervalMP> x;
                if (matrixType.contains("Symetryczna")) {
                    x = Solver::solveCroutSymmetric(A, b);
                } else {
                    x = Solver::solveCroutTridiagonal(A, b);
                }

                QString result;
                for (size_t i = 0; i < x.size(); ++i) {
                    std::stringstream ss;
                    ss.precision(30);
                    ss << "[" << x[i].lower() << ", " << x[i].upper() << "]";
                    result += QString("x%1 = %2\n").arg(i + 1).arg(QString::fromStdString(ss.str()));
                }
                resultDisplay->setText(result);
            }
        } catch (const std::exception &e) {
            resultDisplay->setText(QString("Błąd: ") + e.what());
        }
    });
}


MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    matrixSizeLabel = new QLabel("Rozmiar macierzy:", this);
    matrixSizeSpinBox = new QSpinBox(this);
    matrixSizeSpinBox->setMinimum(2);
    matrixSizeSpinBox->setMaximum(10);
    matrixSizeSpinBox->setValue(3);
    connect(matrixSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::createMatrixInputs);

    dataTypeLabel = new QLabel("Typ danych:", this);
    typeSelector = new QComboBox(this);
    typeSelector->addItem("Liczby zmiennoprzecinkowe (double)");
    typeSelector->addItem("Liczby wysokiej precyzji (mpreal)");
    typeSelector->addItem("Przedziały (interval<mpreal>)");

    matrixTypeLabel = new QLabel("Typ macierzy:", this);
    matrixSelector = new QComboBox(this);
    matrixSelector->addItem("Symetryczna");
    matrixSelector->addItem("Trójdiagonalna");

    inputHeaderLabel = new QLabel("Dane wejściowe:", this);
    matrixALabel = new QLabel("Macierz A:", this);
    vectorBLabel = new QLabel("Macierz B:", this);

    matrixALayout = new QGridLayout();
    vectorBLayout = new QGridLayout();

    matrixInputLayout = new QHBoxLayout();
    QVBoxLayout *matrixAWithLabel = new QVBoxLayout();
    matrixAWithLabel->addWidget(matrixALabel);
    matrixAWithLabel->addLayout(matrixALayout);
    QVBoxLayout *vectorBWithLabel = new QVBoxLayout();
    vectorBWithLabel->addWidget(vectorBLabel);
    vectorBWithLabel->addLayout(vectorBLayout);

    matrixInputLayout->addLayout(matrixAWithLabel);
    matrixInputLayout->addSpacing(20);
    matrixInputLayout->addLayout(vectorBWithLabel);

    solveButton = new QPushButton("Rozwiąż", this);
    resultDisplay = new QTextEdit(this);
    resultDisplay->setReadOnly(true);

    QHBoxLayout *settingsLayout = new QHBoxLayout();
    settingsLayout->addWidget(matrixSizeLabel);
    settingsLayout->addWidget(matrixSizeSpinBox);
    settingsLayout->addSpacing(20);
    settingsLayout->addWidget(dataTypeLabel);
    settingsLayout->addWidget(typeSelector);
    settingsLayout->addSpacing(20);
    settingsLayout->addWidget(matrixTypeLabel);
    settingsLayout->addWidget(matrixSelector);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(settingsLayout);
    mainLayout->addWidget(inputHeaderLabel);
    mainLayout->addLayout(matrixInputLayout);
    mainLayout->addWidget(solveButton);
    mainLayout->addWidget(resultDisplay);
    setLayout(mainLayout);
}

void MainWindow::clearMatrixInputs() {
    for (auto &row : matrixAInputs)
        for (auto *cell : row)
            delete cell;
    for (auto *cell : vectorBInputs)
        delete cell;

    matrixAInputs.clear();
    vectorBInputs.clear();
}

void MainWindow::createMatrixInputs(int size) {
    clearMatrixInputs();

    matrixAInputs.resize(size);
    for (int i = 0; i < size; ++i) {
        matrixAInputs[i].resize(size);
        for (int j = 0; j < size; ++j) {
            QLineEdit *input = new QLineEdit(this);
            input->setFixedWidth(60);
            matrixAInputs[i][j] = input;
            matrixALayout->addWidget(input, i, j);
        }

        QLineEdit *bInput = new QLineEdit(this);
        bInput->setFixedWidth(60);
        vectorBInputs.push_back(bInput);
        vectorBLayout->addWidget(bInput, i, 0);
    }
}
