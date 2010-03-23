/*
 * main.h
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */
#include "RawSeed.h"
#include "SerialDevice.h"
#include <pthread.h>

#define MAX_SENSOR 8

#ifndef MAIN_H_
#define MAIN_H_

typedef struct
{
	int identifier;
	SerialDevice* dev;
	char* pathtofile;
	char* buffer[32];
} Device;

void* gpsAcquisition(void* gpsx);
void* imuAcquisition(void* imux);

#endif /* MAIN_H_ */
