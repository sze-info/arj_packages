#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <math.h>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class CmdGen : public rclcpp::Node
{
public:
    CmdGen() : Node("cmd_gen_node")
    {
        timer_ = this->create_wall_timer(std::chrono::milliseconds(200), std::bind(&CmdGen::loop, this));  
        // /turtle1/cmd_vel plublisher
        cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        RCLCPP_INFO(this->get_logger(), "cmd_gen_node [arj_intro_cpp] has been started");

    }

private:

    void loop()
    {
        // Publish transforms
        auto cmd_msg = geometry_msgs::msg::Twist();
        if (loop_count_ < 20)
        {
            cmd_msg.linear.x = 1.0;
            cmd_msg.angular.z = 0.0;
        }
        else
        {
            cmd_msg.linear.x = -1.0;
            cmd_msg.angular.z = 1.0;
        }
        cmd_pub_->publish(cmd_msg);
        loop_count_++;
        if (loop_count_ > 40)
        {
            loop_count_ = 0;
        }
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    int loop_count_ = 0;

};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CmdGen>());
    rclcpp::shutdown();
    return 0;
}