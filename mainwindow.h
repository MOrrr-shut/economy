#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>

#include <QDebug>
#include <QMessageBox>

#include <cmath>
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

    int number_productov, number_people, number_otrasl;
    double cost_trud, trud_hour;



private slots:
    void on_tab_main_currentChanged(int index);

    void on_line_firm_textChanged(const QString &arg1);

    void on_table_kon_itemChanged(QTableWidgetItem *item);

    void on_line_people_textChanged(const QString &arg1);

    void on_line_otrasl_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
