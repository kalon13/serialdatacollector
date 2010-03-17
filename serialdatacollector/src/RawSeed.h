/*
 * RawSeed.h
 *
 *  Created on: 12/mar/2010
 *      Author: kain
 */

#ifndef RAWSEED_H_
#define RAWSEED_H_

#include <fstream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <time.h>

class RawSeed {
private:

	// Variabili che servono per compiere le operazioni sulle directory
	struct stat tp;
	DIR* path;
	struct dirent *dir_object;

	// Variabili di lavoro per il Dataset
	char* percorso;
	char* location;
	char* data;
	unsigned short type;
	int contatori[4];
	bool isCalib;

	// Variabili che servono per prendere la data di sistema
	time_t rawtime;
	struct tm* leggibile;

public:
	RawSeed();
	virtual ~RawSeed();
	bool creaRawSeed(); //crea la struttura del dataset
	void inizializzaContatori();
	bool nuovaCalibrazione(int indice);
	bool nuovoDataset();
	bool setLocation(char* loc);
	bool setType(unsigned short t);
	void scriviData(char* date);
};

#endif /* RAWSEED_H_ */
