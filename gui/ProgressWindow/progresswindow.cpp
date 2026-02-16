#include "progresswindow.h"
#include "ui_progresswindow.h"

ProgressWindow::ProgressWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProgressWindow)
{
    ui->setupUi(this);
}

ProgressWindow::~ProgressWindow()
{
    delete ui;
}

void ProgressWindow::update(int persent)
{
    ui->pProg->setValue(persent);
    ui->pProg->repaint();
}
