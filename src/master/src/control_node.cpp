#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

class ControlNode : public rclcpp::Node {
public:
    ControlNode() : Node("control_node") {
        motor_pub_ = this->create_publisher<std_msgs::msg::Int32>("/control/motor_pwm", 10);
        steering_pub_ = this->create_publisher<std_msgs::msg::Int32>("/control/steering_degree", 10);

        timer_ = this->create_wall_timer(
            50ms, std::bind(&ControlNode::control_loop, this));
        
        RCLCPP_INFO(this->get_node_logger(), "Control & Kinematics Node Initialized!");
    }

private:
    void control_loop() {
        std_msgs::msg::Int32 motor_msg;
        std_msgs::msg::Int32 steering_msg;

        motor_msg.data = 120;
        steering_msg.data = 0;

        motor_pub_->publish(motor_msg);
        steering_pub_->publish(steering_msg);
    }

    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr motor_pub_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr steering_pub_;
    rclcpp::TimerBase::timer_ptr_ timer_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ControlNode>());
    rclcpp::shutdown();
    return 0;
}