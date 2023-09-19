//
// Created by Philip on 9/18/2023.
//

#pragma once

#include <exception>
#include <string>

namespace EngiGraph {

    /**
     * General runtime exception to indicate a non-fatal problem that needs handling or at the very least needs to be displayed to the user.
     */
    class RuntimeException : public std::exception{
    private:
        std::string message;
    public:
        /**
         * Create a general runtime exception.
         * @param message Detailed exception message.
         */
        RuntimeException(const std::string& message) : message(message) {}

        [[nodiscard]] const char* what() const noexcept override
        {
            return message.c_str(); //This is not great
        }
    };

} // EngiGraph
