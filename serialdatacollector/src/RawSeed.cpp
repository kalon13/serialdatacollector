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
			//percorso = strcat(directory, "/RawSeedDataSet");
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
			//crea le sotto directory nell'array di stringhe percorsi_prova
			//char* percorsi_prova[] = {"/home/kain/RawSeedDataSet/Utils","/home/kain/RawSeedDataSet/Calibration","/home/kain/RawSeedDataSet/DataSet", "/home/kain/RawSeedDataSet/Docs", "/home/kain/RawSeedDataSet/Drawings", "/home/kain/RawSeedDataSet/FileFormat", "/home/kain/RawSeedDataSet/SensorPosition", "/home/kain/RawSeedDataSet/DataSet/Indoor", "/home/kain/RawSeedDataSet/DataSet/Mixed", "/home/kain/RawSeedDataSet/DataSet/Outdoor"};

			string percorsi_prova[] = {(directory + "/Utils"),(directory + "/Calibration"),(directory + "/DataSet"), (directory + "/Docs"), (directory + "/Drawings"), (directory + "/FileFormat"), (directory + "/SensorPosition"), (directory + "/DataSet/Indoor"), (directory + "/DataSet/Mixed"), (directory + "/DataSet/Outdoor")};

			//Crea ciclicamente le directory principali del dataset
			for(int i=0; i<10; i++){
				char* percorsoAttuale; //= percorsi_prova[i];
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
		percorso = new char[percorsi_esaminati[i].length()+1];
		strcpy(percorso, percorsi_esaminati[i].c_str());
		if((path = opendir(percorso)) != NULL)
		{
			dir_object = readdir(path);
			while(dir_object != NULL)
			{
				stat(dir_object->d_name, &tp);
				if (S_ISDIR(tp.st_mode))
				{
					//l'oggetto dir_object é una directory
					++k;
				}
				dir_object = readdir(path);
			}
		}
		contatori[i] = (k - 2); //serve il meno 2 perchè conta anche le directory . e ..
		k = 0;
	}
}


