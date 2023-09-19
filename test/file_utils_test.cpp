//
// Created by Philip on 9/18/2023.
//
#include <gtest/gtest.h>
#include "../src/FileIO/FileUtils.h"
#include "../src/Exceptions/RuntimeException.h"
TEST(UTILITY_TESTS, TEST_FILE_VALIDATION) {
    ASSERT_THROW(EngiGraph::validateFileExistence("./test_files/tex.txt"),EngiGraph::RuntimeException); //does not exist
    ASSERT_NO_THROW(EngiGraph::validateFileExistence("./test_files/text.txt")); //does exist
    ASSERT_THROW(EngiGraph::validateFileExtensions("./test_files/text.txt", {".obj",".foobar",".stl"}),EngiGraph::RuntimeException); //Wrong file type
    ASSERT_NO_THROW(EngiGraph::validateFileExtensions("./test_files/text.txt", {".obj",".foobar",".txt"})); //Right file type
    ASSERT_NO_THROW(EngiGraph::validateFileExtensions("./test_files/text.TXT", {".obj",".foobar",".txt"})); //Right file type capital
    ASSERT_NO_THROW(EngiGraph::validateFileExtensions("./test_files/", {""})); //Right file type directory
    //check output
    try {
        EngiGraph::validateFileExtensions("./test_files/text.txt", {".obj",".foobar",".stl"});
    } catch (const EngiGraph::RuntimeException& exception){
        ASSERT_STREQ(exception.what(), "Path: ./test_files/text.txt : does not match any of the following extensions : .obj .foobar .stl ");
    }
}