#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <math.h>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "../inc/trajectoryMarkers.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class LocalPlanner : public rclcpp::Node
{
public:
    LocalPlanner() : Node("local_planner")
    {
        timer_ = this->create_wall_timer(std::chrono::milliseconds(200), std::bind(&LocalPlanner::loop, this));  
        traj_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>("/planner/trajectory", 10);
        cmd_sub_ = this->create_subscription<geometry_msgs::msg::PoseStamped>("/goal_pose", 10,  std::bind(&LocalPlanner::goalpose_callback, this, std::placeholders::_1));
        RCLCPP_INFO(this->get_logger(), "LocalPlanner has been started");
    }

private:

    // goal pose quantities
    float goal_x {0.0f};
    float goal_y {0.0f};
    float goal_theta {0.0f};

    // coefficient
    double coefficientsGoalPose[4];

    // step size
    float stepSize = 0.5; //m

    // trajectory marker
    trajectoryMarker point;

    void goalpose_callback(const geometry_msgs::msg::PoseStamped input_msg)
    {
        goal_x = input_msg.pose.position.x;
        goal_y = input_msg.pose.position.y;

        double quaternion[4];
        quaternion[0] = input_msg.pose.orientation.x;
        quaternion[1] = input_msg.pose.orientation.y;
        quaternion[2] = input_msg.pose.orientation.z;
        quaternion[3] = input_msg.pose.orientation.w;

        // transform quaternion to Euler
        double siny_cosp = 2 * (quaternion[3] * quaternion[2] + quaternion[0] * quaternion[1]);
        double cosy_cosp = 1 - 2 * (quaternion[1] * quaternion[1] + quaternion[2] * quaternion[2]);
        goal_theta = (float)std::atan2(siny_cosp, cosy_cosp);

        RCLCPP_INFO(this->get_logger(), "New goal pose received: %f, %f, %f", goal_x, goal_y, goal_theta);
    }

    void loop()
    {
        // calculate polynomial
        double Ys = (double)goal_y;
        double dYs = tan((double)goal_theta);
        double s = (double)goal_x;
        
        coefficientsGoalPose[0] = 0.0f;
        coefficientsGoalPose[1] = 0.0f;
        coefficientsGoalPose[3] = (Ys - dYs/2*s)/(-0.5f*pow(s,3));
        coefficientsGoalPose[2] = (dYs - 3*coefficientsGoalPose[3]*pow(s,2))/(2*s);

        // Publish pose
        auto pose_msg = visualization_msgs::msg::MarkerArray();
        double x = 0.0;
        int numberOfPoints = round(s/stepSize);
        for (int i=0; i<numberOfPoints; i++)
        {
            double y = coefficientsGoalPose[0] + coefficientsGoalPose[1]*x + 
            coefficientsGoalPose[2]*pow(x,2) + coefficientsGoalPose[3]*pow(x,3);

            //updating pose
            auto pose = geometry_msgs::msg::Pose();
            pose.position.x = x;
            pose.position.y = y;
            point.marker.pose = pose;
            point.marker.id = i;

            pose_msg.markers.push_back(point.marker);

            x = x + stepSize;
        }
    
        traj_pub_->publish(pose_msg);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr traj_pub_;
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr cmd_sub_;

};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LocalPlanner>());
    rclcpp::shutdown();
    return 0;
}