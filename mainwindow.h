#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QtSql>
#include <QDebug>
#include <QMessageBox>
#include <QSqlTableModel>
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

    //initialization
    bool connectDB();
    void init();

    //record counts functions
    void showCounts();
    void showInsCounts();
    void showCourseCounts();

    //combo_Box load functions
    void loadDept();
    void loadCourse();
    void loadDropCourse(int);

private slots:
    void on_comboBox_currentIndexChanged(int index);

    //student_page

    void on_btn_insert_clicked();

    void on_btnDel_clicked();

    void on_btnFind_clicked();

    void on_btnUpdate_clicked();

    void on_btnQuery_clicked();

    void on_btn_showAll_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    //department_page

    void on_btn_addDept_clicked();

    void on_btn_findDept_clicked();

    void on_btn_showAllDept_clicked();

    void on_btn_delDept_clicked();

    void on_btn_deptUd_clicked();

    //instructor_page

    void on_btn_insert_ins_clicked();

    void on_btnFind_ins_clicked();

    void on_btnDel_ins_clicked();

    void on_btnUpdate_2_clicked();

    void on_pushButton_4_clicked();

    void on_btn_showAll_ins_clicked();

    void on_pushButton_3_clicked();

    void on_btn_fill_ins_clicked();

    //course_page

    void on_btn_addCourse_clicked();

    void on_btnFindCid_clicked();

    void on_btnDelCid_clicked();

    void on_btn_showAll_courses_clicked();

    void on_btn_courseUpdate_clicked();

    void on_btn_courseUpdateClear_clicked();

    void on_btn_fill_course_clicked();

    //enrollment_page

    void on_btn_enroll_clicked();

    void on_lineEdit_drop_textEdited(const QString &arg1);

    void on_btn_drop_clicked();

    void on_btn_showAllEnrollment_clicked();

    void on_bt_enroll_fill_reset_clicked();

    void on_btn_enroll_fill_clicked();

    //log in page

    void on_btn_log_clicked();

    //account page

    void on_btn_acc_add_clicked();

    void on_btn_accDel_clicked();

    void on_btn_acc_update_clicked();

    void on_btn_exit_clicked();

    void on_btn_showpw_pressed();

    void on_btn_showpw_released();

    void on_btn_login_clicked();

    void on_btn_goToAcc_clicked();

private:
    Ui::MainWindow *ui;

    QSqlTableModel *model;

    QSqlDatabase db;

    QString current_role;
};
#endif // MAINWINDOW_H
