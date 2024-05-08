cmake_minimum_required(VERSION 3.8)
project(hk_cam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(OpenCV REQUIRED)


set(JETSON OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message( "Configuring on/for Linux")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      message("Configuring on/for Linux  x86_64")
      set(JETSON OFF)
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
      message("Configuring on/for Linux  aarch64")
      set(JETSON ON)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message( "Configuring on/for macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message( "Configuring on/for Windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "AIX")
    message( "Configuring on/for IBM AIX")
else()
    message( "Configuring on/for ${CMAKE_SYSTEM_NAME}")
endif()


include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(
        hk_lib/include/
)

# 添加可运行程序
add_executable(hk_cam src/hk_cam.cpp)

# 定义包位置
if(JETSON)
  file(GLOB libs "hk_lib/linux/lib*")
  message("Configuring on/for Linux  aarch64")
else()
  file(GLOB libs "hk_lib/linux64/lib*")
  message("Configuring on/for Linux  x86_64")
endif()


# message(STATUS ${libs})

target_link_libraries(hk_cam ${libs} ${OpenCV_LIBS})


target_include_directories(hk_cam PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)


#target_link_libraries(hk_cam PRIVATE ${libs} ${OpenCV_LIBS})

target_compile_features(hk_cam PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

ament_target_dependencies(
  hk_cam
  "rclcpp"
  "std_msgs"
  "sensor_msgs"
)

install(TARGETS hk_cam
  DESTINATION lib/${PROJECT_NAME})

set(HK_LIB hk_lib)

if(JETSON)
  install(DIRECTORY ${HK_LIB}/linux/
          DESTINATION lib/
  )
  message("Configuring on/for Linux  aarch64")
else()
  install(DIRECTORY ${HK_LIB}/linux64/
          DESTINATION lib/
  )
  message("Configuring on/for Linux  x86_64")
endif()



if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()



