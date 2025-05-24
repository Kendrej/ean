// MainWindow.h
#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void clearMatrixInputs();
    void createMatrixInputs(int size);

    /* --- GUI elementy --- */
    QLabel      *matrixSizeLabel;
    QSpinBox    *matrixSizeSpinBox;

    QLabel      *dataTypeLabel;
    QComboBox   *typeSelector;

    QRadioButton *radioSymmetric;
    QRadioButton *radioTridiagonal;

    QLabel      *inputHeaderLabel;
    QLabel      *matrixALabel;
    QLabel      *vectorBLabel;

    QGridLayout *matrixALayout;
    QGridLayout *vectorBLayout;

    QHBoxLayout *matrixInputLayout;
    QVBoxLayout *mainLayout;

    QPushButton *solveButton;
    QTextEdit   *resultDisplay;

    /* --- pola do wprowadzania liczb --- */
    std::vector<std::vector<QLineEdit*>> matrixAInputs;
    std::vector<QLineEdit*>              vectorBInputs;
};
