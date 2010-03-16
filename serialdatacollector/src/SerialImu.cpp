/*
 * Imu.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#include "SerialImu.h"

SerialImu::SerialImu() {
	// TODO Auto-generated constructor stub
	SerialDevice::SerialDevice();
}

SerialImu::~SerialImu() {
	// TODO Auto-generated destructor stub

}


int SerialImu::getEulerAngles(float *pitch, float *roll, float *yaw, bool stable, float* timestamp) {
    unsigned char cmd;
    unsigned char response[11];
    float convertFactor = (360.0/65536.0f);
    int byteSent, byteRead;
    int responseLength = 11;

    if (stable==M3D_INSTANT)
        cmd = (unsigned char) CMD_INSTANT_EULER;
    else
        cmd = (unsigned char) CMD_GYRO_EULER;

    byteSent = SerialDevice::sendData((unsigned char*)&cmd, 1);

    if (byteSent<0)
    	return -1;

	byteRead = SerialDevice::readData(&response[0], responseLength);
	if (byteRead==responseLength) {
		int checkSum = convert2int(&response[responseLength-2]);
		if (checkSum != calcChecksum(&response[0], responseLength)) {
			errorExplained = "Errore nel checksum";
			return -1;
		}
	}
    if(byteRead>0){
        *roll  = convert2short(&response[1]) * convertFactor;
        *pitch = convert2short(&response[3]) * convertFactor;
        *yaw   = convert2short(&response[5]) * convertFactor;
        *timestamp = getTimerSeconds(&response[7]);
        return byteRead;
    }
    else
    	return -1;
}

int SerialImu::getQuaternions(float q[], int stable, float* timestamp) {
    unsigned char cmd;
    unsigned char  response[13];
    int convertFactor = 8192;
    int byteSent, byteRead;
    int i;
    int responseLength = 13;

    if (stable==M3D_INSTANT)
        cmd = (unsigned char) CMD_INSTANT_QUAT;
    else
        cmd = (unsigned char) CMD_GYRO_QUAT;

    byteSent = SerialDevice::sendData((unsigned char*) &cmd,1);

    if (byteSent < 0) {
        return -1;
    }

    byteRead = SerialDevice::readData(&response[0],responseLength);

    /*Manca il controllo del checksum ma sul programma originale non c'era*/

    /*Restituisce i dati*/
    if(byteRead>0) {
		for (i=0; i<4; ++i)
				q[i] = (float) convert2short(&response[1 + i*2])/convertFactor;
        *timestamp = getTimerSeconds(&response[9]);
		return byteRead;
    }
    else
    	return -1;
}

float SerialImu::getTimerSeconds(unsigned char* timestamp) {
	float convertFactor = 0.0065536f;
	return convert2int(timestamp)*convertFactor;
}


/*--------------------------------------------------------------------------
 * convert2int
 * Convert two adjacent bytes to an integer.
 *
 * parameters:  buffer : pointer to first of two buffer bytes.
 * returns:     the converted value.
 *--------------------------------------------------------------------------*/

int SerialImu::convert2int(unsigned char* buffer) {
	int x = (buffer[0]&LSB_MASK)*256 + (buffer[1]&LSB_MASK);
	return x;
}

/*--------------------------------------------------------------------------
 * convert2short
 * Convert two adjacent bytes to a short.
 *
 * parameters:  buffer : pointer to first of two buffer bytes.
 * returns:     the converted value.
 *--------------------------------------------------------------------------*/

short SerialImu::convert2short(unsigned char* buffer) {
	short x = (buffer[0]&LSB_MASK)*256 + (buffer[1]&LSB_MASK);
	return x;
}

/*--------------------------------------------------------------------------
 * calcChecksum
 * Calculate checksum on a received data buffer.
 *
 * Note: The last two bytes, which contain the received checksum,
 *       are not included in the calculation.
 *
 * parameters:  buffer : pointer to the start of the received buffer.
 *              length - the length (in chars) of the buffer.
 *
 * returns:     the calculated checksum.
 *--------------------------------------------------------------------------*/

int SerialImu::calcChecksum(unsigned char* buffer, int length) {
	int CHECKSUM_MASK = 0xFFFF;
	int checkSum, i;

	if (length<4)
		return -1;

	checkSum = buffer[0] & LSB_MASK;
	for (i=1; i<length-2; i = i+2) {
		checkSum += convert2int(&buffer[i]);
	}
	return(checkSum & CHECKSUM_MASK);
}
