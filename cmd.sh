ros2 pkg create --build-type ament_cmake --node-name hk_cam hk_cam --dependencies rclcpp

ros2 pkg create --build-type ament_python hk_cam_slave --node-name hk_cam_slave --dependencies rclpy



ros2 pkg create hk_interfaces --build-type ament_cmake 

colcon build --symlink-install






ros2 pkg create example_ros2_interfaces --build-type ament_cmake --dependencies rosidl_default_generators geometry_msgs


colcon build --symlink-install --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON



sudo systemctl start cyberdog_my_run_launch.service             手动开启  
sudo systemctl stop cyberdog_my_run_launch.service
journalctl -u cyberdog_my_run_launch.service -f


systemctl restart cyberdog_bringup.service



colcon build --packages-select hk_cam
ros2 run hk_cam hk_cam




# This message contains an uncompressed image
# (0, 0) is at top-left corner of image

std_msgs/Header header # Header timestamp should be acquisition time of image
        builtin_interfaces/Time stamp
                int32 sec
                uint32 nanosec
        string frame_id
                             # Header frame_id should be optical frame of camera
                             # origin of frame should be optical center of cameara
                             # +x should point to the right in the image
                             # +y should point down in the image
                             # +z should point into to plane of the image
                             # If the frame_id here and the frame_id of the CameraInfo
                             # message associated with the image conflict
                             # the behavior is undefined

uint32 height                # image height, that is, number of rows
uint32 width                 # image width, that is, number of columns

# The legal values for encoding are in file src/image_encodings.cpp
# If you want to standardize a new string format, join
# ros-users@lists.ros.org and send an email proposing a new encoding.

string encoding       # Encoding of pixels -- channel meaning, ordering, size
                      # taken from the list of strings in include/sensor_msgs/image_encodings.hpp

uint8 is_bigendian    # is this data bigendian?
uint32 step           # Full row length in bytes
uint8[] data          # actual matrix data, size is (step * rows)



{
    "map_name": "506",
    "is_outdoor": false,
    "标签名称1": {
        "x": 0.6342622637748718,
        "y": 4.866499900817871
    },
    "标签名称2": {
        "x": 10.200108528137207,
        "y": 4.774724960327148
    },
    "标签名称3": {
        "x": 9.695706367492676,
        "y": 8.347606658935547
    },
    "标签名称4": {
        "x": 0.7605808973312378,
        "y": 7.439979076385498
    },
    "标签名称5": {
        "x": 0.9713168740272522,
        "y": 1.3004175424575806
    },
    "标签名称6": {
        "x": 5.85131311416626,
        "y": 8.133259773254395
    },
    "标签名称7": {
        "x": 6.579171180725098,
        "y": 4.393634796142578
    }
}