/*
 * main.h
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */
#include "RawSeed.h"
#include "SerialDevice.h"

#ifndef MAIN_H_
#define MAIN_H_

typedef struct
{
	int identifier;
	SerialDevice dev;
	char* buffer[BUFFER_LENGTH];
} Device;

#endif /* MAIN_H_ */
