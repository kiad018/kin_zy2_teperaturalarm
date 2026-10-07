cmake_minimum_required(VERSION 3.8)
project(kin_zy2_teperaturalarm)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(sensor_node src/sensor_node.cpp)
ament_target_dependencies(sensor_node rclcpp sensor_msgs)

add_executable(alarm_node src/alarm_node.cpp)
ament_target_dependencies(alarm_node rclcpp sensor_msgs std_msgs)

install(TARGETS
  sensor_node
  alarm_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()

