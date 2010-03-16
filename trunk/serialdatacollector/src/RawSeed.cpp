/*
 * RawSeed.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: kain
 */

#include "RawSeed.h"
#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sstream>

#define MY_MASK 0777
using namespace std;

RawSeed::RawSeed() {
	// TODO Auto-generated constructor stub
	string directory("/home/kain/RawSeedDataSet");
	while(true)
	{
		cout << "Specifica il percorso (assoluto) dove creare il database" << endl << "(Default directory : " << directory << ")" << endl;
		cin >> directory;
		cout << endl;
		if(directory[0] == '/' && directory[1] != '/'){
			directory += "/RawSeedDataSet";
			break;
		}
		else
			cout << "Errore!! Reinserisci il percorso assoluto,che sia una directory Linux" << endl;
	}
	bool ishere;
	percorso = new char[directory.length()+1];
	strcpy(percorso,directory.c_str());
	if((path = opendir(percorso)) == NULL)
	{
		ishere = creaRawSeed();
		if (ishere == false)
			cout << "C'è stato un errore nella creazione del Data Set";
		else
		{
			cout << "Data Set creato con successo!";
			for(int i=0; i<4; i++)
				contatori[i] = 0;
		}
	}
	else
	{
		cout << "Struttura Raw Seed già esistente nel file system" << endl;
		inizializzaContatori();
		cout << "Percorso Raw Seed letto, pronto per salvare i file..." << endl;
		/*
		for(int i=0; i<4; i++)
			cout << contatori[i] << endl;
		*/
	}
}

RawSeed::~RawSeed() {
	// TODO Auto-generated destructor stub

}

bool RawSeed::creaRawSeed()
{
    //inizia a creare tutte le directory che serviranno per mettere su il dataset
	try
	{
		umask(0);
		int err = mkdir(percorso,MY_MASK);

		//Stampa a video il tipo di errore che ha incontrato, nel caso di -1
		if (err==-1)
			perror(percorso);

		string directory(percorso);

		//se mkdir è riuscito, allora err è = a zero
		if(err == 0)
		{
			string percorsi_prova[] = {(directory + "/Utils"),(directory + "/Calibration"),(directory + "/DataSet"), (directory + "/Docs"), (directory + "/Drawings"), (directory + "/FileFormat"), (directory + "/SensorPosition"), (directory + "/DataSet/Indoor"), (directory + "/DataSet/Mixed"), (directory + "/DataSet/Outdoor")};
			char* percorsoAttuale;
			//Crea ciclicamente le directory principali del dataset
			for(int i=0; i<10; i++){
				percorsoAttuale = new char[percorsi_prova[i].length()+1];
				strcpy(percorsoAttuale, percorsi_prova[i].c_str());
				if ((err = mkdir(percorsoAttuale, 0777))!=0){
					throw err;
				}
			}

			return true;
		}
		else
			throw err;
	}
	catch(int a)
	{

		cout<< "La directory non è stata creata. Codice Errore: " << a << endl;
		return false;
	}
}

void RawSeed::inizializzaContatori()
{
	string esaminata(percorso);
	string percorsi_esaminati[] = {(esaminata + "/Calibration"),(esaminata + "/Drawings"), (esaminata + "/FileFormat"), (esaminata + "/SensorPosition")};
	//int n_calib = 0, n_sensor_position = 0, n_draws = 0, n_fileformat = 0;
	int k = 0;
	for(int i = 0; i < 4; ++i)
	{
		//queste due variabili prendono le directory da esaminare per ogni iterazione del for
		percorso = new char[percorsi_esaminati[i].length()+1];
		strcpy(percorso, percorsi_esaminati[i].c_str());

		//apre la directory con gli oggetti da contare
		if((path = opendir(percorso)) != NULL)
		{
			dir_object = readdir(path);

			//ciclo che scorre tutto il contenuto delle directory
			while(dir_object != NULL)
			{
				++k;
				dir_object = readdir(path);
			}
		}
		contatori[i] = (k - 2); //serve il meno 2 perchè conta anche le directory . e ..
		k = 0;
	}

	/*
	 * queste due righe sono state aggiunte per reimpostare la variabile percorso
	 * al valore iniziale, cioè "percorso scelto dall'utente" + "/RawSeedDataSet"
	 * dato che alla fine del ciclo for valeva lo stesso percorso + "/SensorPosition"
	 */
	percorso = new char [esaminata.length()+1];
	strcpy(percorso, esaminata.c_str());
}

bool RawSeed::nuovaCalibrazione(int indice)
{
	//il contatore delle calibrazioni è il primo dell'array
	//int NN = contatori[0];
	int NN = contatori[indice];

	// Salvo il valore iniziale del percorso del dataset in una stringa
	string salvo(percorso);

	//codice di lavoro
	string dir(percorso);
	string num; // numero
	stringstream out;
	out << NN;
	num = out.str();

	switch(indice)
	{
		case 0:
			dir = dir + "/Calibration/Calibration_" + num;
		break;
		case 1:
			dir = dir + "/Drawings/Drawings_" + num;
		break;
		case 2:
			dir = dir + "/FileFormat/FileFormat_" + num;
		break;
		case 3:
			dir = dir + "/SensorPosition/SensorPosition_" + num;
		break;
	}

	percorso = new char[dir.length()+1];
	strcpy(percorso,dir.c_str());
	int err = mkdir(percorso, MY_MASK);

	if (err==-1)
	{
		perror(percorso);
		return false;
	}

	if(indice==0)
	{
		string dentro_calibration[] = {(dir + "/Images"),(dir + "/Images/All_images"),(dir + "/Images/Used_for_calibration"),(dir + "/Results"), (dir + "/Results/FRONTAL"), (dir + "/Results/OMNI"), (dir + "/Results/CVS")};
		char* percorso_calib;
		for(int i=0; i<7; i++)
		{
			/*percorso_calib*/
			percorso_calib = new char[dentro_calibration[i].length()+1];
			strcpy(percorso_calib, dentro_calibration[i].c_str());
			int err = mkdir(percorso_calib, MY_MASK);
			if (err==-1)
			{
				//perror(percorso);
				return false;
			}
		}
	}
	// rimetto il percorso al valore di partenza e aggiorno i contatori
	percorso = new char[salvo.length()+1];
	strcpy(percorso,salvo.c_str());
	++NN;
	contatori[indice] = NN;
	return true;
}

bool RawSeed::nuovoDataset()
{

}
