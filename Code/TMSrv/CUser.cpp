#include "pch.h"

CUser::CUser()
{
	this->Mode = USER_EMPTY;
	//this->Unk3 = 0;
	//this->IsBillConnect = 0;
	this->LastReceiveTime = 0;

	memset(this->Cargo, 0, sizeof(this->Cargo));

	//this->Admin = 0;
	this->CastleStatus = 0;
	this->MuteChat = 0;
	//this->UseItemTime = 0;
	//this->Message = 0;
	//this->AttackTime = 0;
	//this->LastClientTick = 0;
	//this->PotionTime = 0;

	this->Primary = false;
}

CUser::~CUser()
{

}

void CUser::AcceptUser()
{
	this->cSock.nRecvPosition = 0;
	this->cSock.nProcPosition = 0;
	this->cSock.nSendPosition = 0;
	this->Mode = USER_ACCEPT;
	//this->Unk3 = 0;
}

bool CUser::Disconnect()
{
	this->cSock.CloseSocket();
	this->cSock.Sock = 0;
	//this->IsBillConnect = 0;

	this->Mode = USER_EMPTY;

	this->AccountName[0] = 0;

	return true;
}