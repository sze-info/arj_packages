from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription(
        [
            Node(
                package='foxglove_bridge',
                executable='foxglove_bridge',
                parameters=[
                    {'port': 8765},
                    {'address': '0.0.0.0'},
                    {'tls': False},
                    {'certfile': ''},
                    {'keyfile': ''},
                    # {'topic_whitelist': "'.*'"},
                    {'max_qos_depth': 10},
                    {'num_threads': 0},
                    {'use_sim_time': False},
                ]
            ),
            Node(
                package='arj_intro_py',
                executable='cmd_gen_node',
            ),
            Node(
                package='turtlesim',
                executable='turtlesim_node',
            ),
        ]
    )
