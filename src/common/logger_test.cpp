//
// Created by lz on 10/14/16.
//
#include <string>
#include <gtest/gtest.h>
#include "logger.hpp"

TEST(LoggerTest, TestLoggerSingleton)
{
    testing::internal::CaptureStderr();
    auto logger = getGlobalLogger();
    logger->info("This is a test message!");
    std::string out = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(out.find("This is a test message") != out.npos) <<
                        "Output is " << out;
}

