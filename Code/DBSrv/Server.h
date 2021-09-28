
class CServer
{
private:
	CUser pUser[MAX_SERVER];

public:
	CServer();
	~CServer();

	CPSock ListenSocket;	
	CUser TempUser;

	uint8 LocalIP[4];
	uint16 LastCapsule;

	int SecCounter;
	int MinCounter;
	int HourCounter;
	int ServerIndex;
	int Sapphire;
	
	char temp[1024];

	CUser* getUserDB(int id);
	int getUserSocket(int Sock);
	void ProcessClientMessage(int conn, char* msg);

	int WriteConfig();
	void TextOutWind(const char* str, int color);
	void DrawConfig();
	void ProcessSecTimer();
	void ReadConfig();
	void loadCharacter();
	bool geralStart();
};

class CLog
{
public:
	CLog();
	~CLog();

	FILE* SysFile;
	FILE* WarnFile;
	FILE* ErrFile;
	FILE* PacketFile;
	int SysLogDay;
	int WarnLogDay;
	int ErrLogDay;
	int PacketLogDay;

	void Write(uint32 ip, TL Type, const std::string& str);
	void Start();
};

extern CLog cLog;
extern CServer cSrv;
extern uint32 CurrentTime;

extern HFONT hFont;
extern HFONT h;
extern HDC hDC;
extern int x;
extern int y;