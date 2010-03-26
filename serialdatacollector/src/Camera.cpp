/*
 * Camera.cpp
 *
 *  Created on: 23/mar/2010
 *      Author: kain
 */

#include "Camera.h"

using namespace std;
using namespace cv;

Camera::Camera() {
	// TODO Auto-generated constructor stub
	camera_flag = false;
}

Camera::~Camera() {
	// TODO Auto-generated destructor stub
	cap->release();
	delete(cap);
}

void Camera::get_photo(char* path) {
	string percorso(path);
	Mat frame;
	*cap >> frame; // get a new frame from camera
	clock_gettime(CLOCK_REALTIME, &nano);
	time(&rawtime);
	leggibile = localtime(&rawtime);
	int nano_sec = nano.tv_nsec;
	int second = leggibile->tm_sec;
	int min = leggibile->tm_min;
	int hour = leggibile->tm_hour;
	std::vector<int> params;
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	params.push_back(3);
	stringstream out;
	string ora;
	out << hour << "-" << min << "-" << second << "-" << nano_sec;
	ora = out.str();
	string filename;
	filename = percorso + "/" + ora + ".png";
	imwrite(filename, frame, params);
	//usleep(wait_time);
	waitKey(wait_time);
}

bool Camera::open_camera(int cam, int wait) {
	cap = new VideoCapture(cam); // Open the selected Camera 0 for Default
		if(!cap->isOpened())  // check if we succeeded
			return false;
	wait_time = wait;
	return true;
}

bool Camera::is_Open(){
	return cap->isOpened();
}
