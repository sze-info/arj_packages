#include <chrono>
#include <functional>
#include <memory>
#include <string>
// ROS
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "rcl_interfaces/msg/set_parameters_result.hpp"
// PCL
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/crop_box.h>

using namespace std::chrono_literals;
using std::placeholders::_1;

class LidarFilterSimple : public rclcpp::Node
{
  rcl_interfaces::msg::SetParametersResult parametersCallback(const std::vector<rclcpp::Parameter> &parameters)
  {
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    result.reason = "success";
    for (const auto &param : parameters)
    {
      RCLCPP_INFO_STREAM(this->get_logger(), "Param update: " << param.get_name().c_str() << ": " << param.value_to_string().c_str());
      if (param.get_name() == "minX")
      {
        minX = param.as_double();
      }
      if (param.get_name() == "minY")
      {
        minY = param.as_double();
      }
      if (param.get_name() == "minZ")
      {
        minZ = param.as_double();
      }
      if (param.get_name() == "maxX")
      {
        maxX = param.as_double();
      }
      if (param.get_name() == "maxY")
      {
        maxY = param.as_double();
      }
      if (param.get_name() == "maxZ")
      {
        maxZ = param.as_double();
      }
      if (param.get_name() == "cloud_topic")
      {
        cloud_topic = param.as_string();
        sub_lidar_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(cloud_topic, rclcpp::SensorDataQoS().keep_last(1), std::bind(&LidarFilterSimple::lidar_callback, this, std::placeholders::_1));
      }
      if (param.get_name() == "cloud_frame")
      {
        cloud_frame = param.as_string();
      }
    }
    return result;
  }

public:
  LidarFilterSimple() : Node("lidar_filter_simple_param"), count_(0)
  {
    this->declare_parameter<float>("minX", minX); 
    this->declare_parameter<float>("minY", minY);
    this->declare_parameter<float>("minZ", minZ);
    this->declare_parameter<float>("maxX", maxX);
    this->declare_parameter<float>("maxY", maxY);
    this->declare_parameter<float>("maxZ", maxZ);
    this->declare_parameter<std::string>("cloud_topic", cloud_topic);
    this->declare_parameter<std::string>("cloud_frame", "");
    this->get_parameter("minX", minX);
    this->get_parameter("minY", minY);
    this->get_parameter("minZ", minZ);
    this->get_parameter("maxX", maxX);
    this->get_parameter("maxY", maxY);
    this->get_parameter("maxZ", maxZ);
    this->get_parameter("cloud_topic", cloud_topic);
    this->get_parameter("cloud_frame", cloud_frame);


    RCLCPP_INFO(this->get_logger(), "LidarFilterSimple node has been started.");
    pub_lidar_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("lidar_filter_output", 10);
    sub_lidar_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(cloud_topic, rclcpp::SensorDataQoS().keep_last(1), std::bind(&LidarFilterSimple::lidar_callback, this, std::placeholders::_1));
    callback_handle_ = this->add_on_set_parameters_callback(std::bind(&LidarFilterSimple::parametersCallback, this, std::placeholders::_1));
  }

private:
  void lidar_callback(const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_msg)
  {
    // Print info, only once
    RCLCPP_INFO_ONCE(this->get_logger(), "Filtering from cloud_topic: '%s'", cloud_topic.c_str());

    // Define min and max for X, Y and Z (crop box - rectangular estimation of non-ground points in front of the car)

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
    // Add the same frame_id as the input, if empty string, add the input frame_id
    if (cloud_frame != "")
    {
      output_msg.header.frame_id = cloud_frame;
    }
    else
    {
      output_msg.header.frame_id = input_msg->header.frame_id;
    }
    // Print info, only once
    RCLCPP_INFO_ONCE(this->get_logger(), "Publishing filtered point cloud data, frame_id: '%s'", output_msg.header.frame_id.c_str());
    // Publish the data as a ROS message
    pub_lidar_->publish(output_msg);
  }

  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_lidar_;
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_lidar_;
  OnSetParametersCallbackHandle::SharedPtr callback_handle_;
  float minX = 0.0, minY = -5.0, minZ = -2.0;
  float maxX = 40.0, maxY = +5.0, maxZ = -0.15;
  std::string cloud_topic = "/lexus3/os_center/points", cloud_frame = "";  
  size_t count_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LidarFilterSimple>());
  rclcpp::shutdown();
  return 0;
}