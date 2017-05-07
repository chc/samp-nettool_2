#ifndef _SAMPAUTH_H
#define _SAMPAUTH_H

typedef struct {
	const char *client;
	const char *server;
} SAMPAuthTable;

//auth stuff
const char *GetAuthKey(const char *key);

int gen_gpci(char buf[64], uint32_t factor, const char *source);
#endif //_SAMPAUTH_H