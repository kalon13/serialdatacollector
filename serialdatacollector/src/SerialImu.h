/*
 * Imu.h
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#ifndef IMU_H_
#define IMU_H_

#include "SerialDevice.h"

/*   Costanti per i comandi		*/
#define CMD_RAW_SENSOR         	0x01
#define CMD_GYRO_VECTOR         0x02
#define CMD_INSTANT_VECTOR      0x03
#define CMD_INSTANT_QUAT        0x04
#define CMD_GYRO_QUAT           0x05
#define CMD_CAPTURE_GYRO_BIAS   0x06
#define CMD_TEMPERATURE         0x07
#define CMD_SEND_EEPROM         0x08
#define CMD_PROG_EEPROM         0x09
#define CMD_INSTANT_OR_MATRIX   0x0A
#define CMD_GYRO_OR_MATRIX      0x0B
#define CMD_GYRO_QUAT_VECTOR    0x0C
#define CMD_INSTANT_EULER       0x0D
#define CMD_GYRO_EULER          0x0E
#define CMD_SET_CONTINUOUS      0x10
#define CMD_FIRWARE_VERSION     0xF0
#define CMD_SERIAL_NUMBER       0xF1
#define M3D_INSTANT				1
#define M3D_STABILIZED			2
#define M3D_GYROSCALE_ADDRESS 	130
#define M3D_GYROGAINSCALE 		64
#define LSB_MASK 				0xFF
#define MSB_MASK 				0xFF00

struct EulerAngles
{
	float roll;
	float pitch;
	float yaw;
};

class SerialImu:public SerialDevice {
private:
	short convert2short(unsigned char* buffer);
	int convert2int(unsigned char* buffer);
	int calcChecksum(unsigned char* buffer, int length);

	float getTimerSeconds(unsigned char* timestamp);
public:
	int getEulerAngles(float *pitch, float *roll, float *yaw, bool stable, float* timestamp);
	int getQuaternions(float q[], int stable, float* timestamp);

	SerialImu();
	virtual ~SerialImu();
};

#endif /* IMU_H_ */
