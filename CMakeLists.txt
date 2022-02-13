cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(Carthage)
set(CMAKE_CXX_STANDARD 17)

set(BUFFERS
	basic_buffers/Buffer.cpp
	basic_buffers/Buffer.hpp
	basic_buffers/Buffer.tpp
	basic_buffers/Extension.hpp
	basic_buffers/Extension.cpp
	basic_buffers/ID.hpp
	basic_buffers/ID.cpp
	basic_buffers/LUT_chars.h
	basic_buffers/LUT_IDs.h
	basic_buffers/Name.cpp
	basic_buffers/Name.hpp
	basic_buffers/StackSegment.hpp
	basic_buffers/StackSegment.cpp
	basic_buffers/StackSegment.tpp
	basic_buffers/SystemName.hpp
	basic_buffers/SystemName.cpp
	basic_buffers/main.cpp
)

add_executable(${PROJECT_NAME}
	${BUFFERS})