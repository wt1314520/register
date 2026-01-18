export module registrar:student;
import std;
import registrar:course;

using std::string;
using std::vector;

export class Student
{
public:
    Student(string id, string name);

    bool enrollIn(Course* course);
    string getEnrolledCourses();
    bool hasId(string id);

    // Getters
    string getId() const { return m_id; }
    string getName() const { return m_name; }

private:
    string m_id;
    string m_name;
    vector<Course*> _courses;
};
Student::Student(string id, string name)
    : m_id(std::move(id)), m_name(std::move(name)) {}

bool Student::enrollIn(Course* course) {
    if (!course) return false;
    if (course->acceptEnrollment(this)) {
        _courses.push_back(course);
        return true;
    }
    return false;
}

string Student::getEnrolledCourses() {
    string result = "Student: " + m_name + " (" + m_id + ")\nEnrolled Courses:\n";
    for (auto c : _courses) {
        result += "  - " + c->getName() + " (" + c->getId() + ")\n";
    }
    return result;
}

bool Student::hasId(string id) {
    return m_id == id;
}

