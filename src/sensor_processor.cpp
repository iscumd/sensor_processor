#include "../include/sensor_processor/sensor_processor.hpp"

#include <memory>
#include <functional>
#include "sensor_msgs/msg/image.h"

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
    }

    /// Fixes camera link
    void SensorProcessor::raw_img_raw_callback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        msg->header.frame_id = "camera_link";
        unfiltered_img_raw_publisher_->publish(*msg);
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