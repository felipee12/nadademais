#include "pch.h"

bool CUser::RequestChangeCity(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_STANDARDPARM))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [Exec_MSG_ChangeCity]");
		return false;
	}

	if (auto mob = cSrv.getMob(this->conn))
	{
		int city = BASE_GetVillage(mob->TargetX, mob->TargetY);

		if (city >= 0 && city <= 4)
		{
			mob->MOB.Merchant &= 0x3F;
			mob->MOB.Merchant |= city << 6;
		}

		cLog.Write(0, TL::Pakt, std::format("{} etc,changecity city:{} X:{} Y:{}", this->AccountName, city, mob->TargetX, mob->TargetY));
	}

	return true;
}