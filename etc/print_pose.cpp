// ros2 topic type /lexus3/gps/duro/current_pose
// geometry_msgs/msg/PoseStamped
// ros2 interface show geometry_msgs/msg/PoseStamped

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

using std::placeholders::_1;

class SimplePoseSub : public rclcpp::Node
{
public:
  SimplePoseSub() : Node("simple_pose_sub")
  {
    sub1_ = this->create_subscription<geometry_msgs::msg::PoseStamped>("/lexus3/gps/duro/current_pose", 10, std::bind(&SimplePoseSub::topic_callback, this, _1));
  }

private:
  void topic_callback(const geometry_msgs::msg::PoseStamped &msg) const
  {
    RCLCPP_INFO(this->get_logger(), "x: %.3f, y: %.3f", msg.pose.position.x, msg.pose.position.y);
  }
  rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr sub1_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SimplePoseSub>());
  rclcpp::shutdown();
  return 0;
}