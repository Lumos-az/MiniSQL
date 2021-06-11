cmake_minimum_required(VERSION 3.17)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(MiniSQL
        src/CatalogManager.cpp
        src/CatalogManager.h
        src/Interpreter.cpp
        src/Interpreter.h
        src/test.cpp
        src/Condition.cpp
        src/Condition.h
        src/BufferManager.cpp
        src/BufferManager.h
        src/RecordManager.cpp
        src/RecordManager.h
        src/API.cpp src/API.h)
