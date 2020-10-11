#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QSpinBox>

#include "myopenglwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    // Field: Additional UI
    Ui::MainWindow *ui;
    QLabel *fLabCurFile;
    QProgressBar *progressBar1;
    QSpinBox *spinGrain;
    QDoubleSpinBox *spinTau;
    QSpinBox *spinFPS;
    QSpinBox *spinTimePerRun;

private:
    // Function: Additional UI initialize
    void iniUI();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot: Button Interaction
    void on_actPen_triggered();
    void on_actEraser_triggered();
    void on_actionactAnchor_triggered();
    void on_actStart_triggered();
    void on_actStop_triggered();
    // Slot: Data Input
    void on_spinGrain_valueChanged(int);
    void on_spinTau_valueChanged(double);
    void on_spinFps_valueChanged(int);
    void on_spinTimePerRun_valueChanged(int);
    void on_actChange_triggered();
};
#endif // MAINWINDOW_H
