import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import functools
import time

class CmdGen(Node):

    def __init__(self):
        super().__init__('cmd_gen')
        self.timer = self.create_timer(0.2, self.loop)
        self.cmd_pub = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)
        self.loop_count = 0
        self.get_logger().info("cmd_gen_node [arj_intro_py] has been started")

    def loop(self):
        cmd_msg = Twist()
        if self.loop_count < 20:
            cmd_msg.linear.x = 1.0
            cmd_msg.angular.z = 0.0
        else:
            cmd_msg.linear.x = -1.0
            cmd_msg.angular.z = 1.0
        self.cmd_pub.publish(cmd_msg)
        self.loop_count += 1
        if self.loop_count > 40:
            self.loop_count = 0

def main(args=None):
    rclpy.init(args=args)
    cmd_gen = CmdGen()
    rclpy.spin(cmd_gen)
    cmd_gen.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()