//Divel Test Side Work 
//Requires Leap Motion Controller

#include "Leap.h"
#include "ros/ros.h"
#include "tf/transform_datatypes.h"
#include "leap_motion_controller/Set.h"
#include "geometry_msgs/Twist.h"
 #include "geometry_msgs/Quaternion.h"

leap_motion_controller::Set hand;
double R,P,Y,X;

void callBack(const leap_motion_controller::Set &msg){
	hand = msg;
	if(hand.right_hand.is_present){
		tf::Quaternion q(hand.right_hand.palm_pose.pose.orientation.x, hand.right_hand.palm_pose.pose.orientation.y, hand.right_hand.palm_pose.pose.orientation.z, hand.right_hand.palm_pose.pose.orientation.w);
      	tf::Matrix3x3 m(q);
      	m.getRPY(R,P,Y);
      	X = hand.right_hand.palm_pose.pose.position.z;
	}
}

int main(int argc, char **argv){

	ros::init(argc, argv, "divel_controller");
	ros::NodeHandle n;
	ros::Publisher vel;
	ros::Subscriber hand_sub;
	hand_sub = n.subscribe("leap_motion_output", 10, callBack);
	vel = n.advertise<geometry_msgs::Twist>("cmd_vel", 10);
	geometry_msgs::Twist cmd;

	bool initialised = false;
	double in_R,in_P,in_Y;
	double in_X;

	while(not initialised){
		ROS_INFO("Place your RIGHT hand in base state for 3 seconds..");
		ros::Duration(3.0).sleep();
		if(hand.right_hand.is_present){
			initialised = true;
			tf::Quaternion q(hand.right_hand.palm_pose.pose.orientation.x, hand.right_hand.palm_pose.pose.orientation.y, hand.right_hand.palm_pose.pose.orientation.z, hand.right_hand.palm_pose.pose.orientation.w);
      		tf::Matrix3x3 m(q);
      		m.getRPY(in_R,in_P,in_Y);
      		in_X = hand.right_hand.palm_pose.pose.position.z;
		}
		ros::spinOnce();
	}


	while(n.ok()){
		if(hand.right_hand.is_present){
			cmd.linear.x = in_X - X;
			cmd.angular.z = Y - in_Y ;
			ROS_INFO("V: %f R: %f",in_X - X,Y - in_Y);
			vel.publish(cmd);
		}
		else{
			cmd.linear.x = 0.0;
			cmd.angular.z = 0.0;
		}
		ros::spinOnce();
	}

}
