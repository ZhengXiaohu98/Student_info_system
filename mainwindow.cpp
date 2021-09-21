#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //connect to data base;
    if(!connectDB()) return;

    /*QSqlQuery query;
    query.exec("drop table student");
    query.exec("drop table department");
    query.exec("create table department(deptName varchar(40) primary key, phoneNum varchar(13), officeNumber varchar(10));");
    query.exec("create table student(ID INTEGER primary key, Name varchar(50), Age INTEGER , Gender varchar(1), Department varchar(40) REFERENCES department(deptName) ON DELETE SET NULL ON UPDATE CASCADE);");
    query.exec("create table instructor(IID INTEGER primary key, Name varchar(50), Rank varchar(20), Department varchar(40) REFERENCES department(deptName) ON DELETE SET NULL ON UPDATE CASCADE);");
    query.exec("drop table course");
    query.exec("create table course(CID varchar(10) primary key, courseName varchar(100), Semester varchar(10), IID INTEGER, Instructor varchar(50));");
    query.exec("create table enrollment(SID INTEGER, Name varchar(50), CID varchar(10), courseName varchar(100), Semester varchar(10), CONSTRAINT SCID UNIQUE (SID, CID));");
    query.exec("create table account(Username varchar(40) primary key, Passward varchar(20), Role varchar(20));");
    */

    //add a new sql table model;
    model = new QSqlTableModel(this, db);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
}

bool MainWindow::connectDB()
{
    //create a sql database(sqlite database : static)
    db = QSqlDatabase::addDatabase("QSQLITE");
    //set the database name to data.db
    db.setDatabaseName("data.db");

    //if the databse is not opened, show the warning message then return false;
    if(!db.open())
    {
        QMessageBox::warning(this, "db connection error", db.lastError().text());
        return false;
    }

    return true;
}

void MainWindow::init()
{
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_instructor->setModel(model);
    ui->tableView_instructor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_dept->setModel(model);
    ui->tableView_dept->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_course->setModel(model);
    ui->tableView_course->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_enroll->setModel(model);
    ui->tableView_enroll->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_account->setModel(model);
    ui->tableView_account->setEditTriggers(QAbstractItemView::NoEditTriggers);
    loadDept();
    loadCourse();
    ui->lineEdit_logpw->setEchoMode(QLineEdit::Password);
    on_btn_log_clicked();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->stackedWidget->setCurrentWidget(ui->student_page);
        model->setTable("student");
        model->select();
        showCounts();
        ui->tableView->setColumnWidth(4, 150);
        break;
    case 1:
        ui->stackedWidget->setCurrentWidget(ui->instructor_page);
        model->setTable("instructor");
        model->select();
        showInsCounts();
        ui->tableView_instructor->setColumnWidth(3, 150);
        ui->tableView_instructor->setColumnWidth(2, 150);
        break;
    case 2:
        ui->stackedWidget->setCurrentWidget(ui->department_page);
        model->setTable("department");
        model->select();
        ui->tableView_dept->setColumnWidth(0, 150);
        break;
    case 3:
        ui->stackedWidget->setCurrentWidget(ui->Course_page);
        model->setTable("course");
        showCourseCounts();
        model->select();
        ui->tableView_course->setColumnWidth(1, 194);
        break;
    case 4:
        ui->stackedWidget->setCurrentWidget(ui->page_enroll);
        model->setTable("enrollment");
        model->select();
        ui->tableView_enroll->setColumnWidth(3, 194);
        break;
    default:
        break;
    }
}


void MainWindow::on_btn_insert_clicked()
{
    QSqlQuery query;
    //if any feild is empty, we should not add the record
    if(ui->lineEdit_id->text().isEmpty() || ui->lineEdit_name->text().isEmpty() || ui->lineEdit_age->text().isEmpty())
    {
        QMessageBox::information(this, "Tip", "You must fill the form in order to insert!");
        return;
    }

    //get the data information
    int id = ui->lineEdit_id->text().toInt();
    QString name = ui->lineEdit_name->text();
    int age = ui->lineEdit_age->text().toInt();
    QString gender = ui->comboBox_gender->currentText();
    QString dept = ui->comboBox_dept->currentText();

    QString sql = QString("insert into student(ID, Name, Age, Gender, Department) values(%1, '%2', %3, '%4', '%5');")
            .arg(id).arg(name).arg(age).arg(gender).arg(dept);

    query.exec(sql);

    ui->lineEdit_id->clear();
    ui->lineEdit_name->clear();
    ui->lineEdit_age->clear();
    ui->comboBox_gender->setCurrentIndex(0);
    ui->comboBox_dept->setCurrentIndex(0);

    model->submitAll();
    model->select();
    showCounts();
}


void MainWindow::on_btnDel_clicked()
{
    QSqlQuery query;
    //if there is nothing in the input id box, do nothing
    if(ui->lineEdit_idDel->text().isEmpty()) return;

    //select the data, we should onlt get <= 1 record since ID is the primary key of the table
    int id = ui->lineEdit_idDel->text().toInt();
    query.exec(QString("select * from student where id = %1").arg(id));

    //if we have one record
    if(!query.next())
    {
        QMessageBox::information(this, "Message", QString("The student with student id: %1 does not exist!").arg(id));
        return;
    }

    //get the name and department
    QString name = query.value("Name").toString();
    QString dept = query.value("Department").toString();

    //Comfirm to delete the record
    int ret = QMessageBox::question(this, "Confirmation",
                     QString("Delete student '%1' from department '%2'?").arg(name).arg(dept));

    //Only delete when the user comfirmed on deletion
    if(ret == QMessageBox::Yes)
    {
        query.exec(QString("delete from student where id = '%1'").arg(id));
        query.exec(QString("delete from enrollment where sid = %1").arg(id)); //update the enrollment table
        model->submitAll();
        model->select();
        ui->lineEdit_idDel->clear();
        showCounts();
    }
}

void MainWindow::on_btnFind_clicked()
{
    QSqlQuery query;
    //if there is nothing in the find box, do nothing
    if(ui->lineEdit_idFind->text().isEmpty())
    {
        QMessageBox::information(this, "Message", "You need to input an id to find a student!");
        return;
    }

    //get the id of the student
    int id = ui->lineEdit_idFind->text().toInt();

    //set the selection filter, it is like puting a where statment
    model->setFilter(QString("id = %1").arg(id));
    model->select();
    model->submitAll();
    showCounts();

    ui->lineEdit_idFind->clear(); //reset
}


void MainWindow::on_btnUpdate_clicked()
{
    QSqlQuery query;

    //if the id box is empty, do nothing
    if(ui->lineEditIDupdate->text().isEmpty()) return;

    QString sql = QString("update student set");
    bool first = false;
    int id = ui->lineEditIDupdate->text().toInt();

    if(ui->comboBox_gender_update->currentIndex() != 0)
    {
        sql += QString(" gender = '%1'").arg(ui->comboBox_gender_update->currentText());
        first = true;
    }

    if(ui->comboBox_dept_update->currentIndex() != 0)
    {
        if(!first)
        {
            sql += QString(" department = '%1'").arg(ui->comboBox_dept_update->currentText());
            first = true;
        }
        else
        {
            sql += QString(", department = '%1'").arg(ui->comboBox_dept_update->currentText());
        }
    }

    //if there is a need to update the name
    if(!ui->lineEditNameUpdate->text().isEmpty())
    {
        if(!first)
        {
            sql += QString(" name = '%1'").arg(ui->lineEditNameUpdate->text());
            first = true;
        }
        else
        sql += QString(", name = '%1'").arg(ui->lineEditNameUpdate->text());
    }

    //if there is a need to update the age
    if(!ui->lineEditAgeUpdate->text().isEmpty())
    {
        if(!first)
            sql += QString(" age = %1").arg(ui->lineEditAgeUpdate->text().toInt());
        else
            sql += QString(", age = %1").arg(ui->lineEditAgeUpdate->text().toInt());
    }

    //uodate specific student
    sql += QString(" where id = %1;").arg(id);
    query.exec(sql);

    model->select();
    model->submitAll();
}


void MainWindow::on_btnQuery_clicked()
{
    bool first = false;
    QString filter = "";
    if(!ui->lineEdit_nameFil->text().isEmpty())
    {
        filter += QString("name = '%1'").arg(ui->lineEdit_nameFil->text());
        first = true;
    }
    if(!ui->lineEdit_ageFil->text().isEmpty())
    {
        if(!first)
        {
            filter += QString("age < %1").arg(ui->lineEdit_ageFil->text().toInt());
            first = true;
        }
        else
            filter += QString(" AND age < %1").arg(ui->lineEdit_ageFil->text().toInt());
    }
    if(ui->comboBox_genderFil->currentText() != "All")
    {
        if(!first)
        {
            filter += QString("gender = '%1'").arg(ui->comboBox_genderFil->currentText());
            first = true;
        }
        else
            filter += QString(" AND gender = '%1'").arg(ui->comboBox_genderFil->currentText());
    }
    if(ui->comboBox_deptFil->currentText() != "All")
    {
        if(!first)
            filter += QString("department = '%1'").arg(ui->comboBox_deptFil->currentText());
        else
            filter += QString(" AND department = '%1'").arg(ui->comboBox_deptFil->currentText());
    }

    model->setFilter(filter);
    model->select();
    model->submitAll();
    showCounts();
}

void MainWindow::on_btn_showAll_clicked()
{
    //reset the filter then select all
    model->setFilter("");
    model->select();
    model->submitAll();
    showCounts();
}

//clear button 1 for update
void MainWindow::on_pushButton_clicked()
{
    ui->lineEditIDupdate->clear();
    ui->lineEditNameUpdate->clear();
    ui->lineEditAgeUpdate->clear();
    ui->comboBox_gender_update->setCurrentIndex(0);
    ui->comboBox_dept_update->setCurrentIndex(0);
}

//clear button 2 for filter
void MainWindow::on_pushButton_2_clicked()
{
    ui->lineEdit_nameFil->clear();
    ui->lineEdit_ageFil->clear();
    ui->comboBox_genderFil->setCurrentIndex(0);
    ui->comboBox_deptFil->setCurrentIndex(0);
}

//COUNT HOW MANY students in the current selection
void MainWindow::showCounts()
{
    int cnt = model->rowCount();
    ui->lineEdit_cnt->setText(QString::number(cnt));
}

void MainWindow::on_btn_addDept_clicked()
{
    //we must fill all the form in order to add a department
    if(ui->lineEdit_deptNamADD->text().isEmpty() || ui->lineEdit_deptPnADD->text().isEmpty() || ui->lineEdit_deptOnAdd->text().isEmpty())
    {
        QMessageBox::information(this, "Tip", "You must fill all the information in order to add a department!");
        return;
    }

    QSqlQuery query;
    //get the data from the lineEdit boxes then insert into department table
    QString deptName = ui->lineEdit_deptNamADD->text();
    QString deptPhoneNumber = ui->lineEdit_deptPnADD->text();
    QString deptOfficeNumber = ui->lineEdit_deptOnAdd->text();
    query.exec(QString("insert into department(deptName, phoneNum, officeNumber) values('%1', '%2', '%3');").arg(deptName).arg(deptPhoneNumber).arg(deptOfficeNumber));

    //need to reset the combox
    loadDept();

    //clear the lineEdit box
    ui->lineEdit_deptNamADD->clear();
    ui->lineEdit_deptPnADD->clear();
    ui->lineEdit_deptOnAdd->clear();

    model->setFilter("");
    model->select();
    model->submitAll();
}

//this function mainly maintain the comboBox item list, we need to reset it whenever we need to;
void MainWindow::loadDept()
{
    QSqlQuery query;
    query.exec("Select deptName from department;");
    QStringList l1, l2, l3, i1, i2, i3;
    l1.append(""), l1.append("Undecided*");
    l2.append("All"), l2.append("Undecided*");
    l3.append("Undecided*");
    i1.append(""), i2.append("All");

    while(query.next())
    {
        QString deptName = query.value("deptName").toString();
        l1.append(deptName), l2.append(deptName), l3.append(deptName);
        i1.append(deptName), i2.append(deptName), i3.append(deptName);
    }

    //reset the comboBox item for each combo box
    ui->comboBox_dept->clear();
    ui->comboBox_dept->addItems(l3);

    ui->comboBox_deptFil->clear();
    ui->comboBox_deptFil->addItems(l2);

    ui->comboBox_dept_update->clear();
    ui->comboBox_dept_update->addItems(l1);

    ui->comboBox_insDeptFll->clear();
    ui->comboBox_insDeptFll->addItems(i2);

    ui->comboBox_insDeptAdd->clear();
    ui->comboBox_insDeptAdd->addItems(i3);

    ui->comboBox_insDeptUpdate->clear();
    ui->comboBox_insDeptUpdate->addItems(i1);
}

void MainWindow::on_btn_findDept_clicked()
{
    //if there is nothing in the line edit find box, then do nothing
    if(ui->lineEdit_deptFind->text().isEmpty())
    {
        QMessageBox::information(this, "Message", "Please enter a department Name!");
        return;
    }

    QString deptName = ui->lineEdit_deptFind->text();

    //set the filter and only find dept where the department name == userInput
    model->setFilter(QString("deptName = '%1'").arg(deptName));
    model->select();
    model->submitAll();
    ui->lineEdit_deptFind->clear();
}


void MainWindow::on_btn_showAllDept_clicked()
{
    model->setFilter("");
    model->select();
    model->submitAll();
}


void MainWindow::on_btn_delDept_clicked()
{
    //if there is nothing in the line edit deletion box, then do nothing
    if(ui->lineEdit_deptDel->text().isEmpty())
        return;

    QSqlQuery query;
    QString deptName = ui->lineEdit_deptDel->text();

    query.exec(QString("delete from department where deptName = '%1';").arg(deptName));
    query.exec(QString("update student set Department = 'Undecided*' where Department = '%1'").arg(deptName));

    model->setFilter("");
    model->select();
    model->submitAll();
    loadDept(); //reset dept combo box
}


void MainWindow::on_btn_deptUd_clicked()
{
    if(ui->lineEdit_deptNameUd->text().isEmpty())
    {
        QMessageBox::information(this, "Message", "You must enter a department name to update!");
        return;
    }

    QSqlQuery query;
    QString deptName = ui->lineEdit_deptNameUd->text();
    QString newPhoneNum = ui->lineEdit_deptPnUd->text();
    QString newOfficeNum = ui->lineEdit_deptOnUd->text();

    if(newPhoneNum.isEmpty() && newOfficeNum.isEmpty())
        return;
    else if(newPhoneNum.isEmpty())
        query.exec(QString("update department set officeNumber = '%1' where deptName = '%2';").arg(newOfficeNum).arg(deptName));
    else if(newOfficeNum.isEmpty())
        query.exec(QString("update department set phoneNum = '%1' where deptName = '%2';").arg(newPhoneNum).arg(deptName));
    else
        query.exec(QString("update department set phoneNum = '%1', officeNumber = '%2' where deptName = '%3';").arg(newPhoneNum).arg(newOfficeNum).arg(deptName));

    model->setFilter("");
    model->select();
    model->submitAll();

    ui->lineEdit_deptNameUd->clear();
    ui->lineEdit_deptPnUd->clear();
    ui->lineEdit_deptOnUd->clear();
}

void MainWindow::showInsCounts()
{
    int cnt = model->rowCount();
    ui->lineEdit_insCnt->setText(QString::number(cnt));
}


void MainWindow::on_btn_insert_ins_clicked()
{
    QSqlQuery query;
    //if any feild is empty, we should not add the record
    if(ui->lineEdit_ins_iid->text().isEmpty() || ui->lineEdit_ins_name->text().isEmpty())
    {
        QMessageBox::information(this, "Tip", "You must fill the form in order to insert an instructor!");
        return;
    }

    //get the data information
    int iid = ui->lineEdit_ins_iid->text().toInt();
    QString name = ui->lineEdit_ins_name->text();
    QString rank = ui->comboBox_rankAdd->currentText();
    QString dept = ui->comboBox_insDeptAdd->currentText();

    QString sql = QString("insert into instructor(IID, Name, Rank, Department) values(%1, '%2', '%3', '%4');")
            .arg(iid).arg(name).arg(rank).arg(dept);


    query.exec(sql);

    ui->lineEdit_ins_iid->clear();
    ui->lineEdit_ins_name->clear();
    ui->comboBox_rankAdd->setCurrentIndex(0);
    ui->comboBox_insDeptAdd->setCurrentIndex(0);

    model->submitAll();
    model->select();
    showInsCounts();
}


void MainWindow::on_btnFind_ins_clicked()
{
    QSqlQuery query;
    //if there is nothing in the find box, do nothing
    if(ui->lineEdit_iidFind->text().isEmpty())
    {
        QMessageBox::information(this, "Message", "You need to input an instructor id to find him/her!");
        return;
    }

    //get the id of the instructor
    int iid = ui->lineEdit_iidFind->text().toInt();

    //set the selection filter, it is like puting a where statment
    model->setFilter(QString("iid = %1").arg(iid));
    model->select();
    model->submitAll();
    showInsCounts();

    ui->lineEdit_iidFind->clear(); //reset
}



void MainWindow::on_btnDel_ins_clicked()
{
    QSqlQuery query;
    //if there is nothing in the input iid box, do nothing
    if(ui->lineEdit_iidDel->text().isEmpty()) return;

    //select the data, we should onlt get <= 1 record since IID is the primary key of the instructor table
    int iid = ui->lineEdit_iidDel->text().toInt();
    query.exec(QString("select * from instructor where iid = %1").arg(iid));

    //if we have one record
    if(!query.next())
    {
        QMessageBox::information(this, "Message", QString("The instructor with id: %1 does not exist!").arg(iid));
        return;
    }

    //get the name and department
    QString name = query.value("Name").toString();
    QString dept = query.value("Department").toString();

    //Comfirm to delete the record
    int ret = QMessageBox::question(this, "Confirmation",
                     QString("Delete instructor '%1' from department '%2'?").arg(name).arg(dept));

    //Only delete when the user comfirmed on deletion
    if(ret == QMessageBox::Yes)
    {
        query.exec(QString("delete from instructor where iid = '%1'").arg(iid));
        query.exec(QString("update course set iid = -1, Instructor = 'None' where iid = %1").arg(iid)); //update course table
        model->submitAll();
        model->select();
        ui->lineEdit_iidDel->clear();
        showInsCounts();
    }
}

void MainWindow::on_btnUpdate_2_clicked()
{
    QSqlQuery query;

    //if the id box is empty, do nothing
    if(ui->lineEdit_IID_update->text().isEmpty()) return;

    QString sql = QString("update instructor set");
    bool first = false;
    int iid = ui->lineEdit_IID_update->text().toInt();

    if(ui->comboBox_rankUpdate->currentIndex() != 0)
    {
        sql += QString(" Rank = '%1'").arg(ui->comboBox_rankUpdate->currentText());
        first = true;
    }

    if(ui->comboBox_insDeptUpdate->currentIndex() != 0)
    {
        if(!first)
        {
            sql += QString(" department = '%1'").arg(ui->comboBox_insDeptUpdate->currentText());
            first = true;
        }
        else
        {
            sql += QString(", department = '%1'").arg(ui->comboBox_insDeptUpdate->currentText());
        }
    }

    //if there is a need to update the name
    if(!ui->lineEdit_name_update->text().isEmpty())
    {
        if(!first)
            sql += QString(" name = '%1'").arg(ui->lineEdit_name_update->text());
        else
            sql += QString(", name = '%1'").arg(ui->lineEdit_name_update->text());
    }


    //uodate specific student
    sql += QString(" where iid = %1;").arg(iid);
    query.exec(sql);

    model->select();
    model->submitAll();
}

//clear the content in the instructor update form
void MainWindow::on_pushButton_4_clicked()
{
    ui->lineEdit_IID_update->clear();
    ui->lineEdit_name_update->clear();
    ui->comboBox_rankUpdate->setCurrentIndex(0);
    ui->comboBox_insDeptUpdate->setCurrentIndex(0);
}


void MainWindow::on_btn_showAll_ins_clicked()
{
    model->setFilter("");
    model->select();
    model->submitAll();
    showInsCounts();
}

//reset the content in the instruction filter form
void MainWindow::on_pushButton_3_clicked()
{
    ui->lineEdit_nameFil_ins->clear();
    ui->comboBox_rankFill->setCurrentIndex(0);
    ui->comboBox_insDeptFll->setCurrentIndex(0);
}



void MainWindow::on_btn_fill_ins_clicked()
{
    bool first = false;
    QString filter = "";
    if(!ui->lineEdit_nameFil_ins->text().isEmpty())
    {
        filter += QString("name = '%1'").arg(ui->lineEdit_nameFil_ins->text());
        first = true;
    }

    if(ui->comboBox_rankFill->currentText() != "All")
    {
        if(!first)
        {
            filter += QString("Rank = '%1'").arg(ui->comboBox_rankFill->currentText());
            first = true;
        }
        else
            filter += QString(" AND Rank = '%1'").arg(ui->comboBox_rankFill->currentText());
    }
    if(ui->comboBox_insDeptFll->currentText() != "All")
    {
        if(!first)
            filter += QString("department = '%1'").arg(ui->comboBox_insDeptFll->currentText());
        else
            filter += QString(" AND department = '%1'").arg(ui->comboBox_insDeptFll->currentText());
    }

    model->setFilter(filter);
    model->select();
    model->submitAll();
    showInsCounts();
}

//show the current number of courses
void MainWindow::showCourseCounts()
{
    int cnt = model->rowCount();
    ui->lineEdit_courseCnt->setText(QString::number(cnt));
}


void MainWindow::on_btn_addCourse_clicked()
{
    QSqlQuery query;
    //if any feild is empty, we should not add the record
    if(ui->lineEdit_cid->text().isEmpty() || ui->lineEdit_courseName->text().isEmpty() || ui->lineEdit_insIID->text().isEmpty())
    {
        QMessageBox::information(this, "Tip", "You must fill the form in order to add a course!");
        return;
    }

    //get the data information
    int iid = ui->lineEdit_insIID->text().toInt();
    query.exec(QString("select * from instructor where iid = %1").arg(iid));
    //if we dont have such an instructor in our database, we should not do anything
    if(!query.next())
    {
        QMessageBox::information(this, "Message", "Cannot find the instructor, double check IID!");
        return;
    }

    QString insName = query.value("Name").toString();
    QString cid = ui->lineEdit_cid->text();
    QString courseName = ui->lineEdit_courseName->text();
    QString semester = ui->comboBox_semesterAdd->currentText();

    QString sql = QString("insert into course(CID, courseName, Semester, IID, Instructor) values('%1', '%2', '%3', %4, '%5');")
            .arg(cid).arg(courseName).arg(semester).arg(iid).arg(insName);


    query.exec(sql);

    ui->lineEdit_cid->clear();
    ui->lineEdit_courseName->clear();
    ui->lineEdit_insIID->clear();
    ui->comboBox_semesterAdd->setCurrentIndex(0);

    model->submitAll();
    model->select();
    showCourseCounts();
}


void MainWindow::on_btnFindCid_clicked()
{
    //if there is nothing in the line edit find box, then do nothing
    if(ui->lineEdit_cidFind->text().isEmpty())
    {
        QMessageBox::information(this, "Message", "Please enter a course ID!");
        return;
    }

    QString CID = ui->lineEdit_cidFind->text();

    //set the filter and only find course where the course ID == userInput
    model->setFilter(QString("CID = '%1'").arg(CID));
    model->select();
    model->submitAll();
    ui->lineEdit_cidFind->clear();
    showCourseCounts();
}


void MainWindow::on_btnDelCid_clicked()
{
    QSqlQuery query;
    //if there is nothing in the input iid box, do nothing
    if(ui->lineEdit_cidDel->text().isEmpty()) return;

    //select the data, we should onlt get <= 1 record since IID is the primary key of the instructor table
    QString CID = ui->lineEdit_cidDel->text();
    query.exec(QString("select * from course where cid = '%1'").arg(CID));

    //if we have dont have record
    if(!query.next())
    {
        QMessageBox::information(this, "Message", QString("The course with cid: '%1' does not exist!").arg(CID));
        return;
    }

    //get the course name
    QString name = query.value("courseName").toString();

    //Comfirm to delete the record
    int ret = QMessageBox::question(this, "Confirmation", QString("Delete course '%1' ?").arg(name));

    //Only delete when the user comfirmed on deletion
    if(ret == QMessageBox::Yes)
    {
        query.exec(QString("delete from course where cid = '%1'").arg(CID));
        query.exec(QString("delete from enrollment where cid = '%1'").arg(CID));//update course enrollment as well
        model->submitAll();
        model->select();
        ui->lineEdit_cidDel->clear();
        showCourseCounts();
    }
}

//show all courses
void MainWindow::on_btn_showAll_courses_clicked()
{
    model->setFilter("");
    model->select();
    model->submitAll();
    showCourseCounts();
}



void MainWindow::on_btn_courseUpdate_clicked()
{
    QSqlQuery query;

    //if the cid box is empty, do nothing
    if(ui->lineEdit_CID_update->text().isEmpty()) return;
    QString CID = ui->lineEdit_CID_update->text();

    QString sql = QString("update course set");
    bool first = false;
    int iid = -1;
    QString insName;
    //check if the iid is valid
    if(!ui->lineEdit_course_update_iid->text().isEmpty())
    {
        int tempIID = ui->lineEdit_course_update_iid->text().toInt();
        query.exec(QString("select * from instructor where iid = %1").arg(tempIID));
        //if we dont have such an instructor in our database, we should not update anything
        if(!query.next())
        {
            QMessageBox::information(this, "Message", "Cannot find the instructor, double check IID!");
            return;
        }
        iid = tempIID;
        insName = query.value("Name").toString();
    }

    //check if the courseName is empty, if it is not, then we update the course name
    if(!ui->lineEdit_courseName_update->text().isEmpty())
    {
        sql += QString(" courseName = '%1'").arg(ui->lineEdit_courseName_update->text());
        first = true;
    }

    //we have a valid instructor to update
    if(iid != -1)
    {
        if(!first)
        {
            sql += QString(" IID = %1, Instructor = '%2'").arg(iid).arg(insName);
            first = true;
        }
        else
        {
            sql += QString(", IID = %1, Instructor = '%2'").arg(iid).arg(insName);
        }
    }

    //if there is a need to update the semester
    if(ui->comboBox_semesterUpdate->currentIndex() != 0)
    {
        if(!first)
            sql += QString(" semester = '%1'").arg(ui->comboBox_semesterUpdate->currentText());
        else
            sql += QString(", semester = '%1'").arg(ui->comboBox_semesterUpdate->currentText());
    }


    //uodate specific student
    sql += QString(" where CID = '%1';").arg(CID);
    query.exec(sql);

    model->select();
    model->submitAll();
}

//reset the update form for courses
void MainWindow::on_btn_courseUpdateClear_clicked()
{
    ui->lineEdit_CID_update->clear();
    ui->lineEdit_courseName_update->clear();
    ui->lineEdit_course_update_iid->clear();
    ui->comboBox_semesterUpdate->setCurrentIndex(0);
}



void MainWindow::on_btn_fill_course_clicked()
{
    QSqlQuery query;
    bool first = false;
    QString filter = "";

    if(!ui->lineEdit_iidCourse_fill->text().isEmpty())
    {
        filter += QString("IID = %1").arg(ui->lineEdit_iidCourse_fill->text().toInt());
        first = true;
    }
    if(ui->comboBox_semesterFill->currentIndex()!=0)
    {
        if(!first)
            filter += QString("Semester = '%1'").arg(ui->comboBox_semesterFill->currentText());
        else
            filter += QString(" AND Semester = '%1'").arg(ui->comboBox_semesterFill->currentText());
    }

    model->setFilter(filter);
    model->select();
    model->submitAll();
    showCourseCounts();
}


void MainWindow::on_btn_enroll_clicked()
{
    if(ui->lineEdit_enrollSID->text().isEmpty()) return;
    QSqlQuery query;
    //check if the student exist
    int sid = ui->lineEdit_enrollSID->text().toInt();
    query.exec(QString("select * from student where id = %1").arg(sid));
    if(!query.next())
    {
        QMessageBox::information(this, "Message", QString("There is no student with ID = %1 !").arg(sid));
        return;
    }
    //the student exists, get all the enrollment information
    QString student_name = query.value("Name").toString();
    QString course_name = ui->comboBox_enroll->currentText();
    query.exec(QString("select * from course where courseName = '%1'").arg(course_name));
    query.next();
    QString cid = query.value("CID").toString();
    QString semester = query.value("Semester").toString();

    //insert record into table
    query.exec(QString("insert into enrollment(SID, CID, name, courseName, Semester) values(%1, '%2', '%3', '%4', '%5')").arg(sid).arg(cid).arg(student_name).arg(course_name).arg(semester));

    model->select();
    model->submitAll();
}

//reload the current courses
void MainWindow::loadCourse()
{
    QSqlQuery query;
    query.exec("Select * from course;");
    QStringList list;
    while(query.next())
        list.append(query.value("courseName").toString());

    ui->comboBox_enroll->clear();
    ui->comboBox_enroll->addItems(list);
}

//whenever the box is edited, we need to reload what course the current student has
void MainWindow::on_lineEdit_drop_textEdited(const QString &arg1)
{
    loadDropCourse(arg1.toInt());
}

void MainWindow::loadDropCourse(int id)
{
    int sid = id;
    QSqlQuery query;
    query.exec(QString("select * from enrollment where sid = %1").arg(sid));

    QStringList list;
    while(query.next())
        list.append(query.value("courseName").toString());
    ui->comboBox_drop->clear();
    ui->comboBox_drop->addItems(list);
}

//drop a student from a course
void MainWindow::on_btn_drop_clicked()
{
    int sid = ui->lineEdit_drop->text().toInt();

    QSqlQuery query;
    query.exec(QString("select * from student where id = %1").arg(sid));
    if(!query.next())
    {
        QMessageBox::information(this, "Message", QString("There is no student with ID = %1!").arg(sid));
        return;
    }
    QString courseName = ui->comboBox_drop->currentText();

    int ret = QMessageBox::question(this, "comfirmation", QString("Drop the course '%1'?").arg(courseName));
    if(ret == QMessageBox::Yes)
    {
        query.exec(QString("delete from enrollment where sid = %1 AND courseName = '%2'").arg(sid).arg(courseName));
        model->select();
        model->submitAll();
        loadDropCourse(sid);
    }
}

//show all the records in the enrollment table
void MainWindow::on_btn_showAllEnrollment_clicked()
{
    model->setFilter("");
    model->select();
    model->submitAll();
}

//reset the filter from in the enrollment table
void MainWindow::on_bt_enroll_fill_reset_clicked()
{
    ui->lineEdit_enroll_cid_fill->clear();
    ui->lineEdit_enroll_sid_fill->clear();
    ui->comboBox_semesterFill_enroll->setCurrentIndex(0);
}


void MainWindow::on_btn_enroll_fill_clicked()
{
    QString filter = "";
    bool first = false;

    if(!ui->lineEdit_enroll_sid_fill->text().isEmpty())
    {
        filter += QString("sid = %1").arg(ui->lineEdit_enroll_sid_fill->text().toInt());
        first = true;
    }
    if(!ui->lineEdit_enroll_cid_fill->text().isEmpty())
    {
        if(!first)
        {
            filter += QString("cid = '%1'").arg(ui->lineEdit_enroll_cid_fill->text());
            first = true;
        }
        else
            filter += QString(" AND cid = '%1'").arg(ui->lineEdit_enroll_cid_fill->text());
    }
    if(ui->comboBox_semesterFill_enroll->currentIndex() != 0)
    {
        if(!first)
            filter += QString("semester = '%1'").arg(ui->comboBox_semesterFill_enroll->currentText());
        else
            filter += QString(" AND semester = '%1'").arg(ui->comboBox_semesterFill_enroll->currentText());
    }

    model->setFilter(filter);
    model->select();
    model->submitAll();
}


//go to the log in page
void MainWindow::on_btn_log_clicked()
{
    ui->btn_log->setVisible(false);
    ui->lable_role->setVisible(false);
    ui->label_2->setVisible(false);
    ui->comboBox->setVisible(false);
    ui->btn_goToAcc->setVisible(false);
    ui->stackedWidget->setCurrentIndex(5);
}


void MainWindow::on_btn_acc_add_clicked()
{
    QString user = ui->lineEdit_acc_user_add->text(), pw = ui->lineEdit_acc_pw_add->text(), role = ui->comboBox_role_add->currentText();
    //check if the username or the passward is empty, if any of the two is empty, return;
    if(user.isEmpty() || pw.isEmpty())
    {
        QMessageBox::information(this, "Warning", "The username and passward must be filled!");
        return;
    }
    QSqlQuery query;
    query.exec(QString("insert into account(Username, Passward, Role) values('%1', '%2', '%3');").arg(user).arg(pw).arg(role));
    model->select();
    model->submitAll();

    //reset add account form
    ui->lineEdit_acc_user_add->clear();
    ui->lineEdit_acc_pw_add->clear();
    ui->comboBox_role_add->setCurrentIndex(0);
}


void MainWindow::on_btn_accDel_clicked()
{
    QString user = ui->lineEdit_acc_del->text();
    //if the user didn't enter anything
    if(user.isEmpty()) return;

    QSqlQuery query;
    query.exec(QString("select * from account where Username = '%1';").arg(user));

    //no match account
    if(!query.next())
    {
        QMessageBox::information(this, "Message", "Account didn't match in database!");
        return;
    }

    //ask if the user wants to delete it
    int ret = QMessageBox::question(this, "Confirmation", "Confirm to delete user");

    if(ret == QMessageBox::Yes)
    {
        query.exec(QString("delete from account where Username = '%1';").arg(user));
        model->select();
        model->submitAll();
        ui->lineEdit_acc_del->clear(); //reset the account box
    }
}

void MainWindow::on_btn_acc_update_clicked()
{
    QSqlQuery query;
    QString user = ui->lineEdit_acc_update->text(), pw = ui->lineEdit_pw_update->text();
    //if the user didn't enter anything in the username box
    if(user.isEmpty()) return;

    if(ui->comboBox_role_update->currentIndex() == 0 && pw.isEmpty())
        return;
    else if(ui->comboBox_role_update->currentIndex() == 0)
        query.exec(QString("update account set Passward = '%1' where Username = '%2';").arg(pw).arg(user));
    else if(pw.isEmpty())
        query.exec(QString("update account set Role = '%1' where Username = '%2';").arg(ui->comboBox_role_update->currentText()).arg(user));
    else
        query.exec(QString("update account set Role = '%1', Passward = '%2' where Username = '%3';").arg(ui->comboBox_role_update->currentText()).arg(pw).arg(user));

    model->select();
    model->submitAll();

    ui->lineEdit_acc_update->clear();
    ui->lineEdit_pw_update->clear();
    ui->comboBox_role_update->setCurrentIndex(0);
}

//exit the program
void MainWindow::on_btn_exit_clicked()
{
    int ret = QMessageBox::question(this, "Exit", "Quit the program?");
    if(ret == QMessageBox::Yes)
        exit(1);
}

//when the show passward btn is pressed, we want to show the passward, so we set the echo mode to normal
void MainWindow::on_btn_showpw_pressed()
{
    ui->lineEdit_logpw->setEchoMode(QLineEdit::Normal);
}

//when released reset it to passward mode;
void MainWindow::on_btn_showpw_released()
{
    ui->lineEdit_logpw->setEchoMode(QLineEdit::Password);
}

//when log in is clicked, we check what role then set permission
void MainWindow::on_btn_login_clicked()
{
    QString user = ui->lineEdit_loguser->text(), pw = ui->lineEdit_logpw->text();
    if(user.isEmpty() || pw.isEmpty())
    {
        QMessageBox::information(this,"Message","Please fill the username and passward!");
        return;
    }

    QSqlQuery query;
    //check if the user account is in the db system
    query.exec(QString("select * from account where username = '%1'").arg(user));
    if(!query.next())
    {
        QMessageBox::information(this,"Message","User account did not find!");
        return;
    }

    //check if the user account is correct
    QString correctPW = query.value("Passward").toString(), role = query.value("Role").toString();
    if(correctPW != pw)
    {
        QMessageBox::information(this,"Message","Passward incorrect!");
        return;
    }

    current_role = role;
    ui->lable_role->setText(current_role);
    ui->lable_role->setVisible(true);
    ui->btn_log->setVisible(true);

    if(role == "Root Administrator")
    {
        ui->label_2->setVisible(true);
        ui->comboBox->setVisible(true);
        ui->btn_goToAcc->setVisible(true);
        on_btn_goToAcc_clicked();
    }
    else if(role == "School Administrator")
    {
        ui->label_2->setVisible(true);
        ui->comboBox->setVisible(true);
        ui->stackedWidget->setCurrentIndex(0);
    }
    else if(role == "Instructor")
    {
        ui->stackedWidget->setCurrentWidget(ui->Course_page);
        model->setTable("course");
        showCourseCounts();
        model->select();
        ui->tableView_course->setColumnWidth(1, 194);
    }
    else if(role == "Student")
    {
        ui->stackedWidget->setCurrentWidget(ui->page_enroll);
        model->setTable("enrollment");
        model->select();
        ui->tableView_enroll->setColumnWidth(3, 194);
    }

    ui->lineEdit_loguser->clear();
    ui->lineEdit_logpw->clear();
    loadDept();
    loadCourse();
}

void MainWindow::on_btn_goToAcc_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    model->setTable("account");
    model->select();
    model->submitAll();
    ui->tableView_account->setColumnWidth(2, 176);
}

