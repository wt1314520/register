// Module registrar:dm:student
// File: studentbroker.cppm   Version: 0.1.0   License: AGPLv3
// Created: wangtao      2026-1-17 21:45:12
// Description:
//
// Change Log:
//     [v0.1.1]     2026-1-16 21:50:34
//         *
export module registrar:dm.studentbroker;
import std;
import registrar:student;
#include <pqxx/pqxx>
using std::string;
using std::vector;
using std::shared_ptr;

export class StudentBroker {
    pqxx::connection conn;
    StudentBroker() : conn("dbname=registrar_db user=wt password=123456 hostaddr=127.0.0.1 port=5432") {}
public:
    static StudentBroker& singleton() {
        static StudentBroker instance;
        return instance;
    }

    shared_ptr<Student> findById(string sid) {
        pqxx::work txn(conn);
        auto res = txn.exec("SELECT * FROM students WHERE id = '" + sid + "'");
        txn.commit();
        if (res.empty()) return nullptr;
        return std::make_shared<Student>(res[0][0].as<string>(), res[0][1].as<string>());
    }

    void save(shared_ptr<Student> student) {
        pqxx::work txn(conn);
        txn.exec("INSERT INTO students (id, name) VALUES ('" + student->getId() + "', '" + student->getName() + "')");
        txn.commit();
    }

    void remove(string sid) {
        pqxx::work txn(conn);
        txn.exec("DELETE FROM students WHERE id = '" + sid + "'");
        txn.commit();
    }
};

