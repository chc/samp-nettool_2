#include "main.h"
#include "SelectNetEventManager.h"
#include "NetProxy.h"
#include "SAMP\SAMPServer.h"
std::vector<SAMP::Server *> m_servers;
Net::SelectNetEventManager* g_event_mgr = NULL;
void addSAMPServer(SAMP::Server *server) {
	m_servers.push_back(server);
	g_event_mgr->addNetworkServer(server);
}
int main() {
	#ifdef _WIN32
		WSADATA wsdata;
		WSAStartup(MAKEWORD(1, 0), &wsdata);
	#endif
	g_event_mgr = new Net::SelectNetEventManager(); //must be before py init
	Py::Init();

	
	std::vector<SAMP::Server *>::iterator it;
	for(;;) {
		//proxy->think();
		g_event_mgr->run();

		it = m_servers.begin();		
		while(it != m_servers.end()) {
			(*it)->think(NULL);
			it++;
		}
		
		Py::Tick();
	}
}

uint32_t resolv(const char *host) {
    struct  hostent *hp;
    uint32_t    host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
			return (uint32_t)-1;
        }
        host_ip = *(uint32_t *)hp->h_addr;
    }
    return(host_ip);
}

void get_server_address_port(const char *input, char *address, uint16_t &port) {
	const char *seperator = strrchr(input, ':');
	int len = strlen(input);
	if(seperator) {
		port = atoi(seperator+1);
		len = seperator - input;
	}
	strncpy(address, input, len);
	address[len] = 0;
}


int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}