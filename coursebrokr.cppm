// Module
// File: coursebrokr.cppm  Version: 0.1.0   License: AGPLv3
// Created: hejiahuan      2026-01-11 21:34:21
// Description:
//
export module registrar:dm.coursebroker;
import std;
import registrar:course;
#include <pqxx/pqxx>
using std::string;
using std::shared_ptr;
using std::vector;

export class CourseBroker {
    pqxx::connection conn;
    CourseBroker() : conn("dbname=registrar_db user=wt password=123456 hostaddr=127.0.0.1 port=5432");) {}
public:
    // 单例模式
    static CourseBroker& singleton() {
        static CourseBroker instance;
        return instance;
    }

    // 1. 根据ID查询课程
    shared_ptr<Course> findById(string cid) {
        try {
            pqxx::work txn(conn);
            auto res = txn.exec("SELECT id, name, credit FROM courses WHERE id = '" + cid + "'");
            txn.commit();
            if (res.empty()) return nullptr;
            std::make_shared<Course>(res[0]["id"].as<string>(), res[0]["name"].as<string>();
        } catch (const std::exception& e) {
            std::cerr << "查询课程失败：" << e.what() << std::endl;
            return nullptr;
        }
    }

    // 2. 查询所有课程
    vector<shared_ptr<Course>> findAll() {
        vector<shared_ptr<Course>> courses;
        try {
            pqxx::work txn(conn);
            auto res = txn.exec("SELECT id, name, credit FROM courses");
            txn.commit();
            for (auto& row : res) {
                short credit = row.contains("credit") ? row["credit"].as<short>() : 3;
                courses.push_back(std::make_shared<Course>(
                    row["id"].as<string>(),
                    row["name"].as<string>(),
                    credit
                ));
            }
        } catch (const std::exception& e) {
            std::cerr << "查询所有课程失败：" << e.what() << std::endl;
        }
        return courses;
    }

    // 3. 保存课程信息
    bool save(shared_ptr<Course> course) {
        try {
            pqxx::work txn(conn);
            // 先检查课程是否已存在
            auto res = txn.exec("SELECT id FROM courses WHERE id = '" + course->getId() + "'");
            if (!res.empty()) {
                // 更新已有课程
                txn.exec(
                    "UPDATE courses SET name = '" + course->getName() +
                    "', credit = " + std::to_string(course->getCredit()) +
                    " WHERE id = '" + course->getId() + "'"
                );
            } else {
                // 插入新课程
                txn.exec(
                    "INSERT INTO courses (id, name, credit) VALUES ('" +
                    course->getId() + "', '" + course->getName() + "', " +
                    std::to_string(course->getCredit()) + ")"
                );
            }
            txn.commit();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "保存课程失败：" << e.what() << std::endl;
            return false;
        }
    }

    // 4. 删除课程
    bool remove(string cid) {
        try {
            pqxx::work txn(conn);
            txn.exec("DELETE FROM courses WHERE id = '" + cid + "'");
            txn.commit();
            return txn.rows_affected() > 0;
        } catch (const std::exception& e) {
            std::cerr << "删除课程失败：" << e.what() << std::endl;
            return false;
        }
    }
    // 5. 保存选课记录
    bool saveEnrollment(string studentId, string courseId) {
        try {
            pqxx::work txn(conn);
            // 检查是否已选课
            auto res = txn.exec(
                "SELECT * FROM student_course WHERE student_id = '" + studentId +
                "' AND course_id = '" + courseId + "'"
            );
            if (!res.empty()) {
                txn.commit();
                return false; // 已选过
            }
            // 插入选课记录
            txn.exec(
                "INSERT INTO student_course (student_id, course_id) VALUES ('" +
                studentId + "', '" + courseId + "')"
            );
            txn.commit();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "选课失败：" << e.what() << std::endl;
            return false;
        }
    }

    // 6. 保存/更新学生课程评分
    bool saveScore(string studentId, string courseId, double score, string comment) {
        try {
            pqxx::work txn(conn);
            // 检查是否已有评分（有则更新，无则插入）
            auto res = txn.exec(
                "SELECT id FROM course_scores WHERE student_id = '" + studentId +
                "' AND course_id = '" + courseId + "'"
            );
            if (!res.empty()) {
                // 更新评分
                txn.exec(
                    "UPDATE course_scores SET score = " + std::to_string(score) +
                    ", teacher_comment = '" + comment + "'"
                    " WHERE student_id = '" + studentId + "' AND course_id = '" + courseId + "'"
                );
            } else {
                // 插入新评分
                txn.exec(
                    "INSERT INTO course_scores (student_id, course_id, score, teacher_comment) VALUES ('" +
                    studentId + "', '" + courseId + "', " + std::to_string(score) + ", '" + comment + "')"
                );
            }
            txn.commit();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "保存评分失败：" << e.what() << std::endl;
            return false;
        }
    }

    // 7. 查询学生课程评分
    CourseScore getScore(string studentId, string courseId) {
        CourseScore score{};
        try {
            pqxx::work txn(conn);
            auto res = txn.exec(
                "SELECT score, teacher_comment, create_time FROM course_scores "
                "WHERE student_id = '" + studentId + "' AND course_id = '" + courseId + "'"
            );
            txn.commit();
            if (!res.empty()) {
                score.student_id = studentId;
                score.course_id = courseId;
                score.score = res[0]["score"].as<double>();
                score.teacher_comment = res[0]["teacher_comment"].as<string>();
                score.create_time = res[0]["create_time"].as<string>();
            }
        } catch (const std::exception& e) {
            std::cerr << "查询评分失败：" << e.what() << std::endl;
        }
        return score;
    }
    // 8. 查询课程所有学生的评分
    vector<CourseScore> getCourseAllScores(string courseId) {
        vector<CourseScore> scores;
        try {
            pqxx::work txn(conn);
            auto res = txn.exec(
                "SELECT student_id, course_id, score, teacher_comment, create_time FROM course_scores "
                "WHERE course_id = '" + courseId + "'"
            );
            txn.commit();
            for (auto& row : res) {
                CourseScore score;
                score.student_id = row["student_id"].as<string>();
                score.course_id = row["course_id"].as<string>();
                score.score = row["score"].as<double>();
                score.teacher_comment = row["teacher_comment"].as<string>();
                score.create_time = row["create_time"].as<string>();
                scores.push_back(score);
            }
        } catch (const std::exception& e) {
            std::cerr << "查询课程评分失败：" << e.what() << std::endl;
        }
        return scores;
    }
};
