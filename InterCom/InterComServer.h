#ifndef _INTERCOMSERVER_H
#define _INTERCOMSERVER_H
#include <main.h>
#include <NetServer.h>

#include <string>
#include <vector>
#include <map>

#define MTUSize 1092
#define INTERCOM_MAGIC 0xABCDEF02
namespace InterCom {
	typedef struct {
		uint32_t magic;
		uint8_t chunk_count;
		uint32_t total_size;
		uint8_t chunk_index;
	} InterComHeader;

	class Server : public Net::Server {
	public:
		Server(const char *hostname);
		~Server();
		void think(fd_set *set); //called each time for recv/send
	private:

	};
}
#endif //_INTERCOMSERVER_H