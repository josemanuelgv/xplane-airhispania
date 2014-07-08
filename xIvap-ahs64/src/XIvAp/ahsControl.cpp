
#include "ahsControl.h"
#include <winsock2.h>
#include "common.h"
#include <regex>
#include <algorithm>

AhsControl::AhsControl():
status(0),
html("")
{
	deps.clear();
	freqs.clear();
}

bool compare_str (const string& first, const string& second)
{
	return ( first.stl_str().compare(second.stl_str()));
}

// Calcula el valor mínimo
const int& AhsControl::minimo(const int& a, const int& b)
{
    return (b < a) ? b : a;
}


// Calcula la distancia de levenstein entre 2 cadenas
unsigned int AhsControl::lvdistance(const string& s1, const string& s2)
{
	const size_t len1 = s1.stl_str().size(), len2 = s2.stl_str().size();
	std::vector<std::vector<unsigned int> > d(len1 + 1, std::vector<unsigned int>(len2 + 1));
 
	d[0][0] = 0;
	for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
	for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;
 
	for(unsigned int i = 1; i <= len1; ++i)
		for(unsigned int j = 1; j <= len2; ++j)
 
                      d[i][j] = this->minimo( this->minimo(d[i - 1][j] + 1,d[i][j - 1] + 1),
                                          d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) );
	return d[len1][len2];
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
	chDep.clear();
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

		// Obtiene la lista de canales de TS abiertos
		tschannels.clear();
		std::regex e2("width=\"720\" class=\"txt_04\" align=\"left\">([A-Za-z0-9_\\s]*)\\s\\(");
		std::smatch m2;
		string d1 = html;

		while (std::regex_search(d1.stl_str(),m2,e2)) {
			tschannels.push_back(m2[1].str());
			d1 = m2.suffix().str();
		}
	
		tschannels.sort(compare_str);		// Ordenamos la lista de canales
		tschannels.unique();				// Eliminamos posibles repetidos

		// Calcula los canales de TS más adecuados a cada dependencia usando la distancia de levenstein
		
		std::list<string>::iterator itd = deps.begin();
		std::list<string>::iterator itc;
		int mi;
		int d;
		string c = "";
		while(itd != deps.end() ) {
			mi = -1;
			c = "";
			itc = tschannels.begin();
			while( itc != tschannels.end() ) {
				d = this->lvdistance((*itd), (*itc));
				if((mi < d)&&(d < 6)){           // Un valor de 6 para la distancia de levenstein significa que las cadenas son prácticamente distintas
					mi = d;
					c = (*itc);
				}
				++itc;
			}
			chDep.push_back(c);
			++itd;
		}
	}

	status = 1;
}

int AhsControl::countDep(){
	return deps.size();
}

int AhsControl::countTSChannels(){
	return tschannels.size();
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

string AhsControl::findChannel(string freq)
{
	if(freqs.size() ==0)
		return "";

	std::list<string>::iterator itd = chDep.begin();
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

