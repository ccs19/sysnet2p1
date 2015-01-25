#include <bits/pthreadtypes.h>

void OpenSocket(int);
void DisplayInfo();
void InitAddressStruct(int);
void BindSocketAndListen();
void AcceptConnections();
void ExitOnError(char*);
void InitDetachedThread(pthread_attr_t*);
void HandleClientRequests(void*);
int ParseClientMessage(char*, int);

int XMLParser(const char*,const char*,char*, char*, int);