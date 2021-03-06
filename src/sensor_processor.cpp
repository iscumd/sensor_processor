#include "../include/sensor_processor/sensor_processor.hpp"

#include <memory>
#include <functional>

namespace SensorProcessor
{
    SensorProcessor::SensorProcessor(rclcpp::NodeOptions options)
        : Node("lidar_processor", options)
    {
        raw_img_raw_subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 10,
            std::bind(&SensorProcessor::raw_img_raw_callback, this, std::placeholders::_1));

        unfiltered_img_raw_publisher_ = this->create_publisher<sensor_msgs::msg::Image>(
            "/camera/unfiltered_image_raw", rclcpp::SensorDataQoS());
            
        raw_imu_subscription_ = this->create_subscription<sensor_msgs::msg::Imu>(
            "/robot/imu", 10,
            std::bind(&SensorProcessor::raw_imu_callback, this, std::placeholders::_1));
            
        imu_raw_publisher_ = this->create_publisher<sensor_msgs::msg::Imu>(
            "/imu", rclcpp::SensorDataQoS());

        raw_odom_subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/robot/odom_raw", 10,
            std::bind(&SensorProcessor::raw_odom_callback, this, std::placeholders::_1));

        odom_raw_publisher_ = this->create_publisher<nav_msgs::msg::Odometry>(
            "/kohm/odom", rclcpp::SensorDataQoS());        
    }

    /// Fixes image raw frames
    void SensorProcessor::raw_img_raw_callback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        msg->header.frame_id = "camera_link";
        unfiltered_img_raw_publisher_->publish(*msg);
    }
     /// Fixes image raw frames
    void SensorProcessor::raw_imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg)
    {
        msg->header.frame_id = "imu_link";
        imu_raw_publisher_->publish(*msg);
    }

    void SensorProcessor::raw_odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        msg->pose.pose.position.z = 0.0;
        msg->twist.twist.linear.z = 0.0;
        odom_raw_publisher_->publish(*msg);
    }
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::executors::SingleThreadedExecutor exec;
    rclcpp::NodeOptions options;
    auto sensor_node = std::make_shared<SensorProcessor::SensorProcessor>(options);
    exec.add_node(sensor_node);
    exec.spin();
    rclcpp::shutdown();
    return 0;
}
