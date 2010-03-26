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
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>  // per ora ce lo metto...da verificare se serve!
#include "RawSeed.h"
#include "SerialGps.h"
#include "SerialDevice.h"
#include "SerialImu.h"
#include "main.h"
#include "Sensor.h"
#include "cv.h"
#include "highgui.h"
#include <fcntl.h>
#include "Camera.h"

using namespace std;
using namespace cv;
using namespace boost;

int main(int argc, char** argv) {

	cout << "!!!Hello World!!!" << endl << "Welcome to the best program of Data Collector in the World!!!" << endl;


	Camera cam1;
	bool cam = false;

	RawSeed *dataset = new RawSeed();
	char scelta;
	char* luogo = new char[64];
	unsigned short tipo;
	bool flag = false;
	int i=0;
	char risp;

	Device d[8];

	int threadHandle[8];
	int nt = 0;

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

	do {
		int c, wait;
		cout << "Inserisci il valore della camera che vuoi aprire." << endl << "(Un intero che corrisponde ad un identificativo del dispositivo, 0 --> Camera di Default, 1,2,3 per le successive...)" << endl;
		cin >> c;
		cout << "Inserisci i millisecondi di attesa tra lo scatto di una foto e la successiva. " << endl;
		cin >> wait;
		cam = cam1.open_camera(c, wait);
		if(cam == false)
			cout << "Errore nell'apertura della camera! " << endl;
	} while(!cam);

	// Ottengo il percorso della directory del dataset nel quale salverò le foto
	char* percorso_photo;
	dataset->getDataSet(&percorso_photo);

	cout << "Camera aperta e pronta per la raccolta visiva. Inizio a salvare le foto..." << endl;

	// Faccio partire il thread della Camera
	thread thPhoto(&startGetPhoto, &cam1, percorso_photo);
	cout << "Processo della camera partito! " << endl;

	// Istruzioni di prova per vedere se ero in grado di fermare l'acquisizione video.
	//sleep(20);
	//cam1.stop_camera();

	// Ciclo di inserimento dei dispositivi seriali collegati al pc. Immetto e istanzio i vari sensori
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
				//boost::thread thrGps(&gpsAcquisition, &d[nt]);
				//gpsAcquisition((void*)&d[nt]);
			break;
			}
			case 1:{
				SerialImu* imu = new SerialImu();
				d[nt].identifier = id;
				d[nt].pathtofile = strcat(path, "/IMU_STRETCHED.csv");
				d[nt].dev = imu;
				flag=imu->openCommunication(percorso_porta);
				boost::thread thrImu(&imuAcquisition, &d[nt]);
				//imuAcquisition((void*)&d[nt]);
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
	}
	while(risp!='n');

	cout << "Sono stati inizializzati correttamente " << nt-1 << " dispositivi." << endl;
	cout << "Avvio dei thread di lettura dai dispositivi" << endl;


	cout << "Attendiamo..." << endl;
	cin >> risp;

	cam1.stop_camera();
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

void gpsAcquisition(Device* gpsx) {
	//Device* gps = (Device*)gpsx;
	fstream file;
	bool letturaRiuscita;
	for(int i=0; i<10; ++i) {
		letturaRiuscita = true;
		for(int j=0; j<32; ++j) {
			char* x;
			((SerialGps*)gpsx->dev)->getGPGGAString(&x);
			gpsx->buffer[j] = x;
		}
		file.open(gpsx->pathtofile, ios::app);
		for(int k=0; k< 32; ++k)
			if((file >> gpsx->buffer[k]) < 0)
				letturaRiuscita = false;
		file.close();
		delete(gpsx->buffer);
	}
	cout << "Il gps ha fatto" << endl;
	//return (void *)1;
}

void imuAcquisition(Device* imux) {
	//Device* imu = (Device*)imux;
	ofstream file;
	bool letturaRiuscita;
	for(int i=0; i<10; ++i) {
		letturaRiuscita = true;
		char* x;
		for(int j=0; j<32; ++j) {

			((SerialImu*)imux->dev)->getRawSeedData(&x);
			imux->buffer[j] = x;
		}
		file.open(imux->pathtofile, ios::app);
		for(int k=0; k< 32; ++k)
			if((file << imux->buffer[k]) < 0)
				letturaRiuscita = false;
		file.close();
		delete(imux->buffer);
	}
	file.close();
	cout << "L'imu ha fatto" << endl;
	//return (void*)1;
	//pthread_exit(NULL);
}

void startGetPhoto(Camera* cam, char* photo){
	cam->get_photo((void*) photo);
}
