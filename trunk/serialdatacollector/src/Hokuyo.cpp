/*
 * Hokuyo.cpp
 *
 *  Created on: 13/apr/2010
 *      Author: studenti
 */

#include "Hokuyo.h"
#include "UrgCtrl.h"
#include <stdio.h>
#include <string>

using namespace qrk;
using namespace std;

Hokuyo::Hokuyo() {
	device = new char[16];
	is_connected=false;
	urg = new UrgCtrl();
}

Hokuyo::~Hokuyo() {
	delete(urg);
	delete(device);
}

bool Hokuyo::openCommunication(int port) {
	sprintf(device, "%s%d","/dev/ttyACM", port);
	if (! urg->connect(device)) {
		errorExplained = (char*) urg->what();
		return false;
	}
	is_connected=true;
	return true;
}

int Hokuyo::readData(lvec* data, long* timestamp) {
	  int n = urg->capture(*data, timestamp);
	  if (n < 0) {
		errorExplained = (char*) urg->what();
	    return -1;
	  }
	  return n;
}
