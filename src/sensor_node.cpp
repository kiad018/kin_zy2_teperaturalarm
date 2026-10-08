#include <chrono>
#include <cmath>
#include <memory>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/temperature.hpp"
#include "sensor_msgs/msg/relative_humidity.hpp"

using namespace std::chrono_literals;

class SensorNode : public rclcpp::Node {
public:
  SensorNode() : Node("sensor_node"), step_(0.0), gen_(rd_()), noise_(-0.15, 0.15) {
    temp_pub_ = this->create_publisher<sensor_msgs::msg::Temperature>("/sensor/temperature", 10);
    hum_pub_ = this->create_publisher<sensor_msgs::msg::RelativeHumidity>("/sensor/humidity", 10);

    timer_ = this->create_wall_timer(1000ms, std::bind(&SensorNode::publish_data, this));
    RCLCPP_INFO(this->get_logger(), "SensorNode elindult, hosszu atmenetu hullamzas...");
  }

private:
  void publish_data() {
    // 0.08 szogsebesseg = kb. 78 masodperc egy teljes hideg-meleg ciklus
    // 22.5 C kozeprol indul, 11.5 C es 33.5 C kozott finoman mozdul
    double temp = 22.5 + 11.0 * std::sin(step_ * 0.08) + noise_(gen_);

    // Pára 50% es 80% kozott valtozik hosszu fazisban
    double humidity = 0.65 + 0.15 * std::cos(step_ * 0.08) + (noise_(gen_) * 0.01);
    if (humidity < 0.1) humidity = 0.1;
    if (humidity > 0.98) humidity = 0.98;

    step_ += 1.0;

    auto now = this->now();

    sensor_msgs::msg::Temperature temp_msg;
    temp_msg.header.stamp = now;
    temp_msg.header.frame_id = "temp_sensor";
    temp_msg.temperature = temp;
    temp_pub_->publish(temp_msg);

    sensor_msgs::msg::RelativeHumidity hum_msg;
    hum_msg.header.stamp = now;
    hum_msg.header.frame_id = "humidity_sensor";
    hum_msg.relative_humidity = humidity;
    hum_pub_->publish(hum_msg);

    RCLCPP_INFO(this->get_logger(), "Mert ertekek: %.1f C | %.1f %%", temp, humidity * 100.0);
  }

  rclcpp::Publisher<sensor_msgs::msg::Temperature>::SharedPtr temp_pub_;
  rclcpp::Publisher<sensor_msgs::msg::RelativeHumidity>::SharedPtr hum_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  double step_;
  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_real_distribution<> noise_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SensorNode>());
  rclcpp::shutdown();
  return 0;
}
