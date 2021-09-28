#include "pch.h"

CFileDBSRV cFileDB = CFileDBSRV();

CFileDBSRV::CFileDBSRV()
{
	for (int i = 0; i < MAX_DBACCOUNT; i++)
	{
		this->pAccountList[i].Login = 0;
		this->pAccountList[i].Slot = -1;
		this->pAccountList[i].SecurePass = -1;

		this->pAccountList[i].Mac[0] = 0;
		this->pAccountList[i].Mac[1] = 0;
		this->pAccountList[i].Mac[2] = 0;
		this->pAccountList[i].Mac[3] = 0;

		memset(&this->pAccountList[i].File, 0, sizeof(this->pAccountList[i].File));
	}
}

CFileDBSRV::~CFileDBSRV()
{
}

int CFileDBSRV::SendDBSignal(int svr, unsigned short id, unsigned short signal)
{
	MSG_HEADER sm;

	sm.Type = signal;
	sm.ID = id;
	sm.Size = sizeof(sm);

	if (auto userDB = cSrv.getUserDB(svr))
	{
		if (userDB->cSock.Sock)
			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(sm));
	}

	return TRUE;
}

int CFileDBSRV::SendDBSignalParm(int svr, unsigned short id, unsigned short signal, int parm)
{
	MSG_STANDARDPARM sm;

	sm.Type = signal;
	sm.ID = id;
	sm.Size = sizeof(sm);
	sm.Parm = parm;

	if (auto userDB = cSrv.getUserDB(svr))
	{
		if (userDB->cSock.Sock)
			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(sm));
	}

	return TRUE;
}

int CFileDBSRV::SendDBSignalParm2(int svr, unsigned short id, unsigned short signal, int parm1, int parm2)
{
	MSG_STANDARDPARM2 sm;

	sm.Type = signal;
	sm.ID = id;
	sm.Size = sizeof(sm);
	sm.Parm1 = parm1;
	sm.Parm2 = parm2;

	if (auto userDB = cSrv.getUserDB(svr))
	{
		if (userDB->cSock.Sock)
			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(sm));
	}

	return TRUE;
}

int CFileDBSRV::SendDBSignalParm3(int svr, unsigned short id, unsigned short signal, int parm1, int parm2, int parm3)
{
	MSG_STANDARDPARM3 sm;

	sm.Type = signal;
	sm.ID = id;
	sm.Size = sizeof(sm);
	sm.Parm1 = parm1;
	sm.Parm2 = parm2;
	sm.Parm3 = parm3;

	if (auto userDB = cSrv.getUserDB(svr))
	{
		if (userDB->cSock.Sock)
			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(sm));
	}

	return TRUE;
}

void CFileDBSRV::AddAccountList(int Idx)
{
	if (pAccountList[Idx].Login == TRUE)
	{
		cLog.Write(0, TL::Err, std::format("{} err,addAccountlist - already added", pAccountList[Idx].File.Info.AccountName));
		return;
	}

	int conn = Idx / MAX_USER;

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->Count++;

	pAccountList[Idx].Login = TRUE;
	pAccountList[Idx].Slot = -1;

	return;
}

void CFileDBSRV::RemoveAccountList(int Idx)
{
	if (pAccountList[Idx].Login == FALSE)
		return;

	int conn = Idx / MAX_USER;

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->Count--;

	pAccountList[Idx].Login = FALSE;
	pAccountList[Idx].Slot = -1;

	memset(&pAccountList[Idx].File, 0, sizeof(pAccountList[Idx].File));
}

int CFileDBSRV::CreateCharacter(char* ac, char* ch)
{
	char check[ACCOUNTNAME_LENGTH];

	strncpy(check, ac, ACCOUNTNAME_LENGTH);

	_strupr(check);

	if (check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;
	if (check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;

	strncpy(check, ch, NAME_LENGTH);

	_strupr(check);

	if (check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;
	if (check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;

	char First[128];
	char temp[128];

	BASE_GetFirstKey(check, First);

	sprintf(temp, "./char/%s/%s", First, check);

	FILE* fp = NULL;

	fp = fopen(temp, "r");

	if (fp != NULL)
	{
		fclose(fp);
		cLog.Write(0,TL::Err, std::format("{} err createchar EEXIST", ch));
		return FALSE;
	}

	int Handle = _open(temp, O_RDWR | O_CREAT | O_BINARY, _S_IREAD | _S_IWRITE);

	if (Handle == -1)
	{
		if (errno == EEXIST)
			cLog.Write(0, TL::Err, std::format("{} err createchar EEXIST", ch));
		if (errno == EACCES)
			cLog.Write(0, TL::Err, std::format("{} err createchar EACCES", ch));
		if (errno == EINVAL)
			cLog.Write(0, TL::Err, std::format("{} err createchar EINVAL", ch));
		if (errno == EMFILE)
			cLog.Write(0, TL::Err, std::format("{} err createchar EMFILE", ch));
		if (errno == ENOENT)
			cLog.Write(0, TL::Err, std::format("{} err createchar ENOENT", ch));
		else
			cLog.Write(0, TL::Err, std::format("{} err createchar UNKNOWN", ch));

		return FALSE;
	}

	_write(Handle, ac, ACCOUNTNAME_LENGTH);
	_close(Handle);

	return TRUE;
}

int CFileDBSRV::DeleteCharacter(char* ch, char* account)
{
	char check[ACCOUNTNAME_LENGTH];

	strncpy(check, ch, ACCOUNTNAME_LENGTH);

	_strupr(check);

	if (check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;
	if (check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;

	char First[128];
	char temp[128];

	BASE_GetFirstKey(check, First);

	sprintf(temp, "./char/%s/%s", First, check);

	int ret = DeleteFileA(temp);

	return ret;
}

int CFileDBSRV::GetIndex(int server, int id)
{
	int ret = server * MAX_USER + id;
	return ret;
}

int CFileDBSRV::GetIndex(char* account)
{
	for (int i = 0; i < MAX_DBACCOUNT; i++)
	{
		if (pAccountList[i].Login == 0)
			continue;

		if (strcmp(pAccountList[i].File.Info.AccountName, account) == 0)
			return i;
	}

	return 0;
}

int CFileDBSRV::GetAccountsByMac(int* Mac)
{
	int num = 0;

	for (int i = 0; i < MAX_DBACCOUNT; i++)
	{
		if (pAccountList[i].Login == 0)
			continue;

		if (memcmp(pAccountList[i].Mac, Mac, sizeof(pAccountList[i].Mac)) == 0)
			num++;
	}

	return num;
}

void CFileDBSRV::SendDBSavingQuit(int Idx, int mode)
{
	int conn = Idx / MAX_USER;
	int id = Idx % MAX_USER;

	MSG_DBSavingQuit sm;
	memset(&sm, 0, sizeof(MSG_DBSavingQuit));

	sm.Type = _MSG_DBSavingQuit;
	sm.ID = id;
	sm.Size = sizeof(MSG_DBSavingQuit);
	sm.Mode = mode;

	memcpy(sm.AccountName, pAccountList[Idx].File.Info.AccountName, ACCOUNTNAME_LENGTH);

	if (auto userDB = cSrv.getUserDB(conn))
	{
		if (userDB->cSock.Sock && userDB->Mode != USER_EMPTY)
			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_DBSavingQuit));
	}

	return;
}

int CFileDBSRV::DBWriteAccount(STRUCT_ACCOUNTFILE* account)
{
	char* accname = account->Info.AccountName;

	char check[ACCOUNTNAME_LENGTH];

	strncpy(check, accname, ACCOUNTNAME_LENGTH);

	_strupr(check);

	if (check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;
	if (check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;

	char First[128];

	BASE_GetFirstKey(check, First);

	char temp[128];

	sprintf(temp, "./account/%s/%s", First, check);

	int Handle = _open(temp, O_RDWR | O_CREAT | O_BINARY, _S_IREAD | _S_IWRITE);

	if (Handle == -1)
	{
		if (errno == EEXIST)
			cLog.Write(0, TL::Err, std::format("{} err writeaccount EEXIST", accname));
		if (errno == EACCES)
			cLog.Write(0, TL::Err, std::format("{} err writeaccount EACCES", accname));
		if (errno == EINVAL)
			cLog.Write(0, TL::Err, std::format("{} err writeaccount EINVAL", accname));
		if (errno == EMFILE)
			cLog.Write(0, TL::Err, std::format("{} err writeaccount EMFILE", accname));
		if (errno == ENOENT)
			cLog.Write(0, TL::Err, std::format("{} err writeaccount ENOENT", accname));
		else
			cLog.Write(0, TL::Err, std::format("{} err writeaccount UNKNOWN", accname));

		return FALSE;
	}

	int ret = _lseek(Handle, 0, SEEK_SET);

	if (ret == -1)
	{
		cLog.Write(0, TL::Err, std::format("{} err,writeaccount lseek fail", accname));
		_close(Handle);
		return FALSE;
	}

	ret = _write(Handle, account, sizeof(STRUCT_ACCOUNTFILE));

	if (ret == -1)
	{
		_close(Handle);

		cLog.Write(0, TL::Err, std::format("{} CreateAccount write fail", accname));

		if (errno == EEXIST)
			cLog.Write(0, TL::Err, std::format("{} err CreateAccount EEXIST", accname));
		if (errno == EACCES)
			cLog.Write(0, TL::Err, std::format("{} err CreateAccount EACCES", accname));
		if (errno == EINVAL)
			cLog.Write(0, TL::Err, std::format("{} err CreateAccount EINVAL", accname));
		if (errno == EMFILE)
			cLog.Write(0, TL::Err, std::format("{} err CreateAccount EMFILE", accname));
		if (errno == ENOENT)
			cLog.Write(0, TL::Err, std::format("{} err CreateAccount ENOENT", accname));

		return FALSE;
	}

	_close(Handle);
	return TRUE;
}

int CFileDBSRV::DBExportAccount(STRUCT_ACCOUNTFILE* file)
{
	char check[ACCOUNTNAME_LENGTH];

	char* accname = file->Info.AccountName;

	strncpy(check, accname, 16);

	_strupr(check);

	if (check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;
	if (check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;

	char temp[128];

	sprintf(temp, "S:/export/account%d/%s", cSrv.ServerIndex, check);

	int Handle = _open(temp, O_RDWR | O_CREAT | O_BINARY, _S_IREAD | _S_IWRITE);

	if (Handle == -1)
		return FALSE;

	int ret = _lseek(Handle, 0, SEEK_SET);

	if (ret == -1)
	{
		_close(Handle);

		return FALSE;
	}

	ret = _write(Handle, file, sizeof(STRUCT_ACCOUNTFILE));

	if (ret == -1)
	{
		_close(Handle);
		return FALSE;
	}

	_close(Handle);
	return TRUE;
}

int CFileDBSRV::DBReadAccount(STRUCT_ACCOUNTFILE* file)
{
	file->Info.AccountName[ACCOUNTNAME_LENGTH - 1] = 0;
	file->Info.AccountName[ACCOUNTNAME_LENGTH - 2] = 0;
	file->Info.AccountPass[ACCOUNTPASS_LENGTH - 1] = 0;
	file->Info.AccountPass[ACCOUNTPASS_LENGTH - 2] = 0;

	char check[ACCOUNTNAME_LENGTH];

	strncpy(check, file->Info.AccountName, ACCOUNTNAME_LENGTH);

	_strupr(check);

	if (check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;
	if (check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return FALSE;

	char First[128];

	BASE_GetFirstKey(check, First);

	char temp[128];

	sprintf(temp, "./account/%s/%s", First, check);

	int Handle = _open(temp, O_RDONLY | O_BINARY);

	if (Handle == -1)
	{
		if (errno == EINVAL)
			cLog.Write(0, TL::Err, std::format("{} err readaccount EINVAL", file->Info.AccountName));
		else if (errno == EMFILE)
			cLog.Write(0, TL::Err, std::format("{} err readaccount EEMFILE", file->Info.AccountName));

		return FALSE;
	}

	int length = _filelength(Handle);

	int reqsz = sizeof(STRUCT_ACCOUNTFILE);

	int ret = _read(Handle, file, reqsz);

	if (ret == -1)
	{
		_close(Handle);
		return FALSE;
	}

	_close(Handle);

	if (length < reqsz)
		memset(file->ShortSkill, 0, 8);

	return TRUE;
}

void CFileDBSRV::DBGetSelChar(STRUCT_SELCHAR* sel, STRUCT_ACCOUNTFILE* file)
{
	if (sel && file)
	{
		for (int i = 0; i < MOB_PER_ACCOUNT; i++)
		{
			memcpy(sel->Name[i], file->Char[i].MobName, NAME_LENGTH);
			memcpy(sel->Equip[i], file->Char[i].Equip, sizeof(file->Char[i].Equip));

			if (sel->Equip[i][0].sIndex == 22 || sel->Equip[i][0].sIndex == 23 || sel->Equip[i][0].sIndex == 24 || sel->Equip[i][0].sIndex == 25 || sel->Equip[i][0].sIndex == 32)
				sel->Equip[i][0].sIndex = file->mobExtra[i].ClassMaster == MORTAL ? 21 : file->mobExtra[i].MortalFace + 7;

			sel->Guild[i] = file->Char[i].Guild;

			sel->SPX[i] = file->Char[i].SPX;
			sel->SPY[i] = file->Char[i].SPY;

			sel->Score[i] = file->Char[i].CurrentScore;

			sel->Coin[i] = file->Char[i].Coin;
			sel->Exp[i] = file->Char[i].Exp;
		}
	}
}

void CFileDBSRV::GetAccountByChar(char* acc, char* cha)
{
	char check[NAME_LENGTH];

	strncpy(check, cha, NAME_LENGTH);

	_strupr(check);

	if (check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return;
	if (check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0)
		return;

	char first[16];
	memset(first, 0, 16);

	BASE_GetFirstKey(check, first);

	char temp[128];

	sprintf(temp, "./char/%s/%s", first, check);

	int Handle = _open(temp, O_RDONLY | O_BINARY);

	if (Handle == -1)
	{
		return;
	}

	_read(Handle, acc, ACCOUNTNAME_LENGTH);
	_close(Handle);
}

int CFileDBSRV::GetEncPassword(int idx, int* Enc)
{
	Enc[0] = rand() % 900 + 100;
	Enc[1] = rand() % 900 + 100;
	Enc[2] = rand() % 900 + 100;
	Enc[3] = rand() % 900 + 100;
	Enc[4] = rand() % 900 + 100;
	Enc[5] = rand() % 900 + 100;
	Enc[6] = rand() % 900 + 100;
	Enc[7] = rand() % 900 + 100;

	return FALSE;
}

