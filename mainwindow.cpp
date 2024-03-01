#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ldap = new LdapConnector();

    qDebug() << QSqlDatabase::drivers();
    psqlDB = QSqlDatabase::addDatabase("QPSQL");

    psqlDB.setDatabaseName("SAI");
    psqlDB.setHostName("test.mars");
    psqlDB.setPort(5432);
    psqlDB.setUserName("sai");
    psqlDB.setPassword("test");

    if(!psqlDB.open()) {
        qDebug() << "Не удалось подключится к БД";
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if(ldap->init("172.2.0.50:3268") == LDAP_SUCCESS) {
        qDebug() << "CONNECTED";
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    if(ldap->bind("ldap-query","ldap-query") == LDAP_SUCCESS) {
        qDebug() << "BINDED";
    }
}


void MainWindow::on_searchBtn_clicked()
{
    int r = 0;
    if(ldap->searchUser(ui->lineEdit->text(), r) == LDAP_SUCCESS) {
        qDebug() << "FINDED";
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    if(ldap->close() == LDAP_SUCCESS) {
        qDebug() << "CLOSED";
    }
}


void MainWindow::on_usersBtn_clicked()
{
    if(!psqlDB.open()) {
        qDebug() << "Не удалось подключится к БД";
        return;
    }

    QSqlQuery query(psqlDB);

    QString sql = QString(R"d(
                            SELECT id, "ФИО", "e-mail", "Отдел", "Пароль", "Телефон", "Доступ права",
                            "Комната", "Заблокирован", type
                            FROM public.users
                            WHERE NOT "Заблокирован" AND "e-mail" != '' AND type = 'User'
                            )d");

    if(!query.exec(sql)) {
        qDebug() << "Не удалось выполнить запрос";
        return;
    }

    while(query.next()) {
        QString ldapName = query.value(2).toString();
        int result = 0;
        if(ldap->searchUser(ldapName, result) == LDAP_SUCCESS) {
            qInfo() << "DONE:" << ldapName << "Найдено:" << result;
        } else {
            qCritical() << "FAIL:" << ldapName;
        }
    }
}

