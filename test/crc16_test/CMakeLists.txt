cmake_minimum_required(VERSION 3.20)

project(crc16test)

include_directories(../../inc)

add_executable(${PROJECT_NAME} 
../../src/crc16.c 
crc16_test.c)

message("finish")