cmake_minimum_required(VERSION 3.5)

project(Examples_14)

find_package(Threads)



add_executable(cas_1 cas_1.cpp)
set_target_properties(cas_1 PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
target_link_libraries(cas_1 ${CMAKE_THREAD_LIBS_INIT})

add_executable(spin_lock spin_lock.cpp)
set_target_properties(spin_lock PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
target_link_libraries(spin_lock ${CMAKE_THREAD_LIBS_INIT})

add_executable(cas_2 cas_2.cpp)
set_target_properties(cas_2 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(cas_2 ${CMAKE_THREAD_LIBS_INIT})

add_executable(lock_free lock_free.cpp)
set_target_properties(lock_free PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
target_link_libraries(lock_free ${CMAKE_THREAD_LIBS_INIT})

add_executable(singleton_1 singleton_1.cpp)
set_target_properties(singleton_1 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(singleton_1 ${CMAKE_THREAD_LIBS_INIT})

add_executable(singleton_2 singleton_2.cpp)
set_target_properties(singleton_2 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(singleton_2 ${CMAKE_THREAD_LIBS_INIT})

add_executable(singleton_3 singleton_3.cpp)
set_target_properties(singleton_3 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(singleton_3 ${CMAKE_THREAD_LIBS_INIT})

add_executable(singleton_4 singleton_4.cpp)
set_target_properties(singleton_4 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(singleton_4 ${CMAKE_THREAD_LIBS_INIT})
