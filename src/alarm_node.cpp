#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/temperature.hpp"
#include "sensor_msgs/msg/relative_humidity.hpp"
#include "std_msgs/msg/string.hpp"

// ANSI színkódok a látványos terminálhoz
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[1;31m"
#define ANSI_BLUE    "\033[1;34m"
#define ANSI_YELLOW  "\033[1;33m"
#define ANSI_GREEN   "\033[1;32m"
#define ANSI_BOLD    "\033[1m"

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
    RCLCPP_INFO(this->get_logger(), ANSI_BOLD "🚀 AlarmNode elindult, adatok figyelése..." ANSI_RESET);
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

    std::string alert_raw = "";
    std::string alert_display = "";

    if (last_temp_ > 30.0) {
      alert_raw += "[RIASZTAS: TUL MELEG (" + std::to_string(last_temp_).substr(0, 4) + " C)] ";
      alert_display += ANSI_RED "🔥 [TÚL MELEG: " + std::to_string(last_temp_).substr(0, 4) + " °C]" ANSI_RESET " ";
    } else if (last_temp_ < 15.0) {
      alert_raw += "[RIASZTAS: TUL HIDEG (" + std::to_string(last_temp_).substr(0, 4) + " C)] ";
      alert_display += ANSI_BLUE "❄️  [TÚL HIDEG: " + std::to_string(last_temp_).substr(0, 4) + " °C]" ANSI_RESET " ";
    }

    if (last_hum_ > 0.75) {
      alert_raw += "[RIASZTAS: MAGAS PARATARTALOM (" + std::to_string(last_hum_ * 100.0).substr(0, 4) + " %)]";
      alert_display += ANSI_YELLOW "💧 [MAGAS PÁRA: " + std::to_string(last_hum_ * 100.0).substr(0, 4) + " %]" ANSI_RESET;
    }

    if (!alert_raw.empty()) {
      auto out_msg = std_msgs::msg::String();
      out_msg.data = alert_raw;
      alarm_pub_->publish(out_msg);
      RCLCPP_WARN(this->get_logger(), "%s", alert_display.c_str());
    } else {
      RCLCPP_INFO(this->get_logger(), ANSI_GREEN "✅ Minden érték normális (%.1f °C | %.1f %%)" ANSI_RESET, last_temp_, last_hum_ * 100.0);
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
