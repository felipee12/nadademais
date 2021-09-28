#include "pch.h"

bool CFileDBSRV::Exec_MSG_MagicTrumpet(BYTE* Msg)
{// Alterar depois para realizar o loop em todos os servidores e enviar para todos os jogadores
	for (int i = 0; i < MAX_SERVER; i++)
	{
		if (auto userDB = cSrv.getUserDB(i))
		{
			if (userDB->Mode == USER_EMPTY)
				continue;

			if (userDB->cSock.Sock != 0)
				userDB->cSock.SendOneMessage(Msg, sizeof(MSG_MagicTrumpet));
		}
	}

	/*for (int i = 0; i < MAX_ADMIN; i++)
	{
		if (pAdmin[i].Mode == USER_EMPTY)
			continue;

		if (pAdmin[i].cSock.Sock != 0)
			pAdmin[i].cSock.SendOneMessage(Msg, sizeof(MSG_MagicTrumpet));
	}*/
	return true;
}

bool CFileDBSRV::Exec_MSG_DBNotice(BYTE* Msg)
{
	for (int i = 0; i < MAX_SERVER; i++)
	{
		if (auto userDB = cSrv.getUserDB(i))
		{
			if (userDB->Mode == USER_EMPTY)
				continue;

			if (userDB->cSock.Sock != 0)
				userDB->cSock.SendOneMessage(Msg, sizeof(MSG_DBNotice));
		}
	}
	return true;
}

bool CFileDBSRV::Exec_MSG_DBServerChange(int conn, char* Msg)
{
	MSG_DBServerChange* m = (MSG_DBServerChange*)Msg;

	int ID = m->ID;
	int NewServer = m->NewServerID;
	int Slot = m->Slot;

	if (ID <= 0 || ID >= MAX_USER)
		return false;

	if (NewServer <= 0 || NewServer > MAX_SERVER + 1)
	{
		cLog.Write(0, TL::Err, std::format("-system err,remove server srvnum: {} ", NewServer));
		return false;
	}

	int Idx = this->GetIndex(conn, m->ID);
	int Enc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int EncResult = this->GetEncPassword(Idx, Enc);

	if (auto userDB = cSrv.getUserDB(conn))
	{
		if (EncResult)
		{
			MSG_STANDARDPARM sm;
			memset(&sm, 0, sizeof(MSG_STANDARDPARM));

			sm.Type = _MSG_DBServerSend1;
			sm.Size = sizeof(MSG_STANDARDPARM);
			sm.ID = ID;

			sm.Parm = EncResult;

			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_STANDARDPARM));
		}

		else
		{
			//SetEncPassword(Idx, NewServer, Enc, m->Slot);

			MSG_DBCNFServerChange sm;
			memset(&sm, 0, sizeof(MSG_DBCNFServerChange));

			sm.Type = _MSG_DBCNFServerChange;
			sm.Size = sizeof(MSG_DBCNFServerChange);
			sm.ID = ID;

			strcpy(sm.AccountName, pAccountList[Idx].File.Info.AccountName);

			sprintf(sm.Enc, "*%d %d %d %d %d %d %d %d %d %d", m->NewServerID,
				Enc[0], Enc[1], Enc[2], Enc[3], Enc[4], Enc[5], Enc[6], Enc[7], m->Slot);

			memcpy(pAccountList[Idx].File.TempKey, sm.Enc, sizeof(pAccountList[Idx].File.TempKey));

			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_DBCNFServerChange));
		}
	}
	return true;
}

bool CFileDBSRV::Exec_MSG_DBUpdateSapphire(char* Msg)
{
	MSG_STANDARDPARM* m = (MSG_STANDARDPARM*)Msg;

	if (m->Parm == 1)
		cSrv.Sapphire *= 2;
	else
		cSrv.Sapphire /= 2;

	if (cSrv.Sapphire < 1)
		cSrv.Sapphire = 1;
	else if (cSrv.Sapphire > 64)
		cSrv.Sapphire = 64;

	for (int i = 0; i < MAX_SERVER; i++)
	{
		if (auto userDB = cSrv.getUserDB(i))
		{
			if (userDB->Mode == USER_EMPTY)
				continue;

			if (userDB->cSock.Sock != 0)
				SendDBSignalParm3(i, 0, _MSG_DBSetIndex, -1, cSrv.Sapphire, i);
		}
	}

	cSrv.WriteConfig();
	return true;
}