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

class VehicleModel : public rclcpp::Node
{
public:
    VehicleModel() : Node("vehicle_model")
    {
        timer_ = this->create_wall_timer(std::chrono::milliseconds(200), std::bind(&VehicleModel::loop, this));  
        state_pub_ = this->create_publisher<std_msgs::msg::Float32MultiArray>("/vehicle/state", 10);
        cmd_sub_ = this->create_subscription<std_msgs::msg::Float32>("/vehicle/propulsion", 10,  std::bind(&VehicleModel::propulsion_callback, this, std::placeholders::_1));
        RCLCPP_INFO(this->get_logger(), "vehicle_model has been started");
    }

private:
    // input command
    float Fprop {0.0f};

    // vehicle state array
    std::vector<float> state; //speed, acceleration
    float vx{0.0f};
    float ax{0.0f};

    // load
    float Fload{0.0f};

    // params
    float m {1350.0}; // kg
    float A {1.5f}; // m^2
    float rho {1.0f}; // kg/m^3
    float c {0.33f}; // aerodynamic factor
    float b {0.1f}; // rolling friction, viscosous

    void propulsion_callback(const std_msgs::msg::Float32 input_msg)
    {
        Fprop = input_msg.data;
    }

    void loop()
    {
        // calculate new state based on load, prop force, mass and aerodynamic drag
        float Faero = 0.5*A*rho*c*pow(vx,2);
        float Ffric = b*vx;
        ax = (Fprop - Ffric - Fload - Faero)/m;
        vx = std::max(0.0f, vx + ax*0.1f); // 0.1s is the time step of the model

        // Publish state
        state.clear();
        std_msgs::msg::Float32MultiArray state_msg;
        state.push_back(vx); // m/s
        state.push_back(ax); // m/s^2

        state_msg.data = state;
        state_pub_->publish(state_msg);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr state_pub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr cmd_sub_;

};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VehicleModel>());
    rclcpp::shutdown();
    return 0;
}