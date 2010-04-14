/*
 * Hokuyo.h
 *
 *  Created on: 13/apr/2010
 *      Author: studenti
 */

#ifndef HOKUYO_H_
#define HOKUYO_H_
#include "UrgCtrl.h"
#include "SerialDevice.h"
#include <vector>

typedef std::vector<long> lvec;

class Hokuyo:public SerialDevice {
private:
	bool is_connected;
	char* device;
	qrk::UrgCtrl* urg;


public:
	Hokuyo();
	virtual ~Hokuyo();
	bool openCommunication(int port);
	int readData(lvec* data, long* timestamp);
};

#endif /* HOKUYO_H_ */
