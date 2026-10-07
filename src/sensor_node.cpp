#include <chrono>
#include <memory>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/temperature.hpp"
#include "sensor_msgs/msg/relative_humidity.hpp"

using namespace std::chrono_literals;

class SensorNode : public rclcpp::Node {
public:
  SensorNode() : Node("sensor_node"), temp_(24.0), humidity_(0.50), gen_(rd_()), dist_(-1.0, 1.2) {
    temp_pub_ = this->create_publisher<sensor_msgs::msg::Temperature>("/sensor/temperature", 10);
    hum_pub_ = this->create_publisher<sensor_msgs::msg::RelativeHumidity>("/sensor/humidity", 10);
    timer_ = this->create_wall_timer(1000ms, std::bind(&SensorNode::publish_data, this));
    RCLCPP_INFO(this->get_logger(), "SensorNode elindult, adatok kuldese...");
  }

private:
  void publish_data() {
    temp_ += dist_(gen_);
    humidity_ += (dist_(gen_) * 0.02);
    if (humidity_ < 0.1) humidity_ = 0.1;
    if (humidity_ > 0.95) humidity_ = 0.95;

    auto now = this->now();

    sensor_msgs::msg::Temperature temp_msg;
    temp_msg.header.stamp = now;
    temp_msg.header.frame_id = "temp_sensor";
    temp_msg.temperature = temp_;
    temp_pub_->publish(temp_msg);

    sensor_msgs::msg::RelativeHumidity hum_msg;
    hum_msg.header.stamp = now;
    hum_msg.header.frame_id = "humidity_sensor";
    hum_msg.relative_humidity = humidity_;
    hum_pub_->publish(hum_msg);

    RCLCPP_INFO(this->get_logger(), "Mert ertekek: %.1f C | %.1f %%", temp_, humidity_ * 100.0);
  }

  rclcpp::Publisher<sensor_msgs::msg::Temperature>::SharedPtr temp_pub_;
  rclcpp::Publisher<sensor_msgs::msg::RelativeHumidity>::SharedPtr hum_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  double temp_;
  double humidity_;
  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_real_distribution<> dist_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SensorNode>());
  rclcpp::shutdown();
  return 0;
}
