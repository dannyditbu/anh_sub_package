#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <math.h>

float linearVelX = 0.5;
float angularVelZ = 0.5;

void anhCallback ( const sensor_msgs::LaserScan scanData ){
	float min = 100000; // a variable to store the smallest range
	int indexOfMin = 0; // the index of the smallest range
	for (int i = 0; i < 270; i++){
		if (!std::isinf(scanData.ranges[i])){
			if (scanData.ranges[i] < min){
				//keep updating the smallest distance to a wall
				min = scanData.ranges[i];
				indexOfMin = i;
				if (min < 10){
					/* The closer the obstacle, the slower we should go and the faster we should turn away
					 * meaning the closer the obstacle, the lower the linear velocity and the higher the angular velocity
					 */
					linearVelX = 0.2 * min;

					/* If the smallest distance is on the half right of ranges[], then turn left
					 * if on the half left, then turn right
					 */
					if (indexOfMin < 134){
						angularVelZ = exp(-min / 0.1) + 0.5;
					}
					else{
						angularVelZ = -exp(-min / 0.1) - 0.5;
					}
				}

			}
		}
	}
}

int main ( int argc , char ** argv ){
	
	ros::init(argc, argv, "anh_sub_node");
	
	ros::NodeHandle n ;
	
	ros::Publisher scan_pub = n.advertise<geometry_msgs::Twist>("robot0/cmd_vel", 1);
	ros::Rate loop_rate(10000);
	ros::Subscriber scan_sub;
	
	/* Create a velocity to control robot
	 * including linear and angular velocity
	 */
	geometry_msgs::Twist controlVel;
	
	while (ros::ok()){
		ros::spinOnce();
		scan_sub = n.subscribe("robot0/laser_1", 1, anhCallback);
		controlVel.linear.x = linearVelX;
		controlVel.angular.z = angularVelZ; 
		scan_pub.publish(controlVel);
		ROS_INFO("%f,%f", linearVelX, angularVelZ);
		loop_rate.sleep();
	} 
	return 0 ;
}
