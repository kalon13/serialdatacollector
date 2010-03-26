/*
 * main.h
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */
#include "RawSeed.h"
#include "SerialDevice.h"
#include "Camera.h"
#include <boost/thread.hpp>

#define MAX_SENSOR 8

#ifndef MAIN_H_
#define MAIN_H_

#define DIM_BUFFER_GPS 4
#define DIM_BUFFER_IMU 32

enum DevId {GPS, IMU, WEBCAM};

boost::mutex io_mutex;

struct ThreadedDevice
{
	DevId identifier;
	void* device;
	char* path;
	bool attivo;
};

void wcAcquisition(ThreadedDevice* dev);
bool gpsAcquisition(ThreadedDevice* dev);
bool imuAcquisition(ThreadedDevice* dev);

#endif /* MAIN_H_ */
