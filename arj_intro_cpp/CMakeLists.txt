cmake_minimum_required(VERSION 3.8)
project(arj_intro_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

set(ament_dependencies
  rclcpp
  geometry_msgs
)

add_executable(cmd_gen_node src/cmd_gen_node.cpp)
ament_target_dependencies(cmd_gen_node  ${ament_dependencies})

install(TARGETS cmd_gen_node
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY 
  launch
  DESTINATION share/${PROJECT_NAME})

ament_package()
