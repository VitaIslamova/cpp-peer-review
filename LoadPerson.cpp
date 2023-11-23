#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <tuple>
#include <optional>

using namespace std::literals;

struct Person {
    std::string name;
    int age;
};

enum class DBLogLevel {
    Noop,
    Error,
    Warning,
    Info,
    Debug
};

class DBQuery {
public:
    DBQuery(std::string query) {}
};

class DBHandler {
public:
    DBHandler() = default;

    bool IsOK() const { return true; }

    template<typename... Args>
    std::vector<std::tuple<Args...>> LoadRows(const DBQuery &query) {
        std::vector<std::tuple<Args...>> results;
        std::string name = "John";
        int age = 30;
        results.push_back(make_tuple(name, age));
        return results;
    }

    std::string Quote(const std::string_view str) {
        return std::string(str);
    }

    void Disconnect() {}
};

class DBConnector {
public:
    DBConnector(bool allow_exceptions, DBLogLevel log_level) : allow_exceptions_(allow_exceptions),
                                                               log_level_(log_level) {}

    DBHandler ConnectTmp(std::string_view db_name, int db_connection_timeout) {
        return {};
    }

    DBHandler Connect(std::string_view db_name, int db_connection_timeout) {
        return {};
    }

private:
    [[maybe_unused]] bool allow_exceptions_;
    [[maybe_unused]] DBLogLevel log_level_;
};

class Database {
public:
    class Builder;

    const std::string TMP_PREFIX = "tmp."s;

    DBHandler Connect(const std::string &db_name) {
        if (db_) {
            db_->Disconnect();
        }
        DBConnector connector(allow_exceptions_, log_level_);
        if (db_name.starts_with(TMP_PREFIX)) {
            db_ = connector.ConnectTmp(db_name, connection_timeout_);
        } else {
            db_ = connector.Connect(db_name, connection_timeout_);
        }
        return db_.value();
    }

    ~Database() {
        if (db_ && db_->IsOK()) {
            db_->Disconnect();
        }
    }

private:
    friend Builder;

    std::optional<DBHandler> db_;
    int connection_timeout_ = 1;
    bool allow_exceptions_ = false;
    DBLogLevel log_level_ = DBLogLevel::Noop;
};

class Database::Builder {
public:
    Builder &SetConnectionTimeout(int connectionTimeout) {
        database.connection_timeout_ = connectionTimeout;
        return *this;
    }

    Builder &SetAllowExceptions(bool allowExceptions) {
        database.allow_exceptions_ = allowExceptions;
        return *this;
    }

    Builder &SetLogLevel(DBLogLevel logLevel) {
        database.log_level_ = logLevel;
        return *this;
    }

    DBHandler Build(const std::string &db_name) {
        return database.Connect(db_name);
    }

private:
    Database database;
};

std::optional<std::vector<Person>> GetPersons(DBHandler &db, const DBQuery &query) {
    auto rows = db.LoadRows<std::string, int>(query);
    if (rows.empty()) {
        return std::nullopt;
    }
    std::vector<Person> persons;
    persons.reserve(rows.size());
    for (auto &[name, age]: rows) {
        persons.push_back({std::move(name), age});
    }
    return persons;
}

struct LoadPersonsParams {
    int min_age;
    int max_age;
    std::string_view name_filter;
};

std::optional<std::vector<Person>> LoadPersons(DBHandler &db, const LoadPersonsParams &params) {
    std::ostringstream query_str;
    query_str << "from Persons "s
              << "select Name, Age "s
              << "where Age between "s << params.min_age << " and "s << params.max_age << " "s
              << "and Name like '%"s << db.Quote(params.name_filter) << "%'"s;
    DBQuery query(query_str.str());
    return GetPersons(db, query);
}

int main() {
    std::string db_name = "tmp.db"s;
    int min_age = 16;
    int max_age = 24;
    std::string_view name_filter = "John";

    std::optional<std::vector<Person>> persons;
    try {
        DBHandler db = Database::Builder()
                .SetAllowExceptions(true)
                .SetConnectionTimeout(30)
                .SetLogLevel(DBLogLevel::Debug)
                .Build(db_name);
        persons = LoadPersons(db, LoadPersonsParams{min_age, max_age, name_filter});
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }

    if (!persons) {
        std::cout << "No persons found" << std::endl;
    } else {
        for (auto &p: persons.value()) {
            std::cout << p.name << " " << p.age << std::endl;
        }
    }
    return 0;
}
