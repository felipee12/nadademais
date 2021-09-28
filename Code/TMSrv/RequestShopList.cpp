#include "pch.h"

bool CUser::RequestShopList(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_REQShopList))
	{
		cLog.Write(0, TL::Err, "-packet Size Diferente [MSG_REQShopList]");
		return false;
	}

	auto mob = cSrv.getMob(this->conn);
	if (!mob)
		return false;

	if (mob->MOB.CurrentScore.Hp == 0 || this->Mode != USER_PLAY)
	{
		cSend.SendHpMode(this);
		//AddCrackError(conn, 10, 22);
		return false;
	}

	MSG_REQShopList* m = (MSG_REQShopList*)pMsg;

	int target = m->Target;

	if (target < MAX_USER || target >= MAX_MOB)
	{
		cLog.Write(0, TL::Err, "-err err,MSG_ReqShopList target range");
		return false;
	}

	mob = cSrv.getMob(target);
	if (!mob)
		return false;

	if (mob->MOB.Merchant == 0)
	{
		this->ClientMsg(g_pMessageStringTable[_NN_He_Is_Not_Merchant]);
		return false;
	}

	int view = cGet.GetInView(this->conn, mob);

	if (view == 0)
		return false;

	if (mob->MOB.Merchant == 1)
		this->SendShopList(target, 1);

	else if ((mob->MOB.Merchant) == 19)
		this->SendShopList(target, 3);

	return true;
}