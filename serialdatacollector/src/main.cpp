//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
//#include <stdio.h>
#include <time.h>
#include <stdlib.h>
//#include <boost/thread.hpp>
#include "RawSeed.h"
#include "SerialGps.h"
//#include "SerialDevice.h"
#include "SerialImu.h"
#include "main.h"
#include "cv.h"
#include "highgui.h"
//#include <fcntl.h>
#include "Camera.h"
#include <pthread.h>
#include <vector>

using namespace std;
using namespace cv;
using namespace boost;

int main(int argc, char** argv) {

	cout << "!!!Hello World!!!" << endl << "Welcome to the best program of Data Collector in the World!!!" << endl;

	d.reserve(3);
	pthread_t thr[MAX_SENSOR];
	int num_disp = 0;

	RawSeed *dataset = new RawSeed();
	char scelta;
	char* luogo = new char[64];
	unsigned short tipo;
	bool flag = false;

	char risp;


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


	// Ciclo di inserimento dei dispositivi seriali collegati al pc. Immetto e istanzio i vari sensori
	do
	{
		string porta;
		char* percorso_porta;
		char* path;

		bool ok = false;
		char* errore;

		dataset->getDataSet(&path);
		int id;
		cout << "Specifica l'identificatore del dispositivo che intendi utilizzare nella raccolta dati " << endl
				<< "(0 --> per il GPS, 1 --> per la IMU, 2 --> per la cam)" << endl;
		cin >> id;
		switch (id)
		{
			case GPS:{
				cout << "Inserisci i parametri della porta con la quale vuoi comunicare con il dispositivo " << endl;
				cout << "Nome Porta : " << endl;
				cin >> porta;
				percorso_porta = new char[porta.length()+1];
				strcpy(percorso_porta,porta.c_str());

				SerialGps* gps = new SerialGps();
				ok = gps->openCommunication(percorso_porta);

				if(ok) {
					ThreadedDevice td;
					td.identifier = GPS;
					td.device = (void*)gps;
					td.stato = PRONTO;

					string pcomp(path);
					pcomp.append("/GPS.cvs");
					char* pcomp2 = new char[200];
					strcpy(pcomp2, pcomp.c_str());

					td.path = pcomp2;

					d.push_back(td);
				}
				else
					gps->getError(&errore);

				path = NULL;
				break;
			}
			case IMU:{
				cout << "Inserisci i parametri della porta con la quale vuoi comunicare con il dispositivo " << endl;
				cout << "Nome Porta : " << endl;
				cin >> porta;
				percorso_porta = new char[porta.length()+1];
				strcpy(percorso_porta,porta.c_str());

				SerialImu* imu = new SerialImu();
				ok = imu->openCommunication(percorso_porta);

				if(ok) {
					ThreadedDevice td;
					td.identifier = IMU;
					td.device = (void*)imu;
					td.stato = PRONTO;

					string pcomp(path);
					pcomp.append("/IMU_STRETCHED.cvs");
					char* pcomp2 = new char[200];
					strcpy(pcomp2, pcomp.c_str());
					td.path = pcomp2;

					d.push_back(td);
				}
				else
					imu->getError(&errore);

				path = NULL;
				break;
			}
			case CAM: {
				int c, wait;
				cout << "Inserisci il valore della camera che vuoi aprire." << endl << "(Un intero che corrisponde ad un identificativo del dispositivo, 0 --> Camera di Default, 1,2,3 per le successive...)" << endl;
				cin >> c;
				cout << "Inserisci i millisecondi di attesa tra lo scatto di una foto e la successiva. " << endl;
				cin >> wait;
				Camera* cam = new Camera();
				ok = cam->open_camera(c, wait);
				if(ok) {
					ThreadedDevice td;
					td.identifier = CAM;
					td.device = (void*)cam;
					td.path = path;
					td.stato = PRONTO;

					d.push_back(td);
				}
				else
					cout << "Errore nell'apertura della camera! " << endl;

				path = NULL;
			}
		}

		if(ok)
			++num_disp;
		else
			cout << errore << endl;

		cout << "Vuoi inserire un altro dispositivo? (s/n) " << endl;
		cin >> risp;
		fflush(NULL);
		//cin.clear();
	}
	while(risp!='n');

	cout << "Hai inserito " << num_disp << " dispositiv" << (num_disp==1?"o":"i") << endl;
	if(num_disp>0) {
		cout << "Avvio i thread... attendere prego..." << endl;

		//AVVIO DEI THREAD
		for(int i=0; i<num_disp; ++i) {
			d[i].stato = ATTIVO;
			switch(d[i].identifier) {
				case GPS:
					pthread_create(&thr[i], NULL, gpsAcquisition, (void*) i);
					break;
				case IMU:
					pthread_create(&thr[i], NULL, imuAcquisition, (void*) i);
					break;
				case CAM:
					pthread_create(&thr[i], NULL, camAcquisition, (void*) i);
					break;
				default:
					break;
			}
		}

		cmdShell();
	}
	else
		cout << "Non c'è nessuno dispositivo inizializzato!" << endl;


	/*---------------------------------------------
	 * ...............FINE DEL PROGRAMMA............
	 * 1)Chiudo tutti i thread vivi
	 * 2)Pulisco le variabili in memoria
	 * -------------------------------*/
	for(int i=0; i<num_disp; ++i) {
		if(d[i].stato!=TERMINATO) {
			d[i].stato = TERMINATO;
			cout << "Sto stoppando il thread numero " << i << endl;
			pthread_join(thr[i], NULL);
		}
	}
	d.clear();
	delete(dataset);

}

void cmdShell() {

	//cin.clear();
	char cmd;
	cout << "Attendiamo..." << "(inserire un carattere e premete invio per terminare)" << endl;
	cin >> cmd;
	//cin.get();

}

void* camAcquisition(void* i){
	int n = (long)i;
	ThreadedDevice dev = d.at(n);
	while(dev.stato!=TERMINATO) {
		while(dev.stato==ATTIVO) {
			((Camera*)dev.device)->get_photo(dev.path);
			 dev = d.at(n);
		}
		 dev = d.at(n);
	}
	return (void*) true;
}

void* gpsAcquisition(void* i) {
	char** buffer = new char*[DIM_BUFFER_GPS];
	ofstream file;
	int n = (long)i;
	ThreadedDevice dev = d.at(n);

	cout << "Il thread del gps è partito" << endl;
	while(dev.stato!=TERMINATO) {
		while(dev.stato==ATTIVO) {
			dev = d.at(n);
			int righe_scritte = 0;
			for(int i=0; i<DIM_BUFFER_GPS; ++i) {
				char* x;
				if(((SerialGps*)dev.device)->getGPGGAString(&x)) {
					buffer[i] = x;
					//cout << "Sto leggendo la riga # " << i <<endl;
					++righe_scritte;
				}
				//else
				//	cout << "Non ho letto la riga # " << i << endl;
			}
			file.open(dev.path, ios::app);
			if(!file.is_open()) {
				cout << "Impossibile accedere al file" << endl;
				return (void*) false;
			}
			//cout << "Ho aperto il file " << dev.path << endl;
			for(int i=0; i<righe_scritte; ++i) {
				//cout << "Sto scrivendo la riga # " << i << endl;
				file << buffer[i] << endl;
			}
			cout << "Il gps ha scritto su file" << endl;
			file.close();
		}
		dev = d.at(n);
	}

	delete [] *buffer;
	cout << "gps thread ended" << endl;
	return (void*) true;
}

void* imuAcquisition(void* i) {
	char** buffer = new char*[DIM_BUFFER_IMU];
	ofstream file;
	int n = (long) i;
	ThreadedDevice dev = d.at(n);

	cout << "Il thread della imu è partito"<< endl;
	while(dev.stato!=TERMINATO) {
		while(dev.stato==ATTIVO) {
			dev = d.at(n);
			int righe_scritte = 0;
			for(int i=0; i<DIM_BUFFER_IMU; ++i) {
				char* x;
				if(((SerialImu*)dev.device)->getRawSeedData(&x)) {
					buffer[i] = x;
					//cout << "Sto leggendo la riga # " << i <<endl;
					++righe_scritte;
				}
				//else
				//	cout << "Non ho letto la riga # " << i << endl;
			}
			file.open(dev.path, ios::app);
			if(!file.is_open()) {
				cout << "Impossibile accedere al file";
				return (void*) false;
			}
			//cout << "Ho aperto il file " << dev.path << endl;
			for(int i=0; i<righe_scritte; ++i) {
				//cout << "Sto scrivendo la riga # " << i << endl;
				file << buffer[i] << "\n";
			}
			cout << "L'imu ha scritto su file" << endl;
			file.close();
		}
		dev = d.at(n);
	}

	delete [] *buffer;
	cout << "imu thread ended" << endl;
	return (void*) true;
}
