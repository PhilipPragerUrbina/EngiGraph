//
// Created by Philip on 9/18/2023.
//

#include "FileUtils.h"
#include "../Exceptions/RuntimeException.h"

namespace EngiGraph {

    void validateFileExistence(const std::string &filename) {
        std::filesystem::path path{filename};
        if(!exists(path)) throw RuntimeException("Path: " + filename + " : does not exist.");
    }

    void validateFileExtensions(const std::string &filename, const std::vector<std::string> &allowed_extensions) {
        std::string extension = std::filesystem::path{filename}.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),[](unsigned char c){ return std::tolower(c); });
        for (const std::string& allowed_extension : allowed_extensions) {
            if(allowed_extension == extension){
                return;
            }
        }
        std::string list_allowed_extensions;
        for (const std::string& allowed_extension : allowed_extensions) {
            list_allowed_extensions += allowed_extension + " ";
        }
        throw RuntimeException("Path: " + filename + " : does not match any of the following extensions : " + list_allowed_extensions);
    }

} // EngiGraph