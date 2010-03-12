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

RawSeed::RawSeed() {
	// TODO Auto-generated constructor stub
	 if((path = opendir(percorso)) == NULL)
	        creaRawSeed();
}

RawSeed::~RawSeed() {
	// TODO Auto-generated destructor stub
}

bool RawSeed::creaRawSeed()
{
    //inizia a creare tutte le directory che serviranno per mettere su il dataset

    //crea la directory ~/RawSeedDataSet
    mkdir(percorso,0777);

    //crea le sotto directory
    char** percorsi_prova = {"~/RawSeedDataSet/Utils","~/RawSeedDataSet/Calibration","~/RawSeedDataSet/DataSet", "~/RawSeedDataSet/Docs", "~/RawSeedDataSet/Drawings", "~/RawSeedDataSet/FileFormat", "~/RawSeedDataSet/SensorPosition"};

    while (percorsi_prova == NULL)
    {

    }
}

// Servirà se faremo impostare dall'utente il percorso iniziale del dataset
bool RawSeed::impostaPercorso(char*)
{

}
