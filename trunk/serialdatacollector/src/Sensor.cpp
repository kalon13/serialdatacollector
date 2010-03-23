/*
 * Sensor.cpp
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */

#include "Sensor.h"
#include "SerialGps.h"
#include "SerialImu.h"

Sensor::Sensor() {
	setted=false;
}

Sensor::~Sensor() {
}

bool Sensor::setSensor(int identifier){
	switch(identifier){
			case 0:{
				this->identifier = identifier;
				dev = new SerialGps();
				break;
			}
			case 1: {
				this->identifier = identifier;
				dev = new SerialImu();
				break;
			}
			default: {
				return false;
			}
		}
	setted = true;
	return true;
}

void Sensor::getDev(SerialDevice** d){
	*d = dev;
}

bool Sensor::getBuffer(char* buf[MAX_BUFFER_LENGTH]) {
	buf = buffer;
	return true;
}

int Sensor::getIdentifier() {
	return identifier;
}
