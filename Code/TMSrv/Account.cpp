#include "pch.h"

bool CUser::RequestAccountLogin(int id, MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_AccountLogin))
	{
		cLog.Write(0, TL::Pakt, "packet - Size Diferente [MSG_AccountLogin]");
		return false;
	}

	MSG_AccountLogin* m = (MSG_AccountLogin*)pMsg;

	if (!CheckString(m->AccountName, NAME_LENGTH) || !CheckString(m->AccountPassword, 12))
	{
		cLog.Write(0, TL::Pakt, "err - [Login] String Unicode");
		return false;
	}

	if (this->conn != id)
		this->conn = id;

	//sizes pré definidos de 12 para as strings de login
	if (strlen(m->AccountName) > 12 || strlen(m->AccountPassword) > 12)
	{
		this->ClientMsg("Impossivel Conectar");
		return false;
	}

	this->AccountName[NAME_LENGTH - 1] = 0;

	if (this->conn <= 0 || (this->conn >= (MAX_USER - ADMIN_RESERV)))
	{
		this->ClientMsg(g_pMessageStringTable[_NN_Reconnect]);

		this->cSock.SendMessageA();

		cSrv.CloseUser(this->conn);
		return false;
	}

	if (this->Mode != USER_ACCEPT)
	{
		this->ClientMsg("Login now, wait a moment.");

		//CrackLog(this->conn, " accountlogin");
		this->cSock.SendMessageA();
		return false;
	}

	/*if (m->Size < sizeof(MSG_AccountLogin))
		memset(this->Mac, 0xFF, sizeof(this->Mac));
	else
		memcpy(this->Mac, m->AdapterName, sizeof(this->Mac));*/

	m->Type = _MSG_DBAccountLogin;
	m->ID = this->conn;

	sscanf(m->AccountName, "%s", this->AccountName);

	_strupr(this->AccountName);

	strncpy(m->AccountName, this->AccountName, NAME_LENGTH);

	cSrv.DBSocket.SendOneMessage((BYTE*)m, sizeof(MSG_AccountLogin));

	this->Mode = USER_LOGIN;

	/*if (auto mob = cSrv.getMob(this->conn))
		mob->Mode = MOB_EMPTY;*/

	//if (!userControl(this->conn)) // control map user
	//	userMap.insert(std::make_pair(this->conn, this->Primary));

	return true;
}

bool CUser::RequestSecondPass(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_AccountSecure))
	{
		cLog.Write(0, TL::Pakt, "err Size Diferente [MSG_AccountSecure]");
		return false;
	}

	MSG_AccountSecure* m = (MSG_AccountSecure*)pMsg;

	if (!CheckString(m->NumericToken, 6) || !CheckString(m->Unknown, 10))
	{
		cLog.Write(0, TL::Pakt, "err [Senha2] String Unicode");
		cSrv.CloseUser(conn);
		return false;
	}

	if (conn <= 0 || conn >= MAX_USER)
		return false;

	if (strlen(m->NumericToken) > 6 || strlen(m->Unknown) > 10)
	{
		this->ClientMsg("Erro au autenticar, contate a administração!");
		return false;
	}

	m->ID = conn;

	cSrv.DBSocket.SendOneMessage((BYTE*)m, sizeof(MSG_AccountSecure));

	return true;
}