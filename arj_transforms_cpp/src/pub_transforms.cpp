#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <math.h>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_array.hpp"
#include "tf2_ros/buffer.h"
#include "tf2/exceptions.h"
#include "tf2_ros/transform_broadcaster.h"
#include "rcl_interfaces/msg/set_parameters_result.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class PubTransforms : public rclcpp::Node
{

    rcl_interfaces::msg::SetParametersResult parametersCallback(const std::vector<rclcpp::Parameter> &parameters)
    {
        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        result.reason = "success";
        for (const auto &param : parameters)
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Param update: " << param.get_name().c_str() << ": " << param.value_to_string().c_str());
            if (param.get_name() == "distance1")
            {
                distance1 = param.as_double();
            }
            if (param.get_name() == "distance2")
            {
                distance2 = param.as_double();
            }           
            if (param.get_name() == "speed1")
            {
                speed1 = param.as_double();
            }
            if (param.get_name() == "speed2")
            {
                speed2 = param.as_double();
            }
        }
        return result;
    }

public:
    PubTransforms() : Node("pub_tf_node")
    {

        this->declare_parameter<float>("distance1", distance1);
        this->declare_parameter<float>("distance2", distance2);
        this->declare_parameter<float>("speed1", speed1);
        this->declare_parameter<float>("speed2", speed2);

        this->get_parameter("distance1", distance1);
        this->get_parameter("distance2", distance2);
        this->get_parameter("speed1", speed1);
        this->get_parameter("speed2", speed2);

        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        timer_ = this->create_wall_timer(std::chrono::milliseconds(50), std::bind(&PubTransforms::loop, this));   
        callback_handle_ = this->add_on_set_parameters_callback(std::bind(&PubTransforms::parametersCallback, this, std::placeholders::_1));     
    }

private:

    void loop()
    {
        // Publish transforms
        tr1.header.stamp = this->get_clock()->now();
        tr1.header.frame_id = "map";
        tr1.child_frame_id = "orbit1";
        tr1.transform.translation.x = sin(loop_count_ * speed1) * distance1;
        tr1.transform.translation.y = cos(loop_count_ * speed1) * distance1;
        tf2::Quaternion quaternion1;
        quaternion1.setRPY(0.0, 0.0, loop_count_ * speed1);
        quaternion1=quaternion1.normalize();
        tr1.transform.rotation.x = quaternion1.x();
        tr1.transform.rotation.y = quaternion1.y();
        tr1.transform.rotation.z = quaternion1.z();
        tr1.transform.rotation.w = quaternion1.w();
        tf_broadcaster_->sendTransform(tr1);
        tr2.header.stamp = this->get_clock()->now();
        tr2.header.frame_id = "orbit1";
        tr2.child_frame_id = "orbit2";
        tr2.transform.translation.x = sin(loop_count_ * speed2) * distance2;
        tr2.transform.translation.y = cos(loop_count_ * speed2) * distance2;
        tf_broadcaster_->sendTransform(tr2);
        loop_count_++;
    }
    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    geometry_msgs::msg::TransformStamped tr1, tr2;
    float distance1 = 5.0, distance2 = 2.5; // distance in meters
    float speed1 = 0.1, speed2 = 0.02; // speed 
    int loop_count_ = 0;
    OnSetParametersCallbackHandle::SharedPtr callback_handle_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PubTransforms>());
    rclcpp::shutdown();
    return 0;
}