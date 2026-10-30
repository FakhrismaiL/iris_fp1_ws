#include <chrono>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

struct __attribute__((packed)) ControlPacket {
    char header[4] = {'I', 'R', 'I', 'S'};
    int32_t motor_pwm = 0;       // Range: -255 s.d. 255
    int32_t steering_degree = 0; // Range: -90 s.d. 90
};

class UdpSenderNode : public rclcpp::Node {
public:
    UdpSenderNode() : Node("udp_sender_node") {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            RCLCPP_ERROR(this->get_node_logger(), "Failed to create UDP socket");
        }

        dest_addr_.sin_family = AF_INET;
        dest_addr_.sin_port = htons(12151);
        inet_pton(AF_INET, "127.0.0.1", &dest_addr_.sin_addr);

        //timer
        timer_ = this->create_wall_timer(
            50ms, std::bind(&UdpSenderNode::send_packet, this));
        
        RCLCPP_INFO(this->get_node_logger(), "UDP Sender Node Initialized & Running");
    }

    ~UdpSenderNode() {
        if (sock_ >= 0) close(sock_);
    }

private:
    void send_packet() {
        ControlPacket packet;
        packet.motor_pwm = 100;     // velocity
        packet.steering_degree = 0; 

        ssize_t sent_bytes = sendto(
            sock_, &packet, sizeof(packet), 0,
            (struct sockaddr*)&dest_addr_, sizeof(dest_addr_)
        );

        if (sent_bytes < 0) {
            RCLCPP_WARN(this->get_node_logger(), "Failed to send UDP packet");
        }
    }

    int sock_;
    sockaddr_in dest_addr_;
    rclcpp::TimerBase::timer_ptr_ timer_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<UdpSenderNode>());
    rclcpp::shutdown();
    return 0;
}