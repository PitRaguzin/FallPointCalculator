#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bFileName_clicked();

private:
    /// @brief Открытие окна выбора файла
    /// @param parent - родитель вызываемого окна
    /// @param title - заголовок вызываемого окна
    /// @param currpath - путь, задаваемый для начала поиска
    /// @param icon - иконка для вызываемого окна
    /// @return - путь к выбранному файлу
    QString showFileDialog(QWidget *parent, const QString &title,const QString &currpath, const QIcon &icon);

    /// @brief Открытие окна с сообщением для пользователя
    /// @param parent - родитель вызываемого окна
    /// @param title - заголовок вызываемого окна
    /// @param msg - сообщение пользователю
    /// @param icon - иконка для вызываемого окна
    void showMessageBox(QWidget *parent, const QString &title, const QString &msg, const QIcon &icon);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
