from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from launch.actions import DeclareLaunchArgument


# This is the launch file for the filter node, run this file e.g. with:
# ros2 launch arj_simple_perception filter_a.launch.py  
#  cloud_topic:=/lexus3/os_center/points minX:=0.0 maxX:=40.0 minZ:=-2.0 maxZ:=-0.15

def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time', default='true') # if simulation or ros2 bag (mcap) is used, this should be true
    pkg_name = 'arj_simple_perception'


    return LaunchDescription([
        DeclareLaunchArgument('cloud_topic', default_value="/lexus3/os_center/points", description="a pointcloud topic to process",),
        DeclareLaunchArgument('cloud_frame', default_value="lexus3/os_center_a_laser_data_frame", description="a pointcloud topic to process, default is the topic's own frame",),
        DeclareLaunchArgument('minX', default_value="0.0", description="minimum x value to filter",),
        DeclareLaunchArgument('maxX', default_value="40.0", description="maximum x value to filter",),
        DeclareLaunchArgument('maxY', default_value="+5.0", description="maximum y value to filter",),
        DeclareLaunchArgument('minY', default_value="-5.0", description="minimum y value to filter",),
        DeclareLaunchArgument('minZ', default_value="-2.0", description="minimum z value to filter",),
        DeclareLaunchArgument('maxZ', default_value="-0.15", description="maximum z value to filter",),
        Node(
            package=pkg_name,
            executable='lidar_filter_simple_param',
            output='screen',
            parameters=[
                {'cloud_topic': LaunchConfiguration("cloud_topic")},
                {'cloud_frame': LaunchConfiguration("cloud_frame")},
                {'minX': LaunchConfiguration("minX")},
                {'minY': LaunchConfiguration("minY")},
                {'minZ': LaunchConfiguration("minZ")},
                {'maxX': LaunchConfiguration("maxX")},
                {'maxY': LaunchConfiguration("maxY")},
                {'maxZ': LaunchConfiguration("maxZ")},
                {'use_sim_time': use_sim_time},
            ],
        ),
    ])
