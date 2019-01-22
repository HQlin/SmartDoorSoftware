#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>

#include "slavethread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_modify_clicked();

    void on_modfyback_clicked();

    void on_select_clicked();

    void on_selectall_clicked();

    void on_orderbyasc_clicked();

    void on_orderbydesc_clicked();

    void on_delete_2_clicked();

    void on_insert_clicked();

    void showRequest(const QString &s);
    void processError(const QString &s);
    void processTimeout(const QString &s);
    void activateRunButton();

    void on_runButton_clicked();

    void on_action_Analog_triggered();

    void on_action_Send_triggered();

private:
    Ui::MainWindow *ui;

    QSqlTableModel *model;

    SlaveThread m_thread;
    int m_transactionCount = 0;
    QString responseText = "response success!";
};

#endif // MAINWINDOW_H
