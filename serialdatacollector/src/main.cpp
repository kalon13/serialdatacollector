//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "SerialGps.h"
#include "SerialDevice.h"
#include <stdio.h>
#define DIM 2048

using namespace std;
unsigned char data[DIM];


int main() {


	int byte_read;
	unsigned short int d;
	SerialGps gps1;
	char *port = "/dev/ttyUSB0";
	gps1.openCommunication(port,4800,8,0,1);
	while(1)
	{
	byte_read=gps1.readData(&data[0],200);
	gps1.store_data(&data[0],byte_read);
	}
	return 0;
}
