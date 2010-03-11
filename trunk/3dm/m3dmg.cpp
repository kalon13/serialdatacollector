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
 * m3dmg.c
 *
 * A test application which excercises the functions of the 3DM-G
 * sensor device. This program should be compiled and linked as follows:
 *
 *  Win32 platforms:
 *      cl m3dmg.c m3dmgAdapter.c m3dmgErrors.c m3dmgSerialWin.c -o m3dmg
 *
 *  Linux platform (with gcc) :
 *      gcc -DLINUX_OS m3dmg.c m3dmgAdapter.c m3dmgErrors.c m3dmgSerialLinux.c -o m3dmg
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "m3dmgErrors.h"
#include "m3dmgSerial.h"
#include "m3dmgAdapter.h"
#include "m3dmgUtils.h"

int main(int argc, char **argv) {

    int portNum, deviceNum;
    int i, j;

/* variables used to retieve and store sensor data */
    int sn;
    float temp, ticks;
    float mag[3];       /*  magetic */
    float accel[3];     /*  acceleration */
    float angRate[3];   /*  angular rate */
    char axis[3] = { 'X', 'Y', 'Z'};
    float quat[3];      /*  quaternions */
    float xform[3][3];  /*  transformation matrix */
    float roll, pitch, yaw;
    short address, evalue;  /* EEPROM address and data value */


    char *fw;
    int errorCode;
    int tryPortNum = 1;

	printf("3DM-G C-Serial port test\n");

    if (argc > 1) {
        tryPortNum = atoi(argv[1]);
        if (tryPortNum < 1 || tryPortNum > 9) {
            printf("usage:  m3dmg <portNumber>\n");
            printf("        valid ports are 1..9, default is 1\n");
            exit(1);
        }
    }

    /* open a port, map a device */
    portNum = m3dmg_openPort(tryPortNum, 38400, 8, 0, 1);
    if (portNum<0) {
        printf("port open failed.\n");
        printf("Comm error %d, %s: ", portNum, explainError(portNum));
        return -1;
    }
    printf("Port is open as #%d - OK.\n", portNum);

    /* map device - this is required! */
    deviceNum = m3dmg_mapDevice(1, portNum);
    if (deviceNum <= 0) {
        printf("Could not map the Device to a Port\n");
        printf("%s\n", explainError(deviceNum));
        return -1;
    }
    printf("Device Number is #: %d\n\n", deviceNum);

    /* get serial number */
    errorCode = m3dmg_getSerialNumber(deviceNum, &sn);
    if (errorCode != M3D_OK)
        printf("S/N Error - : %s\n", explainError(errorCode));
    else
        printf("Serial #: %d\n", sn);
    printf("\n");

    /* get firmware number (as a string) */
    fw = (char *) malloc( (size_t)12 );
    errorCode = m3dmg_getFirmwareVersion(deviceNum, fw);
    printf("Firmware ");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else
        printf("#: %s\n", fw);
    printf("\n");

    /* temperature */
    errorCode = m3dmg_getTemperature(deviceNum, &temp);
    printf("Temperature ");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else
        printf("%3.2f degrees C.\n", temp);
    printf("\n");

    /* timer ticks in seconds */
    errorCode = m3dmg_getTimerSeconds(deviceNum, &ticks);
    printf("Timer seconds since last rollover\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else
        printf(" -> %f\n", ticks);
    printf("\n");

    /* raw sensor data */
    errorCode = m3dmg_getRawSensorOutput(deviceNum, mag, accel, angRate);
    printf("Raw sensor data\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("\tMag\t\tAccel\t\tAng Rate\n");
        for (i=0; i<3; i++) {
            printf("  %c\t: %f\t%f\t%f\n", axis[i], mag[i], accel[i], angRate[i]);
        }
    }
    printf("\n");

    /* instantaneous vector */
    errorCode = m3dmg_getVectors(deviceNum, mag, accel, angRate, M3D_INSTANT);
    printf("Instantaneous Vectors\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("\tMag\t\tAccel\t\tAng Rate\n");
        for (i=0; i<3; i++) {
            printf("  %c\t: %f\t%f\t%f\n", axis[i], mag[i], accel[i], angRate[i]);
        }
    }
    printf("\n");

    /* gyro-stabilized vector */
    errorCode = m3dmg_getVectors(deviceNum, mag, accel, angRate, M3D_STABILIZED);
    printf("Stabilized Vectors\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("\tMag\t\tAccel\t\tAng Rate\n");
        for (i=0; i<3; i++) {
            printf("  %c\t: %f\t%f\t%f\n", axis[i], mag[i], accel[i], angRate[i]);
        }
    }
    printf("\n");

    /* instantaneous quaternion */
    errorCode = m3dmg_getQuaternions(deviceNum, quat, M3D_INSTANT);
    printf("Instantaneous Quaternions\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        for (i=0; i<4; i++) {
            printf("  %d : %6.4f\n", i, quat[i]);
        }
    }
    printf("\n");

    /* gyro-stabilized quaternion */
    errorCode = m3dmg_getQuaternions(deviceNum, quat, M3D_STABILIZED);
    printf("Stabilized Quaternions\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        for (i=0; i<4; i++) {
            printf("  %d : %6.4f\n", i, quat[i]);
        }
    }
    printf("\n");

    /* instantaneous orientation matrix */
    errorCode = m3dmg_getOrientMatrix(deviceNum, &xform[0], M3D_INSTANT);
    printf("Instantaneous orientation matrix\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("-M-\t1\t\t2\t\t3\n");
        for (i=0; i<3; i++) {
            printf("  %d :", i+1);
            for (j=0;j<3;j++) {
                printf("\t%f", xform[i][j]);
            }
            printf("\n");
        }
    }
    printf("\n");

    /* Euler angles - instantaneous */
    errorCode = m3dmg_getEulerAngles(deviceNum, &pitch, &roll, &yaw, M3D_INSTANT);
    printf("Instantaneous Euler angles\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("Pitch : %4.2f\n", pitch);
        printf("Roll  : %4.2f\n", roll);
        printf("Yaw   : %4.2f\n", yaw);
    }
    printf("\n");

    /* Euler angles - gyro-stabilized */
    errorCode = m3dmg_getEulerAngles(deviceNum, &pitch, &roll, &yaw, M3D_STABILIZED);
    printf("Stabilized Euler angles\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("Pitch : %4.2f\n", pitch);
        printf("Roll  : %4.2f\n", roll);
        printf("Yaw   : %4.2f\n", yaw);
    }
    printf("\n");

   
    /* quaternions and vectors together (gyro-stabilized) */
    errorCode = m3dmg_getGyroStabQuatVectors(deviceNum, 
                                           quat, 
                                           mag, accel, angRate);

    printf("Stabilized Quaternions & Vectors\n");
	
	axis[0] = 'X';
	axis[1] = 'Y';
	axis[2] = 'Z';
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        for (i=0; i<4; i++) {
            printf("  %d : %6.4f\n", i, quat[i]);
        }
        printf("\tMag\t\tAccel\t\tAng Rate\n");
        for (i=0; i<3; i++) {
			printf("  %c\t: %f\t%f\t%f\n", axis[i], mag[i], accel[i], angRate[i]);
        }
    }
    printf("\n");

    /* gyro-stabilized orientation matrix */
    errorCode = m3dmg_getOrientMatrix(deviceNum, &xform[0], M3D_STABILIZED);
    printf("Stabilized orientation matrix\n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("-M-\t1\t\t2\t\t3\n");
        for (i=0; i<3; i++) {
            printf("  %d :", i+1);
            for (j=0;j<3;j++) {
                printf("\t%f", xform[i][j]);
            }
            printf("\n");
        }
    }
    printf("\n");

    /* read EEPROM value */
    address = 134;  /* address of the serial number */
    errorCode = m3dmg_getEEPROMValue(deviceNum, address, &evalue);
    printf("EEPROM read \n");
    if (errorCode < 0)
        printf("Error - : %s\n", explainError(errorCode));
    else {
        printf("value at addr. %d : %u \n", address, evalue);
    }

    /* free allocated memory */
    free(fw);

    /* close device */
    m3dmg_closeDevice(deviceNum);
	return(0);
}

/*-------------- end of m3dmg.c ----------------------*/
