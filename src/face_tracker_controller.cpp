/*
 * Copyright (C) 2017, Lentin Joseph and Qbotics Labs Inc.

 * Email id : qboticslabs@gmail.com

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the names of Stanford University or Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.


* This code will subscriber centroid of face and move dynamixel : face_tracker_controller.cpp

*/

/*
Value to dynamixel controller

Center = 0  // Dynamixel value = 512
Right = 1   // Dynamixel value = 708
Left = -1   // Dyanmixel value = 316

Differenece is 196 unit from center in Dynamixel
Optimum range = -0.5 to 0.5
*/



#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include <iostream>

#include "face_tracker_pkg/centroid.h"

//Tracker parameters 

int servomaxx, servomaxy, servomin,screenmaxx, screenmaxy,center_offset,center_offsety, center_left, center_right, center_up, center_down;
float servo_step_distancex, current_pos_x, servo_step_distancey, current_pos_y;

std_msgs::Float64 initial_pose;
std_msgs::Float64 current_pose;
std_msgs::Float64 current_posey;

ros::Publisher dynamixel_control;
ros::Publisher dynamixel_control_tilt;

void track_face(int x,int y) 
{
	
	//ROS_INFO"Servo x = %f cur pos l %f  r %f",x ,center_left,center_right)
    //Find out if the X component of the face is to the left of the middle of the screen.
    if(x < (center_left)){

		////ROS_INFO("Face is at Left");
        
		current_pos_x += servo_step_distancex;
		current_pose.data = current_pos_x;
		//ROS_INFO("servo controller current_position = %f",current_pos_x);

        if(current_pos_x < servomaxx and current_pos_x > servomin ){
        	dynamixel_control.publish(current_pose);
		}
	
    } 
	 //Find out if the X component of the face is to the right of the middle of the screen.
    else if(x > center_right){

	//ROS_INFO("Face is at Right");

	current_pos_x -= servo_step_distancex;
	current_pose.data = current_pos_x;

	//ROS_INFO("servo controller current_position = %f",current_pos_x);

 	 	if(current_pos_x < servomaxx and current_pos_x > servomin ){
        	dynamixel_control.publish(current_pose);
		}


    }

    //Find out if the Y component of the face is to the up of the middle of the screen.
    if(y < (center_up)){

	//ROS_INFO("Face is at up");

	current_pos_y -= servo_step_distancey;
	current_posey.data = current_pos_y;
	//ROS_INFO("servo controller current_position = %f",current_pos_y);

        if(current_pos_y < servomaxy and current_pos_y > servomin ){
        	dynamixel_control_tilt.publish(current_posey);
			//ROS_INFO("Publish Up");
		}
	
    }
   
	//Find out if the X component of the face is to the down of the middle of the screen.
    else if(y > center_down){

	//ROS_INFO("Face is at Down");

	current_pos_y += servo_step_distancey;
	current_posey.data = current_pos_y;

	//ROS_INFO("servo controller current_position = %f",current_pos_y);

 	 if(current_pos_y < servomaxy and current_pos_y > servomin ){
        	dynamixel_control_tilt.publish(current_posey);
			//ROS_INFO("Publish Down");
		} else {
			current_pos_y -= servo_step_distancey;
			current_posey.data = current_pos_y;
		 }	
			


    }
   else if(x > center_left and x < center_right and y > center_up and y < center_down){

	    //ROS_INFO("Face is at Center");
        
	}


}
//Callback of the topic /numbers
void face_callback(const face_tracker_pkg::centroid::ConstPtr& msg)
{
	//ROS_INFO("Recieved X = [%d], Y = [%d]",msg->x,msg->y);
	//Calling track face function
	track_face(msg->x,msg->y);

}

int main(int argc, char **argv)
{

	//Loading servo configurations of the dynamixel tracker
	//Initializing ROS node with a name of demo_topic_subscriber
	ros::init(argc, argv,"face_tracker_controller");
	//Created a nodehandle object
	ros::NodeHandle node_obj;
	//Create a publisher object
	ros::Subscriber number_subscriber = node_obj.subscribe("/social/face_centroid",10,face_callback);
    dynamixel_control = node_obj.advertise<std_msgs::Float64>("/pan_controller/command",10);
    dynamixel_control_tilt = node_obj.advertise<std_msgs::Float64>("/tilt_controller/command",10);



	servomaxx = 0.5;   //max degree servo horizontal (x) can turn
	servomaxy = 0.5;   //max degree servo Vertical (y) can turn
	servomin = -0.5;
	screenmaxx = 640;   //max screen horizontal (x)resolution
	screenmaxy = 480;   //max screen horizontal (y)resolution
	center_offset = 100;
	center_offsety = 60;
	servo_step_distancex = 0.005; //x servo rotation steps
	servo_step_distancey = 0.005; //y servo rotation steps
	current_pos_x =0 ;
	current_pos_y =0 ;

	try{
	  node_obj.getParam("servomaxx", servomaxx);
	  node_obj.getParam("servomaxy", servomaxy);
	  node_obj.getParam("servomin", servomin);
	  node_obj.getParam("screenmaxx", screenmaxx);
	  node_obj.getParam("screenmaxy", screenmaxy);
	  
	  node_obj.getParam("center_offset", center_offset);
	  node_obj.getParam("center_offsety", center_offsety);
	  node_obj.getParam("step_distancex", servo_step_distancex);
	  node_obj.getParam("step_distancey", servo_step_distancey);

	  //ROS_INFO("Successfully Loaded tracking parameters");
	  }


	  catch(int e)
	  {
	   
	      ROS_WARN("Parameters are not properly loaded from file, loading defaults");
	
	  }
	  center_left = (screenmaxx / 2) - center_offset;
	  center_right = (screenmaxx / 2) + center_offset;
	  center_up = (screenmaxy / 2) - center_offsety;
	  center_down = (screenmaxy / 2) + center_offsety;


	//Sending initial pose
	initial_pose.data = 0.0;
    dynamixel_control.publish(initial_pose);
	dynamixel_control_tilt.publish(initial_pose);
	
	//Spinning the node
	ros::spin();
	return 0;
}

