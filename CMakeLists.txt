cmake_minimum_required(VERSION 3.26)
project(Project3VehicleEmissions)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project3VehicleEmissions main.cpp
        registry.h
        RBTree.h
        RBTree.cpp
        Splaytree.h
        EPAGrid.h)
