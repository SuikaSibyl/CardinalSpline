#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    iniUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iniUI()
{
    fLabCurFile = new QLabel;
    fLabCurFile->setMinimumWidth(150);
    fLabCurFile->setText("Lab1");
    ui->statusbar->addWidget(fLabCurFile);

    progressBar1=new QProgressBar;
    progressBar1->setMaximumWidth(200);
    progressBar1->setMinimum(5);
    progressBar1->setMaximum(50);
    progressBar1->setValue(ui->textEdit->font().pointSize());
    ui->statusbar->addWidget(progressBar1);

    ui->toolBar->addSeparator();
    spinGrain = new QSpinBox;
    spinGrain->setMinimum(1);
    spinGrain->setMaximum(50);
    spinGrain->setValue(20);
    spinGrain->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel("Grain"));
    ui->toolBar->addWidget(spinGrain);
    connect(spinGrain, SIGNAL(valueChanged(int)), this, SLOT(on_spinGrain_valueChanged(int)));

    ui->toolBar->addSeparator();
    spinTau = new QDoubleSpinBox;
    //spinTau->setRange(0,1);
    spinTau->setValue(0.5);
    spinTau->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel("Tau"));
    ui->toolBar->addWidget(spinTau);
    connect(spinTau, SIGNAL(valueChanged(double)), this, SLOT(on_spinTau_valueChanged(double)));

    ui->toolBar->addSeparator();
    spinFPS = new QSpinBox;
    spinFPS->setMinimum(10);
    spinFPS->setMaximum(100);
    spinFPS->setValue(60);
    spinFPS->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel("Fps"));
    ui->toolBar->addWidget(spinFPS);
    connect(spinFPS, SIGNAL(valueChanged(int)), this, SLOT(on_spinFps_valueChanged(int)));

    ui->toolBar->addSeparator();
    spinTimePerRun = new QSpinBox;
    spinTimePerRun->setMinimum(1);
    spinTimePerRun->setMaximum(100);
    spinTimePerRun->setValue(5);
    spinTimePerRun->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel("Time per run"));
    ui->toolBar->addWidget(spinTimePerRun);
    connect(spinTimePerRun, SIGNAL(valueChanged(int)), this, SLOT(on_spinTimePerRun_valueChanged(int)));

    setCentralWidget(ui->openGLWidget);
    ui->openGLWidget->setMouseTracking(true);
}

void MainWindow::on_actPen_triggered()
{
    ui->openGLWidget->setToolModePen();
}

void MainWindow::on_actEraser_triggered()
{
    ui->openGLWidget->setToolModeEraser();
}

void MainWindow::on_actionactAnchor_triggered()
{
    ui->openGLWidget->setToolModeAnchor();
}

void MainWindow::on_spinGrain_valueChanged(int grain)
{
    ui->openGLWidget->resetGrain(grain);
}

void MainWindow::on_spinTau_valueChanged(double tau)
{
    ui->openGLWidget->resetTau(tau);
}

void MainWindow::on_spinFps_valueChanged(int fps)
{
    ui->openGLWidget->resetFps(fps);
}

void MainWindow::on_spinTimePerRun_valueChanged(int timePerRun)
{
    ui->openGLWidget->resetTimePerRun(timePerRun);
}

void MainWindow::on_actStart_triggered()
{
    ui->openGLWidget->startAnimation();
}

void MainWindow::on_actStop_triggered()
{
    ui->openGLWidget->endAnimation();
}

void MainWindow::on_actChange_triggered()
{
    ui->openGLWidget->resetBondary();
}
