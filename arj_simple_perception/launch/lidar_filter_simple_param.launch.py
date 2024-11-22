from launch import LaunchDescription
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    container = ComposableNodeContainer(
        name='lidar_filter_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container_mt',
        composable_node_descriptions=[
            ComposableNode(
                package='arj_simple_perception',
                plugin='LidarFilterSimple',
                name='lidar_filter_simple_param_composable',
                namespace='lidar_filter_ns',
                parameters=[{
                    'minX': 0.0,
                    'minY': -5.0,
                    'minZ': -2.0,
                    'maxX': 40.0,
                    'maxY': 5.0,
                    'maxZ': -0.15,
                    'cloud_topic': '/lexus3/os_center/points',
                    'cloud_frame': ''
                }]
            )
        ],
        output='screen',
    )

    return LaunchDescription([container])