






void OpenSocket(int);
void DisplayInfo();
void InitAddressStruct(int);
void BindSocketAndListen();
void AcceptConnections();
void ExitOnError(char*);
void InitDetachedThread();
void HandleClientRequests(int);
int ParseClientMessage(char*);