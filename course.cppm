// Module
// File: course.cppm  Version: 0.1.0   License: AGPLv3
// Created: hejiahuan      2026-01-10 20:54:51
// Description:
//
export module registrar:course;
import std;
import registrar:student;
using std::string;
using std::vector;

class Student;

// 新增评分结构体
export struct CourseScore {
    string student_id;
    string course_id;
    double score;
    string teacher_comment;
    string create_time;
};

export class Course
{
public:
    Course(string id, string name, short credit = 3);

    bool acceptEnrollment(Student* student);
    string roster();
    bool hasId(string id);

    // 新增：设置学生评分
    void setStudentScore(string student_id, double score, string comment);
    // 新增：获取学生评分
    CourseScore getStudentScore(string student_id);

    // Getters
    string getId() const { return m_id; }
    string getName() const { return m_name; }
    short getCredit() const { return m_credit; }

private:
    string m_name;
    string m_id;
    short m_credit;
    vector<Student*> _students;
    std::map<string, CourseScore> _scores;  // 存储学生评分
};// 原有方法保持不变

void Course::setStudentScore(string student_id, double score, string comment) {
    // 调用数据层保存
    CourseBroker::singleton().saveScore(student_id, m_id, score, comment);
    // 更新本地缓存
    CourseScore cs;
    cs.student_id = student_id;
    cs.course_id = m_id;
    cs.score = score;
    cs.teacher_comment = comment;
    // 从数据库获取创建时间并更新缓存
    auto dbScore = CourseBroker::singleton().getScore(student_id, m_id);
    cs.create_time = dbScore.create_time;
    _scores[student_id] = cs;
}
CourseScore Course::getStudentScore(string student_id) {
    if (_scores.find(student_id) != _scores.end()) {
        return _scores[student_id];
    }
    // 从数据库查询并缓存
    auto score = CourseBroker::singleton().getScore(student_id, m_id);
    _scores[student_id] = score;
    return score;
}
