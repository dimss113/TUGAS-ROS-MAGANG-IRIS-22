#include <bits/stdc++.h>
#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "IRIS/data.h"
#include <math.h>

void callbackArea(const IRIS::dataConstPtr &msg)
{
    float getArea = 4.0 * msg->ball_radius * msg->ball_radius * M_PI;
    ROS_INFO("data area: [%.f]", getArea);
}

void callbackVolume(const IRIS::dataConstPtr &msg)
{
    float getVolume = 4.0 * M_PI * msg->ball_radius * msg->ball_radius * msg->ball_radius / 3.0;
    ROS_INFO("data volume: [%.2f]", getVolume);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "getData");
    ros::NodeHandle nh;

    ros::Subscriber sub = nh.subscribe("chatter", 1000, callbackArea);
    ros::Subscriber sub2 = nh.subscribe("chatter", 1000, callbackVolume);

    ros::spin();

    return 0;
}