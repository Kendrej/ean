#include <QApplication>
#include "MainWindow.h"
#include <mpreal.h>
#include "Interval.h"
using mpfr::mpreal;
using namespace interval_arithmetic;

int main(int argc, char *argv[]) {
    mpreal::set_default_prec(256);  // zwiększona precyzja dla wszystkich zmiennych mpreal
    interval_arithmetic::Interval<mpreal>::Initialize();

    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
