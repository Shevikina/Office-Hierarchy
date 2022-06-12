#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();


    private slots:
        void on_move_on_clicked();

        void on_back_clicked();

        void on_select_employee_supervisors_clicked();

        void on_select_employees_of_head_clicked();

        void on_blocked_added_textChanged();

        void on_blocked_result_select_textChanged();

        void on_self_select_quetry_clicked();

        void on_added_clicked();

        void on_result_select_clicked();

        void on_added_director_stateChanged(int arg1);

    private:
        Ui::MainWindow *ui;
        void clearText();
        QSqlDatabase employee;
};
#endif // MAINWINDOW_H
