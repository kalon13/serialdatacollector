/*
 * Hokuyo.h
 *
 *  Created on: 13/apr/2010
 *      Author: studenti
 */

#ifndef HOKUYO_H_
#define HOKUYO_H_
#ifndef HOKUYO
#include "UrgCtrl.h"
#include "SerialDevice.h"
#include <vector>

typedef std::vector<long> lvec;

class Hokuyo: public SerialDevice {
private:
	char* device;
	qrk::UrgCtrl* urg;
public:
	Hokuyo();
	virtual ~Hokuyo();
	bool openCommunication(int port);
	bool closeCommunication();
	int readData(lvec* data, long* timestamp);
	bool isConnected();
};

#endif
#endif /* HOKUYO_H_ */
