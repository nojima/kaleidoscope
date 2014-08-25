#pragma once

#include <cstdio>
#include <string>


class Position {
public:
    Position(const std::string& name, size_t line, size_t column):
        name_(name), line_(line), column_(column) {}

    const std::string& name() const noexcept {
        return name_;
    }

    size_t line() const noexcept {
        return line_;
    }

    size_t column() const noexcept {
        return column_;
    }

    std::string toString() const {
        const size_t len = name_.size() + 1 + 20 + 1 + 20 + 1;
        std::string result(len, '\0');
        std::snprintf(&result[0], len, "%s:%zd:%zd", name_.c_str(), line_, column_);
        return std::move(result);
    }

private:
    const std::string& name_;
    size_t line_;
    size_t column_;
};
