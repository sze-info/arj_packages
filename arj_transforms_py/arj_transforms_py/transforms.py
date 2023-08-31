import rclpy
from rclpy.node import Node
from rclpy.parameter import Parameter
from rcl_interfaces.msg import SetParametersResult

from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformBroadcaster
import math

import numpy as np


def quaternion_from_euler(ai, aj, ak):
    ai /= 2.0
    aj /= 2.0
    ak /= 2.0
    ci = math.cos(ai)
    si = math.sin(ai)
    cj = math.cos(aj)
    sj = math.sin(aj)
    ck = math.cos(ak)
    sk = math.sin(ak)
    cc = ci*ck
    cs = ci*sk
    sc = si*ck
    ss = si*sk

    q = np.empty((4, ))
    q[0] = cj*sc - sj*cs
    q[1] = cj*ss + sj*cc
    q[2] = cj*cs - sj*sc
    q[3] = cj*cc + sj*ss

    return q




class parameter_declaration(Node):

    def __init__(self):
        super().__init__('transforms')
        timer_peroid = 1/40
        self.timer=self.create_timer(timer_peroid, self.timer_callback)
        self.tf_broadcaster=TransformBroadcaster(self)
        

        self.loop_count_=1

        self.declare_parameter('distance1',5.0)
        self.declare_parameter('distance2',2.0)
        self.declare_parameter('speed1',0.1)
        self.declare_parameter('speed2',0.02)

        self.distance1=self.get_parameter('distance1').value
        self.distance2=self.get_parameter('distance2').value
        self.speed1=self.get_parameter('speed1').value
        self.speed2=self.get_parameter('speed2').value

        self.add_on_set_parameters_callback(self.parameters_callback)

    def parameters_callback(self,data):
        success = False
        for parameter in data:
            print(vars(parameter))
            if parameter.name == 'distance1':
                success=True
                self.distance1 = parameter.value
            if parameter.name == 'distance2':
                success=True
                self.distance2 = parameter.value
            if parameter.name == 'speed1':
                success=True
                self.speed1 = parameter.value
            if parameter.name == 'speed2':
                success=True
                self.speed2 = parameter.value

        return SetParametersResult (successful=success)

    def timer_callback(self):
        
        # tf_broadcaster= TransformBroadcaster()
        tr1= TransformStamped()
        tr2= TransformStamped()
        tr1.header.stamp = self.get_clock().now().to_msg()
        tr1.header.frame_id = "map";
        tr1.child_frame_id = "orbit1";
        tr1.transform.translation.x = math.sin(self.loop_count_ * self.speed1) * self.distance1;
        tr1.transform.translation.y = math.cos(self.loop_count_ * self.speed1) * self.distance1;

        q = quaternion_from_euler(0, 0, self.loop_count_ * self.speed1)
        tr1.transform.rotation.x = q[0]
        tr1.transform.rotation.y = q[1]
        tr1.transform.rotation.z = q[2]
        tr1.transform.rotation.w = q[3]
        self.tf_broadcaster.sendTransform(tr1)


        tr2.header.stamp = self.get_clock().now().to_msg()
        tr2.header.frame_id = "orbit1";
        tr2.child_frame_id = "orbit2";
        tr2.transform.translation.x = math.sin(self.loop_count_ * self.speed2) * self.distance2;
        tr2.transform.translation.y = math.cos(self.loop_count_ * self.speed2) * self.distance2;
        self.tf_broadcaster.sendTransform(tr2)

        self.loop_count_ += 1
        




def main(args=None):
    rclpy.init(args=args)
    Node=parameter_declaration()
    rclpy.spin(Node)
    node.destroy_node()
    rclpy.shutdown()

if __name__=='__main__':
    main()




    
