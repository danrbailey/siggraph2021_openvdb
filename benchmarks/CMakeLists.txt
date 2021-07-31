cmake_minimum_required(VERSION 3.12)

find_package(OpenVDB REQUIRED)

add_executable(direct_access direct_access/main.cpp)
target_link_libraries(direct_access OpenVDB::openvdb)

add_executable(for_each for_each/main.cpp)
target_link_libraries(for_each OpenVDB::openvdb)

add_executable(iterator_access iterator_access/main.cpp)
target_link_libraries(iterator_access OpenVDB::openvdb)

add_executable(iterator_range iterator_range/main.cpp)
target_link_libraries(iterator_range OpenVDB::openvdb)

add_executable(root_query root_query/main.cpp)
target_link_libraries(root_query OpenVDB::openvdb)
