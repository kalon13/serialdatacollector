/*
 * Camera.h
 *
 *  Created on: 23/mar/2010
 *      Author: kain
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <time.h>
#include <iostream>
#include <time.h>
#include <string>
#include <string.h>
#include "cv.h"
#include "highgui.h"

using namespace cv;

class Camera {
private:
	VideoCapture *cap;
	int wait_time;
	time_t rawtime;
	struct tm* leggibile;
	struct timespec nano;
	bool camera_flag;
public:
	/*
	static VideoCapture *cap;
	static int wait_time;
	static time_t rawtime;
	static struct tm* leggibile;
	static struct timespec nano;
	static bool camera_flag;*/
	Camera();
	virtual ~Camera();
	bool openCommunication(int cam, int wait);
	void getPhoto(char* path);
	bool isConnected();
};

#endif /* CAMERA_H_ */
