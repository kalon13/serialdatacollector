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
#include <stdio.h>
#include "RawSeed.h"
#include <time.h>
#include <stdlib.h>

#define DIM 2048

using namespace std;
unsigned char data[DIM];

int main() {

	cout << "!!!Hello World!!!" << endl << "Welcome to the best program of Data Collector in the World!!!" << endl;

	RawSeed *dataset = new RawSeed();
	char scelta;
	char luogo[64];
	unsigned short tipo;
	bool flag;

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
		//cin.getline(luogo,60);
		fflush(stdin);
		//gets(luogo);
		fgets(luogo, 64, stdin);

		fflush(stdin);
		flag = dataset->setLocation(luogo);
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
