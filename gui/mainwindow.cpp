#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <logfileparser.h>
#include <mathematics.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bFileName_clicked()
{
    QString filename = showFileDialog(this, "Выбор файла с логом событий для БЛА.", "", windowIcon());
    if (filename.isEmpty())
    {
        showMessageBox(this, "ОШИБКА!", "Файл лога не был выбран.", windowIcon());
        return;
    }
    ui->tFileName->setText(filename);

    std::map<uint64_t, mavlink_message_t> messages;
    try
    {
        LogFileParser lfp(filename.toStdString());
        messages = lfp.getMessagesByID(MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
    }
    catch (const std::exception &ex)
    {
        QString err(ex.what());
        showMessageBox(this, "ОШИБКА!", "Ошибка при открытии файла с логом полёта '" + filename + "'" + (err.isEmpty() ? "." : ":\n" + err), windowIcon());
        return;
    }

    if (messages.size() == 0)
    {
        showMessageBox(this, "ОШИБКА!", "Нет данных для расчёта места падения БЛА.", windowIcon());
        return;
    }

    std::array<mavlink_global_position_int_t, 3> glpos; // Нужно три точки для вычисления скорости и ускорения
    std::map<uint64_t, mavlink_message_t>::reverse_iterator rit = messages.rbegin();
    std::array<mavlink_global_position_int_t, 3>::reverse_iterator glposrit = glpos.rbegin();

    int counter = 3;
    for (;counter > 0 && rit != messages.rend();counter--, rit++, glposrit++)
        mavlink_msg_global_position_int_decode(&rit->second, &*glposrit);

    Mathematics::Point lastPoint = Mathematics::CordinateToPoint(Mathematics::Cordinates{glpos.rbegin()->lat, glpos.rbegin()->lon});
    float lastSpeedX;
    float lastSpeedY;
    float lastSpeedZ;

    Mathematics::Point lastPoint2 = Mathematics::CordinateToPoint(Mathematics::Cordinates{glpos[1].lat, glpos[1].lon});
    if (counter != 2) // Есть две или три точки для расчёта
    {
        lastSpeedX = Mathematics::CountTimeChangeParam(lastPoint2.x, lastPoint.x, glpos[2].time_boot_ms - glpos[1].time_boot_ms);
        lastSpeedY = Mathematics::CountTimeChangeParam(lastPoint2.y, lastPoint.y, glpos[2].time_boot_ms - glpos[1].time_boot_ms);
        lastSpeedZ = Mathematics::CountTimeChangeParam(glpos[2].relative_alt / 10, glpos[1].relative_alt / 10, glpos[2].time_boot_ms - glpos[1].time_boot_ms);
    }
    else // Есть только одна точка для расчёта
    {
        lastSpeedX = glpos.rbegin()->vx;
        lastSpeedY = glpos.rbegin()->vy;
        lastSpeedZ = glpos.rbegin()->vz;
    }

    float acceleratX;
    float acceleratY;
    float acceleratZ;

    if (counter == 0) // Есть три точки для расчёта
    {
        Mathematics::Point lastPoint3 = Mathematics::CordinateToPoint(Mathematics::Cordinates{glpos[0].lat, glpos[0].lon});
        float firstSpeedX = Mathematics::CountTimeChangeParam(lastPoint3.x, lastPoint2.x, glpos[1].time_boot_ms - glpos[0].time_boot_ms);
        float firstSpeedY = Mathematics::CountTimeChangeParam(lastPoint3.y, lastPoint2.y, glpos[1].time_boot_ms - glpos[0].time_boot_ms);
        float firstSpeedZ = Mathematics::CountTimeChangeParam(glpos[1].relative_alt / 10, glpos[0].relative_alt / 10, glpos[1].time_boot_ms - glpos[0].time_boot_ms);

        acceleratX = Mathematics::CountTimeChangeParam(firstSpeedX, lastSpeedX, glpos[2].time_boot_ms - glpos[1].time_boot_ms);
        acceleratY = Mathematics::CountTimeChangeParam(firstSpeedY, lastSpeedY, glpos[2].time_boot_ms - glpos[1].time_boot_ms);
        acceleratZ = Mathematics::CountTimeChangeParam(firstSpeedZ, lastSpeedZ, glpos[2].time_boot_ms - glpos[1].time_boot_ms);
    }
    else // Есть только одна или две точки для расчёта
    {
        acceleratX = 0;
        acceleratY = 0;
        acceleratZ = 0;
    }

    Mathematics::Cordinates res;
    try
    {
        Mathematics::Point p = Mathematics::FindNewPosition(lastPoint.x, lastPoint.y, glpos.rbegin()->relative_alt / 10, lastSpeedX, lastSpeedY, lastSpeedZ, acceleratX, acceleratY, acceleratZ);
        res = Mathematics::PointToCordinate(glpos.rbegin()->lat, p);
    }
    catch (const std::exception &ex)
    {
        QString err(ex.what());
        showMessageBox(this, "ОШИБКА!", "Ошибка при расчёте координат падения БЛА" + (err.isEmpty() ? "." : ":\n" + err), windowIcon());
        return;
    }

    ui->tLastLattitude->setText((std::to_string(static_cast<float>(std::abs(glpos.rbegin()->lat)) / Mathematics::DegreeAccuracy) + (glpos.rbegin()->lat > 0 ? " с.ш." : " ю.ш.")).c_str());
    ui->tLastLongitude->setText((std::to_string(static_cast<float>(std::abs(glpos.rbegin()->lon)) / Mathematics::DegreeAccuracy) + (glpos.rbegin()->lon > 0 ? " в.д." : " з.д.")).c_str());
    ui->tLastAltitude->setText((std::to_string(static_cast<float>(glpos.rbegin()->relative_alt) / 100) + " м").c_str());
    ui->tLattitude->setText((std::to_string(static_cast<float>(std::abs(res.lattitude)) / Mathematics::DegreeAccuracy) + (res.lattitude > 0 ? " с.ш." : " ю.ш.")).c_str());
    ui->tLongitude->setText((std::to_string(static_cast<float>(std::abs(res.longitude)) / Mathematics::DegreeAccuracy) + (res.longitude > 0 ? " в.д." : " з.д.")).c_str());
    ui->tAltitude->setText((std::to_string(static_cast<float>(glpos.rbegin()->alt - glpos.rbegin()->relative_alt) / 100) + " м").c_str());

    if (counter == 2)
        showMessageBox(this, "ПЕДУПРЕЖДЕНИЕ!", "Расчёт проводился по одной точке.", windowIcon());

    if (counter == 1)
        showMessageBox(this, "ПЕДУПРЕЖДЕНИЕ!", "Расчёт проводился по двум точкам.", windowIcon());
}

QString MainWindow::showFileDialog(QWidget *parent, const QString &title,const QString &currpath, const QIcon &icon)
{
    QFileDialog fdialog;
    fdialog.setWindowIcon(icon);
    QString fullpath((currpath.isEmpty()) ? QDir::currentPath() : currpath);
    return fdialog.getOpenFileName(parent, title, fullpath, "", nullptr, QFileDialog::DontUseNativeDialog);
}

void MainWindow::showMessageBox(QWidget *parent, const QString &title, const QString &msg, const QIcon &icon)
{
    QMessageBox msbox;
    msbox.setWindowIcon(icon);
    msbox.setIcon(QMessageBox::Icon::Warning);
    msbox.setText("ВНИМАНИЕ!");
    msbox.setWindowTitle(title);
    msbox.setInformativeText(msg);
    msbox.setStandardButtons(QMessageBox::StandardButton::Ok);
    msbox.exec();
}

