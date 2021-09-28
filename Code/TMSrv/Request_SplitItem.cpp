#include "pch.h"

bool CUser::RequestSplitItem(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_SplitItem))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [MSG_SplitItem]");
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

	MSG_SplitItem* m = (MSG_SplitItem*)pMsg;

	if (m->Slot < 0 || m->Slot >= MAX_CARRY - 4)
		return false;

	if (m->Num <= 0 || m->Num >= 120)
		return false;

	if (this->Trade.OpponentID)
	{
		cSrv.RemoveTrade(this->conn);
		return false;
	}

	int slot = m->Slot;

	if (!cGet.GetItemHab(mob->MOB.Carry[slot].sIndex, EF_JOIN))
	{
		cLog.Write(this->conn, TL::Pakt, std::format("{} splititem, no validitem", this->AccountName));
		return false;
	}

	int amount = BASE_GetItemAmount(&mob->MOB.Carry[slot]);

	if (m->Num < 0 || m->Num > 120)
	{
		cLog.Write(this->conn, TL::Pakt, std::format("{} splititem, invalid num", this->AccountName));
		return false;
	}

	if (!cGet.GetFreeSlot(mob, 1))
		return false;

	if (amount == 0 || amount == 1 || amount <= m->Num)
		return false;

	cLog.Write(this->conn, TL::Pakt, std::format("{} splititem, itemindex:{} amount:{} num:{}", mob->MOB.MobName, mob->MOB.Carry[slot].sIndex, amount, m->Num));

	if (amount > 1)
		BASE_SetItemAmount(&mob->MOB.Carry[slot], amount - m->Num);

	else
		memset(&mob->MOB.Carry[slot], 0, sizeof(STRUCT_ITEM));

	STRUCT_ITEM nItem;

	memset(&nItem, 0, sizeof(STRUCT_ITEM));

	nItem.sIndex = mob->MOB.Carry[slot].sIndex;

	BASE_SetItemAmount(&nItem, m->Num);

	this->PutItem(mob, &nItem);
	this->SendItem(ItSlot::Carry, slot, &mob->MOB.Carry[slot]);
	return true;
}