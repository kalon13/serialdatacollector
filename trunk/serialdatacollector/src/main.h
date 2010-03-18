/*
 * main.h
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */
#include "RawSeed.h"
#include "SerialDevice.h"

#define MAX_SENSOR 8

#ifndef MAIN_H_
#define MAIN_H_

typedef struct Device
{
	int identifier;
	SerialDevice dev;
	char* buffer[BUFFER_LENGTH];
}Device;

#endif /* MAIN_H_ */
