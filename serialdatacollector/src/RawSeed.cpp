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
#include <time.h>
#include <fstream>

#define MY_MASK 0777
using namespace std;

RawSeed::RawSeed() {
	bool flag = false;
	do
	{
		do
		{
			flag = specificaPath();
		}
		while(!flag);
		if((path = opendir(percorso)) == NULL)
		{
			flag = creaRawSeed();
			if (flag == false)
				cout << "C'è stato un errore nella creazione del Data Set";
			else
			{
				cout << "Data Set creato con successo!";
				for(int i=0; i<4; i++)
					contatori[i] = 0;
				flag = true;
			}
		}
		else
		{
			cout << "Struttura Raw Seed già esistente nel file system" << endl;
			inizializzaContatori();
			cout << "Percorso Raw Seed letto, pronto per salvare i file..." << endl;
			flag = true;
		}
	}
	while(!flag);
}

RawSeed::~RawSeed() {

}

RawSeed::RawSeed(char* pathrs){
	bool flag = false;
	percorso=pathrs;
	do
	{
		if((path = opendir(percorso)) == NULL)
		{
			flag = creaRawSeed();
			if (flag == false){
				cout << "C'è stato un errore nella creazione del Data Set" << endl << "Specifica il percorso manualmente" << endl;
				flag = specificaPath();
			}
			else
			{
				cout << "Data Set creato con successo!";
				for(int i=0; i<4; i++)
					contatori[i] = 0;
				flag = true;
			}
		}
		else
		{
			cout << "Struttura Raw Seed individuata nel file system" << endl;
			inizializzaContatori();
			cout << "Percorso Raw Seed letto, pronto per salvare i file..." << endl;
			flag = true;
		}
	}
	while(!flag);
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
			//Copia il file index.php nella directory rawseed quando la crea
			string php(directory);
			php.append("/index.php");
			ifstream in ("index.php"); // open original file
			ofstream out(php.c_str()); // open target file
			out << in.rdbuf(); // read original file into target
			out.close(); // explicit close, unnecessary in this case
			in.close();// explicit close, unnecessary in this case

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
		closedir(path);
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
				return false;
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
	/*
	 * per creare un nuovo dataset, abbiamo bisogno di:
	 * 1. Prendere la locazione della raccolta dati, la data dal sistema e il tipo di raccolta(se statica o dinamica)
	 * 2. Link simbolici alle 4 directory con _NN (calibration, drawings, fileformat, sensorposition)
	 * 3. File contenenti i dati raccolti dai sensori, che sono il data set vero e proprio
	 *
	 * NOTA: il punti numero 2, porta a directory facenti parte del raw seed che NON
	 * contengono Dati raccolti, quelle 4 directory contengono file accessori per
	 * la corretta interpretazione dei dati che vanno fatti con appositi programmi
	 *
	 */

	data = new char[10];
	scriviData(data);
	string date(data);
	string salva(percorso);
	string locazione(location);
	string dataset(percorso);
	dataset = dataset + "/DataSet/Outdoor/" + locazione + "_" + date;
	if(type == 1)
		dataset += "_Static";
	else
		dataset += "_Dynamic";
	percorso = new char[dataset.length()+1];
	strcpy(percorso,dataset.c_str());
	int err = mkdir(percorso, MY_MASK);
	if (err != 0)
	{
		perror(percorso);
		return false;
	}
	else
	{
		datasetAttuale = new char[dataset.length()+1];
		strcpy(datasetAttuale, dataset.c_str());
		percorso = new char[salva.length()+1];
		strcpy(percorso, salva.c_str());
		return true;
	}
}

bool RawSeed::setLocation(char* loc)
{
	location = loc;
	string verifica(location);
	int k = verifica.length();
	for(int i=0; i < k; ++i)
		if(verifica[i] == '/')
			return false;
	return true;
}

bool RawSeed::setType(unsigned short t)
{
	try
	{
		type = t;
		return true;
	}
	catch(...)
	{
		return false;
	}
}

void RawSeed::scriviData(char* date)
{
	time (&rawtime);
	leggibile = localtime(&rawtime);
	int gg = leggibile->tm_mday;
	int mm = (leggibile->tm_mon) + 1;
	int yyyy = (leggibile->tm_year) + 1900;
	sprintf(date, "%d-%d-%d", yyyy, mm, gg);
}

bool RawSeed::salvaFile(int identifier, char* buffer[BUFFER_LENGTH])
{
	fstream file;
	string salvataggio(datasetAttuale);
	string nome_file;
	bool letturaRiuscita = true;
	switch(identifier)
	{
		// E' il GPS
		case 0:
			nome_file = salvataggio + "/GPS.csv";
		break;
		// E' la IMU
		case 1:
			nome_file = salvataggio + "/IMU_STRETCHED.csv";
		break;
		// E' la web_cam
		case 2:

		break;
	}
	file.open(nome_file.c_str(), ios::app);
	for(int i=0; i < BUFFER_LENGTH; ++i)
		if((file << buffer[i]) < 0)
			letturaRiuscita = false;
	file.close();
	return letturaRiuscita;
}

bool RawSeed::getDataSet(char** dsa) {
	/*TODO	Inserimento Controlli
	 * C'è da inserire dei controlli
	 *
	 * ma anche no invece.....
	 */
	*dsa = datasetAttuale;
	return true;
}

bool RawSeed::specificaPath(){
	string directory("/home/user/RawSeedDataSet");
	cout << "Specifica il percorso (assoluto) dove creare il database" << endl << "(Default directory : " << directory << ")" << endl;
	cin >> directory;
	cout << endl;
	if(directory[0] == '/' && directory[1] != '/'){
		directory += "/RawSeedDataSet";
		percorso = new char[directory.length()+1];
		strcpy(percorso,directory.c_str());
		return true;
	}
	else
	{
		cout << "Errore!! Reinserisci il percorso assoluto,che sia una directory Linux" << endl;
		return false;
	}
}
