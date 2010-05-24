//============================================================================
// Name			: serialdatacollector
// Author		:
// Version		: 0.1
// Last Modify	: 28/04/2010
// Copyright	: GPL v3
// Description	: A program that collects data from serial device, like imu, gps, Hokuyo etc....
//============================================================================

#include <iostream>
#include <time.h>
#include <pthread.h>
#include <vector>
#include <string.h>
#include <termios.h>

#include "main.h"
#include "RawSeed.h"
#include "SerialGps.h"
#include "SerialImu.h"
#include "Camera.h"
#include "Hokuyo.h"

using namespace std;

int main(int argc, char* argv[]) {

	d.reserve(3);
	num_disp = 0;

	if(--argc>0)
		dataset = new RawSeed(argv[1]);
	else
	{
		dataset = new RawSeed();
		cin.ignore();
	}

	char* luogo = new char[64];
	unsigned short tipo;
	bool flag = false , luogo_ok = false, tipo_ok = false;

	/*TODO: Miglioramento comandi
	 * Vedere se si possono mettere tutte queste richieste come comandi
	 */

	cout << "Sto iniziando l'inizializzazione dei dati...." << endl;

	do
	{
		flag = false;
		do
		{
			luogo_ok = false;
			cout << "Specificare il luogo in cui si intende raccogliere i dati: " << endl;
			cin.getline(luogo,64);
			luogo_ok = dataset->setLocation(luogo);
			if(luogo_ok == false)
				cout << "Errore nell'inserimento del luogo, NON puoi mettere il carattere speciale '/'!!" << endl;
		}
		while(!luogo_ok);

		do
		{
			tipo_ok = false;
			tipo = 0;
			cout << "Specificare il tipo di raccolta dati compiuta dal Robot" << endl << "(1 --> Raccolta dati Statica, 2 --> Raccolta dati Dinamica)" << endl;
			cin >> tipo;
			tipo_ok = dataset->setType(tipo);
			if(tipo_ok == false) {
				cout << "Inserisci solo 1 o 2！" << endl;
			}
		}
		while(!tipo_ok);


		flag = dataset->nuovoDataset();
		if(flag)
			cout << endl << "Directory per la raccolta dati creata con successo!" << endl;
		else
		{
			cout << endl << "C'è stato un errore nella creazione della directory!! " << endl;
			cin.ignore();
		}
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
	 * ------------------------------------------*/
	cout << "Chiusura del programma in corso... attendere prego..." << endl;
	if(some_thread_active(true)){
		for(int i=0; i<num_disp; ++i) {
			if(d[i].stato!=TERMINATO) {
				d[i].stato = TERMINATO;
				cout << "Sto stoppando il thread numero " << i << endl;
				pthread_join(d[i].pid, NULL);
			}
		}
	}
	cout << "Arrivederci!" << endl;
	d.clear();
	delete(dataset);

}

void* Shell() {
	bool quit = false;
	bool invio = false;

	cout << "Benvenuto nella console di comando di serialdatacollector!" << endl;
	cout << "Non sono stati inseriti dispositivi. Per inserirne uno digitare 'insert'\n";
	cout << "Per ricevere aiuto digitare 'help'\n\n";

	cin.ignore();

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

		/* ----------------------------- *
		 * PARSE MACRO COMMAND				 *
		 * ----------------------------- */
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
		else if(cmd[0] == '\n')
			invio = true;

	}
	while(!quit);
	return 0;
}


void cmdStart(svec arg) {
	if(!arg.empty()){
		int start;
		int stop;
		bool ok;
		if(arg[0].compare("all")==0) {
			start = 0;
			stop = num_disp;
			ok=true;
		}
		else {
			int number;
			istringstream ss(arg[0]);
			ss >> number;
			if(number >=0 && number < num_disp) {
				start = number;
				stop = number+1;
				ok = true;
			}
			else
				ok = false;
		}
		if(ok){
			if(num_disp>0) {
				for(int i=start; i<stop; ++i) {
					if(d[i].stato == PRONTO) {
						d[i].stato = ATTIVO;
						cout << "Sto avviando il thread " << i << "..." << endl;
						switch(d[i].identifier) {
							case GPS:
								pthread_create(&d[i].pid, NULL, gpsAcquisition, (void*) i);
								break;
							case IMU:
								pthread_create(&d[i].pid, NULL, imuAcquisition, (void*) i);
								break;
							case CAM:
								pthread_create(&d[i].pid, NULL, camAcquisition, (void*) i);
								break;
							case HOK:
								pthread_create(&d[i].pid, NULL, hokAcquisition, (void*) i);
								break;
							default:
								break;
						}
						cout << "Ho avviato il thread " << i << " con pid " << d[i].pid << endl;
					}
					else if(d[i].stato == PAUSA) {
						d[i].stato = ATTIVO;
						cout << "Ho avviato il thread " << i <<  endl;
					}
				}
			}
			else
				cout << "Non è stato inserito alcun dispostivo" << endl;
			return;
		}
	}
	cout << "Per info sull'uso di 'start' digitare: 'help start'" << endl;
	return;
}

/* TODO: Rivedere stop/pause
 * Rivedere gestione dello start/stop/pause
 */
void cmdStop(svec arg) {
	if(!arg.empty()){
		int start;
		int stop;
		bool ok;
		if(arg[0].compare("all")==0) {
			start = 0;
			stop = num_disp;
			ok=true;
		}
		else {
			int number;
			istringstream ss(arg[0]);
			ss >> number;
			if(number >=0 && number < num_disp) {
				start = number;
				stop = number+1;
				ok = true;
			}
			else
				ok = false;
		}
		if(ok) {
			if(num_disp>0) {
				if(some_thread_active()){
					for(int i=start; i<stop; ++i) {
						if(d[i].stato!=TERMINATO) {
							d[i].stato = TERMINATO;
							cout << "Terminazione del thread " << i << " in corso..." << endl;
							pthread_join(d[i].pid, NULL);
							cout << "Il thread " << i << " e' stato fermato." << endl;
						}
					}
				}
				else
					cout << "Non c'e' nessun thread attivo!" << endl;
			}
			else
				cout << "Non è stato inserito alcun dispositivo!" << endl;
			return;
		}
	}
	cout << "Per info sull'uso di 'stop' digitare: 'help stop'" << endl;
	return;
}
void cmdPause(svec arg) {
	if(!arg.empty()){
		int start;
		int stop;
		bool ok;
		if(arg[0].compare("all")==0) {
			start = 0;
			stop = num_disp;
			ok=true;
		}
		else {
			int number;
			istringstream ss(arg[0]);
			ss >> number;
			if(number >=0 && number < num_disp) {
				start = number;
				stop = number+1;
				ok = true;
			}
			else
				ok = false;
		}

		if(ok)
			if(num_disp>0) {
				if(some_thread_active(true)) {
					for(int i=start; i<stop; ++i) {
						if(d[i].stato==ATTIVO) {
							d[i].stato = PAUSA;
							cout << "Il thread " << i << " e' stato messo in pausa." << endl;
						}
					}
				}
				else
					cout << "Non c'è nessun thread!" << endl;
			}
			else
				cout << "Non è stato inserito alcun dispositivo!" << endl;
		return;
	}
	cout << "Per info sull'uso di 'start' digitare: 'help stop'" << endl;
	return;
}

void cmdInsert(svec arg) {
	string porta;
	char* percorso_porta = new char[32];
	char* path = NULL;
	bool ok = false;
	char* errore;
	int id = -1;

	dataset->getDataSet(&path);

	if(arg.size()>0) {
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

	if(arg.empty() || id==-1) {
		cout << "Specifica l'identificatore del dispositivo che intendi utilizzare nella raccolta dati " << endl
				<< "(0 --> per il GPS, 1 --> per la IMU, 2 --> per la cam, 3 --> per l'hokuyo)" << endl;
		cin >> id;
	}
	/*TODO: Miglioramento insert
	 * Accorciare inserimento indicando direttamente la porta nel comando insert,
	 * pur tuttavia facendo rimanere la possibilità di inserirlo dopo(come è adesso)
	 * Da fare per hokuyo e cam. Imu e gps ok.
	 * */
	/*TODO: Inserimento parametri insert
	 * fare in modo che si possano specificare baud_rate, parità, ecc... oltre alla porta.
	 * in assenza fare come è attualmente
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
			//ok = true;
			if(ok) {
				ThreadedDevice td;
				td.identifier = GPS;
				td.device = (void*)gps;
				td.stato = PRONTO;
				td.debug = 0;

				string pcomp(path);
				pcomp.append("/GPS.csv");
				char* pcomp2 = new char[200];
				strcpy(pcomp2, pcomp.c_str());

				td.path = pcomp2;

				d.push_back(td);
				cout << "Il gps alla porta " << percorso_porta << " salverà i dati in " << pcomp2 << endl;
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
				pcomp.append("/IMU_STRETCHED.csv");
				char* pcomp2 = new char[200];
				strcpy(pcomp2, pcomp.c_str());
				td.path = pcomp2;

				d.push_back(td);
				cout << "La imu alla porta " << percorso_porta << " salverà i dati in " << pcomp2 << endl;
			}
			else
				imu->getError(&errore);

			path = NULL;
			break;
		}
		case CAM: {
#ifndef CAMERA
			int c, wait, tipo=2;
			string ip_cam("http://192.168.10.100/mjpg/video.mjpg");
			do
			{
				cout << "Specifica il tipo di telecamare che disponi nel robot" << endl << "(specifica:" << endl << "0 --> USB Camera" << endl << "1 --> Ip Camera" << endl << "2 --> Ip camera di Default(modello Axis 211)" << endl;
				cin >> tipo;
			}
			while(tipo < 0 || tipo > 2);

			if(tipo == 1)
			{
				cout << "Inserisci l'indirizzo http della telecamera, specificando tutto il percorso fino al video." << endl << "(Formato consigliato: mjpeg)" << endl;
				cin >> ip_cam;
			}
			else if(tipo == 0)
			{
				cout << "Inserisci il valore della camera che vuoi aprire." << endl << "(Un intero che corrisponde ad un identificativo del dispositivo, 0 --> Camera di Default, 1,2,3 per le successive...)" << endl;
				cin >> c;
			}

			cout << "Inserisci i millisecondi di attesa tra lo scatto di una foto e la successiva. " << endl;
			cin >> wait;

			Camera* cam = new Camera();

			if(tipo)
				ok = cam->openCommunication(ip_cam.c_str(), wait);
			else
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
#else
			cout << "Dispositivo non abilitato" << endl;
			ok = false;
#endif
			break;
		}
		case HOK: {
#ifndef HOKUYO
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
				pcomp.append("/HOKUYO.csv");
				char* pcomp2 = new char[200];
				strcpy(pcomp2, pcomp.c_str());

				td.path = pcomp2;

				d.push_back(td);
			}
			else
				hok->getError(&errore);
#else
			cout << "Dispositivo non abilitato" << endl;
			ok = false;
#endif
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

/* TODO: Inserimento Comando
 * Comando show port: Fa vedere una lista di porte valide per
 * l'inserimento dei dispositivi
 * Fare un tryOpenCommunication(metodo di serialdevice)
 * su tutte le /dev/ttySX e /dev/ttyUSBX)
 */
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
						cout << "\tPid thread: " << d[i].pid << endl;
				}
			}
			else
				cout << "Nessun dispositivo inserito" << endl;
		}
		else if(arg[0].compare("port")==0) {
			vector<string> porte_valide;
			string seriali("/dev/ttyS");
			string usb("/dev/ttyUSB");
			string modem("/dev/ttyACM");
			for(int i=0; i<4; ++i) {
				stringstream numero;
				numero << i;
				string porta(seriali.append(numero.str()));
				if(SerialDevice::tryOpenCommunication((char*)porta.c_str()))
					porte_valide.push_back(porta);
			}
			for(int i=0; i<4; ++i) {
				stringstream numero;
				numero << i;
				string porta(modem.append(numero.str()));
				if(SerialDevice::tryOpenCommunication((char*)porta.c_str()))
					porte_valide.push_back(porta);
			}
			for(int i=0; i<32; ++i) {
				stringstream numero;
				numero << i;
				string porta(usb.append(numero.str()));
				if(SerialDevice::tryOpenCommunication((char*)porta.c_str()))
					porte_valide.push_back(porta);
			}
			for(unsigned int i=0; i<porte_valide.size(); ++i)
				cout << porte_valide[i] << endl;
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
	if(qualcuno_attivo) {
		cout << "Ci sono alcuni thread ancora attivi..." << endl;
		cout << "Sei sicuro di voler chiudere il programma?(s/N) ";
		cin >> r;
	}
	else
		r='s';
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

void* camAcquisition(void* i){
	/*TODO: Miglioramento foto
	 * Vedere se si riesce a far salvare ogni tanto una foto con un nome specifico
	 */
#ifndef CAMERA
	int n = (long)i;
	ThreadedDevice dev = d.at(n);
	while(dev.stato!=TERMINATO) {
		while(dev.stato==ATTIVO) {
			//((Camera*)dev.device)->getPhoto(dev.path);
			((Camera*)dev.device)->readData();
			if(((Camera*)dev.device)->writeData(dev.path))
				cout << "Immagine scritta!" << endl;
			else
				cout << "Errore: immagine non scritta!" << endl;
			 dev = d.at(n);
		}
		 dev = d.at(n);
	}
#endif
	return (void*) true;
}

void* hokAcquisition(void* i){
	/*TODO: nominal scan frequency
	 * Inserire la nominal scan frequency
	 */
	/*TODO: Verifica timestamp
	 * Verificare il formato del timestamp dell'hokuyo
	 */
#ifndef HOKUYO
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
						cout << "#" << i  << ": " << vet[0] << endl;
				}
				else
					if(dev.debug>1)
						cout << "# " << i << ": Errore di lettura" << endl;
			}

			file.open(dev.path, ios::app);
			if(!file.is_open()) {
					cout << "Impossibile accedere al file" << dev.path;
			}
			for(int j=0; j<righe_scritte; ++j) {
				file << buffer[j] << "\n";
			}

			if(dev.debug>0)
				cout << "L'hokuyo ha scritto su file" << endl;
			file.close();

			buffer.clear();
			dev = d.at(n);
		}
		dev = d.at(n);
	}

	if(dev.debug>0)
		cout << "Hokuyo thread ended" << endl;
#endif
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
				/* NMEA_GPGGA, NMEA_GPRMC, NMEA_GPSV, NMEA_GSA
				 * Si possono decidere le stringhe da estrarre passandole come parametri
				 * ES: readData(&x,NMEA_GPGGA | NMEA_GPRMC)
				 * per estrarre le gpgga e le gprmc */
				if(((SerialGps*)dev.device)->readData(&x,NMEA_GPGGA)) {
					stringstream ss;
					ss << (int)ts.tv_sec << "." << (int)ts.tv_nsec << "," << x;
					buffer.push_back(ss.str());
					++righe_scritte;
					if(dev.debug>1)
						cout << "#" << i  << ": " << x << endl;
				}
				else
					if(dev.debug>1)
						cout << "# " << i << ": Errore di lettura" << endl;
			}
			file.open(dev.path, ios::app);
			if(!file.is_open()) {
					cout << "Impossibile accedere al file" << dev.path;
			}
			for(int i=0; i<righe_scritte; ++i) {
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

			buffer.clear();
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
				if(((SerialImu*)dev.device)->readData(&x)) {
					stringstream ss;
					ss << (int)ts.tv_sec << "." << (int)ts.tv_nsec << "," << x;
					buffer.push_back(ss.str());
					++righe_scritte;
					if(dev.debug>1)
						cout << "#" << i  << ": " << x << endl;
				}
				else
					if(dev.debug>1)
						cout << "# " << i << ": Errore di lettura" << endl;
			}
			file.open(dev.path, ios::app);
			if(!file.is_open()) {
					cout << "Impossibile accedere al file" << dev.path;
			}
			for(int i=0; i<righe_scritte; ++i) {
				file << buffer[i] << "\n";
			}

			if(dev.debug>0)
				cout << "L'imu ha scritto su file" << endl;
			file.close();

			if(conta==16) {
				file_ultimo.open(p2, ios::out);
				//for(int i=0; i<righe_scritte; ++i) {
					file_ultimo << buffer[0] << "\n";
				//}
				file_ultimo.close();
				conta=-1;
			}
			++conta;

			buffer.clear();
		}
		dev = d.at(n);
	}

	if(dev.debug>0)
		cout << "Imu thread ended" << endl;
	return (void*) true;
}

bool some_thread_active(bool conta_in_pausa) {
	int n = d.size();
	for(int i=0; i<n; ++i) {
		if(conta_in_pausa) {
			if(d[i].stato == ATTIVO)
				return true;
		}
		else {
			if(d[i].stato == ATTIVO || d[i].stato == PAUSA)
				return true;
		}
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
