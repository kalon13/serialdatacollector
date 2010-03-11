/*--------------------------------------------------------------------------
 *
 * 3DM-G Interface Software
 *
 * (c) 2003 Microstrain, Inc.
 * All rights reserved.
 *
 * www.microstrain.com
 * 310 Hurricane Lane, Suite 4
 * Williston, VT 05495 USA
 * Tel: 802-862-6629
 * Fax: 802-863-4093
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * m3dmgAdapter.c
 *
 * 3DM-G Sensor device functions.
 *
 * This platform independent module relies on the communication functions found
 * in either m3dmgSerialWin.c or m3dmgSerialLinux.c 
 * (your choice, depending on platform).
 *--------------------------------------------------------------------------*/
 
#ifndef DEBUG
#define DEBUG 0
#endif

#include <stdio.h>
#include <string.h>
#include "m3dmgAdapter.h"
#include "m3dmgUtils.h"
#include "m3dmgErrors.h"
#include "m3dmgSerial.h"

int deviceMap[MAX_DEVICENUM];
int gyroGainScale[MAX_DEVICENUM];

/*--------------------------------------------------------------------------
 * m3dmg_mapDevice
 *
 * parameters   deviceNum    : the number of the sensor device (1..MAX_DEVICENUM)
 *              portNum      : a port number (1..MAX_PORT_NUM)
 *                             be instantaneous or gyro-stabilized.
 *
 * returns:     errorCode : the  device number if succeeded, otherwise returns an
 *                          error code as a negative value.
 *--------------------------------------------------------------------------*/

int m3dmg_mapDevice(int deviceNum, int portNum) {

    /* allow up to 16 devices for */
    if (deviceNum<0 || deviceNum>MAX_DEVICENUM) {
        return M3D_INVALID_DEVICENUM;
    }

    if (portNum<0 || portNum>MAX_PORT_NUM) {
        return M3D_COMM_INVALID_PORTNUM;
    }
    deviceMap[deviceNum] = portNum;

    /* intialize the gryoScale value for this device */
    gyroGainScale[deviceNum] = M3D_GYROGAINSCALE;
    return deviceNum;
}

/*--------------------------------------------------------------------------
 * m3dmg_getGyroScale
 *
 * parameters   deviceNum    : the number of the sensor device (1..MAX_DEVICENUM)
 *              scaleValue   : a pointer to an integer which will contain the 
 *                             returned scale value.
 
 *
 * returns:     errorCode : the  device number if succeeded, otherwise returns an
 *                          error code as a negative value.
 *--------------------------------------------------------------------------*/

int m3dmg_getGyroScale(int deviceNum, int *scaleValue) {
    int status;
    short address, value;
    address = M3D_GYROSCALE_ADDRESS;

    status = m3dmg_getEEPROMValue(deviceNum, address, &value);
    if (status=M3D_OK)
        *scaleValue = value;
    else
        *scaleValue = 64; /* the default */
    return status;
}

/*--------------------------------------------------------------------------
 * m3dmg_sendCommand
 *
 * parameters   deviceNum      : the number of the sensor device (1..MAX_DEVICENUM)
 *              command        : a single character command (see the header file).
 *              response       : a pointer to the response buffer which will contain
 *                               the resposne data.
 *              responseLength : number of bytes allocated to the response buffer.
 *                               returned scale value.
 *
 * Note: the calling routine must allocate sufficient space for the response buffer
 *       and set the responseLength value accordingly.
 *
 * returns:     errorCode : M3D_COMM_OK, or an error code (negative value).
 *                          see m3dmgErrors.h for error codes.
 *--------------------------------------------------------------------------*/

int m3dmg_sendCommand(int deviceNum, char command, char *response, int responseLength) {
    int status;
    int checkSum;
    char outCommand = (char) command;
    int portNum = deviceMap[deviceNum];

    if (DEBUG)printf("m3dmg_send: port #: %d\n", portNum);

    /* send data out the port */
    status = sendData(portNum, &command, 1);
    if (DEBUG) printf("m3dmg_send: tx status : %d\n", status);
    if (status == M3D_COMM_OK) {
    	/* receive data if expected, evaluate checksum. */
    	if (responseLength>0) {
    		status = receiveData(portNum, &response[0], responseLength);
            if (DEBUG) printf("m3dmg_send: rx status : %d\n", status);
    		if (status==M3D_COMM_OK) {
                checkSum = convert2int(&response[responseLength-2]);
                if (checkSum != calcChecksum(&response[0], responseLength)) {
                    status = M3D_CHECKSUM_ERROR;
                }
            }
            else
                status = M3D_COMM_READ_ERROR;
    	}
    }
    if (DEBUG) printf("m3dmg_send: final status : %d\n", status);
    return status;
}

/*--------------------------------------------------------------------------
 * m3dmg_closeDevice
 *
 * parameters   deviceNum      : the number of the sensor device (1..MAX_DEVICENUM)
 *
 * Close a device, and also any underlying port.
 *--------------------------------------------------------------------------*/

void m3dmg_closeDevice(int deviceNum) {
    int portNum;
    if (deviceNum<0 || deviceNum>MAX_DEVICENUM) {
        portNum = deviceMap[deviceNum];
        closePort(portNum);
        deviceMap[deviceNum] = 0;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_openPort
 *
 * Open a serial communications port. (platform independent).
 *
 * parameters   portNum  : a port number (1..n).
 *--------------------------------------------------------------------------*/

int m3dmg_openPort(int portNum, int baudrate, int size, int parity, int stopbits) {
    int errcheck;
    int porth;

    /* open port */
    errcheck = openPort(portNum);
    if (errcheck<0) {
        return errcheck;
    }
    porth = errcheck;  /* no error, so this is the port number. */

    /* set communications parameters */
    errcheck = setCommParameters(porth, baudrate, size, parity, stopbits);
    if (errcheck!=M3D_COMM_OK) {
        return errcheck;
    }

    /* set timeouts */
    errcheck = setCommTimeouts(porth, 50, 50);
    if (errcheck!=M3D_COMM_OK) {
        return errcheck;
    }
    return porth;
}

/*--------------------------------------------------------------------------
 * m3dmg_getSerialNumber
 *
 * parameters   deviceNum : the number of the sensor device (1..n)
 *              serialNum : a pointer to an int to contain the serial number.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_getSerialNumber(int deviceNum, int *serialNum) {
    char cmd = (char) CMD_SERIAL_NUMBER;
    char  responseBuffer[128];
    int status;

    int responseLength = 5;
    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);
    if (status < 0) {
        *serialNum = 0;
        return status;
    }
    else {
        *serialNum = convert2int(&responseBuffer[1]);
        return M3D_OK;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_getFirmwareVersion
 *
 * parameters   deviceNum : the number of the sensor device (1..n)
 *              firmware  : a pointer to char string, already allocated.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *
 * WARNING - does not check to see if you have allocated enough space
 *           12 bytes for the string to contain the firmware version.
 *--------------------------------------------------------------------------*/

int m3dmg_getFirmwareVersion(int deviceNum, char *firmware) {
    char cmd = (char) CMD_FIRWARE_VERSION;
    char  responseBuffer[128];
    int firmwareNum;
    int majorNum, minorNum, buildNum;
    char *dot = ".";
    int status;

    int responseLength = 5;

    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);
    if (status < 0) {
        strcpy(firmware,"UNK");
        return status;
    }

    firmwareNum = convert2int(&responseBuffer[1]);

    if (firmwareNum > 0) {
        /* format for firmware number is #.#.## */
        majorNum = firmwareNum / 1000;
        minorNum = (firmwareNum % 1000) / 100;
        buildNum = firmwareNum % 100;
        sprintf(firmware, "%d.%d.%d", majorNum, minorNum, buildNum);
    }
    return M3D_OK;
}

/*--------------------------------------------------------------------------
 * m3dmg_getTemperature
 *
 * parameters   deviceNum : the number of the sensor device (1..n)
 *              temp      : a pointer to a float to contain the temperature
 *                          in degrees Celcius.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_getTemperature(int deviceNum, float *temp) {
    char cmd = CMD_TEMPERATURE;
    char  responseBuffer[128];
    //float convertFactor = 0.12207f;
    int status;

    int responseLength = 7;
    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        *temp = 0.0f;
        return status;
    }
    else {
       // *temp = convert2int(&responseBuffer[1])* convertFactor;
         *temp = (10*convert2int(&responseBuffer[1])-65536)*0.0008;
        return M3D_OK;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_getTimerSeconds
 *
 * parameters   deviceNum : the number of the sensor device (1..n)
 *              ticks     : a pointer to a float which will contain
 *                          the number seconds (decimal) since the
 *                          last timer clock rollover.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *
 * Note: This uses the temperature function of the sensor to
 *       retrieve the timer tick value.
 *--------------------------------------------------------------------------*/

int m3dmg_getTimerSeconds(int deviceNum, float *ticks) {
    char cmd = CMD_TEMPERATURE;
    float convertFactor = 0.0065536f;
    char  responseBuffer[128];
    int status;

    int responseLength = 7;
    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        *ticks = 0.0f;
        return status;
    }
    else {
        *ticks = convert2int(&responseBuffer[3])*convertFactor;
        return M3D_OK;
    }
}

/* 
 * To be added: 
 *     int getLastTimerTick(int deviceNum, int *ticks)
 */

/*--------------------------------------------------------------------------
 * m3dmg_getRawSensorOutput
 *
 * parameters   deviceNum : the number of the sensor device (1..n)
 *              mag       : array which will contain mag data (3 elements)
 *              accel     : array which will contain accleration data (3 elements)
 *              angRate   : array which will contain angular rate data (3 elements)
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_getRawSensorOutput(int deviceNum, float mag[], float accel[], float angRate[]) {
    char cmd = CMD_RAW_SENSOR;
    char  responseBuffer[128];
    int status;
    int i;

    int responseLength = 23;
    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        return status;
    }
    else {
        for (i=0; i<3; i++) {
            mag[i]     = (float)convert2int(&responseBuffer[1 + i*2]);
            accel[i]   = (float)convert2int(&responseBuffer[7 + i*2]);
            angRate[i] = (float)convert2int(&responseBuffer[13+ i*2]);
        }
        return M3D_OK;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_getVectors
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *              mag       : array which will contain mag data (3 elements)
 *              accel     : array which will contain accleration data (3 elements)
 *              angRate   : array which will contain angular rate data (3 elements)
 *              stableOption : a flag indicating whether data retrieved should
 *                             be instantaneous or gyro-stabilized.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_getVectors(int deviceNum, float mag[], float accel[], float angRate[], int stableOption) {
    char cmd = CMD_RAW_SENSOR;
    char  responseBuffer[128];
    float convertFactor = 8192.0f;

    float angularFactor = gyroGainScale[deviceNum]*8192.0f*0.0065536f;
    int status;
    int i;

    int responseLength = 23;

    if (stableOption==M3D_INSTANT)
        cmd = (char) CMD_INSTANT_VECTOR;
    else
        cmd = (char) CMD_GYRO_VECTOR;


    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        return status;
    }
    else {
        for (i=0; i<3; i++) {
            mag[i]     = (float) convert2short(&responseBuffer[1 + i*2])/convertFactor;
            accel[i]   = (float) convert2short(&responseBuffer[7 + i*2])/convertFactor;
            angRate[i] = (float) convert2short(&responseBuffer[13+ i*2])/angularFactor;
        }
        return M3D_OK;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_getQuaternions
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *              q            : array which will contain quaternion data.
 *              stableOption : a flag indicating whether data retrieved should
 *                             be instantaneous or gyro-stabilized.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_getQuaternions(int deviceNum, float q[], int stableOption) {
    char cmd;
    char  responseBuffer[128];
    int convertFactor = 8192;
    int status;
    int i;
    int responseLength = 13;

    if (stableOption==M3D_INSTANT)
        cmd = (char) CMD_INSTANT_QUAT;
    else
        cmd = (char) CMD_GYRO_QUAT;

    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        return status;
    }
    else {
        for (i=0; i<4; i++) {
            q[i] = (float) convert2short(&responseBuffer[1 + i*2])/convertFactor;
        }
        return M3D_OK;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_captureGyroBias
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_captureGyroBias(int deviceNum) {
    char cmd;
    char  responseBuffer[128];
    int status;
    int responseLength = 5;

    cmd = (char) CMD_CAPTURE_GYRO_BIAS;

    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0)
        return status;
    else
        return M3D_OK;
}

/*--------------------------------------------------------------------------
 * m3dmg_getOrientMatrix
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *              mx           : pointer to a 3x3 float matrix
 *                             which will contain the transform data
 *                             upon return.
 *              stableOption : a flag indicating whether data retrieved should
 *                             be instantaneous or gyro-stabilized.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_getOrientMatrix(int deviceNum, float mx[][3], int stableOption) {
    char cmd;
    char  responseBuffer[128];
    float convertFactor = 8192.0f;
    int status;
    int i, j;
    int responseLength = 23;

    if (stableOption==M3D_INSTANT)
        cmd = (char) CMD_INSTANT_OR_MATRIX;
    else
        cmd = (char) CMD_GYRO_OR_MATRIX;

    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        return status;
    }
    else {
        for (i=0; i<3; i++) {
            for (j=0;j<3;j++) {
                mx[i][j] = (float) convert2short(&responseBuffer[1+2*(j*3+i)])/convertFactor;
            }
        }
        return M3D_OK;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_getGyroStabQuatVectors
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *                             be instantaneous or gyro-stabilized.
 *              q            : array which will contain quaternion data.
 *              mag       : array which will contain mag data (3 elements)
 *              accel     : array which will contain accleration data (3 elements)
 *              angRate   : array which will contain angular rate data (3 elements)
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/


int m3dmg_getGyroStabQuatVectors(int deviceNum, 
                               float q[], 
                               float mag[],
                               float accel[],
                               float angRate[]) {

    char cmd = CMD_GYRO_QUAT_VECTOR;
    char  responseBuffer[128];
    float convertFactor = 8192.0f;
    
    float angularFactor = gyroGainScale[deviceNum]*8192.0f*0.0065536f;
    int status;
    int i;

    int responseLength = 31;

    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        return status;
    }
    else {
        /* quaternion data */
        for (i=0; i<4; i++) {
            q[i] = (float) convert2short(&responseBuffer[1 + i*2])/convertFactor;
        }

        /* vector data */
        for (i=0; i<3; i++) {
            mag[i]     = (float) convert2short(&responseBuffer[9 + i*2])/convertFactor;
            accel[i]   = (float) convert2short(&responseBuffer[15+ i*2])/convertFactor;
            angRate[i] = (float) convert2short(&responseBuffer[21+ i*2])/angularFactor;
        }

        return M3D_OK;
    }
}

/*--------------------------------------------------------------------------
 * m3dmg_getEulerAngles
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *              pitch        : pointer to a float to contain pitch angle in degrees
 *              roll         : pointer to a float to contain roll angle in degrees
 *              yaw          : pointer to a float to contain yaw angle in degrees
 *              stableOption : a flag indicating whether data retrieved should
 *                             be instantaneous or gyro-stabilized.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/

int m3dmg_getEulerAngles(int deviceNum, float *pitch, float *roll, float *yaw, int stableOption) {
    char cmd;
    char  responseBuffer[128];
    float convertFactor = (360.0/65536.0f);
    int status;
    int responseLength = 11;

    if (stableOption==M3D_INSTANT)
        cmd = (char) CMD_INSTANT_EULER;
    else
        cmd = (char) CMD_GYRO_EULER;

    status = m3dmg_sendCommand(deviceNum, cmd, responseBuffer, responseLength);

    if (status < 0) {
        return status;
    }
    else {
        *roll  = convert2short(&responseBuffer[1]) * convertFactor;
        *pitch = convert2short(&responseBuffer[3]) * convertFactor;
        *yaw   = convert2short(&responseBuffer[5]) * convertFactor;
        return M3D_OK;
    }
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

int m3dmg_getEEPROMValue(int deviceNum, short address, short *value) {
    int status;
    int portNum = deviceMap[deviceNum];
    char cmdBuffer[5];
    char dataBuffer[2];

    /* check the address range - only 256 locations permitted. */
    if (address <0 || address > 255) {
        return (int) M3D_EERPOM_ADDR_ERROR;
    }

    /* command requires an address */
    cmdBuffer[0] = CMD_SEND_EEPROM;
    cmdBuffer[1] = (char) address;


    status = sendData(portNum, &cmdBuffer[0], 2);
    if (status == M3D_COMM_OK) {
    	/* receive data if expected */
        status = receiveData(portNum, &dataBuffer[0], 2);
        if (status == M3D_COMM_OK) {
            status = M3D_OK;
            *value = convert2short(&dataBuffer[0]);
        }
        else
            status = M3D_COMM_READ_ERROR;
    }
    return status;
}

/*--------------------------------------------------------------------------
 * m3dmg_setEEPROMValue
 *
 * parameters   deviceNum    : the number of the sensor device (1..n)
 *              address      : the EEPROM address location
 *              value        : the value to program at the address specified
 *
 * Similar to m3dmg_getEEPROMValue, but the command buffer is longer here
 * to contain the outgoing data to be written.
 *
 * returns:     errorCode : M3D_OK if succeeded, otherwise returns an
 *                          error code.
 *--------------------------------------------------------------------------*/
int m3dmg_setEEPROMValue(int deviceNum, short address, short *value) {
    int status;
    int portNum = deviceMap[deviceNum];
    short valueCheck;
    char cmdBuffer[6];
    char dataBuffer[2];

    /* check the address range - only 256 locations permitted. */
    if (address <0 || address > 255) {
        return M3D_EERPOM_ADDR_ERROR;
    }

    /* command buffer */
    cmdBuffer[0] = CMD_PROG_EEPROM;
    cmdBuffer[1] = 0x71;
    cmdBuffer[2] = (char) address;
    cmdBuffer[3] = (*value & MSB_MASK) >> 8;
    cmdBuffer[4] =  *value & LSB_MASK;
    cmdBuffer[5] = (char) 0xAA;  /* end of buffer */

    status = sendData(portNum, &cmdBuffer[0], 6);
    if (status == M3D_COMM_OK) {
    	/* receive data if expected */
        status = receiveData(portNum, &dataBuffer[0], 2);
        if (status == M3D_COMM_OK) {
            status = M3D_OK;
            /* check that returned datum matches what was to be written */
            valueCheck = convert2short(&dataBuffer[0]);
            if (valueCheck != *value)
                status = M3D_EERPOM_DATA_ERROR;
            if (DEBUG) printf("eeprom check returned: %d\n", valueCheck);
        }
        else
            status = M3D_COMM_READ_ERROR;
    }
    return status;
}

/*-------------- end of m3dmgAdapter.c ----------------------*/
