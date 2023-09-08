from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time', default='true') # if simulation or ros2 bag (mcap) is used, this should be true
    pkg_name = 'arj_simple_perception'
    pkg_dir = get_package_share_directory(pkg_name)


    return LaunchDescription([
        Node(
            package='arj_simple_perception',
            executable='lidar_filter_simple_param',
            parameters=[{'use_sim_time': use_sim_time}],
        ),
        Node(
            package='rviz2',
            namespace='',
            executable='rviz2',
            parameters=[{'use_sim_time': use_sim_time}],
            arguments=['-d', [os.path.join(pkg_dir, 'config', 'rviz_config1.rviz')]]
        ),
        Node(
            package='rqt_reconfigure',
            namespace='',
            parameters=[{'use_sim_time': use_sim_time}],
            executable='rqt_reconfigure',
        ),
    ])