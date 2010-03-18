/*
 * Imu.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#include "SerialImu.h"
#include <stdio.h>

SerialImu::SerialImu() {
	// TODO Auto-generated constructor stub
	SerialDevice::SerialDevice();
	gyroGainScale = M3D_GYROGAINSCALE;
}

SerialImu::~SerialImu() {
	// TODO Auto-generated destructor stub

}
/*
bool SerialImu::openCommunication(char* port, int baudRate, int dataBits, int parity=0, int stopBits=1) {
	return SerialDevice::openCommunication(port,baudRate,dataBits,parity,stopBits);
}*/

bool SerialImu::openCommunication(char* port) {
	int baudRate=38400, dataBits=8, parity=0, stopBits=1;
	return SerialDevice::openCommunication(port,baudRate,dataBits,parity,stopBits);
}

int SerialImu::getEulerAngles(float *pitch, float *roll, float *yaw, bool stableOption, float* timestamp) {
    unsigned char cmd;
    unsigned char response[11];
    float convertFactor = (360.0/65536.0f);
    int byteSent, byteRead;
    int responseLength = 11;

    if (stableOption==M3D_INSTANT)
        cmd = (unsigned char) CMD_INSTANT_EULER;
    else
        cmd = (unsigned char) CMD_GYRO_EULER;

    byteSent = SerialDevice::sendData((unsigned char*)&cmd, 1);

    if (byteSent>0){
		byteRead = SerialDevice::readData(&response[0], responseLength);
		if (byteRead==responseLength) {
			int checkSum = convert2int(&response[responseLength-2]);
			if (checkSum == calcChecksum(&response[0], responseLength)) {
		        *roll  = convert2short(&response[1]) * convertFactor;
		        *pitch = convert2short(&response[3]) * convertFactor;
		        *yaw   = convert2short(&response[5]) * convertFactor;
		        *timestamp = getTimerSeconds(&response[7]);
			}
			else
				return -1;	//Checksum Error
		}
		return byteRead;
    }
    return byteSent;	//Errore di invio dati
}

int SerialImu::getQuaternions(float q[], int stableOption, float* timestamp) {
    unsigned char cmd;
    unsigned char  response[13];
    int convertFactor = 8192;
    int byteSent, byteRead;
    int i;
    int responseLength = 13;

    if (stableOption==M3D_INSTANT)
        cmd = (unsigned char) CMD_INSTANT_QUAT;
    else
        cmd = (unsigned char) CMD_GYRO_QUAT;

    byteSent = SerialDevice::sendData((unsigned char*) &cmd,1);

	if (byteSent > 0) {
		byteRead = SerialDevice::readData(&response[0],responseLength);
		if (byteRead==responseLength) {
			if (calcChecksum(&response[0], responseLength)) {
				for (i=0; i<4; ++i)
						q[i] = (float) convert2short(&response[1 + i*2])/convertFactor;
				*timestamp = getTimerSeconds(&response[9]);
			}
			else
				return -1;	//Checksum Error
		}
		return byteRead;
	}
	return byteSent;	//Errore di invio dati
}

/*--------------------------------------------------------------------------
 * getVectors
 *
 * parameters   mag       : array which will contain mag data (3 elements)
 *              accel     : array which will contain accleration data (3 elements)
 *              angRate   : array which will contain angular rate data (3 elements)
 *              stableOption : a flag indicating whether data retrieved should
 *                             be instantaneous or gyro-stabilized.
 *
 * returns:     byte read or sent:	se sono negativo indica un errore di
 * 									lettura/scrittura.
 *--------------------------------------------------------------------------*/
int SerialImu::getVectors(float mag[], float accel[], float angRate[], int stableOption, float* timestamp) {
    char cmd = CMD_RAW_SENSOR;
    int responseLength = 23;
    unsigned char  response[23];
    float convertFactor = 8192.0f;
    float angularFactor = gyroGainScale*8192.0f*0.0065536f;

    int byteRead, byteSent;
    int i;

    if (stableOption==M3D_INSTANT)
        cmd = (char) CMD_INSTANT_VECTOR;
    else
        cmd = (char) CMD_GYRO_VECTOR;


    byteSent = SerialDevice::sendData((unsigned char*) &cmd, 1);

    if (byteSent > 0) {
    	byteRead = SerialDevice::readData(&response[0],responseLength);
    	if(byteRead==responseLength){
    		if (calcChecksum(&response[0], responseLength)) {
				for (i=0; i<3; i++) {
					mag[i]     = (float) convert2short(&response[1 + i*2])/convertFactor;
					accel[i]   = (float) convert2short(&response[7 + i*2])/convertFactor;
					angRate[i] = (float) convert2short(&response[13+ i*2])/angularFactor;
				}
		        *timestamp = getTimerSeconds(&response[responseLength-4]);
    		}
    		else
    			return -1;	//Checksum Error
    	}
        return byteRead;
    }
    return byteSent;	//errore di invio dati
}

/*--------------------------------------------------------------------------
 * m3dmg_getOrientMatrix
 *
 * parameters   mx           : pointer to a 3x3 float matrix
 *                             which will contain the transform data
 *                             upon return.
 *              stableOption : a flag indicating whether data retrieved should
 *                             be instantaneous or gyro-stabilized.
 *
 * returns:     byte read or sent:	se sono negativo indica un errore di
 * 									lettura/scrittura.
 *--------------------------------------------------------------------------*/

int SerialImu::getOrientMatrix(float mx[][3], int stableOption, float* timestamp) {
	char cmd;
	int responseLength = 23;
	unsigned char  response[23];
	float convertFactor = 8192.0f;

    int byteRead, byteSent;
    int i,j;

	if (stableOption==M3D_INSTANT)
		cmd = (char) CMD_INSTANT_OR_MATRIX;
	else
		cmd = (char) CMD_GYRO_OR_MATRIX;

	byteSent = SerialDevice::sendData((unsigned char*) &cmd, 1);

	if (byteSent > 0) {
	byteRead = SerialDevice::readData(&response[0],responseLength);
		if(byteRead==responseLength){
			if (calcChecksum(&response[0], responseLength)) {
				for (i=0; i<3; i++)
					mx[i][j] = (float) convert2short(&response[1+2*(j*3+i)])/convertFactor;
		        *timestamp = getTimerSeconds(&response[responseLength-4]);
			}
			else
				return -1;	//Checksum Error
		}
	   return byteRead;
	}
	return byteSent;	//errore di invio dati
}

/*--------------------------------------------------------------------------
 * getRawSeedString
 *
 * parameters
 *
 * returns:		A csv string formatted in this way:
 * 					# Unit Range
 * 					Sample counter
 * 					Acceleration along X, Y, Z
 * 					Angular velocity along X, Y, Z
 * 					Earth magnetic field along X, Y, Z
 * 					Orient matrix row by row
 * 					Nominal scan frequency
 *--------------------------------------------------------------------------*/
bool SerialImu::getRawSeedString(char* str) {
    float xform[3][3];
    float mag[3];
    float accel[3];
    float angRate[3];
    float ts1, ts2;
    char* final;

    if(getVectors(mag, accel, angRate, M3D_INSTANT, &ts1)>0 && getOrientMatrix(&xform[0], M3D_INSTANT, &ts2)>0)
    {
		sprintf(final,"%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
				1,(ts1+ts2)/2,accel[0],accel[1],accel[2],angRate[0],angRate[1],angRate[2],mag[0],mag[1],mag[2],
				xform[0][0],xform[0][1],xform[0][2],xform[1][0],xform[1][1],xform[1][2],xform[2][0],xform[2][1],
				xform[1][2],76.29);
		str = final;
		return true;
    }
    else
    	return false;
}

/*--------------------------------------------------------------------------
 * m3dmg_getGyroScale
 *
 * parameters
 * returns:     gyroScale value(read or default)
 *--------------------------------------------------------------------------*/

int SerialImu::getGyroScale() {
    int status;
    short address, value;
    address = M3D_GYROSCALE_ADDRESS;

    status = getEEPROMValue(address, &value);
    if (status>0)
        return value;
    return M3D_GYROGAINSCALE;
}

/*--------------------------------------------------------------------------
 * m3dmg_getEEPROMValue
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *              address      : the EEPROM address location
 *              value        : the value to get at the address specified
 *
 * This function differs enough so that we don't use the m3dmg_sendCommand
 * as with other functions. The command in this case is multiple bytes
 * and the response has no checksum.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int SerialImu::getEEPROMValue(short address, short *value) {
    unsigned char cmdBuffer[5];
    unsigned char dataBuffer[2];
    int byteRead, byteSent;

    /* check the address range - only 256 locations permitted. */
    if (address <0 || address > 255) {
    	errorExplained = "Locazione di memoria non ammessa.";
        return -1;
    }

    /* command requires an address */
    cmdBuffer[0] = CMD_SEND_EEPROM;
    cmdBuffer[1] = (char) address;

    byteSent = SerialDevice::sendData(&cmdBuffer[0], 2);
    if (byteSent>0) {
    	/* receive data if expected */
        byteRead = SerialDevice::readData(&dataBuffer[0], 2);
        if (byteRead>0)
            *value = convert2short(&dataBuffer[0]);
        return byteRead;
    }
    return byteSent;	//Errore di invio dati
}

void SerialImu::setGyroScale(){
	gyroGainScale = getGyroScale();
}

/*--------------------------------------------------------------------------
 * getTimerSeconds
 * Restituisce il valore del tick in secondi.
 *
 * parameters:  timestamp : primo byte del tick.
 * returns:     il valore in secondi.
 *--------------------------------------------------------------------------*/
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

bool SerialImu::calcChecksum(unsigned char* buffer, int length) {
	int CHECKSUM_MASK = 0xFFFF;
	int checkSum, i;

	if (length>4){
		checkSum = buffer[0] & LSB_MASK;
		for (i=1; i<length-2; i = i+2) {
			checkSum += convert2int(&buffer[i]);
		}
		if((checkSum & CHECKSUM_MASK)==convert2int(&buffer[length-2]))
			return true;
		else
			errorExplained = "Checksum Error";
	}
	return false;
}
