import std;
import registrar:student;
import registrar:course;
import registrar:dm.studentbroker;
import registrar:dm.coursebroker;

int main() {
    // 1. 查询已有课程（C001：C++程序设计）
    auto cppCourse = CourseBroker::singleton().findById("C001");
    if (cppCourse) {
        std::cout << "查询到课程：" << cppCourse->getName() << std::endl;
    }

    // 2. 老师给张三（S001）的C++课程打分
    cppCourse->setStudentScore("S001", 98.0, "优秀，代码规范，逻辑清晰");
    std::cout << "已为张三的C++课程打分：98分" << std::endl;

    // 3. 查询张三的C++课程评分
    auto score = cppCourse->getStudentScore("S001");
    if (score.score > 0) {
        std::cout << "张三的C++成绩：" << score.score << "分" << std::endl;
        std::cout << "老师评语：" << score.teacher_comment << std::endl;
    }

    // 4. 查询C001课程所有学生的评分
    auto allScores = CourseBroker::singleton().getCourseAllScores("C001");
    std::cout << "\nC++程序设计课程所有评分：" << std::endl;
    for (auto& s : allScores) {
        std::cout << "学生ID：" << s.student_id << " 分数：" << s.score << std::endl;
    }

    // 5. 新增一门课程（面向对象设计已存在，演示更新）
    auto oopCourse = std::make_shared<Course>("C003", "面向对象设计", 4);
    CourseBroker::singleton().save(oopCourse);
    std::cout << "\n已更新C003课程学分：4分" << std::endl;

    return 0;
}
