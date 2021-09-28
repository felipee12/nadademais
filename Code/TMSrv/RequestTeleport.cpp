#include "pch.h"

bool CUser::RequestTeleport(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_STANDARDPARM))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [Exec_MSG_ReqTeleport]");
		return false;
	}

	if (auto mob = cSrv.getMob(this->conn))
	{
		int posx = mob->TargetX;
		int posy = mob->TargetY;

		if ((posx / 4) == 491 && (posy / 4) == 443)
		{
			this->ClientMsg(g_pMessageStringTable[_NN_Only_By_Water_Scroll]);
			return false;
		}

		int reqcoin = cGet.GetTeleportPosition(this, &posx, &posy);

		auto clan = mob->MOB.Clan;

		if (g_pGuildZone[4].Clan == clan)
			goto label_tel;

		if (reqcoin <= mob->MOB.Coin)
		{
			if (reqcoin > 0)
			{
				mob->MOB.Coin -= reqcoin;
				this->SendEtc();

				int master = cSrv.GuildImpostoID[4];
				if (auto npcimpost = cSrv.getMob(master))
				{
					if (cSrv.GuildImpostoID[4] >= MAX_USER && master < MAX_MOB && npcimpost->Mode != MOB_EMPTY && npcimpost->MOB.Guild == g_pGuildZone[4].ChargeGuild)
					{
						if (npcimpost->MOB.Exp < 2000000000000)
							npcimpost->MOB.Exp += reqcoin / 2;
					}
				}
			}

		label_tel:
			if (posx != mob->TargetX || posy != mob->TargetY)
			{
				cSrv.DoTeleport(this->conn, posx, posy);
				cLog.Write(this->conn, TL::Sys, std::format("{} etc,reqteleport char:{} X:{} Y:{} price:{}", this->AccountName, mob->MOB.MobName, mob->TargetX, mob->TargetY, reqcoin));
			}
		}
		else
			this->ClientMsg(g_pMessageStringTable[_NN_Not_Enough_Money]);
	}

	return true;
}