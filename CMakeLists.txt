cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(Carthage LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(BUFFERS_SRC
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
)

set(DYNAMIC_BUFFERS_SRC
	dynamic_buffers/InputBuffer.hpp
	dynamic_buffers/InputBuffer.tpp
	dynamic_buffers/InputBuffer.cpp
	dynamic_buffers/OutputBuffer.hpp
	dynamic_buffers/OutputBuffer.tpp
	dynamic_buffers/OutputBuffer.cpp
)

set(VFS_SRC
	VFS/FSBlock.cpp
	VFS/FSBlock.hpp
	VFS/FSObject.hpp
	VFS/FSObject.cpp
	VFS/FSType.hpp
	VFS/utils.cpp
	VFS/utils.h
	VFS/File.hpp 
	VFS/File.cpp 
	VFS/Folder.hpp 
	VFS/Folder.cpp 
	VFS/Versionable.hpp 
	VFS/Versionable.cpp 
	VFS/Container.hpp
	VFS/Container.cpp 
	VFS/FileSystem.hpp 
	VFS/FileSystem.cpp
	VFS/arguments.hpp
	VFS/arguments.cpp
	VFS/arguments.tpp
	VFS/arg_blocks.hpp
	VFS/arg_blocks.cpp
	VFS/VFS_IO.hpp
	VFS/VFS_IO.cpp
	VFS/VFS_IO.tpp
)

set(ROOT_SRC
	main.cpp
)

# = = = = BUFFERS = = = = 

add_library(BUFFERS_LIB
	${BUFFERS_SRC}
)

target_include_directories(BUFFERS_LIB 
	PUBLIC basic_buffers
)

add_executable(Buffers_test
	${BUFFERS_SRC}
	basic_buffers/main.cpp
)

# = = = = DYNAMIC BUFFERS = = = = 

add_library(DYNAMIC_BUFFERS_LIB
	${DYNAMIC_BUFFERS_SRC}
)

target_include_directories(DYNAMIC_BUFFERS_LIB 
	PUBLIC basic_buffers
	PUBLIC dynamic_buffers
)

add_executable(Dynamic_buffers_test
	${DYNAMIC_BUFFERS_SRC}
	dynamic_buffers/main.cpp
)

target_link_libraries(Dynamic_buffers_test
	PUBLIC BUFFERS_LIB
)

# = = = = VFS = = = = 

add_library(VFS_LIB
	${VFS_SRC}
)

target_include_directories(VFS_LIB 
	PUBLIC basic_buffers
	PUBLIC dynamic_buffers
	PUBLIC VFS
)

add_executable(VFS_test
	${VFS_SRC}
	VFS/main.cpp
)

target_link_libraries(VFS_test
	PUBLIC BUFFERS_LIB
	PUBLIC DYNAMIC_BUFFERS_LIB
)

# = = = = PROJECT = = = = 

add_executable(${PROJECT_NAME}
	${ROOT_SRC}
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC basic_buffers
	PUBLIC dynamic_buffers
	PUBLIC VFS
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC BUFFERS_LIB
	PUBLIC DYNAMIC_BUFFERS_LIB
	PUBLIC VFS_LIB
)
