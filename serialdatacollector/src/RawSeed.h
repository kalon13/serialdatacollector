/*
 * RawSeed.h
 *
 *  Created on: 12/mar/2010
 *      Author: kain
 */

#ifndef RAWSEED_H_
#define RAWSEED_H_

class RawSeed {
public:
	RawSeed();
	virtual ~RawSeed();
	bool creaRawSeed(); //crea la struttura del dataset
	bool impostaPercorso(char*);
private:
	struct stat tp;
	DIR *path;
	struct dirent *dir_object;
	char* percorso = "~/RawSeedDataSet";
	int n_calib = 0, n_sensor_position = 0, n_draws = 0, n_fileformat = 0;
};

#endif /* RAWSEED_H_ */
