#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ldapconnector.h"

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_searchBtn_clicked();

    void on_pushButton_3_clicked();

    void on_usersBtn_clicked();

private:
    Ui::MainWindow *ui;
    LdapConnector *ldap;
    QSqlDatabase psqlDB;
};
#endif // MAINWINDOW_H
