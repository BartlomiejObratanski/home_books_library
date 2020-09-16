//*******************************************************************************************************************************
// Author:
// Bartlomiej Obratanski
//
//*******************************************************************************************************************************

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

     QSqlDatabase db;
     QString date_people;
     QString date_book;
     QString date_borrow;
     QStringList people_list;
     QStringList books_list;
     QStringList books_list_to_remove;
     QStringList people_to_remove;

     void update_remove_list_book(QSqlDatabase db);
     void update_books_list(QSqlDatabase db);
     void update_remove_list_people(QSqlDatabase db);

private slots:
     void on_borrow_button_clicked();

     void on_return_book_button_clicked();

     void on_add_person_clicked();

     void on_add_book_button_clicked();

     void on_remove_book_button_clicked();

     void on_remove_button_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
