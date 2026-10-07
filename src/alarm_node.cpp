#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/temperature.hpp"
#include "sensor_msgs/msg/relative_humidity.hpp"
#include "std_msgs/msg/string.hpp"

class AlarmNode : public rclcpp::Node {
public:
  AlarmNode() : Node("alarm_node"), has_temp_(false), has_hum_(false) {
    sub_temp_ = this->create_subscription<sensor_msgs::msg::Temperature>(
      "/sensor/temperature", 10,
      std::bind(&AlarmNode::temp_callback, this, std::placeholders::_1));

    sub_hum_ = this->create_subscription<sensor_msgs::msg::RelativeHumidity>(
      "/sensor/humidity", 10,
      std::bind(&AlarmNode::hum_callback, this, std::placeholders::_1));

    alarm_pub_ = this->create_publisher<std_msgs::msg::String>("/alarm", 10);
    RCLCPP_INFO(this->get_logger(), "AlarmNode elindult, adatok figyelese...");
  }

private:
  void temp_callback(const sensor_msgs::msg::Temperature::SharedPtr msg) {
    last_temp_ = msg->temperature;
    has_temp_ = true;
    check_status();
  }

  void hum_callback(const sensor_msgs::msg::RelativeHumidity::SharedPtr msg) {
    last_hum_ = msg->relative_humidity;
    has_hum_ = true;
    check_status();
  }

  void check_status() {
    if (!has_temp_ || !has_hum_) return;

    std::string alert = "";
    if (last_temp_ > 30.0) {
      alert += "[RIASZTAS: TUL MELEG (" + std::to_string(last_temp_).substr(0, 4) + " C)] ";
    } else if (last_temp_ < 15.0) {
      alert += "[RIASZTAS: TUL HIDEG (" + std::to_string(last_temp_).substr(0, 4) + " C)] ";
    }

    if (last_hum_ > 0.75) {
      alert += "[RIASZTAS: MAGAS PARATARTALOM (" + std::to_string(last_hum_ * 100.0).substr(0, 4) + " %)]";
    }

    if (!alert.empty()) {
      auto out_msg = std_msgs::msg::String();
      out_msg.data = alert;
      alarm_pub_->publish(out_msg);
      RCLCPP_WARN(this->get_logger(), "%s", alert.c_str());
    } else {
      RCLCPP_INFO(this->get_logger(), "Minden ertek normalis tartomanyban.");
    }
  }

  rclcpp::Subscription<sensor_msgs::msg::Temperature>::SharedPtr sub_temp_;
  rclcpp::Subscription<sensor_msgs::msg::RelativeHumidity>::SharedPtr sub_hum_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr alarm_pub_;

  double last_temp_{0.0};
  double last_hum_{0.0};
  bool has_temp_;
  bool has_hum_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AlarmNode>());
  rclcpp::shutdown();
  return 0;
}
