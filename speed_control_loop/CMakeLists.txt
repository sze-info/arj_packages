cmake_minimum_required(VERSION 3.8)
project(speed_control_loop)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)

set(ament_dependencies
  rclcpp
  geometry_msgs
  std_msgs
)

add_executable(vehicle_model src/vehicle_model.cpp)

add_executable(speed_control src/speed_control.cpp)

ament_target_dependencies(vehicle_model  ${ament_dependencies})
ament_target_dependencies(speed_control  ${ament_dependencies})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

install(TARGETS vehicle_model
  DESTINATION lib/${PROJECT_NAME})

install(TARGETS speed_control
  DESTINATION lib/${PROJECT_NAME})

ament_package()
