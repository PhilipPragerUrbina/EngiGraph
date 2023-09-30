//
// Created by Philip on 9/24/2023.
//
#include <gtest/gtest.h>
#include "../src/FileIO/ImageIo.h"

TEST(LOADER_TESTS, TEST_IMAGE_LOADER) {
    //Test 4 channel
    ASSERT_NO_THROW(EngiGraph::Image<uint32_t> png = EngiGraph::readImage("./test_files/loading_test.png"));
    EngiGraph::Image<uint32_t> png = EngiGraph::readImage("./test_files/loading_test.png");
    ASSERT_EQ(png.getWidth(),32);
    ASSERT_EQ(png.getHeight(),32);
    //Test 3 channel
    ASSERT_NO_THROW(EngiGraph::Image<uint32_t> jpg = EngiGraph::readImage("./test_files/loading_test.jpg"));
    EngiGraph::Image<uint32_t> jpg = EngiGraph::readImage("./test_files/loading_test.jpg");
    ASSERT_EQ(jpg.getWidth(),256);
    ASSERT_EQ(jpg.getHeight(),256);
    uint32_t color = jpg.getPixel(0,0);
    ASSERT_NE(color,255); //Assert not black
    ASSERT_EQ(((uint8_t*)&color)[3], 255); //Test generated alpha

}