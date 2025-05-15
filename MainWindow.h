#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLabel *matrixSizeLabel;
    QSpinBox *matrixSizeSpinBox;
    QLabel *dataTypeLabel;
    QComboBox *typeSelector;
    QLabel *matrixTypeLabel;
    QComboBox *matrixSelector;
    QLabel *inputHeaderLabel;
    QLabel *matrixALabel;
    QLabel *vectorBLabel;
    QPushButton *solveButton;
    QTextEdit *resultDisplay;

    QVBoxLayout *mainLayout;
    QHBoxLayout *matrixInputLayout;
    QGridLayout *matrixALayout;
    QGridLayout *vectorBLayout;

    QVector<QVector<QLineEdit*>> matrixAInputs;
    QVector<QLineEdit*> vectorBInputs;

    void setupUI();
    void clearMatrixInputs();
    void createMatrixInputs(int size);
};
