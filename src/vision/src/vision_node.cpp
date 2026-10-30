#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

class VisionNode : public rclcpp::Node {
public:
    VisionNode() : Node("vision_node") {
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/image_raw", 10,
            std::bind(&VisionNode::image_callback, this, std::placeholders::_1)
        );
        RCLCPP_INFO(this->get_node_logger(), "Vision Node (OpenCV) Initialized & Ready!");
    }

private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        try {
            cv::Mat frame = cv_bridge::toCvShare(msg, "bgr8")->image;

            // convert BGR to HSV
            cv::Mat hsv, mask;
            cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

            // color thresholding
            cv::inRange(hsv, cv::Scalar(20, 100, 100), cv::Scalar(30, 255, 255), mask);

            // contour
            cv::Moments m = cv::moments(mask, true);
            if (m.m00 > 0) {
                int cx = static_cast<int>(m.m10 / m.m00);
                int img_center = frame.cols / 2;
                
                int steering_error = cx - img_center;
                
                RCLCPP_INFO_THROTTLE(
                    this->get_node_logger(), *this->get_clock(), 1000,
                    "Line detected! X-Center: %d | Steering Error: %d", cx, steering_error
                );
            } else {
                RCLCPP_WARN_THROTTLE(
                    this->get_node_logger(), *this->get_clock(), 2000,
                    "Line lost! Mencari jalur..."
                );
            }

        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_node_logger(), "cv_bridge exception: %s", e.what());
        }
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VisionNode>());
    rclcpp::shutdown();
    return 0;
}