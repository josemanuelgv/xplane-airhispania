
#include "ahsControl.h"
#include <winsock2.h>
#include "common.h"
#include <regex>

AhsControl::AhsControl():
status(0),
html("")
{
	deps.clear();
	freqs.clear();
}

void AhsControl::parse()
{
	WSADATA WSAData;
    WSAStartup(MAKEWORD(2,0), &WSAData);

    SOCKET sock;
    SOCKADDR_IN sin;

    char buffer[1024];


    std::string sreq = "GET /modact01.php?tCod=mod_actahs HTTP/1.1\r\n";  
    sreq += "Host: www.airhispania.com\r\n";
    sreq += "Connection: close\r\n";
    sreq += "User-Agent: X-Ivap AHS\r\n";
    sreq += "\r\n";

	size_t requete_taille = sreq.size() + 1;

    char crequete[5000];
    strncpy( crequete, sreq.c_str(), requete_taille );

    int i = 0;
	html = "";

    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_addr.s_addr = inet_addr(AHSCONTROL_IP);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80); // port HTTP.

    connect(sock, (SOCKADDR *)&sin, sizeof(sin)); 
    send(sock, crequete, strlen(crequete), 0); 

    do
    {
        i = recv(sock, buffer, sizeof(buffer), 0); 
        html += buffer;
    } while (i != 0);


    closesocket(sock); 

	WSACleanup();

	// Obtiene los pares dependencia-frecuencia de la lista de controles activos
	deps.clear();
	freqs.clear();
	if(html.stl_str().size()>0){
		string data = html;
		std::regex e (">([A-Z]{4}_[A-Z]{2,3})<\/td><td align=\"left\" class=\"txt_04\">([0-9]{3}\\.[0-9]{3})<");
		std::smatch m;
		while (std::regex_search (data.stl_str(),m,e)) {
			if(m.size()>2){
				deps.push_back(m[1].str());
				freqs.push_back(m[2].str());
			}
			data = m.suffix().str();
		 }
	}
	status = 1;
}

int AhsControl::numDep(){
	return deps.size();
}

void AhsControl::saveToFile()
{
	// Save data
	if(status > 0){
		FILE *fp = fopen(AHSCONTROL_FILE, "w");
		fputs(html, fp);
		fclose(fp);
	}
}

string AhsControl::findDep(string freq)
{
	if(freqs.size() ==0)
		return "";

	std::list<string>::iterator itd = deps.begin();
	std::list<string>::iterator itf = freqs.begin();
	bool found = false;
	while(itf != freqs.end() ) {
		if(!(freq == (*itf))){
			std::advance(itd, 1);
		}else{
			found = true;
			break;
		}
		++itf;
	}
	if(found)
		return (*itd);
	else
		return "";
}

int AhsControl::getStatus()
{
	return status;
}