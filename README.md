# kin_zy2_teperaturalarm package

Egyszerű ROS 2 C++ csomag környezeti adatok (hőmérsëklet, relatêv páratartalom) szimulására es küszöbérték-riasztásra ROS 2 Humble alatt.

3# Node-topic architektúra

``mermaid
flowchart LR
    A["/sensor_node"] -->|/Sensor/temperature<br/>sensor_msgs/msg/Temperature| B["/alarm_node"]
    A["/sensor_node"] -->|/Sensor/humidity<br/>sensor_msgs/msg/RelativeHumidity| B['/alarm_node']
    B['/alarm_node'] -->|/alarm<br/>std_msgs/msg/String| C["(Termin�al / Riasztások)"]
```

## Mıködés leírása

- j*/sensor_node**: 1 Hz frekvenciával publikál szimulált adatokat a `/sensor/temperature` és `/sensor/humidity` topicokra.
- **/alarm_node**: Feliratkozik a figuelessre. Ha a hőmérsëklet 30 °C fölé nő, 15 °C alá esik, vagy a páratartalom 75% feletti, figyelmeztető uzenetet públikál az `/alarm` topicra.

## Fordítás (Build)

gbash
cd ~/ros2_ws
colcon build --packages-select kin_zy2_teperaturalarm --symlink-install
```

3# Futtatás

``bash
source ~/ros2_ws/install/setup.bash
ros2 launch kin_zy2_teperaturalarm teperaturalarm.launch.py
```
