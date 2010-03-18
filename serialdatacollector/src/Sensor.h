/*
 * Sensor.h
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "SerialDevice.h"

#define MAX_BUFFER_LENGTH 32

class Sensor {
private:
	int identifier;
	SerialDevice* dev;
	char* buffer[MAX_BUFFER_LENGTH];
	bool setted;
public:
	int getIdentifier();
	void getDev(SerialDevice* d);
	bool getBuffer(char* buf[MAX_BUFFER_LENGTH]);

	bool setSensor(int identifier);
	Sensor();
	virtual ~Sensor();
};

#endif /* SENSOR_H_ */
