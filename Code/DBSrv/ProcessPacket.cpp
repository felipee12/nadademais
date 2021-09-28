#include "pch.h"

bool CFileDBSRV::ProcessMessage(int conn, char* Msg)
{
	MSG_HEADER* std = (MSG_HEADER*)Msg;

	switch (std->Type)
	{
		//case _MSG_ReqTransper:				return this->Exec_MSG_ReqTransper(conn, Msg);
	case _MSG_GuildZoneReport:			//return this->Exec_MSG_GuildZoneReport(Msg);
	case _MSG_War:						//return this->Exec_MSG_War(Msg);
	case _MSG_GuildAlly:				//return this->Exec_MSG_GuildAlly(Msg);
	case _MSG_GuildInfo:				//return this->Exec_MSG_GuildInfo(Msg);
	case _MSG_DBUpdateSapphire:			return this->Exec_MSG_DBUpdateSapphire(Msg);
		//case _MSG_DBNewAccount:				return this->Exec_MSG_DBNewAccount(conn, Msg);
		/*case _MSG_MessageDBRecord:			return this->Exec_MSG_MessageDBRecord(conn, Msg);
		case _MSG_NPAppeal:					return this->Exec_MSG_NPAppeal(std);
		case _MSG_MessageDBImple:			return this->Exec_MSG_MessageDBImple(Msg);*/
	case _MSG_DBAccountLogin:			return this->Exec_MSG_DBAccountLogin(conn, Msg);
	case _MSG_DBCreateCharacter:		return this->Exec_MSG_DBCreateCharacter(conn, Msg);
	case _MSG_DBCharacterLogin:			return this->Exec_MSG_DBCharacterLogin(conn, Msg);
	case _MSG_DBNoNeedSave:				return this->Exec_MSG_DBNoNeedSave(conn, Msg);
	case _MSG_DBSaveMob:				return this->Exec_MSG_DBSaveMob(conn, Msg);
	case _MSG_SavingQuit:				return this->Exec_MSG_SavingQuit(conn, Msg);
	case _MSG_DBDeleteCharacter:		return this->Exec_MSG_DBDeleteCharacter(conn, Msg);
	case _MSG_AccountSecure:			return this->Exec_MSG_AccountSecure(conn, Msg);
	case _MSG_DBCreateArchCharacter:	return this->Exec_MSG_DBCreateArchCharacter(conn, Msg);
	case _MSG_MagicTrumpet:				return this->Exec_MSG_MagicTrumpet((BYTE*)Msg);
	case _MSG_DBNotice:					return this->Exec_MSG_DBNotice((BYTE*)Msg);
	case _MSG_DBCapsuleInfo:			return this->Exec_MSG_DBCapsuleInfo(conn, Msg);
	case _MSG_DBPutInCapsule:			return this->Exec_MSG_DBPutInCapsule(conn, Msg);
	case _MSG_DBOutCapsule:				return this->Exec_MSG_DBOutCapsule(conn, Msg);
	case _MSG_DBServerChange:			return this->Exec_MSG_DBServerChange(conn, Msg);
		/*case _MSG_UpdateExpRanking:			return this->Exec_MSG_UpdateExpRanking(conn, Msg);
		case _MSG_DBItemDayLog:				return this->Exec_MSG_DBItemDayLog(Msg);*/
	case _MSG_DBPrimaryAccount:			return this->Exec_MSG_DBPrimaryAccount(conn, Msg);
	}

	return false;
}