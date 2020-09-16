#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , date_people("")
    , date_book("")
    , date_borrow("")
    , people_list()
    , books_list()
    , books_list_to_remove()
    , people_to_remove()
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->books->setText("");
    ui->people->setText("");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("BO_books.sqlite");

    if(!db.open())
    {
        qDebug() << "Problem with opening file";
        return;
    }

    QSqlQuery query(db);
    int counter = 0;
    int counter2 = 0;

    counter = query.exec("SELECT count(*) FROM people");
    counter2 = query.exec("SELECT count(*) FROM book");

    if(counter == 0 && counter2 == 0)
    {

        QString temp = "CREATE TABLE people ("
                       "ID integer primary key not null,"
                       "NAME varchar(30),"
                       "SURNAME varchar(30))";


        query.exec(temp);

        temp = "CREATE TABLE book ("
               "ID integer primary key not null,"
               "TITLE varchar(50),"
               "AUTHOR_NAME varchar(30),"
               "AUTHOR_SURNAME varchar(30),"
               "AVAILABLE Boolean,"
               "ID_BOOK integer)";

        query.exec(temp);
    }

    query.exec("select name, surname from people");

    QStringList books_to_return;
    QString temp;
    QString temp2;

    while (query.next())
    {
        temp = query.value(0).toString();
        date_people += query.value(0).toString();
        temp += " ";
        date_people += " ";
        temp += query.value(1).toString();
        date_people += query.value(1).toString();
        date_people += "\n";
        people_list.append(temp);
    }

    ui->people->setText(date_people);

    update_books_list(db);

    query.exec("select title, name, surname from book INNER JOIN people on book.ID_BOOK = people.ID where AVAILABLE = 'false'");

    while (query.next())
    {
        temp2 = query.value(0).toString();
        date_borrow += query.value(0).toString();
        date_borrow += " lend by ";
        date_borrow += query.value(1).toString();
        date_borrow += " " + query.value(2).toString();
        date_borrow += "\n";
        books_to_return.append(temp2);
    }

    ui->books_borrow->setText(date_borrow);
    ui->people_list->addItems(people_list);
    ui->books_return->addItems(books_to_return);

    update_remove_list_book(db);
    update_remove_list_people(db);
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

void MainWindow::update_remove_list_book(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("select title from book where AVAILABLE = 'true'");
    books_list_to_remove.clear();
    ui->remove_book->clear();

    QString temp="";

    while (query.next())
    {
        temp = query.value(0).toString();
        books_list_to_remove.append(temp);
    }

    ui->remove_book->addItems(books_list_to_remove);
}

void MainWindow::update_books_list(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("select title, author_name, author_surname from book where AVAILABLE = 'true'");
    QString temp;

    temp="";
    date_book="";
    books_list.clear();
    ui->books_list->clear();

    while (query.next())
    {
        temp = query.value(0).toString();
        date_book += query.value(0).toString();
        date_book += " / " + query.value(1).toString();
        date_book += " " + query.value(2).toString();
        date_book += "\n";
        books_list.append(temp);
    }

    ui->books->setText(date_book);
    ui->books_list->addItems(books_list);
}

void MainWindow::update_remove_list_people(QSqlDatabase db)
{
    QSqlQuery query(db);

    QString temp;

    std::vector<QString> vec;
    std::vector<QString> vec2;
    std::vector<QString> vec3;

    temp = "select ID_BOOK from people INNER JOIN book on people.ID = book.ID_BOOK";
    query.exec(temp);

    while (query.next())
    {
        vec2.push_back(query.value(0).toString());
    }

    query.exec("select id from people");

    while (query.next())
    {
        vec.push_back(query.value(0).toString());
    }

    for (auto i = vec.cbegin(); i != vec.cend(); i++)
    {
        std::vector<QString>::iterator it;
        it = std::find (vec2.begin(), vec2.end(), *i);
        if (it == vec2.end())
        {
            vec3.push_back(*i);
        }
    }

    if(!vec3.empty())
    {

        QString value = "select name, surname from people where ";

        for (auto i = vec3.cbegin(); i != vec3.cend(); i++)
        {
            value += "id = '";
            value += *i;
            value += "' or ";
        }

        value = value.mid(0, value.length() - 4);

        query.exec(value);

        QStringList temp_value;
        temp = "";
        ui->remove_person->clear();

        while (query.next())
        {
            temp = query.value(0).toString();
            temp += " ";
            temp += query.value(1).toString();
            temp_value.append(temp);
        }

        ui->remove_person->addItems(temp_value);
    }

    else
    {
        ui->remove_person->clear();
    }
}


void MainWindow::on_borrow_button_clicked()
{
    QSqlQuery query(db);
    QString temp = "update book set AVAILABLE = 'false' where title = '";
    temp += ui->books_list->currentText();
    temp += "'";

    query.exec(temp);

    QString fullname = ui->people_list->currentText();
    int position = fullname.indexOf(" ");
    QString name = fullname.mid(0, position);
    QString surname = fullname.mid(position + 1, fullname.length() - 1);

    QString value = "select id from people where name = '";
    value += name;
    value += "'";
    value += " and surname = '";
    value += surname;
    value += "'";

    query.exec(value);
    QString id=0;

    while (query.next())
    {
        id = query.value(0).toString();
    }

    value ="";
    value = "update book set id_book = '";
    value += id;
    value += "'";
    value += " where title = '";
    value += ui->books_list->currentText();
    value += "'";

    query.exec(value);

    update_books_list(db);

    query.exec("select title, name, surname from book INNER JOIN people on book.ID_BOOK = people.ID where AVAILABLE = 'false'");

    QStringList temp_list_to_return;
    QString temp2="";
    date_borrow="";
    ui->books_return->clear();

    while (query.next())
    {
        temp2 = query.value(0).toString();
        date_borrow += query.value(0).toString();
        date_borrow += " lend by ";
        date_borrow += query.value(1).toString();
        date_borrow += " " + query.value(2).toString();
        date_borrow += "\n";
        temp_list_to_return.append(temp2);
    }

    ui->books_borrow->setText(date_borrow);
    ui->books_return->addItems(temp_list_to_return);

    update_remove_list_book(db);
    update_remove_list_people(db);
}

void MainWindow::on_return_book_button_clicked()
{
    QSqlQuery query(db);
    QString temp = "update book set AVAILABLE = 'true' where title = '";
    temp += ui->books_return->currentText();
    temp += "'";

    query.exec(temp);

    QString current_temp = "update book set id_book = 'NULL' where title = '";
    current_temp += ui->books_return->currentText();
    current_temp += "'";

    query.exec(current_temp);
    update_books_list(db);

    query.exec("select title, name, surname from book INNER JOIN people on book.ID_BOOK = people.ID where AVAILABLE = 'false'");

    QStringList temp_list_to_return;
    QString temp2="";
    date_borrow="";
    ui->books_return->clear();

    while (query.next())
    {
        temp2 = query.value(0).toString();
        date_borrow += query.value(0).toString();
        date_borrow += " lend by ";
        date_borrow += query.value(1).toString();
        date_borrow += " " + query.value(2).toString();
        date_borrow += "\n";
        temp_list_to_return.append(temp2);
    }

    ui->books_borrow->setText(date_borrow);
    ui->books_return->addItems(temp_list_to_return);

    query.exec("select title from book where AVAILABLE = 'true'");
    books_list_to_remove.clear();
    ui->remove_book->clear();

    temp="";

    while (query.next())
    {
        temp = query.value(0).toString();
        books_list_to_remove.append(temp);
    }

    ui->remove_book->addItems(books_list_to_remove);
    update_remove_list_people(db);
}

void MainWindow::on_add_person_clicked()
{
    QString temp = "insert into people (NAME, SURNAME) values('";
    temp += ui->new_name->text();
    temp += "', '";
    temp += ui->new_surname->text();
    temp += "')";

    QSqlQuery query(db);

    QString check = "SELECT count(name), count(surname) from people where name = '";
    check += ui->new_name->text();
    check += "' and surname = '";
    check += ui->new_surname->text();
    check += "'";

    query.exec(check);
    int counter;

    while (query.next())
    {
        counter = query.value(0).toInt() + query.value(1).toInt();
    }

    if(counter != 2) query.exec(temp);

    query.exec("select name, surname from people");

    temp="";
    date_people="";
    ui->people_list->clear();
    people_list.clear();

    while (query.next())
    {
        temp = query.value(0).toString();
        date_people += query.value(0).toString();
        temp += " ";
        date_people += " ";
        temp += query.value(1).toString();
        date_people += query.value(1).toString();
        date_people += "\n";
        people_list.append(temp);
    }

    ui->people->setText(date_people);
    ui->people_list->addItems(people_list);

    ui->new_name->clear();
    ui->new_surname->clear();
    update_remove_list_people(db);

}

void MainWindow::on_add_book_button_clicked()
{
    QString temp = "insert into book (TITLE, AUTHOR_NAME, AUTHOR_SURNAME, AVAILABLE) values('";
    temp += ui->new_title->text();
    temp += "', '";
    temp += ui->new_author_name->text();
    temp += "', '";
    temp += ui->new_author_surname->text();
    temp += "', 'true')";

    QSqlQuery query(db);

    QString check = "SELECT count(title), count(author_name), count(author_surname) from book where title = '";
    check += ui->new_title->text();
    check += "' and author_name = '";
    check += ui->new_author_name->text();
    check += "' and author_surname = '";
    check += ui->new_author_surname->text();
    check += "'";

    query.exec(check);
    int counter;

    while (query.next())
    {
        counter = query.value(0).toInt() + query.value(1).toInt() + query.value(2).toInt();
    }

    if(counter != 3) query.exec(temp);

    update_books_list(db);

    ui->new_title->clear();
    ui->new_author_name->clear();
    ui->new_author_surname->clear();

    update_remove_list_book(db);
}

void MainWindow::on_remove_book_button_clicked()
{
    QSqlQuery query(db);

    QString temp;

    temp = "DELETE from book WHERE title = '";
    temp += ui->remove_book->currentText();
    temp += "'";

    query.exec(temp);

    update_books_list(db);

    update_remove_list_book(db);

}

void MainWindow::on_remove_button_clicked()
{
    QSqlQuery query(db);

    QString temp;

    QString fullname = ui->remove_person->currentText();
    int position = fullname.indexOf(" ");
    QString name = fullname.mid(0, position);
    QString surname = fullname.mid(position + 1, fullname.length() - 1);

    temp = "DELETE from people WHERE name = '" ;
    temp += name;
    temp += "' and surname = '";
    temp += surname;
    temp += "'";

    query.exec(temp);

    update_remove_list_people(db);
    query.exec("select name, surname from people");

    temp = "";
    date_people="";
    people_list.clear();
    ui->people_list->clear();

    while (query.next())
    {
        temp = query.value(0).toString();
        date_people += query.value(0).toString();
        temp += " ";
        date_people += " ";
        temp += query.value(1).toString();
        date_people += query.value(1).toString();
        date_people += "\n";
        people_list.append(temp);
    }

    ui->people->setText(date_people);
    ui->people_list->addItems(people_list);
}
