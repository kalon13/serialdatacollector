/*
 * Hokuyo.cpp
 *
 *  Created on: 13/apr/2010
 *      Author: studenti
 */

#ifndef HOKUYO
#include "Hokuyo.h"
#include "UrgCtrl.h"
#include <stdio.h>
#include <string>

using namespace qrk;
using namespace std;

Hokuyo::Hokuyo() {
	device = new char[16];
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
	return true;
}

int Hokuyo::readData(lvec* data, long* timestamp) {
	if(urg->isConnected()) {
	  int n = urg->capture(*data, timestamp);
	  if (n < 0) {
		errorExplained = (char*) urg->what();
	    return -1;
	  }
	  return n;
	}
	errorExplained = "Il dispositivo non e' stato aperto correttamente";
	return -1;
}

bool Hokuyo::isConnected() {
	return urg->isConnected();
}

bool Hokuyo::closeCommunication() {
	if(urg->isConnected()) {
		urg->disconnect();
		return true;
	}
	else
		return false;
}

#endif
