//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "RawSeed.h"
#include "SerialGps.h"
#include "SerialImu.h"
#include "main.h"
#include "Camera.h"
#include "Hokuyo.h"
#include <pthread.h>
#include <vector>
#include <string>

using namespace std;
using namespace cv;
using namespace boost;

int main(int argc, char* argv[]) {

	d.reserve(3);
	num_disp = 0;

	if(--argc>0)
		dataset = new RawSeed(argv[1]);
	else
		dataset = new RawSeed();

	char* luogo = new char[64];
	unsigned short tipo;
	bool flag = false;

	/*TODO: Miglioramento
	 * Vedere se si possono mettere tutte queste richieste come comandi
	 */
	do
	{
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
	}
	while(!flag);

	/*-------------------------------------------
	 * Avvio shell dei comandi
	 -------------------------------------------*/
	Shell();

	/*---------------------------------------------
	 * FINE DEL PROGRAMMA
	 * 1)Chiudo tutti i thread vivi
	 * 2)Pulisco le variabili in memoria
	 * -------------------------------*/
	cout << "Chiusura del programma in corso... attendere prego" << endl;
	if(some_thread_active()){
		for(int i=0; i<num_disp; ++i) {
			if(d[i].stato!=TERMINATO) {
				d[i].stato = TERMINATO;
				cout << "Sto stoppando il thread numero " << i << endl;
				pthread_join(thr[i], NULL);
			}
		}
	}
	cout << "Arrivederci" << endl;
	d.clear();
	delete(dataset);

}

void* camAcquisition(void* i){
	/*TODO: Miglioramento
	 * Vedere se si riesce a far salvare ogni tanto una foto con un nome specifico
	 */
	int n = (long)i;
	ThreadedDevice dev = d.at(n);
	while(dev.stato!=TERMINATO) {
		while(dev.stato==ATTIVO) {
			((Camera*)dev.device)->getPhoto(dev.path);
			 dev = d.at(n);
		}
		 dev = d.at(n);
	}
	return (void*) true;
}

void* hokAcquisition(void* i){
	/*TODO: nominal scan frequency
	 * Inserire la nominal scan frequency
	 */
	/*TODO: verifica timestamp
	 * Verificare il formato del timestamp dell'hokuyo
	 */
	vector<string> buffer;
	buffer.reserve(DIM_BUFFER_HOKUYO);
	int n = (long)i;
	ThreadedDevice dev = d.at(n);
	ofstream file;
	stringstream out;
	lvec vet;
	long timestamp;
	int righe_scritte;

		if(dev.debug>0)
			cout << "Il thread dell'hokuyo è partito"<< endl;
		while(dev.stato!=TERMINATO) {
			while(dev.stato==ATTIVO) {
				righe_scritte = 0;
				for(int j=0; j<DIM_BUFFER_HOKUYO; ++j) {
					int t = ((Hokuyo*)dev.device)->readData(&vet, &timestamp);
					if(t > 0){
						out << timestamp;
						for(int k=0; k<t; ++k)
							out << "," << vet[k];
						out << endl;
						++righe_scritte;
						buffer.push_back(out.str());
						if(dev.debug>1)
							cout << "Sto leggendo la riga # " << i <<endl;
					}
					else if(dev.debug>1)
						cout << "Non ho letto la riga # " << i <<endl;
				}

				file.open(dev.path, ios::app);
				if(!file.is_open()) {
						cout << "Impossibile accedere al file" << dev.path;
					return (void*) false;
				}
				for(int j=0; j<righe_scritte; ++j) {
					if(dev.debug>1)
						cout << "Sto scrivendo la riga # " << i << endl;
					file << buffer[j] << "\n";
				}

				if(dev.debug>0)
					cout << "L'hokuyo ha scritto su file" << endl;
				file.close();

				dev = d.at(n);
			}
			dev = d.at(n);
		}
	buffer.clear();
	if(dev.debug>0)
		cout << "Hokuyo thread ended" << endl;

	return (void*) true;
}

void* gpsAcquisition(void* i) {
	/*TODO: nominal scan frequency
	 * Inserire la nominal scan frequency
	 */
	vector<string> buffer;
	buffer.reserve(DIM_BUFFER_GPS);
	ofstream file;
	int n = (long)i;
	ThreadedDevice dev = d.at(n);

	int conta=0;
	ofstream file_ultimo;
	string p1(dev.path);
	p1.append(".u");
	char* p2 = new char[200];
	strcpy(p2, p1.c_str());

	if(dev.debug>0)
		cout << "Il thread del gps è partito"<< endl;
	while(dev.stato!=TERMINATO) {
		while(dev.stato==ATTIVO) {
			dev = d.at(n);
			int righe_scritte = 0;
			for(int i=0; i<DIM_BUFFER_GPS; ++i) {
				char* x;
				struct timespec ts;
				clock_gettime(CLOCK_REALTIME, &ts);
				if(((SerialGps*)dev.device)->getData(&x, GPGGA)) {
					stringstream ss;
					ss << (int)ts.tv_sec << "." << (int)(ts.tv_nsec/100) << "," << x;
					buffer.push_back(ss.str());
					if(dev.debug>1)
						cout << "Sto leggendo la riga # " << i <<endl;
				}
				else
					if(dev.debug>1)
						cout << "Non ho letto la riga # " << i << endl;
			}
			file.open(dev.path, ios::app);
			if(!file.is_open()) {
					cout << "Impossibile accedere al file" << dev.path;
				return (void*) false;
			}
			for(int i=0; i<righe_scritte; ++i) {
				if(dev.debug>1)
					cout << "Sto scrivendo la riga # " << i << endl;
				file << buffer[i] << "\n";
			}

			if(dev.debug>0)
				cout << "Il gps ha scritto su file" << endl;
			file.close();

			if(conta==8) {
				file_ultimo.open(p2, ios::out);
				for(int i=0; i<righe_scritte; ++i) {
					file_ultimo << buffer[i] << "\n";
				}
				file_ultimo.close();
				conta=-1;
			}
			++conta;
		}
		dev = d.at(n);
	}

	buffer.clear();
	if(dev.debug>0)
		cout << "Gps thread ended" << endl;
	return (void*) true;
}

void* imuAcquisition(void* i) {
	/*TODO: nominal scan frequency
	 * Inserire la nominal scan frequency
	 */
	vector<string> buffer;
	buffer.reserve(DIM_BUFFER_IMU);
	ofstream file;
	int n = (long) i;
	ThreadedDevice dev = d.at(n);

	int conta=0;
	ofstream file_ultimo;
	string p1(dev.path);
	p1.append(".u");
	char* p2 = new char[200];
	strcpy(p2, p1.c_str());

	if(dev.debug>0)
		cout << "Il thread della imu è partito"<< endl;
	while(dev.stato!=TERMINATO) {
		while(dev.stato==ATTIVO) {
			dev = d.at(n);
			int righe_scritte = 0;
			for(int i=0; i<DIM_BUFFER_IMU; ++i) {
				char* x;
				struct timespec ts;
				clock_gettime(CLOCK_REALTIME, &ts);
				if(((SerialImu*)dev.device)->getData(&x)) {
					stringstream ss;
					ss << (int)ts.tv_sec << "." << (int)(ts.tv_nsec/100) << "," << x;
					buffer.push_back(ss.str());
					++righe_scritte;
					if(dev.debug>1)
						cout << "Sto leggendo la riga # " << i <<endl;
				}
				else
					if(dev.debug>1)
						cout << "Non ho letto la riga # " << i << endl;
			}
			file.open(dev.path, ios::app);
			if(!file.is_open()) {
					cout << "Impossibile accedere al file" << dev.path;
				return (void*) false;
			}
			for(int i=0; i<righe_scritte; ++i) {
				if(dev.debug>1)
					cout << "Sto scrivendo la riga # " << i << endl;
				file << buffer[i] << "\n";
			}

			if(dev.debug>0)
				cout << "L'imu ha scritto su file" << endl;
			file.close();

			if(conta==16) {
				file_ultimo.open(p2, ios::out);
				for(int i=0; i<righe_scritte; ++i) {
					file_ultimo << buffer[i] << "\n";
				}
				file_ultimo.close();
				conta=-1;
			}
			++conta;
		}
		dev = d.at(n);
	}

	buffer.clear();
	if(dev.debug>0)
		cout << "Imu thread ended" << endl;
	return (void*) true;
}


void* Shell() {
	bool quit = false;
	cout << "Benvenuto nella console di comando di serialdatacollector!" << endl;
	cout << "Non sono stati inseriti dispositivi. Per inserirne uno digitare 'insert'\n";
	cout << "Per ricevere aiuto digitare 'help'\n\n";
	bool invio = false;
	do {
		string riga, token;
		svec parameters;
		string cmd;
		if(!invio) cout << ">";
		invio=false;

		getline(cin, riga);
		istringstream iss(riga);
		int arg=0;
		while(getline(iss, token, ' ') )
		{
			if(arg==0)
				cmd = token;
			else
				parameters.push_back(token);
			arg++;
		}

		/*
		 * PARSE COMMAND
		 * */
		if(cmd.compare("insert")==0)
			cmdInsert(parameters);
		else if(cmd.compare("start")==0)
			cmdStart(parameters);
		else if(cmd.compare("stop")==0)
			cmdStop(parameters);
		else if(cmd.compare("pause")==0)
			cmdPause(parameters);
		else if(cmd.compare("show")==0)
			cmdShow(parameters);
		else if(cmd.compare("debug")==0)
			cmdDebug(parameters);
		else if(cmd.compare("calibration")==0)
			cmdCalibration();
		else if(cmd.compare("quit")==0)
			quit = cmdQuit();
		else if(cmd.compare("help")==0)
			cmdHelp(parameters);
		else
			invio = true;

	}
	while(!quit);
	return 0;
}


void cmdStart(svec arg) {
	if(!arg.empty()){
		if(arg[0].compare("all")==0) {
			if(num_disp>0) {
				for(int i=0; i<num_disp; ++i) {
					if(d[i].stato == PRONTO) {
						d[i].stato = ATTIVO;
						cout << "Sto avviando il thread " << i << "..." << endl;
						switch(d[i].identifier) {
							case GPS:
								d[i].pid_t = pthread_create(&thr[i], NULL, gpsAcquisition, (void*) i);
								break;
							case IMU:
								d[i].pid_t = pthread_create(&thr[i], NULL, imuAcquisition, (void*) i);
								break;
							case CAM:
								d[i].pid_t = pthread_create(&thr[i], NULL, camAcquisition, (void*) i);
								break;
							case HOK:
								d[i].pid_t = pthread_create(&thr[i], NULL, hokAcquisition, (void*) i);
								break;
							default:
								break;
						}
						cout << "Ho avviato il thread " << i << " con pid " << d[i].pid_t << endl;
					}
					else if(d[i].stato == PAUSA) {
						d[i].stato = ATTIVO;
						cout << "Ho avviato il thread " << i <<  endl;
					}
				}
			}
			else
				cout << "Non è presente nessun dispositivo" << endl;
		}
		else{
			int number;
			istringstream ss(arg[0]);
			ss >> number;
			if (number < num_disp) {
				if(d[number].stato == PRONTO) {
					d[number].stato = ATTIVO;
					cout << "Sto avviando il thread " << number << "..." << endl;
					switch(d[number].identifier) {
						case GPS:
							d[number].pid_t = pthread_create(&thr[number], NULL, gpsAcquisition, (void*) number);
							break;
						case IMU:
							d[number].pid_t = pthread_create(&thr[number], NULL, imuAcquisition, (void*) number);
							break;
						case CAM:
							d[number].pid_t = pthread_create(&thr[number], NULL, camAcquisition, (void*) number);
							break;
						case HOK:
							d[number].pid_t = pthread_create(&thr[number], NULL, hokAcquisition, (void*) number);
							break;
						default:
							break;
					}
					cout << "Ho avviato il thread " << number << " con pid " << d[number].pid_t << endl;
				}
				else if(d[number].stato == PAUSA) {
					d[number].stato = ATTIVO;
					cout << "Ho riattivato il thread " << number << endl;
				}
			}
			else
				cout << "Per info sull'uso di 'start' digitare: 'help start'" << endl;

		}
	}
	else
		cout << "Per info sull'uso di 'start' digitare: 'help start'" << endl;
}
void cmdStop(svec arg) {
	if(!arg.empty()){
		if(arg[0].compare("all")==0) {
			if(num_disp>0) {
				for(int i=0; i<num_disp; ++i) {
					if(d[i].stato!=TERMINATO) {
						d[i].stato = TERMINATO;
						cout << "Terminazione del thread " << i << " in corso..." << endl;
						pthread_join(thr[i], NULL);
						cout << "Il thread " << i << " e' stato fermato." << endl;
					}
				}
			}
			else
				cout << "Non è presente nessun dispositivo" << endl;
		}
		else{
			int number;
			istringstream ss(arg[0]);
			ss >> number;
			if (number>=0 && number<num_disp)
			{
				if(d[number].stato!=TERMINATO) {
					d[number].stato = TERMINATO;
					cout << "Terminazione del thread " << number << " in corso..." << endl;
					pthread_join(thr[number], NULL);
					cout << "Il thread " << number << " e' stato fermato." << endl;
				}
				else
					cout << "Il thread " << number << " e' gia terminato." << endl;
			}
			else
				cout << "Per info sull'uso di 'stop' digitare: 'help stop'" << endl;

		}
	}
	else
		cout << "Per info sull'uso di 'stop' digitare: 'help stop'" << endl;
}
void cmdPause(svec arg) {
	if(!arg.empty()){
		if(arg[0].compare("all")==0) {
			if(num_disp>0) {
				for(int i=0; i<num_disp; ++i) {
					if(d[i].stato==ATTIVO) {
						d[i].stato = PAUSA;
						cout << "Il thread " << i << " e' stato messo in pausa." << endl;
					}
				}
			}
			else
				cout << "Non è presente nessun dispositivo" << endl;
		}
		else{
			int number;
			istringstream ss(arg[0]);
			ss >> number;
			if(number>=0 && number<num_disp)
			{
				if(d[number].stato==ATTIVO) {
					d[number].stato = PAUSA;
					cout << "Il thread " << number << " e' stato messo in pausa." << endl;
				}
				else
					cout << "Impossibile mettere in pausa il thread " << number << endl;
			}
			else
				cout << "Per info sull'uso di 'pause' digitare: 'help pause'" << endl;

		}
	}
	else
		cout << "Per info sull'uso di 'start' digitare: 'help stop'" << endl;
}

void cmdInsert(svec arg) {
	string porta;
	char* percorso_porta = new char[32];
	char* path = NULL;
	bool ok = false;
	char* errore;
	int id = -1;

	dataset->getDataSet(&path);

	if(arg.empty()) {
		cout << "Specifica l'identificatore del dispositivo che intendi utilizzare nella raccolta dati " << endl
				<< "(0 --> per il GPS, 1 --> per la IMU, 2 --> per la cam, 3 --> per l'hokuyo)" << endl;
		cin >> id;
	}
	else if(arg.size()>0) {
		if(arg[0].compare("gps")==0)
			id=0;
		else if(arg[0].compare("imu")==0)
			id=1;
		else if(arg[0].compare("cam")==0)
			id=2;
		else if(arg[0].compare("hok")==0)
			id=3;
		else
			id=-1;
	}
	/*TODO: Miglioramento
	 * Accorciare inserimento indicando direttamente la porta nel comando insert,
	 * pur tuttavia facendo rimanere la possibilità di inserirlo dopo(come è adesso)
	 * Da fare per hokuyo e cam. Imu e gps ok.
	 * */
	switch (id)
	{
		case GPS:{
			if(arg.size()>1)
				strcpy(percorso_porta,arg[1].c_str());
			else {
				cout << "Inserisci i parametri della porta con la quale vuoi comunicare con il dispositivo " << endl;
				cout << "Nome Porta : " << endl;
				cin >> porta;
				strcpy(percorso_porta,porta.c_str());
			}

			SerialGps* gps = new SerialGps();
			ok = gps->openCommunication(percorso_porta);

			if(ok) {
				ThreadedDevice td;
				td.identifier = GPS;
				td.device = (void*)gps;
				td.stato = PRONTO;
				td.debug = 0;

				string pcomp(path);
				pcomp.append("/GPS.cvs");
				char* pcomp2 = new char[200];
				strcpy(pcomp2, pcomp.c_str());

				td.path = pcomp2;

				d.push_back(td);
			}
			else
				gps->getError(&errore);

			path = NULL;
			break;
		}
		case IMU:{
			if(arg.size()>1)
				strcpy(percorso_porta,arg[1].c_str());
			else {
				cout << "Inserisci i parametri della porta con la quale vuoi comunicare con il dispositivo " << endl;
				cout << "Nome Porta : " << endl;
				cin >> porta;
				strcpy(percorso_porta,porta.c_str());
			}

			SerialImu* imu = new SerialImu();
			ok = imu->openCommunication(percorso_porta);

			if(ok) {
				ThreadedDevice td;
				td.identifier = IMU;
				td.device = (void*)imu;
				td.stato = PRONTO;
				td.debug = 0;

				string pcomp(path);
				pcomp.append("/IMU_STRETCHED.cvs");
				char* pcomp2 = new char[200];
				strcpy(pcomp2, pcomp.c_str());
				td.path = pcomp2;

				d.push_back(td);
			}
			else
				imu->getError(&errore);

			path = NULL;
			break;
		}
		case CAM: {
			int c, wait;
			cout << "Inserisci il valore della camera che vuoi aprire." << endl << "(Un intero che corrisponde ad un identificativo del dispositivo, 0 --> Camera di Default, 1,2,3 per le successive...)" << endl;
			cin >> c;
			cout << "Inserisci i millisecondi di attesa tra lo scatto di una foto e la successiva. " << endl;
			cin >> wait;
			Camera* cam = new Camera();
			ok = cam->openCommunication(c, wait);
			if(ok) {
				ThreadedDevice td;
				td.identifier = CAM;
				td.device = (void*)cam;
				td.path = path;
				td.stato = PRONTO;
				td.debug = 0;

				d.push_back(td);
			}
			else
				cout << "Errore nell'apertura della camera! " << endl;

			path = NULL;
			break;
		}
		case HOK: {
			int c;

			cout << "Inserisci il numero della porta /dev/ttyACM che vuoi aprire." << endl;
			cin >> c;

			Hokuyo* hok = new Hokuyo();
			ok = hok->openCommunication(c);

			if(ok) {
				ThreadedDevice td;
				td.identifier = HOK;
				td.device = (void*)hok;
				td.stato = PRONTO;
				td.debug = 0;

				string pcomp(path);
				pcomp.append("/HOKUYO.cvs");
				char* pcomp2 = new char[200];
				strcpy(pcomp2, pcomp.c_str());

				td.path = pcomp2;

				d.push_back(td);
			}
			else
				hok->getError(&errore);
			break;
		}
		default:
			ok = false;
			break;
	}

	if(ok) {
		++num_disp;
		cout << "Dispositivo inserito correttamente\n";
	}
	else
		cout << errore << endl;

	delete(percorso_porta);

	cout << "Fino ad ora, in totale, hai inserito " << num_disp << " dispositiv" << (num_disp==1?"o":"i") << endl << endl;
}

void cmdShow(svec arg) {
	if(!arg.empty()){
		if(arg[0].compare("device")==0) {
			if(num_disp>0) {
				cout << "Numero di dispositivi inseriti: " << num_disp << "\n\n";
				for(int i=0; i<num_disp; ++i) {
					cout << "\nDispositivo " << i << endl;
					cout << "\t" << devKind(d[i].identifier) << endl;
				}
			}
			else
				cout << "Non è presente nessun dispositivo" << endl;
		}
		else if(arg[0].compare("thread")==0) {
			if(num_disp>0) {
				for(int i=0; i<num_disp; ++i) {
					cout << "\nThread " << i << endl;
					cout << "\tDispositivo: " << devKind(d[i].identifier) << endl;
					cout << "\tStato thread: " << thrState(d[i].stato) << endl;
					if(d[i].stato==ATTIVO || d[i].stato==PAUSA)
						cout << "\tPid thread: " << d[i].pid_t << endl;
				}
			}
			else
				cout << "Nessun dispositivo inserito" << endl;
		}
		else
			cout << "Per info sull'uso di 'show' digitare: 'help show'" << endl;
	}
	else
		cout << "Per info sull'uso di 'show' digitare: 'help show'" << endl;
}

void cmdCalibration() {
	bool riuscita;
	int index = 0;
	do {
		cout << "Inserire un valore corrispondente alla nuova directory creata : " << endl << "(0 --> nuova calibrazione, 1 --> nuovo disegno," << endl << "2 --> nuovo formato file, 3 --> nuova posizione sensori)" << endl << "Default: 0" << endl;
		cin >> index;
	}
	while(index < 0 || index > 3);
	riuscita = dataset->nuovaCalibrazione(index);
	if(riuscita == true)
		cout << "Directory della calibrazione create con successo" << endl;
	else
		cout << "Errore nella creazione directory della calibrazione" << endl;
}

void cmdDebug(svec arg) {
	if(!arg.empty())
	{
		int disp;
		istringstream ss(arg[0]);
		ss >> disp;
		if (disp>=0 && disp<num_disp){
			int deb;
			istringstream ss2(arg[1]);
			ss2 >> deb;
			if((deb>=0 && deb<3)) {
				d[disp].debug = deb;
				cout << "Debug del dispositivo settato correttamente a " << deb <<endl;
			}
		}
		else
			cout << "Dispositivo non esistente!" << endl;
	}
	else
		cout << "Per info sull'uso di 'debug' digitare: 'help debug'" << endl;
}

bool cmdQuit() {
	bool qualcuno_attivo = some_thread_active();
	char r;
	if(qualcuno_attivo)
		cout << "Ci sono alcuni thread ancora attivi..." << endl;
	cout << "Sei sicuro di voler chiudere il programma?(s/N) ";
	cin >> r;
	if(r=='s')
		return true;
	return false;
}

void cmdHelp(svec arg) {
	cout << "Programma di aiuto di serialdatacollector\n\n";
	if(arg.empty()) {
		cout << "\thelp\t\tRichiama questa guida\n";
		cout << "\tquit\t\tChiude il programma\n";
		cout << "\tcalibration\t\tInserisce una nuova calibrazione\n";
		cout << "\tinsert\t\tInserisce un dispositivo\n";
		cout << "\tstart\t\tAvvia tutti od un thread specifico\n";
		cout << "\tstop\t\tFerma tutti od un thread specifico\n";
		cout << "\tpause\t\tMette in pausa tutti od un thread specifico\n";
		cout << "\tdebug\t\tImposta le stringhe di debug da visualizzare\n";
		cout << "\tshow\t\tMostra lo stato di thread e dispositivi\n";
		cout << "\nPer guide specifiche sui comandi digitare: help <comando>\n\n";
	}
	else if(arg[0].compare("insert")==0) {
		cout << "USO DI insert\n";
		cout << "insert [dispositivo]\n\n";
		cout << "ARGOMENTI:\n";
		cout << "\[dispositivo]\t\tInserisce uno specifico dispositivo. Valori ammessi:\n";
		cout << "\t\t\t\tgps=Inserimento di un gps\n\t\t\t\timu=Inserimento di una imu"
				"\n\t\t\t\tcam=Inserimento di una camera\n\t\t\t\thok=Inserimento dell'hokuyo\n";
	}
	else if(arg[0].compare("start")==0) {
		cout << "USO DI start\n\n";
		cout << "ARGOMENTI:\n";
		cout << "\tall\t\tAvvia tutti i thread dei dispositivi inseriti\n";
		cout << "\t[num]\t\tValore numerico del dispositivo di cui avviare il thread\n";
	}
	else if(arg[0].compare("stop")==0) {
		cout << "USO DI stop\n\n";
		cout << "ARGOMENTI:\n";
		cout << "\tall\t\tFerma tutti i thread dei dispositivi inseriti\n";
		cout << "\t<num>\t\tValore numerico del dispositivo di cui fermare il thread\n";
	}
	else if(arg[0].compare("pause")==0) {
		cout << "USO DI pause\n\n";
		cout << "ARGOMENTI:\n";
		cout << "\tall\t\tMette in pausa tutti i thread dei dispositivi inseriti\n";
		cout << "\t<num>\t\tValore numerico del dispositivo di cui mettere in pausa il thread\n";
	}
	else if(arg[0].compare("debug")==0) {
		cout << "USO DI debug:\n";
		cout << "debug <num> <val>\n\n";
		cout << "ARGOMENTI:\n";
		cout << "\t<num>\t\tNumero del dispositivo da impostare\n";
		cout << "\t<val>\t\tValore di debug:\n";
		cout << "\t\t\t\t0=Nessuna stringa(default)\n\t\t\t\t1=Stringhe base\n\t\t\t\t2=Tutte le stringhe(caotico)\n";
	}
	else if(arg[0].compare("show")==0) {
		cout << "USO DI show:\n";
		cout << "show <device/thread>\n\n";
		cout << "ARGOMENTI:\n";
		cout << "\tdevice\t\tVisualizza informazioni relative ai dispositivi inseriti\n";
		cout << "\tthread\t\tVisualizza informazioni relativi ai thread:\n";
	}
	else if(arg[0].compare("calibration")==0) {
		cout << "USO DI calibration:\n";
		cout << "calibration\n\n";
	}
	else
		cout << "comando " << arg[0] << " non esistente o ancora non inserito nella guida\n";
}

bool some_thread_active() {
	for(int i=0; i<num_disp; ++i) {
		if(d[i].stato == ATTIVO)
			return true;
	}
	return false;
}

string devKind(DevId n) {
	switch(n) {
		case GPS:
			return "GPS";
			break;
		case IMU:
			return "IMU";
			break;
		case CAM:
			return "CAM";
			break;
		case HOK:
			return "HOKUYO";
			break;
		default:
			return "SCONOSCIUTO";
			break;
	}
}

string thrState(Stato stato) {
	switch(stato) {
	case PRONTO:
		return "PRONTO";
		break;
	case ATTIVO:
		return "ATTIVO";
		break;
	case PAUSA:
		return "IN PAUSA";
		break;
	case TERMINATO:
		return "TERMINATO";
		break;
	}
}
