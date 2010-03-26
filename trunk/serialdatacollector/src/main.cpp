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
#include <boost/thread.hpp>
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

	ThreadedDevice d[MAX_SENSOR];
	boost::thread* thr[MAX_SENSOR];
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
				<< "(0 --> per il GPS, 1 --> per la IMU, 2 --> per la web cam)" << endl;
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

				if(!ok) {
					gps->getError(&errore);
				}

				d[num_disp].identifier = GPS;
				d[num_disp].device = (void*)gps;
				d[num_disp].path = path;

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

				if(!ok) {
					imu->getError(&errore);
				}

				d[num_disp].identifier = IMU;
				d[num_disp].device = (void*)imu;
				d[num_disp].path = path;

				path = NULL;
			break;
			}
			case WEBCAM: {
				int c, wait;
				cout << "Inserisci il valore della camera che vuoi aprire." << endl << "(Un intero che corrisponde ad un identificativo del dispositivo, 0 --> Camera di Default, 1,2,3 per le successive...)" << endl;
				cin >> c;
				cout << "Inserisci i millisecondi di attesa tra lo scatto di una foto e la successiva. " << endl;
				cin >> wait;
				Camera* cam = new Camera();
				ok = cam->open_camera(c, wait);
				if(!ok)
					cout << "Errore nell'apertura della camera! " << endl;

				d[num_disp].identifier = WEBCAM;
				d[num_disp].device = (void*)cam;
				d[num_disp].path = path;

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
	}
	while(risp!='n');

	cout << "Hai inserito " << num_disp << " dispositivi" << endl;
	if(num_disp>0) {
		cout << "Avvio i thread... attendere prego..." << endl;

		//AVVIO DEI THREAD
		for(int i=0; i<num_disp; ++i) {
			switch(d[i].identifier) {
				case GPS:
					thr[i] = new boost::thread(gpsAcquisition, &d[i]);
					break;
				case IMU:
					thr[i] = new boost::thread(imuAcquisition, &d[i]);
					break;
				case WEBCAM:
					thr[i] = new boost::thread(wcAcquisition, &d[i]);
					break;
			}
			d[i].attivo = true;
		}

		cout << "Attendiamo..." << "(inserire un carattere e premete invio per terminare)" << endl;
		cin >> risp;

		//STOPPO TUTTI I THREAD
		for(int i=0; i<num_disp; ++i) {
			d[i].attivo = true;
			cout << "Sto stoppando il thread numero " << i << endl;
			thr[i]->join();
			delete(d[i].device);
		}
		delete[](thr);
	}
	else
		cout << "Non c'è nessuno dispositivo inizializzato!" << endl;

	delete(dataset);

}

void wcAcquisition(ThreadedDevice* dev){
	while(dev->attivo) {
		((Camera*)dev->device)->get_photo(dev->path);
	}
}

bool gpsAcquisition(ThreadedDevice* dev) {
	char* buffer[DIM_BUFFER_GPS];
	ofstream file;
	cout << "Il thread del gps è partito" << endl;
	while(dev->attivo) {
		for(int i=0; i<DIM_BUFFER_GPS; ++i) {
			char* x;
			if(io_mutex.try_lock())
				io_mutex.lock();
			((SerialGps*)dev->device)->getGPGGAString(&x);
			if(!io_mutex.try_lock())
				io_mutex.unlock();
			buffer[i] = x;
		}
		io_mutex.lock();
		file.open(strcat(dev->path,"/GPS.cvs"), ios::app);
		io_mutex.unlock();
		if(!file.is_open()) {
			cout << "Impossibile accedere al file" << endl;
			return false;
		}

		for(int i=0; i<8; ++i) {
			file << buffer[i] << endl;
		}
		cout << "Il gps ha scritto su file" << endl;
		io_mutex.lock();
		file.close();
		io_mutex.unlock();
	}
	cout << "gps thread ended" << endl;
	return true;
}

bool imuAcquisition(ThreadedDevice* dev) {
	char* buffer[DIM_BUFFER_IMU];
	ofstream file;
	cout << "Il thread della imu è partito"<< endl;
	while(dev->attivo) {
		for(int i=0; i<DIM_BUFFER_IMU; ++i) {
			char* x;
			io_mutex.try_lock();
			((SerialImu*)dev->device)->getRawSeedData(&x);
			io_mutex.unlock();
			buffer[i] = x;
		}
		io_mutex.try_lock();
		file.open(strcat(dev->path,"/IMU_STRETCHED.cvs"), ios::app);
		io_mutex.unlock();
		if(!file.is_open()) {
			cout << "Impossibile accedere al file";
			return false;
		}
		for(int i=0; i<32; ++i) {
			file << buffer[i];
		}
		cout << "L'imu ha scritto su file" << endl;
		io_mutex.try_lock();
		file.close();
		io_mutex.unlock();
	}
	cout << "imu thread ended" << endl;
	return true;
}
