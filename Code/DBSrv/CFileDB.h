
#ifndef _CFILEDB_
#define _CFILEDB_

typedef struct
{
	int Login;
	int Slot;
	STRUCT_ACCOUNTFILE File;
	int SecurePass;
	int Mac[4];
} STRUCT_ACCOUNTLIST;

int ProcessRecord(int conn, char* str);

class CFileDBSRV
{
public:
	STRUCT_ACCOUNTLIST pAccountList[MAX_DBACCOUNT];

	CFileDBSRV();
	~CFileDBSRV();

	int  AddAccount(char* id, char* pass, char* name, int ssn1, int ssn2, char* email, char* tel, char* addr, int bonus);
	int  UpdateAccount(char* id, char* pass);

	bool  ProcessMessage(int conn, char* Msg);

	int  SendDBSignal(int svr, unsigned short id, unsigned short signal);
	int  SendDBSignalParm(int svr, unsigned short id, unsigned short signal, int parm);
	int  SendDBSignalParm2(int svr, unsigned short id, unsigned short signal, int parm1, int parm2);
	int  SendDBSignalParm3(int svr, unsigned short id, unsigned short signal, int parm1, int parm2, int parm3);

	int  SendDBMessage(int svr, unsigned short id, char* Msg);

	void AddAccountList(int Idx);
	void RemoveAccountList(int Idx);

	int  CreateCharacter(char* ac, char* ch);
	int  DeleteCharacter(char* ch, char* account);

	int  GetIndex(int server, int id);
	int	 GetIndex(char* account);
	int  GetAccountsByMac(int* Mac);

	void SendDBSavingQuit(int Idx, int mode);

	int  DBWriteAccount(STRUCT_ACCOUNTFILE* account);
	int  DBExportAccount(STRUCT_ACCOUNTFILE* file);
	int  DBReadAccount(STRUCT_ACCOUNTFILE* file);

	int  DBCheckImpleName(char** ilchat, char* Str);

	void DBGetSelChar(STRUCT_SELCHAR* sell, STRUCT_ACCOUNTFILE* file);

	void GetAccountByChar(char* acc, char* cha);

	void InitAccountList(int idx);
	void SendGuildInfo(int srv, int Guild);
	int  GetEncPassword(int idx, int* Enc);
	void SetEncPassword(int idx, int newsrv, int* Enc, int Unk);

	////////////////////////////////////////////////

	bool Exec_MSG_ReqTransper(int conn, char* Msg);
	bool Exec_MSG_GuildZoneReport(char* Msg);
	bool Exec_MSG_War(char* Msg);
	bool Exec_MSG_GuildAlly(char* Msg);
	bool Exec_MSG_GuildInfo(char* Msg);
	bool Exec_MSG_DBUpdateSapphire(char* Msg);
	bool Exec_MSG_DBNewAccount(int conn, char* Msg);
	bool Exec_MSG_MessageDBRecord(int conn, char* Msg);
	bool Exec_MSG_NPAppeal(MSG_STANDARD* Msg);
	bool Exec_MSG_MessageDBImple(char* Msg);
	bool Exec_MSG_DBAccountLogin(int conn, char* Msg);
	bool Exec_MSG_DBCreateCharacter(int conn, char* Msg);
	bool Exec_MSG_DBCharacterLogin(int conn, char* Msg);
	bool Exec_MSG_DBNoNeedSave(int conn, char* Msg);
	bool Exec_MSG_DBSaveMob(int conn, char* Msg);
	bool Exec_MSG_SavingQuit(int conn, char* Msg);
	bool Exec_MSG_DBDeleteCharacter(int conn, char* Msg);
	bool Exec_MSG_AccountSecure(int conn, char* Msg);
	bool Exec_MSG_DBCreateArchCharacter(int conn, char* Msg);
	bool Exec_MSG_MagicTrumpet(BYTE* Msg);
	bool Exec_MSG_DBNotice(BYTE* Msg);
	bool Exec_MSG_DBCapsuleInfo(int conn, char* Msg);
	bool Exec_MSG_DBPutInCapsule(int conn, char* Msg);
	bool Exec_MSG_DBOutCapsule(int conn, char* Msg);
	bool Exec_MSG_DBServerChange(int conn, char* Msg);
	bool Exec_MSG_UpdateExpRanking(int conn, char* Msg);
	bool Exec_MSG_DBItemDayLog(char* Msg);
	bool Exec_MSG_DBPrimaryAccount(int conn, char* Msg);
};

extern CFileDBSRV cFileDB;
#endif