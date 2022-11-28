#include "ros/ros.h"
#include <bits/stdc++.h>
#include "image_transport/image_transport.h"
#include "cv_bridge/cv_bridge.h"
#include "sensor_msgs/image_encodings.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "IRIS/data.h"
#include "std_msgs/Float32.h"

using namespace std;
using namespace cv;

class gambar
{
private:
    float x_position;
    float y_position;
    float ballRadius;

public:
    void ball_threshold(Mat img)
    {
        int hmin = 0, smin = 35, vmin = 0;
        int hmax = 41, smax = 255, vmax = 255;

        Mat imgResize, imgHSV, imgFlip, imgEdit, mask;
        Point2f center;
        float radius;
        rotate(img, imgEdit, ROTATE_90_CLOCKWISE);
        cvtColor(imgEdit, imgHSV, COLOR_BGR2HSV);

        Scalar lower(hmin, smin, vmin);
        Scalar upper(hmax, smax, vmax);
        inRange(imgHSV, lower, upper, mask);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
        for (int i = 0; i < contours.size(); i++)
        {
            vector<Point> contours_lagi;
            contours_lagi = contours[i];
            minEnclosingCircle(contours_lagi, center, radius);
            if (radius >= 180 && radius <= 360)
            {
                circle(imgEdit, center, radius, Scalar(255, 0, 0), 3);
                this->x_position = center.x;
                this->y_position = center.y;
                this->ballRadius = radius * 0.0264583333;
            }
        }
        imshow("Image", imgEdit);
        waitKey(0);
    }

    float getPosX()
    {
        return this->x_position;
    }

    float getPosY()
    {
        return this->y_position;
    }

    float getRadius()
    {
        return this->ballRadius;
    }

    void data(const IRIS::data::ConstPtr &data)
    {
        // this->data = 10;
        this->x_position = data->ball_pos_x;
        this->y_position = data->ball_pos_y;
        this->ballRadius = data->ball_radius;
        ROS_INFO("Mendapatkan data pos x bola: [%.f]", this->x_position);
        ROS_INFO("Mendapatkan data pos y bola: [%.f]", this->y_position);
    }
};

ros::Publisher pub;
gambar data_gambar = gambar();

void timercallback1hz(const ros::TimerEvent &e)
{
    ros::Rate loop_rate(10);
    IRIS::data msg;
    while (ros::ok())
    {
        msg.ball_pos_x = data_gambar.getPosX();
        msg.ball_pos_y = data_gambar.getPosY();
        msg.ball_radius = data_gambar.getRadius();

        ROS_INFO("data pos x bola: [%.f]", msg.ball_pos_x);
        ROS_INFO("data pos y bola: [%.f]", msg.ball_pos_y);
        ROS_INFO("%data radius bola: [%.f]", msg.ball_radius);
        pub.publish(msg);

        ros::spinOnce();
        loop_rate.sleep();
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "ball_threshold");
    ros::NodeHandle nh;
    ros::MultiThreadedSpinner spn;

    Mat img = imread("/home/dimasf/Desktop/modul_ros_programming/opencv_ros/src/IRIS/Resources/ball.jpg");

    pub = nh.advertise<IRIS::data>("chatter", 1000);

    ros::Subscriber sub = nh.subscribe("chatter", 1000, &gambar::data, &data_gambar);

    data_gambar.ball_threshold(img);

    ros::Timer timer = nh.createTimer(ros::Duration(1), timercallback1hz);

    spn.spin();
}
