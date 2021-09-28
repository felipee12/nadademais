#ifndef _CUSER_
#define _CUSER_

#define USER_EMPTY    0
#define USER_ACCEPT   1

class CUser
{
public:
	uint32    IP;
	int    Mode;
	CPSock cSock;
	int    Count;
	int    Level;
	int    Encode1;
	int    Encode2;
	char   Name[ACCOUNTNAME_LENGTH];
	int    DisableID;

	int Year;
	int YearDay;

public:
	CUser();
	~CUser();

	BOOL AcceptUser(int ListenSocket, int WSA);
};

#endif