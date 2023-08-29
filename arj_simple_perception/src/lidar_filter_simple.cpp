#include <chrono>
#include <functional>
#include <memory>
#include <string>
// ROS
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
// PCL
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/crop_box.h>


using namespace std::chrono_literals;

class LidarFilterSimple : public rclcpp::Node
{
public:
  LidarFilterSimple() : Node("lidar_filter_simple"), count_(0)
  {
    RCLCPP_INFO(this->get_logger(), "LidarFilterSimple node has been started.");
    pub_lidar_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("lidar_filter_output", 10);
    sub_lidar_ = this->create_subscription<sensor_msgs::msg::PointCloud2>("/lexus3/os_center/points", rclcpp::SensorDataQoS().keep_last(1), std::bind(&LidarFilterSimple::lidar_callback, this, std::placeholders::_1));
  }

private:
  void lidar_callback(const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_msg)
  {
    //RCLCPP_INFO(this->get_logger(), "frame_id: '%s'", input_msg->header.frame_id.c_str());
    
    // Define min and max for X, Y and Z (crop box - rectangular estimation of non-ground points in front of the car)
    float minX = 0.0, minY = -5.0, minZ = -2.0;
    float maxX = 40.0, maxY = +5.0, maxZ = -0.15;
    // Filter point cloud data
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
    pcl::fromROSMsg(*input_msg, *cloud);
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZI>);
    pcl::CropBox<pcl::PointXYZI> crop;
    crop.setInputCloud(cloud);
    // Filter out points outside of the box
    crop.setMin(Eigen::Vector4f(minX, minY, minZ, 1.0));
    crop.setMax(Eigen::Vector4f(maxX, maxY, maxZ, 1.0));
    crop.filter(*cloud_filtered);
    // Convert to ROS data type
    sensor_msgs::msg::PointCloud2 output_msg;
    pcl::toROSMsg(*cloud_filtered, output_msg);
    // Add the same frame_id as th input, it is not included in pcl PointXYZI
    output_msg.header.frame_id = input_msg->header.frame_id;
    // Publish the data as a ROS message
    pub_lidar_->publish(output_msg);
  }

  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_lidar_;
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_lidar_;
  size_t count_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LidarFilterSimple>());
  rclcpp::shutdown();
  return 0;
}