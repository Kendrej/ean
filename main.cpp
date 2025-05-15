#include <QApplication>
#include "MainWindow.h"
#include <mpreal.h>

using mpfr::mpreal;

int main(int argc, char *argv[]) {
    mpreal::set_default_prec(256);  // zwiększona precyzja dla wszystkich zmiennych mpreal

    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
