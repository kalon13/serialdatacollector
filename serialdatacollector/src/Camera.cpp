/*
 * Camera.cpp
 *
 *  Created on: 23/mar/2010
 *      Author: kain
 */



#include "Camera.h"

#ifndef CAMERA
using namespace std;
using namespace cv;

Camera::Camera() {
	cap = new VideoCapture();
	shot=false;
}

Camera::~Camera() {
	cap->release();
	delete(cap);
}

/* Quando ho fatto diventare camera sottoclasse di SerialDevice
 * questo metodo è inutile.... si usano gli altri 2 */
void Camera::getPhoto(char* path) {
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

bool Camera::readData() {
	if(isConnected()) {
		*cap >> frame;
		clock_gettime(CLOCK_REALTIME, &nano);
		time(&rawtime);
		leggibile = localtime(&rawtime);

		params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		//params.push_back(CV_IMWRITE_JPEG_QUALITY);
		params.push_back(3);
		shot=true;
		return true;
	}
	return false;
}

bool Camera::writeData(char* path, char* name) {
	if(shot) {
		string percorso(path);
		bool ok;

		int nano_sec = nano.tv_nsec;
		int second = leggibile->tm_sec;
		int min = leggibile->tm_min;
		int hour = leggibile->tm_hour;
		stringstream out;
		string ora;
		out << hour << "-" << min << "-" << second << "-" << nano_sec;
		ora = out.str();
		string filename;
		if(name == NULL)
			filename = percorso + "/" + ora + ".png";
		else
			filename = percorso + "/" + name + ".png";
		ok = imwrite(filename, frame, params);

		shot=false;

		waitKey(wait_time);
		return ok;
	}
	return false;
}

bool Camera::openCommunication(int cam, int wait) {
	cap->open(cam);
	if(!cap->isOpened())  // check if we succeeded
		return false;
	wait_time = wait;
	return true;
}
bool Camera::openCommunication(const char* ip, int wait) {
	cap->open(ip);
	if(!cap->isOpened())  // check if we succeeded
		return false;
	wait_time = wait;
	return true;
}

bool Camera::isConnected(){
	return cap->isOpened();
}

#endif
