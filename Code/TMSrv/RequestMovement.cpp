#include "pch.h"

bool CUser::RequestMovement(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_Action))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [MSG_Action]");
		return false;
	}

	if (this->Mode != USER_PLAY)
	{
		cSend.SendHpMode(this);
		return false;
	}

	MSG_Action* m = (MSG_Action*)pMsg;

	auto mob = cSrv.getMob(this->conn);

	if (mob->MOB.CurrentScore.Hp == 0)
	{
		cSend.SendHpMode(this);
		//AddCrackError(conn, 5, 3);
		return false;
	}

	if (this->Trade.OpponentID)
	{
		//RemoveTrade(pUser[conn].Trade.OpponentID);
		//RemoveTrade(conn);
		return false;
	}
	if (this->TradeMode)
	{
		//RemoveTrade(conn);
		return false;
	}

	if ((mob->TargetX / 128) == 9 && (mob->TargetY / 128) == 1 || (mob->TargetX / 128) == 8 && (mob->TargetY / 128) == 2 || (mob->TargetX / 128) == 10 && (mob->TargetY / 128) == 2)
	{
		if ((mob->Extra.ClassMaster == CELESTIAL || mob->Extra.ClassMaster == CELESTIALCS || mob->Extra.ClassMaster == SCELESTIAL) && mob->MOB.CurrentScore.Level >= 180)
		{
			//DoRecall(conn);
			return false;
		}
	}

	int movetime = m->ClientTick;
	int checktime = 0;

	if (CurrentTime <= 0x1D4C0)
		checktime = 0;
	else
		checktime = CurrentTime - 120000;

	//Skill Ilusão
	if (m->Type == _MSG_Action3)
	{
		if (mob->MOB.Class != 3 || (mob->MOB.LearnedSkill & 2) == 0)
		{
			if (mob->MOB.CurrentScore.Level < MAX_LEVEL)
				/*if (AddCrackError(conn, 10, 28) == TRUE)
					return;*/

				return false;
		}

		/*if (m->Effect != TRUE && m->Speed != 6)
		{
			CloseUser(conn);

			return;
		}*/

		int mana = g_pSpell[73].ManaSpent;

		if (mob->MOB.CurrentScore.Mp < mana)
		{
			cSend.SendSetHpMp(this);
			return false;
		}

		mob->MOB.CurrentScore.Mp -= mana;
		//		pUser[conn].ReqMp -= mana;

		if (this->LastActionTick != 235543242 && movetime < this->LastIllusionTick + 900)
		{
			cLog.Write(this->conn, TL::Pakt, std::format("{} err,illusion {} {} 900ms limit", this->AccountName, movetime, this->LastIllusionTick));
			//AddCrackError(conn, 1, 105);
			return false;
		}
		if ((unsigned int)movetime > CurrentTime + 15000 || movetime < checktime)
		{
			cLog.Write(this->conn, TL::Pakt, std::format("{} err,illusion {} {} 15000ms limit", this->AccountName, movetime, CurrentTime));
			//AddCrackError(conn, 1, 104);
			return false;
		}
		this->LastIllusionTick = movetime;
	}

	else if (m->Type == _MSG_Action)
	{
		if (this->LastActionTick != 235543242 && movetime < this->LastIllusionTick + 900)
		{
			cLog.Write(this->conn, TL::Pakt, std::format("{} err,illusion {} {} 900ms limit", this->AccountName, movetime, this->LastIllusionTick));
			//AddCrackError(conn, 1, 103);
			return false;
		}

		if ((unsigned int)movetime > CurrentTime + 15000 || movetime < checktime)
		{
			cLog.Write(this->conn, TL::Pakt, std::format("{} err,illusion {} {} 15000ms limit", this->AccountName, movetime, CurrentTime));
			//AddCrackError(conn, 1, 102);
			return false;
		}

		//if (m->Effect != 0) { CloseUser(conn); return; }

		this->LastIllusionTick = movetime;
	}

	else if (m->Type == _MSG_Action2)
	{
		if (this->LastMove == _MSG_Action2)
		{
			cLog.Write(this->conn, TL::Pakt, std::format("{} err,can't send MSG_Stop continuously", this->AccountName));
			//AddCrackError(conn, 1, 101);
			return false;
		}

		//if (m->Effect != 0) { CloseUser(conn); return; }
	}

	if (m->Speed > (mob->MOB.CurrentScore.AttackRun & 0xF))
	{
		//AddCrackError(conn, 5, 4);
		cLog.Write(this->conn, TL::Pakt, std::format("{} etc,diffrent movement. {} {}", this->AccountName, m->Speed, mob->MOB.CurrentScore.AttackRun));
		m->Speed = mob->MOB.CurrentScore.AttackRun & 0xF;
	}

	int posx = mob->TargetX;
	int posy = mob->TargetY;

	if (/*m->Effect != 1 && m->Effect != 2 && */(m->TargetX < posx - VIEWGRIDX || m->TargetX  > posx + VIEWGRIDX || m->TargetY < posy - VIEWGRIDY || m->TargetY > posy + VIEWGRIDY))
	{
		if (m->TargetX < posx - VIEWGRIDX * 2 || m->TargetX > posx + VIEWGRIDX * 2 || m->TargetY < posy - VIEWGRIDY * 2 || m->TargetY > posy + VIEWGRIDY * 2)
		{
			MSG_Action sm_action;
			memset(&sm_action, 0, sizeof(MSG_Action));

			cGet.GetAction(conn, posx, posy, (MSG_Action*)&sm_action);
			sm_action.Type = _MSG_Action3;
			sm_action.Speed = 6;

			this->cSock.SendOneMessage((BYTE*)&sm_action, sizeof(MSG_Action));
		}
		//AddCrackError(conn, 1, 5);
		return false;
	}

	/*if (m->TargetX >= 3378 && m->TargetX <= 3398 && m->TargetY >= 1427 && m->TargetY <= 1448)
	{
		int partyleader = mob->Leader;

		if (partyleader <= 0)
			partyleader = conn;

		if (partyleader != conn || conn != Pista[2].Party[0].LeaderID && conn != Pista[2].Party[1].LeaderID && conn != Pista[2].Party[2].LeaderID)
		{
			int rd = rand() % 3;
			DoTeleport(conn, PistaPos[2][rd][0], PistaPos[2][rd][1]);
			return;
		}
	}*/

	if (m->TargetX <= 0 || m->TargetX >= 4096 || m->TargetY <= 0 || m->TargetY >= 4096)
	{
		cLog.Write(this->conn, TL::Pakt, std::format("{} err,action - viewgrid", mob->MOB.MobName));
		return false;
	}

	if (m->TargetX != mob->TargetX || m->TargetY != mob->TargetY)
	{
		unsigned char mapAttribute = cGet.GetAttribute(m->TargetX, m->TargetY);

		if (mapAttribute & 0x80 && (mob->MOB.CurrentScore.Level >= 35 && mob->MOB.CurrentScore.Level <= 999 || mob->Extra.ClassMaster != MORTAL))
		{
			this->ClientMsg(g_pMessageStringTable[_NN_Newbie_zone]);
			//DoRecall(conn);
			return false;
		}

		if (mapAttribute & 0x20)
		{
			if (mob->MOB.CurrentScore.Level <= MAX_LEVEL)
			{
				int Zone = BASE_GetGuild(m->TargetX, m->TargetY);

				if (Zone >= 0 && Zone < MAX_GUILDZONE && mob->MOB.Guild != g_pGuildZone[Zone].ChargeGuild)
				{
					this->ClientMsg(g_pMessageStringTable[_NN_Only_Guild_Members]);
					//DoRecall(conn);
					return false;
				}
			}
		}

		//CCastleZakum::CheckMove(conn, m->TargetX, m->TargetY);

		this->LastMove = m->Type;

		mob->LastSpeed = 0;

		strncpy(mob->Route, m->Route, MAX_ROUTE);

		/*m->TargetY = 5000;
		m->TargetX = 5000;*/

		if (cSrv.pMobGrid[m->TargetY][m->TargetX] && cSrv.pMobGrid[m->TargetY][m->TargetX] != this->conn)
		{
			int destx = m->TargetX;
			int desty = m->TargetY;

			int len = strlen(m->Route);

			int i = 0;

			cGet.GetEmptyMobGrid(this->conn, &destx, &desty);

			for (i = len; i >= 0; i--)
			{
				if (cSrv.pMobGrid[desty][destx] && cSrv.pMobGrid[desty][destx] != this->conn)
					continue;

				BASE_GetRoute(m->PosX, m->PosY, &destx, &desty, m->Route, i, (char*)cSrv.pHeightGrid);
			}

			if (i == -1)
			{
				m->TargetX = destx;
				m->TargetY = desty;

				m->Effect = 0;

				this->cSock.SendOneMessage((BYTE*)m, sizeof(m));
				return false;
			}
			m->TargetX = destx;
			m->TargetY = desty;

			this->cSock.AddMessage((BYTE*)m, sizeof(m));
		}

		memcpy(mob->Route, m->Route, MAX_ROUTE);

		cSend.GridMulticast(this->conn, m->TargetX, m->TargetY, (MSG_HEADER*)pMsg);

		if (m->Type == _MSG_Action3)
		{
			this->cSock.SendOneMessage((BYTE*)m, sizeof(MSG_Action));
			cSend.SendSetHpMp(this);
		}

		int len = strlen(mob->Route);
		unsigned char Unk = 0;

		if (len > 0 && len < MAX_ROUTE - 3)
		{
			Unk = mob->Route[len] + 3;
			Unk = Unk - 48;

			if (Unk < 1 || Unk > 9)
				Unk = 0;
			else
				Unk = 16 * Unk;
		}

		mob->MOB.CurrentScore.Merchant = mob->MOB.CurrentScore.Merchant | Unk & 0xF0;
		mob->MOB.BaseScore.Merchant = mob->MOB.CurrentScore.Merchant;


		/*if (BrState && BRItem > 0)
		{
			int tx = pMob[conn].TargetX;
			int ty = pMob[conn].TargetY;

			int lvl = pMob[conn].MOB.CurrentScore.Level;

			if (tx >= 2604 && ty >= 1708 && tx <= 2648 && ty <= 1744)
			{
				if (!BrGrid && lvl >= 100 && lvl < 1000)
					DoRecall(conn);

				if (BrGrid == 1 && lvl >= 200 && lvl < 1000)
					DoRecall(conn);
			}
		}

		if (Colo150Limit)
		{
			int lvl = pMob[conn].MOB.CurrentScore.Level;

			if (lvl >= 150)
			{
				int tx = pMob[conn].TargetX;
				int ty = pMob[conn].TargetY;

				if (tx >= 2604 && ty >= 1708 && tx <= 2648 && ty <= 1744)
					DoRecall(conn);

			}
		}*/
	}

	return true;
}