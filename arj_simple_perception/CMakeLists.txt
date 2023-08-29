cmake_minimum_required(VERSION 3.8)
project(arj_simple_perception)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED) 

# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

include_directories(
  include
  SYSTEM
  ${PCL_COMMON_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
)

set(ament_dependencies
  pcl_conversions
  rclcpp
  std_msgs
  sensor_msgs
  nav_msgs
  visualization_msgs
  tf2
  tf2_msgs
  tf2_ros
  tf2_geometry_msgs  
  PCL
)

add_executable(lidar_filter_simple src/lidar_filter_simple.cpp)
target_link_libraries(lidar_filter_simple ${PCL_LIBRARIES})
ament_target_dependencies(lidar_filter_simple  ${ament_dependencies} )


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()
install(TARGETS
  lidar_filter_simple

  DESTINATION lib/${PROJECT_NAME})

ament_package()
