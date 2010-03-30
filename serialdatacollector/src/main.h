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
#include <vector>

#define MAX_SENSOR 8

#ifndef MAIN_H_
#define MAIN_H_

#define DIM_BUFFER_GPS 1
#define DIM_BUFFER_IMU 32

enum DevId {NOTHING=9, GPS=0, IMU=1, CAM=2};
enum Stato {PRONTO, ATTIVO, PAUSA, TERMINATO};

struct ThreadedDevice {
	DevId identifier;
	void* device;
	char* path;
	Stato stato;
};

vector<ThreadedDevice> d;

void* camAcquisition(void* i);
void* gpsAcquisition(void* i);
void* imuAcquisition(void* i);

void cmdShell();

#endif /* MAIN_H_ */
