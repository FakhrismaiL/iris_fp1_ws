#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

class ControlNode : public rclcpp::Node {
public:
    ControlNode() : Node("control_node") {
        cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/catvehicle/cmd_vel", 10);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(50), std::bind(&ControlNode::control_loop, this));
        RCLCPP_INFO(this->get_logger(), "Control Node Initialized & Connected to /cmd_vel!");
    }

private:
    void control_loop() {
        auto twist_msg = geometry_msgs::msg::Twist();
        
        twist_msg.linear.x = 1.5; 
        twist_msg.angular.z = 0.0;

        cmd_pub_->publish(twist_msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ControlNode>());
    rclcpp::shutdown();
    return 0;
}