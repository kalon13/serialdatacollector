//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "RawSeed.h"
#include "SerialGps.h"
#include "SerialDevice.h"
#include "SerialImu.h"
#include "main.h"
#include "Sensor.h"
//#include "cv.h"
//#include "highgui.h"

using namespace std;
//using namespace cv;

int main() {

	cout << "!!!Hello World!!!" << endl << "Welcome to the best program of Data Collector in the World!!!" << endl;

		/*VideoCapture cap(0); // open the default camera
	    if(!cap.isOpened())  // check if we succeeded
	        return -1;

	    Mat edges;
	    namedWindow("edges",1);
	    for(;;)
	    {
	        Mat frame;
	        cap >> frame; // get a new frame from camera
	        cvtColor(frame, edges, CV_XYZ2RGB);
	        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
	        //Canny(edges, edges, 0, 30, 3);
	        imshow("edges", edges);
	        if(waitKey(30) >= 0) break;
	    }*/

	RawSeed *dataset = new RawSeed();
	char scelta;
	char* luogo = new char[64];
	unsigned short tipo;
	bool flag = false;
	int i=0;
	char risp;

	Device d[8];

	int threadHandle[8];
	pthread_t thread[8];
	pthread_attr_t attr;
	size_t stacksize;
	int nt = 0;

	/*
	pthread_attr_init(&attr);
	pthread_attr_getstacksize (&attr, &stacksize);
	cout << "Default Stacksize: " << stacksize << endl;
	stacksize = sizeof(char) * 128 + sizeof(Device);
	cout << "Quantità di memoria richiesta: " << stacksize << endl;
	pthread_attr_setstacksize (&attr, stacksize);
	cout << "Operazione eseguita...." << endl;
	 */

	while(true)
	{
		bool riuscita;
		cout << "Vuoi inserire una nuova calibrazione?(S o N)" << endl;
		cin >> scelta;
		if(scelta == 'S')
		{
			int index = 0;
			while(true)
			{
				cout << "Inserire un valore corrispondente alla nuova directory creata : " << endl << "(0 --> nuova calibrazione, 1 --> nuovo disegno," << endl << "2 --> nuovo formato file, 3 --> nuova posizione sensori)" << endl << "Default: 0" << endl;
				cin >> index;
				if(index >= 0 && index <= 3)
					break;
			}
			riuscita = dataset->nuovaCalibrazione(index);
			if(riuscita == true)
				cout << "Directory creata con successo" << endl;
			else
				cout << "Errore nella creazione directory" << endl;
		}
		if(scelta == 'N')
			break;
	}

	do
	{
		cout << "Specificare il luogo in cui si intende raccogliere i dati: " << endl;
		cin.ignore();
		cin.getline(luogo,64);
		flag = dataset->setLocation(luogo);
		if(flag == false)
			cout << "Errore nell'inserimento del luogo, NON puoi mettere il carattere speciale /!!" << endl;
	}
	while(!flag);

	do
	{
		cout << "Specificare il tipo di raccolta dati compiuta dal Robot" << endl << "(1 --> Raccolta dati Statica, 2 --> Raccolta dati Dinamica)" << endl;
		cin >> tipo;
		flag = dataset->setType(tipo);
	}
	while(!flag);

	flag = dataset->nuovoDataset();
	if(flag)
		cout << endl << "Directory per la raccolta dati creata con successo!!" << endl;
	else
		cout << endl << "C'è stato un errore nella creazione della directory!! " << endl;

	do
	{
		string porta;
		char* percorso_porta;
		char* path;
		dataset->getDataSet(&path);
		int id;
		cout << "Specifica l'identificatore del dispositivo che intendi utilizzare nella raccolta dati " << endl << "(0 --> per il GPS, 1 --> per la IMU, 2 --> per la web cam)" << endl;
		cin >> id;
		cout << "Inserisci i parametri della porta con la quale vuoi comunicare con il dispositivo " << endl;
		cout << "Nome Porta : " << endl;
		cin >> porta;
		percorso_porta = new char[porta.length()+1];
		strcpy(percorso_porta,porta.c_str());
		switch (id)
		{
			case 0:{
				SerialGps* gps = new SerialGps();
				d[nt].identifier = id;
				d[nt].pathtofile = strcat(path, "/GPS.csv");
				d[nt].dev = gps;
				flag=gps->openCommunication(percorso_porta);
				//threadHandle[nt] = pthread_create(&thread[nt], NULL, gpsAcquisition, (void*)&d[nt]);
				gpsAcquistion((void*)&d[nt]);
			break;
			}
			case 1:{
				SerialImu* imu = new SerialImu();
				d[nt].identifier = id;
				d[nt].pathtofile = strcat(path, "/IMU_STRETCHED.csv");
				d[nt].dev = imu;
				flag=imu->openCommunication(percorso_porta);
				//threadHandle[nt] = pthread_create(&thread[nt], NULL, imuAcquisition, (void*)&d[nt]);
				imuAcquisition((void*)&d[nt]);
			break;
			}
		}
		if(flag == false) {
			char* e;
			d[nt].dev->getError(&e);
			cout << e << endl;
		}
		else
			++nt;

		cout << "Vuoi inserire un altro dispositivo? (s/n) " << endl;
		cin >> risp;
		//fflush(stdin);
	}
	while(risp!='n');

	cout << "Sono stati inizializzati correttamente " << nt-1 << " dispositivi." << endl;
	cout << "Avvio dei thread di lettura dai dispositivi" << endl;
	for(int i=0; i<nt; ++i) {
		pthread_detach(thread[i]);
	}

	cout << "Attendiamo..." << endl;
	cin >> risp;

	delete(dataset);

	/*SerialGps *gp = new SerialGps();
	char* x;
	gp->openCommunication("/dev/ttyUSB0");
	char* er;
	if(gp->getGPGGAString(&x))
		cout << x;
	else {
		gp->getError(&er);
		cout << er;
	}

	char* c;
	SerialImu *imu = new SerialImu();
	imu->openCommunication("/dev/ttyS0");
	if(imu->getRawSeedString(&c))
		cout << c;*/
}

void* gpsAcquisition(void* gpsx) {
	Device* gps = (Device*)gpsx;
	fstream file;
	bool letturaRiuscita;
	for(int i=0; i<10; ++i) {
		letturaRiuscita = true;
		for(int j=0; j<32; ++j) {
			char* x;
			((SerialGps*)gps->dev)->getGPGGAString(&x);
			gps->buffer[j] = x;
		}
		file.open(gps->pathtofile, ios::app);
		for(int k=0; k< 32; ++k)
			if((file >> gps->buffer[k]) < 0)
				letturaRiuscita = false;
		file.close();
		delete(gps->buffer);
	}
	cout << "Il gps ha fatto" << endl;
	return (void *)1;
}
void* imuAcquisition(void* imux) {

	Device* imu = (Device*)imux;
	ofstream file;
	bool letturaRiuscita;
	for(int i=0; i<10; ++i) {
		letturaRiuscita = true;
		char* x;
		for(int j=0; j<32; ++j) {

			((SerialImu*)imu->dev)->getRawSeedData(&x);
			imu->buffer[j] = x;
		}
		file.open(imu->pathtofile, ios::app);
		for(int k=0; k< 32; ++k)
			if((file << imu->buffer[k]) < 0)
				letturaRiuscita = false;
		file.close();
		delete(imu->buffer);
	}
	/*char* paths = imu->pathtofile;
	char* x;
	file.open(imu->pathtofile, ios_base::app); // QUI
	for(int i=0; i<32; ++i) {
		//file.open(imu->pathtofile, ios::app);
		x = new char[128];
		((SerialImu*)imu->dev)->getRawSeedData(&x);
		cout << x;
		if((file << x) < 0)
			letturaRiuscita = false;
		//file.close();
	}*/
	file.close();
	cout << "L'imu ha fatto" << endl;
	//return (void*)1;
	pthread_exit(NULL);
}
