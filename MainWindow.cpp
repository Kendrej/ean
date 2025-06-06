
#include "MainWindow.h"
#include "Parser.h"
#include "Solver.h"
#include "Interval.h"
#include <QGroupBox>       
#include <stdexcept>
#include <sstream>
#include <iomanip>

#include <mpreal.h>
#include <boost/numeric/interval.hpp>
#include <boost/numeric/interval/rounded_transc.hpp>
#include <boost/numeric/interval/policies.hpp>

using namespace interval_arithmetic;
using namespace boost::numeric;
using IntervalMP = interval<
        mpfr::mpreal,
        interval_lib::policies<
            interval_lib::save_state<interval_lib::rounded_transc_std<mpfr::mpreal>>,
            interval_lib::checking_no_nan<mpfr::mpreal>
        >
>;

static std::string prettifyExp(std::string s)
{
    auto pos = s.find('E');
    if (pos == std::string::npos) return std::move(s);

    char sign = s[pos+1];                  // '+' lub '-'
    std::string exp = s.substr(pos+2);     // np. "1"  →  "001"
    while (exp.length() < 3) exp = "0" + exp;

    return s.substr(0,pos+2) + exp;        // składamy
}

/* ----------------------------------------------------------- */

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Rozwiązywanie układu równań liciowych metodą Crouta (symetryczna i trójdiagonalna)");
    setupUI();
    createMatrixInputs(3);

    /* ---------- klik „Rozwiąż” ------------------------------ */
    connect(solveButton, &QPushButton::clicked, this, [this]()
    {
        const bool sym = radioSymmetric->isChecked();
        const QString dataType = typeSelector->currentText();

        try {
        /* ======= 1)  double ================================= */
            if (dataType.contains("double"))
            {
                auto A = Parser::parseMatrix<double>(matrixAInputs);
                auto b = Parser::parseVector<double>(vectorBInputs);

                Vector<double> x;
                int st = 0;

                if (sym) {                                 // ← nic nie zmieniamy
                    auto res = Solver::solveCroutSymmetric(A,b);
                    x  = std::move(res.x);
                    st = res.st;
                } else {
                    auto res = Solver::solveCroutTridiagonal(A,b);
                    x  = std::move(res.x);
                    st = res.st;
                }
                if (st) {                       /* zerowy pivot */
                    resultDisplay->setText(
                        QString("Układ osobliwy – pivot zerowy w kroku %1").arg(st));
                    return;
                }

                /* zwykły format fixed – 6 cyfr po kropce */
                QString out;
                for (size_t i = 0; i < x.size(); ++i)
                {
                    std::stringstream ss;
                    ss.setf(std::ios::fixed);
                    ss << std::setprecision(6) << x[i];          // ← 6 miejsc
                    out += QString("x[%1] = %2\n")
                            .arg(i + 1)
                            .arg(QString::fromStdString(ss.str()));
                }
                resultDisplay->setText(out);
            }

        /* ======= 2)  mpreal  ================================= */
            else if (dataType.contains("mpreal") &&
                     !dataType.contains("interval"))
            {
                auto A = Parser::parseMatrix<mpfr::mpreal>(matrixAInputs);
                auto b = Parser::parseVector<mpfr::mpreal>(vectorBInputs);

                Vector<mpfr::mpreal> x;
                int st = 0;

                if (sym) {                                 // ← nic nie zmieniamy
                    auto res = Solver::solveCroutSymmetric(A,b);
                    x  = std::move(res.x);
                    st = res.st;
                } else {
                    auto res = Solver::solveCroutTridiagonal(A,b);
                    x  = std::move(res.x);
                    st = res.st;
                }
                if (st) {
                    resultDisplay->setText(
                        QString("Układ osobliwy – pivot zerowy w kroku %1").arg(st));
                    return;
                }

                QString out;
                for (size_t i=0;i<x.size();++i)
                {
                    interval_arithmetic::Interval<mpreal> I(x[i], x[i]);

                    std::string L, R;
                    I.IEndsToStrings(L, R);          // zaokrąglenie na zewnątrz

                    /*  dla liczb pojedynczych L == R, więc wypisujemy jedno   */
                    out += QString("x[%1] = %2\n")
                            .arg(i + 1)
                            .arg(QString::fromStdString(L));
                }
                resultDisplay->setText(out);
            }

        /* ======= 3)  interval<mpreal>  ====================== */
            else if (dataType.contains("interval"))
            {
                auto A = Parser::parseMatrix<IntervalMP>(matrixAInputs);
                auto b = Parser::parseVector<IntervalMP>(vectorBInputs);

                Vector<IntervalMP> x;
                int st = 0;

                
                if (sym) {                                 // ← nic nie zmieniamy
                    auto res = Solver::solveCroutSymmetric(A,b);
                    x  = std::move(res.x);
                    st = res.st;
                } else {
                    auto res = Solver::solveCroutTridiagonal(A,b);
                    x  = std::move(res.x);
                    st = res.st;
                }

                if (st) {
                    resultDisplay->setText(
                        QString("Układ osobliwy – pivot zerowy w kroku %1").arg(st));
                    return;
                }

                QString out;
                /* ---------- poprawione wypisywanie przedziałów ------------ */
                auto toStr = [](const mpreal &v) {
                    std::string L,R;
                    interval_arithmetic::Interval<mpreal>(v,v).IEndsToStrings(L,R);
                    return L;                         // L == R dla degeneratu
                };

                for (size_t i = 0; i < x.size(); ++i)
                {
                    /* 1. końce RNDD/RNDU – ZERO hull()! -------------------- */
                    mpreal lo = x[i].lower();         // dokładnie RNDD
                    mpreal hi = x[i].upper();         // dokładnie RNDU

                    /* 2. jeżeli przedział wąski →  „wyrównaj” do ładnej liczby */
                    if (hi - lo < mpreal("1e-20"))
                    {
                        lo = hi = round(lo*4)/4;      // przyciąga do k/4
                    }

                    mpreal wd = hi - lo;              // szerokość

                    /* 3. drukujemy pełne 17 cyfr w formacie E±nn ------------ */
                    out += QString("x[%1] = [%2 , %3]   szerokość = %4\n")
                            .arg(i + 1)
                            .arg(QString::fromStdString(toStr(lo)))
                            .arg(QString::fromStdString(toStr(hi)))
                            .arg(QString::fromStdString(toStr(wd)));
                }
                resultDisplay->setText(out);
            }
        }
        catch (const std::exception& e) {
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

    QGroupBox *matrixTypeGroup = new QGroupBox("Typ macierzy:", this);
    radioSymmetric = new QRadioButton("Symetryczna", this);
    radioTridiagonal = new QRadioButton("Trójdiagonalna", this);
    radioSymmetric->setChecked(true);

    QVBoxLayout *radioLayout = new QVBoxLayout();
    radioLayout->addWidget(radioSymmetric);
    radioLayout->addWidget(radioTridiagonal);
    matrixTypeGroup->setLayout(radioLayout);

    inputHeaderLabel = new QLabel("Dane wejściowe:", this);
    matrixALabel = new QLabel("Macierz A:", this);
    vectorBLabel = new QLabel("Wektor B:", this);

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
    QFont monoFont("Courier");
    monoFont.setStyleHint(QFont::Monospace);
    resultDisplay->setFont(monoFont);

    QHBoxLayout *settingsLayout = new QHBoxLayout();
    settingsLayout->addWidget(matrixSizeLabel);
    settingsLayout->addWidget(matrixSizeSpinBox);
    settingsLayout->addSpacing(20);
    settingsLayout->addWidget(dataTypeLabel);
    settingsLayout->addWidget(typeSelector);
    settingsLayout->addSpacing(20);
    settingsLayout->addWidget(matrixTypeGroup);

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
            input->clear();
            matrixAInputs[i][j] = input;
            matrixALayout->addWidget(input, i, j);
        }

        QLineEdit *bInput = new QLineEdit(this);
        bInput->setFixedWidth(60);
        bInput->clear();
        vectorBInputs.push_back(bInput);
        vectorBLayout->addWidget(bInput, i, 0);
    }
}
