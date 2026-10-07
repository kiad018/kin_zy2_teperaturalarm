from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='kin_zy2_teperaturalarm',
            executable='sensor_node',
            name='sensor_node',
            output='screen'
        ),
        Node(
            package='kin_zy2_teperaturalarm',
            executable='alarm_node',
            name='alarm_node',
            output='screen'
        ),
    ])