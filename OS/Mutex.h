#ifndef _OS_MUTEX_H
#define _OS_MUTEX_H
namespace OS {
	class CMutex {
		public:
			CMutex() { };
			virtual ~CMutex() {};
			virtual void lock() = 0;
			virtual void unlock() = 0;
	};
	
	CMutex *CreateMutex();
}
#endif //_OS_MUTEX_H