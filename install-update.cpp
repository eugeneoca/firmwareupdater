//TITLE		INSTALLER/UPDATE AUTOMATION FOR DEC TELEMETRY DEVICES

// Libraries
#include <iostream>
#include <thread>
#include <fstream>
#include <string>


using namespace std;

// Prototype methods
string readFile(string path);
string getMAC();
void install(string db, string prev);
int getConnectivity();
void update(string db);
void update_csv(string mac);
void update_location(string mac);
void remote_ssh(string mac);
void force_lp(string mac);
string splitter(string data, string delimiter, int position);

int main(){
	string info;
	string cmd;
	int rslt;
	
	system("clear");
	cout << "DEC Firmware Install/Update Software" << endl;
	cout << "MAC ADDRESS: " << getMAC() << endl;

	// Check internet connectivity
	cout << "Checking Internet Connectivity...";
	int connection = getConnectivity();
	if(connection!=1){
		cout << "Failed... Terminating." << endl;
		exit(0);
	}else{
		cout << "OK" << endl;
	}
	// Check if git exists, install if not
	cout << "Checking Git console..." << endl;
	system("sudo rm -f info.log");
	system("sudo sh -c 'git --version > info.log'");
	info = readFile("info.log");
	if(info.length()==0){
		system("sudo apt-get install git");
	}else{
		cout << "Git Version: " << info << endl;
	}
	system("sudo rm -f info.log");

	// Check if no resources found, Install scripts first
	//system("sudo sh -c 'sudo du -sh /projects/scripts' > check.txt");	
	cmd = "[ ! -d /projects/scripts/.git ]"; //check if directory exists
	rslt = system(cmd.c_str());
	if(rslt==0){ //if directory does not exist
		cout << "scripts directory does not exist... cloning from github." << endl;
		system("cd /projects/ && sudo git clone ***HIDDEN: CONTAINS PRIAVTE CREDENTIALS*** > /dev/null 2>&1");
		//system("sudo chmod +x /projects/scripts/update_git.sh");
	}
	
	cmd = "[ ! -d /projects/config_file/.git ]"; //check if directory exists
	rslt = system(cmd.c_str());
	if(rslt==0){ //if directory does not exist
		cout << "config_file directory does not exist... cloning from github." << endl;
		system("cd /projects/ && sudo git clone ***HIDDEN: CONTAINS PRIAVTE CREDENTIALS*** > /dev/null 2>&1");
		//system("sudo chmod +x /projects/scripts/update_git.sh");
	}
	//system("sudo rm -rf check.txt");

	//Install then Update
	cout << "Checking DEC Firmware directories..." << endl;
	install("/projects/scripts/install.db", "null");
	update("/projects/scripts/update.db");
	update_csv(getMAC());
	update_location(getMAC());
	remote_ssh(getMAC());
	force_lp(getMAC());
}

void update_csv(string mac){
	// Update Meter Batch File
	int is_changed = 0;
	if(mac!=""){
		cout << "Checking for meter_batch.csv update..." << endl;
		// Check if this machine must update through mac address
		cout << "Checking if I am tagged to update... ";
		// Get server IP
		string ip = "";
		ifstream server_ip;
		server_ip.open("/projects/config_file/server_ip.txt");
		if(server_ip.is_open()){
			while(!server_ip.eof()){
				getline(server_ip, ip);
				if(ip!=""){
					break;
				}
			}
		}
		//cout << "SERVER IP ADDRESS: "<< ip << endl;
		server_ip.close();
		// Fetch from server
		string update_query = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/get_update_csv > res.info";
		system(update_query.c_str());
		ifstream result;
		result.open("res.info");
		string result_extract;
		if(result.is_open()){
			while(!result.eof()){
				result >> result_extract;
				if(result_extract=="1"){
					// Update Meter Batch List
					
					cout << "I am tagged to update... \r\ndownloading... ";
					string content = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/get_content_csv > /projects/amr/upload/meter_batch.csv";
					system(content.c_str());
					system("chown www-data /projects/amr/upload/meter_batch.csv");
					// Reset Update Status Indicator
					string reset_query = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/reset_update_csv";
					system(reset_query.c_str());
					is_changed = 1;
				}
			}
		}
		result.close();
		system("sudo rm -rf res.info");
	}
	if(is_changed==0){
		cout << "No update for meter_batch.csv." << endl;
	}else{
		is_changed=0;
		cout << "Done." << endl;
	}
}

void update_location(string mac){
	// Update Meter Batch File
	int is_changed = 0;
	if(mac!=""){
		cout << "Checking for location.cfg update..." << endl;
		// Check if this machine must update through mac address
		cout << "Checking if I am tagged to update... ";
		// Get server IP
		string ip = "";
		ifstream server_ip;
		server_ip.open("/projects/config_file/server_ip.txt");
		if(server_ip.is_open()){
			while(!server_ip.eof()){
				getline(server_ip, ip);
				if(ip!=""){
					break;
				}
			}
		}
		//cout << "SERVER IP ADDRESS: "<< ip << endl;
		server_ip.close();
		// Fetch from server
		string update_query = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/get_update_location > res.info";
		system(update_query.c_str());
		ifstream result;
		result.open("res.info");
		string result_extract;
		if(result.is_open()){
			while(!result.eof()){
				result >> result_extract;
				if(result_extract=="1"){
					// Update Meter Batch List					
					cout << "I am tagged to update... \r\ndownloading... ";
					string content = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/get_content_location > /projects/amr/meter/location.cfg";
					system(content.c_str());
					//system("chown www-data /projects/amr/upload/meter_batch.csv");
					// Reset Update Status Indicator
					string reset_query = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/reset_update_location";
					system(reset_query.c_str());
					is_changed = 1;
				}
			}
		}
		result.close();
		system("sudo rm -rf res.info");
	}
	if(is_changed==0){
		cout << "No update for location.cfg." << endl;
	}else{
		is_changed=0;
		cout << "Done." << endl;
	}
}

void remote_ssh(string mac){
	if(mac!=""){
		cout << "Checking if remote ssh is requested..." << endl;
				// Get server IP
		string ip = "";
		ifstream server_ip;
		server_ip.open("/projects/config_file/server_ip.txt");
		if(server_ip.is_open()){
			while(!server_ip.eof()){
				getline(server_ip, ip);
				if(ip!=""){
					break;
				}
			}
		}
		//cout << "SERVER IP ADDRESS: "<< ip << endl;
		server_ip.close();
		string update_query = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/rtu_remote_ssh > res.info";
		system(update_query.c_str());
		ifstream result;
		result.open("res.info");
		string result_extract;
		if(result.is_open()){
			while(!result.eof()){
				result >> result_extract;
				if(result_extract=="1"){
					// remote ssh requested				
					cout << "remote ssh requested... ";
					system("/projects/scripts/ngrok.sh 1");
				}else{
					system("/projects/scripts/ngrok.sh 0");
				}
			}
		}
		result.close();
		system("sudo rm -rf res.info");
	}
}	

void force_lp(string mac){
	// Update Meter Batch File
	if(mac!=""){
		cout << "Checking force lp..." << endl;
		// Get server IP
		string ip = "";
		ifstream server_ip;
		server_ip.open("/projects/config_file/server_ip.txt");
		if(server_ip.is_open()){
			while(!server_ip.eof()){
				getline(server_ip, ip);
				if(ip!=""){
					break;
				}
			}
		}
		server_ip.close();
		// Fetch from server
		string update_query = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/force_lp > res.info";
		system(update_query.c_str());
		ifstream result;
		result.open("res.info");
		string result_extract;
		if(result.is_open()){
			while(!result.eof()){
				result >> result_extract;
				if(result_extract=="1"){
					system("touch /tmp/lp_read");
					// Reset force lp Indicator
					string reset_query = "sudo curl --connect-timeout 30 http://" + ip + "/rtu/index.php/rtu/rtu_check_update/" + mac + "/reset_force_lp";
					system(reset_query.c_str());
				}
			}
		}
		result.close();
		system("sudo rm -rf res.info");
	}
}
	
string readFile(string path){
	// Read file from path
	ifstream file;
	file.open(path);
	string output;
	if(file.is_open()){
		while(!file.eof()){
			file >> output;
		}
	}
	file.close();
	return output;
}

int getConnectivity(){
	// Get connectivity boolean
	ifstream file;
	system("sudo sh -c 'wget --spider -q -T 20 -t 2 http://google.com;echo $? > connectivity.log'");
	file.open("connectivity.log");
	string output;
	if(file.is_open()){
		while(!file.eof()){
			getline(file, output);
			if(output!=""){
				if(output=="0"){
					system("sudo rm -rf connectivity.log");
                    file.close();
					return 1;
				}else{
					system("sudo rm -rf connectivity.log");
					file.close();
					return 0;
				}
			}
		}
	}
	system("sudo rm -rf connectivity.log");
	file.close();
	return 0;
}

string getMAC(){
	// Get MAC Address Using 'ifconfig'
	system("/sbin/ifconfig > info.log");
	ifstream file;
	file.open("info.log");
	string output="";
	string mac = "";
	int next_mac = 0;
	if(file.is_open()){
		while(!file.eof()){
			file >> mac;
			if(next_mac==1){
				file.close();
				system("sudo rm -rf info.log");
				return mac;
			}
			if(mac=="HWaddr"){
				next_mac = 1;
			}
		}
		if(next_mac==0){
			file.close();
			system("sudo rm -rf info.log");
			return "";
		}
	}
	file.close();
	system("sudo rm -rf info.log");
	return mac;
}

void install(string db, string prev){
	// Installation
	ifstream database;
	database.open(db);
	string output="";
	bool new_install=false;
	int rslt=0;
	string cmd;
	cmd = "[ ! -d /projects/amr/.git ]";
	rslt = system(cmd.c_str());
	cout << rslt << endl;
	if(rslt==0){
		new_install=true;
	}
	
	if(database.is_open()){
		while(!database.eof()){
			getline(database, output);
			if(output.length()!=0){
				output = "sudo sh -c '" + output + "' > check.txt";
				system(output.c_str());
			}
		}
	}
	database.close();
	system("sudo rm -rf check.txt");
	
	/*
	// Git existence, else, recursion.
	system("sudo rm -f log.txt");
	ifstream fstruc;
	string structure;
	fstruc.open("/projects/scripts/structure.db");
	if(fstruc.is_open()){
		while(!fstruc.eof()){			
			getline(fstruc, structure);
			//cout << "reading structure " + structure << endl;
			if(structure.length()!=0){
				string path = structure;
				structure = "sudo sh -c 'sudo du -sh " + structure + "' > log.txt";
				system(structure.c_str());
				string data = readFile("log.txt");
				//cout << "Data length: " + data << endl;
				if(data.length()==0){
					system("sudo rm -f log.txt");
					cout << "Git files not present, reinstalling..." << endl;
					new_install = true;
					path.erase(path.end()-4,path.end());
					path = "sudo rm -r " + path;
					system(path.c_str());
					if(path==prev){
						break;
					}
					install(db, path);
				}
			}
		}
		fstruc.close();
		system("sudo rm -f log.txt");
	}
	*/
	
	// Run once if new installation
	if(new_install){
		cout << "New installation... execute run_once script." << endl;
		ifstream run_once;
		run_once.open("/projects/scripts/run_once.db");
		string cmd = "";
		if(run_once.is_open()){
			while(!run_once.eof()){
				getline(run_once, cmd);
				if(cmd.length()!=0){
					cmd = "sudo sh -c '"+cmd+"'";
					system(cmd.c_str());
				}
			}
		}
		run_once.close();
	}
}

void update(string db){
	// Update 
	ifstream database;
	string output = "";
	string lastupdate = "";
	cout << "Looking for updates..." << endl;
	database.open(db);
	if(database.is_open()){
		while(!database.eof()){
			getline(database, output);
			output = "sudo sh -c '" + output + "'";
			system(output.c_str());
		}
	}
	// Update server ip
	cout << "Updating server IP..." << endl;
	system("sudo sh -c '/projects/scripts/update_server_ip.sh'");
	/*string base = "";
	ifstream servers_table;
	servers_table.open("scripts/servers_table.db");
	if(servers_table.is_open()){
		while(!servers_table.eof()){
			getline(servers_table, base);
			string mac_address = splitter(base.c_str(), ",;",0);
			string ip_address = splitter(base.c_str(), ",;", 1);
			if(getMAC()==mac_address){
				string cmd = "sudo sh -c 'echo " + ip_address + "' > /projects/amr/config_file/server_ip.txt";
				system(cmd.c_str());
			}
		}
	}
	servers_table.close();*/
	database.close();
}

string splitter(string data, string delimiter, int position){
	string temp = "";
	int current_pos = 0;
	for(int i=0; i<=data.length(); i++){
		if(data[i]!=delimiter[0] && data[i]!=delimiter[1]){
			temp.append(string(1, data[i]));
		}else{
			if(current_pos==position){
				return temp;
			}else{
				temp="";
				current_pos++;
			}
		}
	}
	return "";
}
