#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <math.h>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include "std_msgs/msg/float32.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class SpeedControl : public rclcpp::Node
{
public:
    SpeedControl() : Node("speed_control")
    {
        timer_ = this->create_wall_timer(std::chrono::milliseconds(200), std::bind(&SpeedControl::loop, this));  

        cmd_pub_ = this->create_publisher<std_msgs::msg::Float32>("/vehicle/propulsion", 10);
        state_sub_ = this->create_subscription<std_msgs::msg::Float32MultiArray>("/vehicle/state", 10, std::bind(&SpeedControl::state_callback, this, std::placeholders::_1));
        cmd_sub_ = this->create_subscription<std_msgs::msg::Float32>("/vehicle/cmd", 10, std::bind(&SpeedControl::cmd_callback, this, std::placeholders::_1));

        this->declare_parameter("/control/P", 100.0f);
        this->declare_parameter("/control/I", 5.0f);
        this->declare_parameter("/control/D", 10.0f);

        RCLCPP_INFO(this->get_logger(), "speed_control has been started");
    }

private:
    // output command
    float Fprop {0.0f};

    // vehicle state
    float vx{0.0f};
    float ax{0.0f};

    // params
    float P {10.0f};
    float I {0.0f};
    float D {0.0f};
    float Fprop_max {12000.0f}; // N

    // set speed
    float vSet{0.0f};

    // control quantities
    float error {0.0f};
    float integrated_error {0.0f};

    void state_callback(const std_msgs::msg::Float32MultiArray input_msg)
    {
        vx = input_msg.data[0];
        ax = input_msg.data[1];
    }

    void cmd_callback(const std_msgs::msg::Float32 input_msg)
    {
        vSet = input_msg.data;
    }

    void loop()
    {
        P = (float)this->get_parameter("/control/P").as_double();
        I = (float)this->get_parameter("/control/I").as_double();
        D = (float)this->get_parameter("/control/D").as_double();
        // calculate new state based on load, prop force, mass and aerodynamic drag
        float Fprop_D = D*((vSet-vx)-error)/0.1;

        float error = vSet-vx;
        float Fprop_P = P*error;
        float Fprop_I = I*integrated_error;

        Fprop = Fprop_P+Fprop_I+Fprop_D;

        Fprop = std::min(std::max(-Fprop_max, Fprop), Fprop_max);

        // Publish cmd
        std_msgs::msg::Float32 cmd_msg;
        cmd_msg.data = Fprop;

        cmd_pub_->publish(cmd_msg);

        integrated_error+= error*0.1f;
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr cmd_pub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr cmd_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr state_sub_;

};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SpeedControl>());
    rclcpp::shutdown();
    return 0;
}