#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <logfileparser.h>
#include <mathematics.h>
#include <kalman.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->cbMethod->addItems(m_cbMethodItems.values());
    ui->cbMethod->setCurrentIndex(static_cast<int>(cbMethodItemIndexes::Geocentric));
}

MainWindow::~MainWindow()
{
    delete ui;
}

struct PointsData
{
    Mathematics::Point xyz;             ///< Координаты точки в геоцентрических координатах
    Mathematics::GeographicPoint geo;   ///< Координаты точки в географических координатах
    double vx;                          ///< Скорость по оси X, см/с
    double vy;                          ///< Скорость по оси Y, см/с
    double vz;                          ///< Скорость по оси Z, см/с
    double vh;                          ///< Скорость снижения, см/с
    double accx;                        ///< Ускорение по оси X, см/с^2
    double accy;                        ///< Ускорение по оси Y, см/с^2
    double accz;                        ///< Ускорение по оси Z, см/с^2
    double acch;                        ///< Ускорение снижения, см/с^2
};

void MainWindow::on_bFileName_clicked()
{
    // 1. Выбор файла с логом пакетов
    QString filename = showFileDialog(this, "Выбор файла с логом событий для БЛА.", "", windowIcon());
    if (filename.isEmpty())
    {
        showMessageBox(this, "ОШИБКА!", "Файл лога не был выбран.", windowIcon());
        return;
    }
    ui->tFileName->setText(filename);

    // 2. Получение отсортированного по времени списка MAVLink-пакетов GLOBAL_POSITION_INT
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

    // 3. Преобразование данных в указанную систему и пропускание через фильтр Кальмана в отдельный список данных
    int32_t deltah = 0; // Разница высот над поверхностью Земли и над уровнем моря
    std::map<uint32_t, PointsData> filteredblh;
    Kalman kf(0.1);

    // ui->lData->appendPlainText("Время,мс\tДолгота,гр.\tШирота,гр.\tВысота,см\tX,см\tY,см\tZ,см\tVx,см/с\tVy,см/с\tVz,см/с\tAx,см/с^2\tAy,см/с^2\tAz,см/с^2");
    ui->lData->appendPlainText("Время,мс\tДолгота,гр.\tШирота,гр.\tВысота,см\tX,см\tY,см\tZ,см");

    for (const std::pair<uint64_t, mavlink_message_t> &val : messages)
    {
        PointsData pdata; // Данные о точке

        // Преобразование из массива в структуру данных MAVLink
        mavlink_global_position_int_t glpos;
        mavlink_msg_global_position_int_decode(&val.second, &glpos);
        pdata.geo = {glpos.lat, glpos.lon, glpos.alt / 10};

        // Нахождение координат в зависимости от заданного метода преобразования
        switch (static_cast<cbMethodItemIndexes>(ui->cbMethod->currentIndex()))
        {
        case cbMethodItemIndexes::Projection:
            pdata.xyz = Mathematics::WGS84ToProjection(pdata.geo);
            break;
        case cbMethodItemIndexes::Geocentric:
            pdata.xyz = Mathematics::WGS84ToXYZ(pdata.geo);
            break;
        }

        // Сохранение разницы высот относительно поверхности Земли и относительно уровня моря
        deltah = (glpos.alt - glpos.relative_alt) / 10;
        pdata.xyz.dz = deltah;

        // Вычисление периода времени текущего измерения относительно предыдущего
        uint32_t dt = 0; // Период времени от предыдущего измерения
        if (filteredblh.size() > 0) // Если это не первая порция данных
            dt = glpos.time_boot_ms - filteredblh.rbegin()->first;

        // Фильтрация данных фильтром Калмана, если это требуется
        if (ui->chKalman->isChecked())
        {
            if (dt != 0) // Если удалось определить время для предсказания
            {
                kf.setPredictionPeriod(static_cast<double>(dt) / 1000);
                kf.predict();
            }
            Vector3d v(pdata.xyz.x, pdata.xyz.y, pdata.xyz.z);
            kf.update(v);
            pdata.xyz = {static_cast<int32_t>(kf.getState().x()), static_cast<int32_t>(kf.getState().y()), static_cast<int32_t>(kf.getState().z()), deltah};
        }

        // Подсчёт скоростей по всем осям и высоте, если это возможно
        if (filteredblh.size() > 0)
        {
            pdata.vx = Mathematics::CountTimeChangeParam(filteredblh.rbegin()->second.xyz.x, pdata.xyz.x, dt);
            pdata.vy = Mathematics::CountTimeChangeParam(filteredblh.rbegin()->second.xyz.y, pdata.xyz.y, dt);
            pdata.vz = Mathematics::CountTimeChangeParam(filteredblh.rbegin()->second.xyz.z, pdata.xyz.z, dt);
            pdata.vh = Mathematics::CountTimeChangeParam(pdata.geo.altitude, filteredblh.rbegin()->second.geo.altitude, dt);
        }
        else
            pdata.vx = pdata.vy = pdata.vz = pdata.vh = 0;

        // Подсчёт ускорений по всем осям и высоте, если это возможно
        if (filteredblh.size() > 1)
        {
            pdata.accx = Mathematics::CountTimeChangeParam(filteredblh.rbegin()->second.vx, pdata.vx, dt);
            pdata.accy = Mathematics::CountTimeChangeParam(filteredblh.rbegin()->second.vy, pdata.vy, dt);
            pdata.accz = Mathematics::CountTimeChangeParam(filteredblh.rbegin()->second.vz, pdata.vz, dt);
            pdata.acch = Mathematics::CountTimeChangeParam(filteredblh.rbegin()->second.vh, pdata.vh, dt);
        }
        else
            pdata.accx = pdata.accy = pdata.accz = pdata.acch = 0;

        // Вывод данных в окно на главном окне
        QString s = QString("%1\t%2\t%3\t%4\t%5\t%6\t%7"/*\t%8\t%9\t%10\t%11\t%12\t%13"*/).arg(glpos.time_boot_ms).arg(glpos.lat / Mathematics::DegreeAccuracy)
                        .arg(glpos.lon / Mathematics::DegreeAccuracy).arg(glpos.alt / 10).arg(pdata.xyz.x).arg(pdata.xyz.y).arg(pdata.xyz.z)
                        // .arg(pdata.vx).arg(pdata.vy).arg(pdata.vz).arg(pdata.accx).arg(pdata.accy).arg(pdata.accz)
            ;
        ui->lData->appendPlainText(s);

        filteredblh.emplace(glpos.time_boot_ms, std::move(pdata)); // Сохранение данных в сортированный контейнер
    }

    // 4. Преобразование в WGS84
    Mathematics::GeographicPoint res;
    try
    {
        PointsData ppp = filteredblh.rbegin()->second;
        // Поиск точки падения
        Mathematics::Point p = Mathematics::FindNewPosition(filteredblh.rbegin()->second.xyz.x, filteredblh.rbegin()->second.xyz.y, filteredblh.rbegin()->second.xyz.z,
                                                            filteredblh.rbegin()->second.geo.altitude - filteredblh.rbegin()->second.xyz.dz, filteredblh.rbegin()->second.vx,
                                                            filteredblh.rbegin()->second.vy, filteredblh.rbegin()->second.vz, filteredblh.rbegin()->second.vh,
                                                            filteredblh.rbegin()->second.accx, filteredblh.rbegin()->second.accy, filteredblh.rbegin()->second.accz,
                                                            filteredblh.rbegin()->second.acch);

        // Обратное преобразование в географическую систему в зависимости от указанного метода
        switch (static_cast<cbMethodItemIndexes>(ui->cbMethod->currentIndex()))
        {
        case cbMethodItemIndexes::Projection:
            res = Mathematics::ProjectionToWGS84(filteredblh.rbegin()->second.geo.lattitude, p);
            break;
        case cbMethodItemIndexes::Geocentric:
            res = Mathematics::XYZToWGS84(p);
            break;
        }
    }
    catch (const std::exception &ex)
    {
        QString err(ex.what());
        showMessageBox(this, "ОШИБКА!", "Ошибка при расчёте координат падения БЛА" + (err.isEmpty() ? "." : ":\n" + err), windowIcon());
        return;
    }

    // 5. Отображение результатов
    ui->tLastLattitude->setText((std::to_string(static_cast<float>(std::abs(filteredblh.rbegin()->second.geo.lattitude)) / Mathematics::DegreeAccuracy) + (filteredblh.rbegin()->second.geo.lattitude > 0 ? " с.ш." : " ю.ш.")).c_str());
    ui->tLastLongitude->setText((std::to_string(static_cast<float>(std::abs(filteredblh.rbegin()->second.geo.longitude)) / Mathematics::DegreeAccuracy) + (filteredblh.rbegin()->second.geo.longitude > 0 ? " в.д." : " з.д.")).c_str());
    ui->tLastAltitude->setText((std::to_string(static_cast<float>(filteredblh.rbegin()->second.geo.altitude - filteredblh.rbegin()->second.xyz.dz) / 100) + " м").c_str());
    ui->tLattitude->setText((std::to_string(static_cast<float>(std::abs(res.lattitude)) / Mathematics::DegreeAccuracy) + (res.lattitude > 0 ? " с.ш." : " ю.ш.")).c_str());
    ui->tLongitude->setText((std::to_string(static_cast<float>(std::abs(res.longitude)) / Mathematics::DegreeAccuracy) + (res.longitude > 0 ? " в.д." : " з.д.")).c_str());
    ui->tAltitude->setText((std::to_string(static_cast<float>(res.altitude) / 100) + " м").c_str());

    if (filteredblh.size() == 1)
        showMessageBox(this, "ПЕДУПРЕЖДЕНИЕ!", "Расчёт проводился по одной точке.", windowIcon());

    if (filteredblh.size() == 2)
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

