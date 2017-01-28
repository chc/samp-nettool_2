#ifndef _SELECTNETEVENTMGR_H
#define _SELECTNETEVENTMGR_H
#include "NetEventManager.h"
#include <vector>
namespace Net {
	class SelectNetEventManager : public IEventManager {
	public:
		SelectNetEventManager();
		~SelectNetEventManager();
		void run();
	private:
		int setup_fdset();
		fd_set  m_fdset;
	};
}
#endif //_SELECTNETEVENTMGR_H