#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <termios.h>

#include "nmea.h"
#include <cstring>

#define DIM 2048

//#define SIM

unsigned short int decode(unsigned char *sentence);

void decode_GPRMC(unsigned char *sentence, NMEA_GPRMC *gprmc);
void decode_GPGGA(unsigned char *sentence, NMEA_GPGGA *gpgga);
void decode_GPGSA(unsigned char *sentence);
void decode_GPGSV(unsigned char *sentence);
int setCommParameters(int portNum,int baudrate,int dataBits,int parity,int stopBits);

int main(int argc, char *argv[])
{

  #ifdef SIM
  FILE *fp;
  #else
  int fd;
  #endif

  FILE *fp_save;
  //dati relativi alla stringa GPRMC

  NMEA_GPRMC gprmc;
  NMEA_GPGGA gpgga;

  unsigned char sentence[DIM],data[DIM];

  unsigned int byte_read,i,j,count,type_sentence;

  struct timespec start, stop, start_decode;
  char *port = "/dev/ttyS0";

  printf("Start NMEA decode\n");

  //system("cat nmea");

  #ifdef SIM
  fp = fopen("./nmea","rw");
  if (fp == NULL )
	{
	printf("Error during file opening\n");
	exit(0);
	}
  #else

    //fd = open( "/home/dati_gps", "b" );
    //system("/dev/ttyS0 baud 4800 < /dev/sr0");
  //system("cat /dev/ttyS0 > /tmp/nmea");
  //fd = open("tmp/nmea", O_RDWR);
   fd = open(port, O_RDWR  );
   setCommParameters(fd, 4800,8,0,1);
    //fp_save = fopen("/tmp/nmea","w+");

  if(fd == -1)
  {
      printf("Error during file opening\n");
      exit(0);
  }
  #endif

  i = 0;
  j = 0;
  count = 0;

 	printf("Start reading from serial port\n");
	//clock_gettime( CLOCK_REALTIME, &start);
  int g=0;
  while(1)
  {
    //usleep(1000);
    #ifdef SIM
   byte_read = fread(data,sizeof(unsigned char), DIM * sizeof(unsigned char),fp); //read data from GPS Helibot
    #else
    byte_read = read(fd,data,sizeof(data));
    #endif
//    printf("byte read: %d\n",byte_read);
    if(byte_read > 0)

      
  //lunghezza

  for(i=0; i<byte_read; i++)
  {
      if(data[i] == '$')
           {
              count++;
           }
           else
           {
              count = 0;
           }

           if(count == 1)
           {
              j = 1;
              sentence[0] = '$';
              //clock_gettime( CLOCK_REALTIME, &start_decode);
           }

           if(j > 0 && data[i]!='$')
           {
              sentence[j] = data[i];
              j++;
           }

           if(data[i] == '\n' && j > 0)
           {
              sentence[j] = '\0';
              j = -1;

//              printf("\nDecode Sentence %s",sentence);

              type_sentence = decode(sentence);
              //store data if sentence is good
              switch(type_sentence)
              {
                   case 1:
                        //printf("GPRMC Sentence\n");
                       /* decode_GPRMC(sentence,&gprmc);
                        printf("UTC:\t%lf\nSTATO:\t%c\nLATITUDINE:\t%lf\nLAT-EMI:\t%c\nLONGITUDINE:\t%lf\n"
                                "LONG-EMI:\t%c\nVELOCITA:\t%lf\nCOURSE:\t%lf\nDATA UTC\t%d\nVAR MAGNETICA:\t%lf\n"
                                "DIR VAR MAG:\t%c\nMODALITA:\t%c\n",
    gprmc.utc,gprmc.status,gprmc.latitude,gprmc.latitude_hemisphere,gprmc.longitude,gprmc.longitude_hemisphere,
    gprmc.speed,gprmc.course,gprmc.utc_date,gprmc.magnetic_variation,gprmc.magnetic_variation_direction,
    gprmc.mode_indicator);
                       /* fprintf(fp_save,"%lf,%c,%lf,%c,%lf,%c,%lf,%lf,%d,%lf,%c,%c,",
    gprmc.utc,gprmc.status,gprmc.latitude,gprmc.latitude_hemisphere,gprmc.longitude,gprmc.longitude_hemisphere,
    gprmc.speed,gprmc.course,gprmc.utc_date,gprmc.magnetic_variation,gprmc.magnetic_variation_direction,
    gprmc.mode_indicator);*/
                        break;
                   case 2:
                        //printf("GPGGA Sentence ");
                        decode_GPGGA(sentence,&gpgga);
//                        clock_gettime( CLOCK_REALTIME, &stop);
                        printf("N°SAT:\t%d\n",gpgga.number_of_satellites);
                                /*
                        printf("UTC:\t%lf\nLATITUDINE:\t%lf\nLAT-EMI:\t%c\nLONGITUDINE:\t%lf\nLONG-EMI:\t%c\n"
                                "FIX POS:\t%d\nN°SAT:\t%d\nO.D.P.:\t%lf\nALT.ANT:\t%lf\nUNIT ALT:\t%c\n"
                                "ALT GEOIDALE:\t%lf\nUNIT SEP:\t%c\nULTIMO AGG:\t%lf\nDGPS ID:\t%d\n"
                                "SATRT TV_SEC:\t%d\nSTART TV_NSEC:\t%ld\nSTOP TV_SEC:\t%d\nSTOP TV_NSEC:\t%ld\n",
    gpgga.utc,gpgga.latitude,gpgga.latitude_hemisphere,gpgga.longitude,gpgga.longitude_hemisphere,
    gpgga.position_fix,gpgga.number_of_satellites,gpgga.horizontal_dilution_precision,gpgga.antenna_height,
    gpgga.units_height,gpgga.geoidal_height,gpgga.units_separation,gpgga.time_last_update,gpgga.dgps_id,
    start_decode.tv_sec, start_decode.tv_nsec,stop.tv_sec,stop.tv_nsec);*/
                     /*   fprintf(fp_save, "%lf,%lf,%c,%lf,%c,%d,%d,%lf,%lf,%c,%lf,%c,%lf,%d,%d,%ld,%d,%ld\n",
    gpgga.utc,gpgga.latitude,gpgga.latitude_hemisphere,gpgga.longitude,gpgga.longitude_hemisphere,
    gpgga.position_fix,gpgga.number_of_satellites,gpgga.horizontal_dilution_precision,gpgga.antenna_height,
    gpgga.units_height,gpgga.geoidal_height,gpgga.units_separation,gpgga.time_last_update,gpgga.dgps_id,
    start_decode.tv_sec, start_decode.tv_nsec,stop.tv_sec,stop.tv_nsec);
                        printf("speed = %f course = %f\n",gprmc.speed * 0.514, gprmc.course);*/
                        break;

                   case 3:
                        //printf("GPGSA Sentence\n");
                        decode_GPGSA(sentence);
                        break;
                   case 4:
                        //printf("GPGSV Sentence\n");
                        decode_GPGSV(sentence);
                        break;
                   default:
                        printf("No Valid Sentences\n");
                        break;
              }
           }
  	}
  	/*if(abs(stop.tv_sec - start.tv_sec) > 120)
  	{
  		printf("Fatto\n");
  		break;
  	}*/

   }
close(fd);
   //fclose(fp_save);
   return 0;
}

//ritorna un numero da 0 a 4 in base al valore della sentence che gli passo...ancora non so bene perchè
unsigned short int decode(unsigned char *sentence)
{
   // const char sentenc = &sentence;
         if(strncmp((const char *)sentence,"$GPRMC",6) == 0)
         {
         return 1;
         }
         if(strncmp((const char *)sentence,"$GPGGA",6) == 0)
         {
         return 2;
         }
         if(strncmp((const char *)sentence,"$GPGSA",6) == 0)
         {
         return 3;
         }
         if(strncmp((const char *)sentence,"$GPGSV",6) == 0)
         {
         return 4;
         }
         else

         return 0;


}

//legge i dat contenuti in gprmc
void decode_GPRMC(unsigned char *sentence,NMEA_GPRMC *gprmc)
{
    
   /* char * pEnd;
    double x;
    char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",sentence);
  pch = strtok ((char*)sentence,",");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    *(&gprmc->utc) = strtod(pch,&pEnd);
    pch = strtok (NULL, " ,.-");
    *(&gprmc->status) = *(pch);

  }*/

    sscanf((const char*)sentence+7, "%lf,%c,%lf,%c,%lf,%c,%lf,%lf,%d,%lf,%c,%c",
    (&gprmc->utc),(&gprmc->status),(&gprmc->latitude),(&gprmc->latitude_hemisphere),(&gprmc->longitude),(&gprmc->longitude_hemisphere),
    (&gprmc->speed),(&gprmc->course),(&gprmc->utc_date),(&gprmc->magnetic_variation),(&gprmc->magnetic_variation_direction),
    (&gprmc->mode_indicator));

}

//legge i dati contenuti in gpgga
void decode_GPGGA(unsigned char *sentence, NMEA_GPGGA *gpgga)
{
    sscanf((const char*)sentence+7, "%lf,%lf,%c,%lf,%c,%d,%d,%lf,%lf,%c,%lf,%c,%lf,%d",
    &gpgga->utc,&gpgga->latitude,&gpgga->latitude_hemisphere,&gpgga->longitude,&gpgga->longitude_hemisphere,
    &gpgga->position_fix,&gpgga->number_of_satellites,&gpgga->horizontal_dilution_precision,&gpgga->antenna_height,&gpgga->units_height,&gpgga->geoidal_height,&gpgga->units_separation,&gpgga->time_last_update,&gpgga->dgps_id);
}


//NON servono a quanto pare.....

void decode_GPGSA(unsigned char *sentence)
{
     //ToDo if necessary NO
}

void decode_GPGSV(unsigned char *sentence)
{
     //ToDo if necessary NO
}

int setCommParameters(int portNum,int baudrate,int dataBits,int parity,int stopBits ) {

    struct termios portOptions;
    int portHandle;
    int status;
    int BAUD, DATABITS, PARITY, PARITYON, STOPBITS;

    portHandle = portNum;
    /*  get port options for speed, etc. */
    tcgetattr(portHandle, &portOptions);
    tcflush(portHandle, TCIFLUSH);

    /* baudrate settings */
    switch (baudrate) {
    case 38400:
        BAUD = B38400;
        break;
    case 19200:
        BAUD  = B19200;
        break;
    case 9600:
        BAUD  = B9600;
        break;
    default:
        BAUD = B4800;
    }

    /* databits settings */
    switch (dataBits) {
    case 8:
        DATABITS = CS8;
        break;
    case 7:
        DATABITS = CS7;
        break;
    default:
        DATABITS = CS8;
    }

    /* stop bits */
    switch (stopBits) {
    case 1:
        STOPBITS = 0;
        break;
    case 2:
        STOPBITS = CSTOPB;
        break;
    default:
        STOPBITS = 0;

    }

    /* parity */
    switch (parity) {
    case 0: /* no parity */
        PARITYON = 0;
        PARITY = 0;
        break;
    case 1: /* odd parity */
        PARITYON = PARENB;
        PARITY = PARODD;
        break;
    case 2: /* event parity */
        PARITYON = PARENB;
        PARITY = 0;
        break;
    default: /* none */
        PARITYON = 0;
        PARITY = 0;
    }

    /* We are not setting CRTSCTS which turns on hardware flow control */
    portOptions.c_cflag = BAUD | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;

    /*  set for non-canonical (raw processing, no echo, etc.) */
    portOptions.c_iflag = IGNPAR; /* ignore parity check */
    portOptions.c_oflag = 0; /* raw output */
    portOptions.c_lflag = 0; /* raw input  */

    cfsetospeed(&portOptions, BAUD);  /* redundant with the cflag setting, above */
    cfsetispeed(&portOptions, BAUD);
    status = tcsetattr(portHandle, TCSANOW, &portOptions);

    if (status != 0)
        return 0;
    else
        return 1;
}