
class CLog
{
public:
	CLog();
	~CLog();

	FILE* SysFile;
	FILE* WarnFile;
	FILE* ErrFile;
	FILE* PacketFile;
	FILE* ItemFile;
	int SysLogDay;
	int WarnLogDay;
	int ErrLogDay;
	int PacketLogDay;
	int ItemLogDay;

	void Write(int conn, TL Type, const std::string& str);
	void Start();
};

class CServer
{
private:
	CUser pUser[MAX_USER];
public:
	CMob pMob[MAX_MOB];
	CItem pItem[MAX_ITEM];
	CNPCGenerator mNPCGen;
	CNPCSummon mSummon;

	CServer();
	~CServer();

	uint16 pMobGrid[MAX_GRIDY][MAX_GRIDX];
	uint16 pItemGrid[MAX_GRIDY][MAX_GRIDX];
	char pHeightGrid[MAX_GRIDY][MAX_GRIDX];
	STRUCT_MERC pMobMerc[MAX_MOB_MERC];

	int GuildImpostoID[MAX_GUILDZONE];

	CPSock ListenSocket;
	CPSock DBSocket;
	uint8 LocalIP[4];

	uint32 SecCounter;
	uint32 MinCounter;
	bool KefraLive;
	int ServerDown;
	int DBServerPort;

	int ServerGroup;
	int NumServerInGroup;
	int ServerIndex;

	int NewbieEventServer;
	int CurrentWeather;
	int g_EmptyMob;
	int g_dwInitItem;

	char DBServerAddress[32];
	char temp[1024];

	CUser* getUser(int id);
	CMob* getMob(int id);
	int GetEmptyUser();
	uint32 GetUserFromSocket(uint32 Sock);
	void Reboot();
	bool StartServer();
	bool ProcessClientMSG(int conn, char* pMsg, BOOL isServer);
	bool ProcessDBMessage(char* Msg);
	void TimerSec();
	void TimerMin();
	void CloseUser(int conn);
	int ReadMob(STRUCT_MOB* mob, const char* dir);
	void SetReqHp(CMob* mob);
	void SetReqMp(CMob* mob);
	void SaveUser(int conn, int Export);
	void CharLogOut(int conn);
	void RemoveTrade(int conn);
	void DeleteMob(int conn, int Type);
	void RemoveParty(int conn);
	void GenerateMob(int index, int PosX, int PosY);
	int GetEmptyNPCMob();
	bool SetAffect(int conn, int skillnum, int time, int Level);
	bool SetTick(int mob, int skillnum, int Delay, int Level);
	int UpdateItem(int Gate, int state, int* height);
	int CreateItem(int x, int y, STRUCT_ITEM* item, int rotate, int Create);
	int CreateMob(const char* MobName, int PosX, int PosY, const char* folder, int Type);
	int GetEmptyItem();
	void DoTeleport(int id, int x, int y);
	void MountProcess(int conn, STRUCT_ITEM* Mount);

};

extern CLog cLog;
extern CServer cSrv;
extern uint32 CurrentTime;