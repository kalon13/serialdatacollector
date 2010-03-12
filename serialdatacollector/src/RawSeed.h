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

class RawSeed {
private:
	struct stat tp;
	DIR* path;
	struct dirent *dir_object;
	char* percorso;
	int n_calib, n_sensor_position, n_draws, n_fileformat;
public:
	RawSeed();
	virtual ~RawSeed();
	bool creaRawSeed(); //crea la struttura del dataset
};

#endif /* RAWSEED_H_ */
