typedef struct NMEA_GPRMC{         
         double utc;     
         unsigned char status;
         double latitude;
         unsigned char latitude_hemisphere;
         double longitude;
         unsigned char longitude_hemisphere;
         double speed;
         double course;
         unsigned int utc_date;
         double magnetic_variation;
         unsigned char magnetic_variation_direction;
         unsigned char mode_indicator;
         //short unsigned int CHECKSUM;                 
         }NMEA_GPRMC;
                  
typedef struct NMEA_GPGGA{
         double utc;
         double latitude;
         unsigned char latitude_hemisphere;
         double longitude;
         unsigned char longitude_hemisphere;
         int position_fix;
         int number_of_satellites;
         double horizontal_dilution_precision;
         double antenna_height;
         unsigned char units_height;
         double geoidal_height;
         unsigned char units_separation;
         double time_last_update;
         int dgps_id;

         //DGPS1;                       
         //DGPS2;           
         short unsigned int CHECKSUM;
         }NMEA_GPGGA;
         
