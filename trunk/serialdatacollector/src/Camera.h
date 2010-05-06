/*
 * Camera.h
 *
 *  Created on: 23/mar/2010
 *      Author: kain
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#ifndef CAMERA
#include <time.h>
#include <iostream>
#include <time.h>
#include <string>
#include <string.h>
#include "cv.h"
#include "highgui.h"
#include "SerialDevice.h"

using namespace cv;

class Camera: public SerialDevice {
private:
	VideoCapture *cap;
	int wait_time;

	Mat frame;
	std::vector<int> params;
	time_t rawtime;
	struct tm* leggibile;
	struct timespec nano;
	bool shot;

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
	bool openCommunication(int cam=0, int wait=200);
	void getPhoto(char* path);
	bool isConnected();
	bool readData();
	bool writeData(char* path);
};

#endif
#endif /* CAMERA_H_ */
