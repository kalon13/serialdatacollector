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
	//percorso = "/home/kain/RawSeedDataSet";
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
			cout << "Data Set creato con successo!";
	}
	else
		cout << "Struttura Raw Seed già esistente nel file system";
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
		int lunghezza_iniziale = directory.length();

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

