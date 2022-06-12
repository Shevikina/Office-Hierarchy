#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    employee = QSqlDatabase::addDatabase("QSQLITE");
    employee.setDatabaseName("office.db");
    if (!employee.open()) {
        qDebug() << "Ошибка открытия базы данных " << employee.lastError().text();
    }
    else {
        QSqlQuery create_employee_table("CREATE TABLE employee("
                                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                        "fio TEXT)");
        QSqlQuery create_link_table("CREATE TABLE link("
                                    "id_employee INTEGER UNIQUE,"
                                    "id_supervisor INTEGER,"
                                    "FOREIGN KEY(id_employee) REFERENCES employee(id),"
                                    "FOREIGN KEY(id_supervisor) REFERENCES employee(id))");


        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_2->setCurrentIndex(3);

        ui->verticalLayout_4->setStretchFactor(ui->query_output, 2);
        ui->verticalLayout_4->setStretchFactor(ui->horizontalLayout_4, 1);

        ui->added->setEnabled(false);
        ui->result_select->setEnabled(false);

        QObject::connect(ui->fio_employee, &QLineEdit::textChanged, this, &MainWindow::on_blocked_added_textChanged);
        QObject::connect(ui->fio_supervisor, &QLineEdit::textChanged, this, &MainWindow::on_blocked_added_textChanged);
        QObject::connect(ui->employee, &QLineEdit::textChanged, this, &MainWindow::on_blocked_result_select_textChanged);
        QObject::connect(ui->supervisior, &QLineEdit::textChanged, this, &MainWindow::on_blocked_result_select_textChanged);
        QObject::connect(ui->select, &QTextEdit::textChanged, this, &MainWindow::on_blocked_result_select_textChanged);

        ui->employee->setClearButtonEnabled(true);
        ui->supervisior->setClearButtonEnabled(true);
        ui->fio_employee->setClearButtonEnabled(true);
        ui->fio_supervisor->setClearButtonEnabled(true);


        QSqlQuery query;
        int id_director = 0;
        query.exec("SELECT id_employee FROM link WHERE id_employee=id_supervisor");
        if (query.first())
            id_director = query.value(0).toInt();
        if (id_director != 0)
            ui->added_director->setEnabled(false);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_move_on_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    clearText();
}


void MainWindow::on_back_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    clearText();
}


void MainWindow::on_select_employee_supervisors_clicked() {
    ui->stackedWidget_2->setCurrentIndex(0);
    clearText();
    ui->query_output->clear();
}


void MainWindow::on_select_employees_of_head_clicked() {
    ui->stackedWidget_2->setCurrentIndex(1);
    clearText();
    ui->query_output->clear();
}

void MainWindow::on_self_select_quetry_clicked() {
    ui->stackedWidget_2->setCurrentIndex(2);
    clearText();
    ui->query_output->clear();
    ui->select->setText("SELECT * FROM ;");
}

void MainWindow::clearText() {
    ui->fio_employee->clear();
    ui->fio_supervisor->clear();
    ui->employee->clear();
    ui->supervisior->clear();
    ui->select->clear();
}

void MainWindow::on_blocked_added_textChanged() {
    if (ui->stackedWidget->currentIndex() == 0) {
        if (ui->fio_employee->text() != "" && ui->fio_supervisor->text() != "")
            ui->added->setEnabled(true);

        else {
            if (ui->added_director->isChecked()) {
                if (ui->fio_employee->text() != "" && ui->fio_employee->text() != " ")
                    ui->added->setEnabled(true);
                else
                    ui->added->setEnabled(false);
            }
            else
                ui->added->setEnabled(false);
        }

    }
}

void MainWindow::on_blocked_result_select_textChanged() {
    if (ui->stackedWidget->currentIndex() == 1) {
        switch (ui->stackedWidget_2->currentIndex()) {
            case 3: {
                ui->result_select->setEnabled(false);
                break;
            }
            case 0: {
                if (ui->employee->text() != "")
                    ui->result_select->setEnabled(true);
                else
                    ui->result_select->setEnabled(false);
                break;
            }
            case 1: {
                if (ui->supervisior->text() != "")
                    ui->result_select->setEnabled(true);
                else
                    ui->result_select->setEnabled(false);
                break;
            }
            case 2: {
                if (ui->select->toPlainText() != "")
                    ui->result_select->setEnabled(true);
                else
                    ui->result_select->setEnabled(false);
                break;
            }
        }
    }
}

void MainWindow::on_added_clicked() {
    QString fio_employee = ui->fio_employee->text();
    QString fio_supervisor = ui->fio_supervisor->text();

    QString query_for_select("SELECT id FROM employee WHERE fio='%1'");
    QString query_for_insert_employee("INSERT INTO employee(fio) VALUES ('%1')");
    QString query_for_insert_link("INSERT INTO link(id_employee,id_supervisor) VALUES (%1,%2)");

    if (ui->added_director->isChecked()) {
        QSqlQuery query(query_for_select.arg(fio_employee));
        int id_director = 0;
        if (query.first())
            id_director = query.value(0).toInt();
        if (id_director == 0) {
            query.exec(query_for_insert_employee.arg(id_director));
            query.exec(query_for_select.arg(fio_employee));
            if (query.first())
                id_director = query.value(0).toInt();
        }
        query.exec(query_for_insert_link.arg(id_director).arg(id_director));
        ui->added_director->setChecked(false);
        ui->added_director->setEnabled(false);
    }
    else {
        if (fio_employee != fio_supervisor) {


            int id_employee = 0;
            QSqlQuery select_query(query_for_select.arg(fio_employee), employee);
            if (select_query.first())
                id_employee = select_query.value(0).toInt();
            qDebug() << id_employee;

            if (id_employee == 0) {
                QSqlQuery insert_employee(query_for_insert_employee.arg(fio_employee));
                qDebug() << "Добавлен новый сотрудник" << fio_employee;
            }
            select_query.exec(query_for_select.arg(fio_employee));
            if (select_query.first())
                id_employee = select_query.value(0).toInt();

            int id_supervisor = 0;

            select_query.exec(query_for_select.arg(fio_supervisor));
            if (select_query.first())
                id_supervisor = select_query.value(0).toInt();

            if (id_supervisor == 0) {
                QSqlQuery insert_supervisor(query_for_insert_employee.arg(fio_supervisor));
                qDebug() << "Добавлен новый сотрудник" << fio_supervisor;
                QSqlQuery query(query_for_select.arg(fio_supervisor));
                if (query.first())
                    id_supervisor = query.value(0).toInt();

            }
            qDebug() << id_employee << id_supervisor;
            QSqlQuery query(query_for_insert_link.arg(id_employee).arg(id_supervisor));
            if (query.isActive())
                qDebug() << "Теперь сотрудник" << fio_supervisor << "руководит содрудником" <<
                         fio_employee;
            else
                qDebug() <<
                         "У одного сотрудника тожет быть только один конечный руководитель";
        }
        else
            qDebug() << "Нельзя быть руководителем сабя самого";
    }
}


void MainWindow::on_result_select_clicked() {
    QSqlQuery is_empty_table("SELECT fio FROM employee LIMIT 1");
    QString first_fio = "";
    if (is_empty_table.first())
        first_fio = is_empty_table.value(0).toString();
    if (first_fio != "") {
        QString fio = "";
        QString fio_query = "SELECT fio FROM employee WHERE id";
        QSqlQuery query;
        if (ui->select_employee_supervisors->isChecked()) {
            fio = ui->employee->text();
            fio_query.append("=(SELECT id_supervisor FROM link WHERE id_employee=(SELECT id FROM employee WHERE fio='%1'))");
        }
        else if (ui->select_employees_of_head->isChecked()) {
            fio = ui->supervisior->text();
            fio_query.append(" in (SELECT id_employee FROM link WHERE id_supervisor=(SELECT id FROM employee WHERE fio='%1'))");
        }
        else {
            fio_query = ui->select->toPlainText();

        }
        query.exec(fio_query.arg(fio));

        QString query_output = "";
        qDebug() << query.executedQuery();
        while (query.next()) {
            query_output.append(query.value(0).toString() + "\n");
        }
        ui->query_output->setText(query_output);
    }
    else
        qDebug() << "Таблица пуста. Заполните перед тем как кого-то выбирать.";
}


void MainWindow::on_added_director_stateChanged(int arg1) {

    if (ui->added_director->isChecked())
        ui->fio_supervisor->setEnabled(false);
    else {
        ui->fio_supervisor->setEnabled(true);
        on_blocked_added_textChanged();
    }
}

