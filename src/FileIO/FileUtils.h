//
// Created by Philip on 9/18/2023.
//

#pragma once

#include <filesystem>

namespace EngiGraph {

    /**
     * Validate that a file exists.
     * @param filename Location of file.
     * @throws RuntimeException The file does not exist.
     */
    void validateFileExistence(const std::string& filename);

    /**
     * Validate that a file has specific file extensions.
     * @param filename Location of file. Uppercase extensions will be converted to lowercase.
     * @param allowed_extensions List of allowed extensions with the '.', in lowercase. Putting an empty string here allows for folders.
     * @throws RuntimeException File does not match extensions.
     */
    void validateFileExtensions(const std::string& filename, const std::vector<std::string>& allowed_extensions);

} // EngiGraph
