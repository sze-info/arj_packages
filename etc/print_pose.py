# ros2 topic type /lexus3/gps/duro/current_pose
# geometry_msgs/msg/PoseStamped
# ros2 interface show geometry_msgs/msg/PoseStamped

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped

class SimplePoseSub(Node):

    def __init__(self):
        super().__init__('simple_pose_sub')
        self.sub1_ = self.create_subscription(PoseStamped, '/lexus3/gps/duro/current_pose', self.topic_callback, 10)

    def topic_callback(self, msg):
        self.get_logger().info('x: %.3f, y: %.3f', msg.pose.position.x, msg.pose.position.y)

def main(args=None):
    rclpy.init(args=args)
    simple_pose_sub = SimplePoseSub()
    rclpy.spin(simple_pose_sub)
    simple_pose_sub.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()