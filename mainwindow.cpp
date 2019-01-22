#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QMessageBox>
#include <QSqlError>
#include <QSerialPortInfo>
#include <QSpinBox>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QSqlTableModel(this);
    model->setTable("student");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select(); //选取整个表的所有行
    //不显示name属性列,如果这时添加记录，则该属性的值添加不上
    // model->removeColumn(1);
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //使其不可编辑
    //ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        ui->serialPortComboBox->addItem(info.portName());
    }
    connect(&m_thread, &SlaveThread::request, this,&MainWindow::showRequest);
    connect(&m_thread, &SlaveThread::error, this, &MainWindow::processError);
    connect(&m_thread, &SlaveThread::timeout, this, &MainWindow::processTimeout);

    connect(ui->serialPortComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::activateRunButton);
}

void MainWindow::showRequest(const QString &s)
{
    ui->trafficLabel->setText(tr("Traffic, transaction #%1:"
                               "\n\r-request: %2"
                               "\n\r-response: %3")
                            .arg(++m_transactionCount)
                            .arg(s)
                            .arg(responseText));
    ui->cardLabel->setText(s);
}

void MainWindow::processError(const QString &s)
{
    activateRunButton();
    ui->statusLabel->setText(tr("Status: Not running, %1.").arg(s));
    ui->trafficLabel->setText(tr("No traffic."));
}

void MainWindow::processTimeout(const QString &s)
{
    ui->statusLabel->setText(tr("Status: Running, %1.").arg(s));
    ui->trafficLabel->setText(tr("No traffic."));
}

void MainWindow::activateRunButton()
{
    ui->runButton->setEnabled(true);
}

void MainWindow::on_runButton_clicked()
{
    ui->runButton->setEnabled(false);
    ui->statusLabel->setText(tr("Status: Running, connected to port %1.")
                           .arg(ui->serialPortComboBox->currentText()));
    m_thread.startSlave(ui->serialPortComboBox->currentText(),
                        10000,
                        responseText);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_modify_clicked()
{
    model->database().transaction(); //开始事务操作
     if (model->submitAll()) {
        model->database().commit(); //提交
     } else {
        model->database().rollback(); //回滚
        QMessageBox::warning(this, tr("tableModel"),
                              tr("数据库错误: %1")
                              .arg(model->lastError().text()));
     }
}

void MainWindow::on_modfyback_clicked()
{
    model->revertAll();
}

void MainWindow::on_select_clicked()
{
    QString name = ui->lineEdit->text();
    //根据姓名进行筛选
    model->setFilter(QString("name = '%1'").arg(name));
    //显示结果
    model->select();
}

void MainWindow::on_selectall_clicked()
{
    model->setTable("student");   //重新关联表
    model->select();   //这样才能再次显示整个表的内容
}

void MainWindow::on_orderbyasc_clicked()
{
    // 升序
    model->setSort(0, Qt::AscendingOrder); //id属性即第0列，升序排列
    model->select();
}

void MainWindow::on_orderbydesc_clicked()
{
    // 降序
    model->setSort(0, Qt::DescendingOrder);
    model->select();
}

void MainWindow::on_delete_2_clicked()
{
    //获取选中的行
    int curRow = ui->tableView->currentIndex().row();

    //删除该行
    model->removeRow(curRow);

    int ok = QMessageBox::warning(this,tr("删除当前行!"),tr("你确定"
                                                 "删除当前行吗？"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
       model->revertAll(); //如果不删除，则撤销
    }
    else model->submitAll(); //否则提交，在数据库中删除该行
}

void MainWindow::on_insert_clicked()
{
    int rowNum = model->rowCount(); //获得表的行数
    int id = rowNum;
    model->insertRow(rowNum); //添加一行
    model->setData(model->index(rowNum,0),id);
    model->setData(model->index(rowNum,3),ui->cardLabel->text());
    //model->submitAll(); //可以直接提交
}



void MainWindow::on_action_Analog_triggered()
{
    // 模拟串口
    QProcess::startDetached(QCoreApplication::applicationDirPath() + "\\vspdconfig.exe",QStringList());
}

void MainWindow::on_action_Send_triggered()
{
    // 发送数据
    QProcess::startDetached(QCoreApplication::applicationDirPath() + "\\blockingmaster.exe",QStringList());
}


