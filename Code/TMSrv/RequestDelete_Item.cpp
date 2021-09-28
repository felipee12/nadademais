#include "pch.h"

bool CUser::RequestDeleteItem(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_DeleteItem))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [MSG_DeleteItem]");
		return false;
	}

	if (this->Mode != USER_PLAY)
	{
		cSend.SendHpMode(this);
		return false;
	}

	auto mob = cSrv.getMob(this->conn);
	if (!mob)
		return false;

	MSG_DeleteItem* m = (MSG_DeleteItem*)pMsg;

	if (m->Slot < 0 || m->Slot >= MAX_CARRY - 4)
		return false;

	if (m->sIndex <= 0 || m->sIndex >= MAX_ITEMLIST)
		return false;

	if (this->Trade.OpponentID)
	{
		cSrv.RemoveTrade(this->Trade.OpponentID);
		cSrv.RemoveTrade(this->conn);
		return false;
	}

	/*if (this->IsBlocked)
	{
		RemoveTrade(pUser[conn].Trade.OpponentID);
		SendClientMsg(conn, "Sua conta está com o bloqueio ativo para essa operação.");
		RemoveTrade(conn);
		return;
	}*/

	char itemtmp[2048];
	BASE_GetItemCode(&mob->MOB.Carry[m->Slot], itemtmp);

	memset(&mob->MOB.Carry[m->Slot], 0, sizeof(STRUCT_ITEM));

	cLog.Write(this->conn, TL::Pakt, std::format("{} deleteitem, {}", mob->MOB.MobName, itemtmp));
	return true;
}