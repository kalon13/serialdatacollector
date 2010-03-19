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
#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

int main() {

	cout << "!!!Hello World!!!" << endl << "Welcome to the best program of Data Collector in the World!!!" << endl;

	 VideoCapture cap(0); // open the default camera
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
	    }

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
		flag_comunication = dummy->openCommunication(percorso_porta,4800,8,0,1);
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
