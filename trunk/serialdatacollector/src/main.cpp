//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "SerialGps.h"
#include "SerialDevice.h"
#include "SerialImu.h"
#include <stdio.h>
#include "RawSeed.h"
#include <time.h>
#include <stdlib.h>
#include "main.h"
#include "Sensor.h"

using namespace std;

int main() {

	cout << "!!!Hello World!!!" << endl << "Welcome to the best program of Data Collector in the World!!!" << endl;

	RawSeed *dataset = new RawSeed();
	char scelta;
	char* luogo = new char[64];
	unsigned short tipo;
	bool flag = false, flag_comunication = false;
	int i=0;
	char* risp;

	// Array di Sensor, che contiene le stringhe che escono dai dispositivi
	Sensor *s[MAX_SENSOR];

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
		SerialDevice* dummy;
		int id;
		cout << "Specifica l'identificatore del dispositivo che intendi utilizzare nella raccolta dati " << endl << "(0 --> per il GPS, 1 --> per la IMU, 2 --> per la web cam)" << endl;
		cin >> id;
		cout << "Inserisci i parametri della porta con la quale vuoi comunicare con il dispositivo " << endl;
		cout << "Nome Porta : " << endl;
		cin >> porta;
		percorso_porta = new char[porta.length()+1];
		strcpy(percorso_porta,porta.c_str());
		s[i] = new Sensor();
		flag = s[i]->setSensor(id);
		if(flag == false)
			cout << "Errore nella creazione degli elementi dei sensori. " << endl;
		s[i]->getDev(&dummy);
		switch (id)
		{
			case 0:{
				SerialGps* gps;
				gps = (SerialGps *)dummy;
				flag_comunication = gps->openCommunication(percorso_porta);
			break;
			}
			case 1:{
				SerialImu* imu;
				imu = (SerialImu *)dummy;
				flag_comunication = imu->openCommunication(percorso_porta);
			break;
			}
		}
		if(flag_comunication == false)
			cout << "Errore nell'apertura della porta! " << endl;
	}
	while(!(flag && flag_comunication));

	delete(dataset);

/*
	int byte_read;
	unsigned short int d;
	SerialGps gps1;
	char *port = "/dev/ttyUSB0";
	if(!gps1.openCommunication(port,4800,8,0,1))
	{
		//char *er;
		cout << gps1.getError();
		//cout << er;
	}
	gps1.setDebug(true);
	int i=0;
	while(i<10)
	{
		byte_read=gps1.readData(&data[0],200);
		gps1.store_data(&data[0],byte_read);
		++i;
	}
	return 0;
*/
}
