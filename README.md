# Student_info_system
A GUI application allows different roles(student, instructors, administrators) to manage the course table
---
### Tables and database
The application did not use MYSQL since it does not have a huge amount of data, instead this application was built based on SQLite. And all the data are maintained in a local file system.
The application will contain 5 tables:

Account table: (Username varchar(40) primary key, Passward varchar(20), Role varchar(20))
  - <img src="https://github.com/ZhengXiaohu98/Student_info_system/blob/master/gifDemo_account.gif" width=750><br>
Student table: (ID INTEGER primary key, Name varchar(50), Age INTEGER , Gender varchar(1), Department varchar(40) REFERENCES department(deptName)
  - <img src="https://github.com/ZhengXiaohu98/Student_info_system/blob/master/gifDemo_student.gif" width=750><br>
Instructor table:(IID INTEGER primary key, Name varchar(50), Rank varchar(20), Department varchar(40) REFERENCES department(deptName)
  - <img src="https://github.com/ZhengXiaohu98/Student_info_system/blob/master/gifDemo_instructor.gif" width=750><br>
Course table: (CID varchar(10) primary key, courseName varchar(100), Semester varchar(10), IID INTEGER, Instructor varchar(50))
  - <img src="https://github.com/ZhengXiaohu98/Student_info_system/blob/master/gifDemo_course.gif" width=750><br>
Department table: (deptName varchar(40) primary key, phoneNum varchar(13), officeNumber varchar(10)) same features like other tables

### User Stories
For root user: a root user have access to all the tables, thus can add accounts and delete accounts. There is no restriction to any actions.
For student: a student can only add course or drop course. Students don't have access to other actions.
For school: school can add department/instructor/student, but it cannot have access to account table. Account table can only be accessed by root user.
For instructor: a instructor can add a course or delete a course he wants to teach.

### Notes
Since it is a local file system, you can change the code to drop the tables first, then create your own account tables to manage the system. If you just want to test the application,the 
**initial root username is: xiaohu, and the passward is: 1228.**
