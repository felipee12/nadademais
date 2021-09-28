#include "pch.h"

bool CUser::RequestCreateChar(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_CreateCharacter))
	{
		cLog.Write(0, TL::Pakt, "Size Diferente [MSG_CreateCharacter]");
		return false;
	}

	MSG_CreateCharacter* m = (MSG_CreateCharacter*)pMsg;

	if (!CheckString(m->MobName, NAME_LENGTH))
	{
		cLog.Write(0, TL::Pakt, "[CreateChacarcter] String Unicode");
		cSrv.CloseUser(this->conn);
		return false;
	}

	if (strlen(m->MobName) > 12)
	{
		this->ClientMsg("Erro ao tentar criar o personagem, contate a administração!");
		return false;
	}

	m->MobName[NAME_LENGTH - 1] = 0;
	m->MobName[NAME_LENGTH - 2] = 0;

	if (this->Mode != USER_SELCHAR)
	{
		cLog.Write(0, TL::Pakt, std::format("{} err,createchar not user_selchar ID:{} MODE:{}", this->AccountName, this->conn, this->Mode));
		this->SendClientSignal(0, _MSG_NewCharacterFail);
		return false;
	}

	if (BASE_CheckValidString(m->MobName))
	{
		m->Type = _MSG_DBCreateCharacter;
		m->ID = conn;

		this->Mode = USER_WAITDB;

		cSrv.DBSocket.SendOneMessage((BYTE*)m, sizeof(MSG_CreateCharacter));

		cLog.Write(this->conn, TL::Pakt, std::format("{} etc,createchar name:{} ID:{} MODE:{}", this->AccountName, m->MobName, this->conn, this->Mode));
	}
	else
		this->SendClientSignal(0, _MSG_NewCharacterFail);

	return true;
}

bool CUser::RequestDeleteCharacter(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_DeleteCharacter))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [MSG_DeleteCharacter]");
		return false;
	}

	MSG_DeleteCharacter* m = (MSG_DeleteCharacter*)pMsg;

	if (!CheckString(m->MobName, NAME_LENGTH) || !CheckString(m->Password, 12))
	{
		cLog.Write(0, TL::Pakt, "-err [DeleteChar] String Unicode");
		cSrv.CloseUser(this->conn);
		return false;
	}

	if (strlen(m->MobName) > 12 || strlen(m->Password) > 10)
		return false;

	m->MobName[NAME_LENGTH - 1] = '\0';
	m->MobName[NAME_LENGTH - 2] = '\0';
	m->Password[10] = '\0';

	if (this->Mode == USER_SELCHAR)
	{
		m->Type = _MSG_DBDeleteCharacter;
		m->ID = conn;

		this->Mode = USER_WAITDB;

		cSrv.DBSocket.SendOneMessage((BYTE*)m, sizeof(MSG_DeleteCharacter));

		cLog.Write(this->conn, TL::Pakt, std::format("{} etc,delchar name: {} {} {} ", this->AccountName, m->MobName, this->conn, this->Mode));
	}
	else
	{
		this->ClientMsg("Deleting Character. wait a moment.");

		cLog.Write(this->conn, TL::Pakt, std::format("{} err,delchar not user_selchar {} {}", this->AccountName, this->conn, this->Mode));
	}

	return true;
}

bool CUser::RequestCharacterLogin(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_CharacterLogin))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [MSG_CharacterLogin]");
		return false;
	}

	MSG_CharacterLogin* m = (MSG_CharacterLogin*)pMsg;

	if (m->Slot < 0 || m->Slot >= MOB_PER_ACCOUNT)
	{
		this->ClientMsg(g_pMessageStringTable[_NN_SelectCharacter]);
		return false;
	}

	if (this->Mode == USER_SELCHAR)
	{
		m->Type = _MSG_DBCharacterLogin;
		m->ID = this->conn;

		this->Mode = USER_CHARWAIT;
		cSrv.pMob[this->conn].Mode = MOB_USER;

		cSrv.pMob[this->conn].MOB.Merchant = 0;

		cSrv.DBSocket.SendOneMessage((BYTE*)m, sizeof(MSG_CharacterLogin));
	}
	else
	{
		this->ClientMsg("Wait a moment.");
		cLog.Write(this->conn, TL::Pakt, std::format("{} err,charlogin not user_selchar {} {}", this->AccountName, this->conn, this->Mode));
	}

	return true;
}

bool CUser::RequestLogout(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_HEADER))
	{
		cLog.Write(this->conn, TL::Pakt, std::format("{} Size Diferente [Exec_MSG_CharacterLogout] - Chegou: {} - Tamanho: {}", this->AccountName, pMsg->Size, sizeof(MSG_HEADER)));
		return false;
	}

	if (this->Mode != USER_PLAY)
	{
		cLog.Write(0, TL::Pakt, "-packet Modo do Usuario não é USER_PLAY [Exec_MSG_CharacterLogout]");
		return false;
	}

	cSrv.RemoveParty(conn);
	cSrv.CharLogOut(conn);

	this->cSock.SendMessageA();
	return true;
}