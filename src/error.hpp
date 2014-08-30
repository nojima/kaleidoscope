#pragma once

#include "position.hpp"

namespace kaleidoscope {

    class BasicError {
    public:
        BasicError(const Position& position, const std::string& message):
            position_(position), message_(message) {}

        virtual ~BasicError() {}

        const Position& position() const noexcept {
            return position_;
        }

        const std::string& message() const noexcept {
            return message_;
        }

        std::string what() const {
            return position_.toString() + ": " + message_;
        }

    private:
        Position position_;
        std::string message_;
    };

}   // namespace kaleidoscope
