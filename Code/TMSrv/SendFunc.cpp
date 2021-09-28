#include "pch.h"

CSend cSend = CSend();

void CUser::ClientMsg(const std::string& Message)
{
	if (Message.size() > MESSAGE_LENGTH)
		return;

	auto sm = TP_HEADER(_MSG_MessagePanel, 0, MSG_MessagePanel());	
	strncpy(sm.Data.String, Message.c_str(), MESSAGE_LENGTH);
	this->cSock.AddMessage((BYTE*)&sm, sizeof(sm));
}

void CUser::SendClientSignal(int16 id, uint16 signal)
{
	MSG_HEADER sm;
	memset(&sm, 0, sizeof(MSG_HEADER));

	sm.Type = signal;
	sm.ID = id;

	this->cSock.AddMessage((BYTE*)&sm, sizeof(sm));
}

void CSend::SendRemoveMob(CUser* user, int sour, int Type, int bSend)
{
	if (user)
	{
		MSG_RemoveMob sm;
		memset(&sm, 0, sizeof(MSG_RemoveMob));

		sm.Type = _MSG_RemoveMob;
		sm.Size = sizeof(MSG_RemoveMob);
		sm.ID = sour;
		sm.RemoveType = Type;

		user->cSock.AddMessage((BYTE*)&sm, sizeof(MSG_RemoveMob));

		if (bSend)
			user->cSock.SendMessageA();
	}
}

void CSend::SendRemoveItem(CUser* user, int itemid, int bSend)
{
	if (user)
	{
		MSG_DecayItem sm_deci;
		memset(&sm_deci, 0, sizeof(MSG_DecayItem));

		sm_deci.Type = _MSG_DecayItem;
		sm_deci.Size = sizeof(MSG_DecayItem);
		sm_deci.ID = ESCENE_FIELD;
		sm_deci.ItemID = 10000 + itemid;
		sm_deci.unk = 0;

		user->cSock.AddMessage((BYTE*)&sm_deci, sizeof(MSG_DecayItem));

		if (bSend)
			user->cSock.SendMessageA();
	}
}

void CSend::GridMulticast(int conn, int tx, int ty, MSG_HEADER* msg)
{
	if (msg == nullptr)
		return;

	auto user = cSrv.getUser(conn);
	if (!user)
		return;
	auto mob = cSrv.getMob(conn);
	if (!mob)
		return;

	if (conn == 0 || mob->TargetX == 0)
	{
		cLog.Write(0, TL::Err, "-system err,GridMulticast mobidx,pos");
		return;
	}

	int mobx = mob->TargetX;
	int moby = mob->TargetY;

	int currentgrid = cSrv.pMobGrid[moby][mobx];

	if (currentgrid != conn && currentgrid != 0)
	{
		if (conn >= MAX_USER)
		{
			cLog.Write(0, TL::Err, "-system NPC do not have his own grid");;
		}
		else
		{
			cLog.Write(0, TL::Err, "-system PC do not have his own grid");;
		}

		cSrv.pMobGrid[moby][mobx] = 0;
	}
	else
	{
		cSrv.pMobGrid[moby][mobx] = 0;
	}

	if (cSrv.pMobGrid[ty][tx] != conn && cSrv.pMobGrid[ty][tx] != 0)
	{
		if (conn >= MAX_USER)
		{
			cLog.Write(0, TL::Err, "-system NPC charge other mob's grid");;
		}
		else
		{
			cLog.Write(0, TL::Err, "-system PC step in other mob's grid");;
		}
		cSrv.pMobGrid[ty][tx] = conn;
	}
	else
	{
		cSrv.pMobGrid[ty][tx] = conn;
	}

	int SizeY = VIEWGRIDY;
	int SizeX = VIEWGRIDX;
	int StartX = mob->TargetX - HALFGRIDX;
	int StartY = mob->TargetY - HALFGRIDY;

	if (StartX + SizeX >= MAX_GRIDX)
		SizeX = SizeX - (StartX + SizeX - MAX_GRIDX);

	if (SizeY + StartY >= MAX_GRIDY)
		SizeY = SizeY - (StartY + SizeY - MAX_GRIDY);

	if (StartX < 0)
		StartX = 0;

	if (StartY < 0)
		StartY = 0;

	int sx1 = StartX;
	int sy1 = StartY;
	int sx2 = StartX + SizeX;
	int sy2 = StartY + SizeY;

	SizeY = VIEWGRIDY;
	SizeX = VIEWGRIDX;
	StartX = tx - HALFGRIDX;
	StartY = ty - HALFGRIDY;

	if (StartX + SizeX >= MAX_GRIDX)
		SizeX = SizeX - (StartX + SizeX - MAX_GRIDX);

	if (SizeY + StartY >= MAX_GRIDY)
		SizeY = SizeY - (StartY + SizeY - MAX_GRIDY);

	if (StartX < 0)
		StartX = 0;

	if (StartY < 0)
		StartY = 0;

	int tx1 = StartX;
	int ty1 = StartY;
	int tx2 = StartX + SizeX;
	int ty2 = StartY + SizeY;

	for (int y = sy1; y < sy2; y++)
	{
		for (int x = sx1; x < sx2; x++)
		{
			int tmob = cSrv.pMobGrid[y][x];
			
			auto pUser = cSrv.getUser(tmob);
			if (!pUser)
				continue;

			if (tmob == 0 || tmob == conn)
				continue;

			if (msg != NULL && tmob < MAX_USER)
				pUser->cSock.AddMessage((BYTE*)msg, msg->Size);

			if (x < tx1 || x >= tx2 || y < ty1 || y >= ty2 && tx)
			{
				if (tmob < MAX_USER)
					this->SendRemoveMob(pUser, conn, 0, 0);

				if (conn < MAX_USER)
					SendRemoveMob(user, tmob, 0, 0);
			}
		}
	}

	for (int y = sy1; y < sy2; y++)
	{
		for (int x = sx1; x < sx2; x++)
		{
			int titem = cSrv.pItemGrid[y][x];

			if (titem == 0)
				continue;

			if (x < tx1 || x >= tx2 || y < ty1 || y >= ty2 && tx)
			{
				if (titem > 0 && titem < MAX_ITEM && cSrv.pItem[titem].Mode && cSrv.pItem[titem].ITEM.sIndex && conn > 0 && conn < MAX_USER)
					SendRemoveItem(user, titem, 0);
			}
		}
	}

	for (int y = ty1; y < ty2; y++)
	{
		for (int x = tx1; x < tx2; x++)
		{
			int titem = cSrv.pItemGrid[y][x];
			int tmob = cSrv.pMobGrid[y][x];

			if (x < sx1 || x >= sx2 || y < sy1 || y >= sy2)
			{
				if (titem > 0 && titem < MAX_ITEM && cSrv.pItem[titem].Mode && cSrv.pItem[titem].ITEM.sIndex && conn < MAX_USER)
				{
					if (cSrv.pItem[titem].Mode)
						SendCreateItem(user, titem, 0);

					else
						cSrv.pItemGrid[y][x] = 0;
				}

				if (tmob != conn && tmob)
				{
					auto userObj = cSrv.getUser(tmob);
					if (!userObj)
						continue;

					if (tmob < MAX_USER)
					{
						SendCreateMob(userObj, conn, 0);
						SendPKInfo(userObj, conn);
					}

					if (conn < MAX_USER)
					{
						SendCreateMob(user, tmob, 0);
						SendPKInfo(user, tmob);
					}

					if (msg != NULL && tmob > 0 && tmob < MAX_USER)
					{
						if (userObj->cSock.AddMessage((BYTE*)msg, msg->Size) == 0)
						{
							userObj->AccountName[ACCOUNTNAME_LENGTH - 1] = 0;
							userObj->AccountName[ACCOUNTNAME_LENGTH - 2] = 0;

							cLog.Write(0, TL::Err, std::format("-system err,gridmulticast add {}-{} {}", tmob, userObj->Mode, userObj->AccountName));
						}
					}
				}
			}
		}
	}

	MSG_Action* sm = (MSG_Action*)msg;

	mob->LastTime = sm->ClientTick;
	mob->LastSpeed = sm->Speed;

	mob->LastX = sm->PosX;
	mob->LastY = sm->PosY;

	mob->TargetX = tx;
	mob->TargetY = ty;
}

void CSend::GridMulticast(int tx, int ty, MSG_HEADER* msg, int skip)
{
	if (msg == nullptr)
		return;

	int SizeY = VIEWGRIDY;
	int SizeX = VIEWGRIDX;
	int StartX = tx - HALFGRIDX;
	int StartY = ty - HALFGRIDY;

	if (StartX + SizeX >= MAX_GRIDX)
		SizeX = SizeX - (StartX + SizeX - MAX_GRIDX);

	if (SizeY + StartY >= MAX_GRIDY)
		SizeY = SizeY - (StartY + SizeY - MAX_GRIDY);

	if (StartX < 0)
	{
		StartX = 0;
		SizeX = SizeX + StartX;
	}

	if (StartY < 0)
	{
		StartY = 0;
		SizeY = SizeY + StartY;
	}

	int sx1 = StartX;
	int sy1 = StartY;
	int sx2 = StartX + SizeX;
	int sy2 = StartY + SizeY;

	for (int y = sy1; y < sy2; y++)
	{
		for (int x = sx1; x < sx2; x++)
		{
			int tmob = cSrv.pMobGrid[y][x];

			if (tmob <= 0 || tmob == skip)
				continue;

			if (msg != NULL && tmob < MAX_USER)
			{
				if (msg->Type == _MSG_CreateMob)
				{
					int xx = ((MSG_CreateMob*)msg)->PosX;
					int yy = ((MSG_CreateMob*)msg)->PosY;

					if (xx >= 896 && yy >= 1405 && xx <= 1150 && yy <= 1538)
					{
						STRUCT_ITEM hcitem;

						memset(&hcitem, 0, sizeof(STRUCT_ITEM));

						hcitem.sIndex = 3505;

						((MSG_CreateMob*)msg)->Equip[1] = BASE_VisualItemCode(&hcitem, 1);
						((MSG_CreateMob*)msg)->AnctCode[1] = BASE_VisualAnctCode(&hcitem);

						hcitem.sIndex = 3999;
						((MSG_CreateMob*)msg)->Equip[15] = BASE_VisualItemCode(&hcitem, 15);
						((MSG_CreateMob*)msg)->AnctCode[15] = BASE_VisualAnctCode(&hcitem);
					}

				}

				auto user = cSrv.getUser(tmob);
				if (!user)
					continue; // return?

				if (msg->Type == _MSG_CNFMobKill)
				{
					auto mob = cSrv.getMob(tmob);
					if (!mob)
						continue;

					((MSG_CNFMobKill*)msg)->Exp = mob->MOB.Exp;
					((MSG_CNFMobKill*)msg)->Hold = mob->Extra.Hold;

					//int Segment = mob->CheckGetLevel();

					//if (Segment >= 1 && Segment <= 4)
					//{
					//	switch (Segment)
					//	{
					//	case 4:
					//	{
					//		/*SetCircletSubGod(tmob);
					//		SendClientMsg(tmob, g_pMessageStringTable[_NN_Level_Up]);

					//		if (pMob[tmob].Extra.ClassMaster == MORTAL)
					//			DoItemLevel(tmob);*/

					//	}break;
					//	case 3: cSend.ClientMsg(user, g_pMessageStringTable[_NN_3_Quarters_Bonus]); break;
					//	case 2: cSend.ClientMsg(user, g_pMessageStringTable[_NN_2_Quarters_Bonus]); break;
					//	case 1: cSend.ClientMsg(user, g_pMessageStringTable[_NN_1_Quarters_Bonus]); break;
					//	}
					//

					//	/*SendScore(tmob);
					//	SendEmotion(tmob, 14, 3);

					//	if (Segment == 4)
					//	{
					//		SendEtc(tmob);

					//		int PKPoint = GetPKPoint(tmob) + 5;
					//		SetPKPoint(tmob, PKPoint);

					//		MSG_CreateMob sm_lupc;
					//		memset(&sm_lupc, 0, sizeof(MSG_CreateMob));
					//		GetCreateMob(tmob, &sm_lupc);

					//		GridMulticast(pMob[tmob].TargetX, pMob[tmob].TargetY, (MSG_STANDARD*)&sm_lupc, 0);

					//		snprintf(temp, sizeof(temp), "lvl %s level up to %d", pMob[tmob].MOB.MobName, pMob[tmob].MOB.BaseScore.Level);
					//		Log(temp, pUser[tmob].AccountName, pUser[tmob].IP);
					//	}*/
					//}
				}

				user->cSock.AddMessage((BYTE*)msg, msg->Size);
			}
		}
	}
}


void CSend::SendCreateMob(CUser* user, int otherconn, int bSend)
{
	if (user->Mode != USER_PLAY)
		return;

	if (!user->cSock.Sock)
		return;

	/*auto otherUser = cSrv.getUser(otherconn);
	if (!otherUser)
		return;*/

	MSG_CreateMob sm;
	memset(&sm, 0, sizeof(MSG_CreateMob));

	if (otherconn <= 0 || otherconn >= MAX_USER /*|| otherUser->TradeMode != 1*/)
	{
		cGet.GetCreateMob(otherconn, &sm);

		if (user->cSock.AddMessage((BYTE*)&sm, sizeof(MSG_CreateMob)))
			user->cSock.SendMessageA();

		return;
	}

	MSG_CreateMobTrade sm2;
	memset(&sm2, 0, sizeof(MSG_CreateMobTrade));

	cGet.GetCreateMobTrade(otherconn, &sm2);

	if (user->cSock.AddMessage((BYTE*)&sm2, sizeof(MSG_CreateMobTrade)))
		user->cSock.SendMessageA();
}

void CUser::SendItem(short type, short Slot, STRUCT_ITEM* item)
{
	if (item)
	{
		if (this->Mode != USER_PLAY)
			return;

		if (this->cSock.Sock == 0)
			return;

		MSG_SendItem sm_si;
		memset(&sm_si, 0, sizeof(MSG_SendItem));

		sm_si.Type = _MSG_SendItem;
		sm_si.Size = sizeof(MSG_SendItem);
		sm_si.ID = conn;

		sm_si.invType = type;
		sm_si.Slot = Slot;

		memcpy(&sm_si.item, item, sizeof(STRUCT_ITEM));

		this->cSock.AddMessage((BYTE*)&sm_si, sizeof(MSG_SendItem));
	}
}

void CUser::SendEquip(int skip)
{
	if (auto mob = cSrv.getMob(this->conn))
	{
		MSG_UpdateEquip sm_ue;
		memset(&sm_ue, 0, sizeof(MSG_UpdateEquip));

		sm_ue.Type = _MSG_UpdateEquip;
		sm_ue.Size = sizeof(MSG_UpdateEquip);
		sm_ue.ID = this->conn;

		int SendMount = 0;

		for (int i = 0; i < MAX_EQUIP; i++)
		{
			STRUCT_ITEM* item = &mob->MOB.Equip[i];

			sm_ue.Equip[i] = BASE_VisualItemCode(item, i);

			sm_ue.AnctCode[i] = BASE_VisualAnctCode(item);

			if (i == 14 && sm_ue.Equip[14] >= 2360 && sm_ue.Equip[i] < 2390 && mob->MOB.Equip[i].stEffect[0].sValue <= 0)
			{
				sm_ue.Equip[i] = 0;
				SendMount = 1;
				continue;
			}

			if (i == 14 && sm_ue.Equip[14] >= 2360 && sm_ue.Equip[i] < 2390)
			{
				int MountLevel = mob->MOB.Equip[i].stEffect[1].cEffect; // level

				MountLevel /= 10;

				if (MountLevel > 13)
					MountLevel = 13;
				if (MountLevel < 0)
					MountLevel = 0;

				MountLevel = MountLevel * 4096;
				sm_ue.Equip[i] += MountLevel;

				continue;
			}
		}

		cSend.GridMulticast(mob->TargetX, mob->TargetY, (MSG_HEADER*)&sm_ue, skip);

		if (SendMount != 0)
			this->SendItem(ITEM_PLACE_EQUIP, 14, &mob->MOB.Equip[14]);
	}
}

bool CUser::PutItem(CMob* mob, STRUCT_ITEM* item)
{
	if (item)
	{
		if (this->Mode == USER_PLAY)
		{
			auto SlotId = cGet.GetSlot(mob, 0);
			if (SlotId == -1)
			{
				this->ClientMsg(g_pMessageStringTable[_NN_You_Have_No_Space_To_Trade]);
				return false;
			}

			memcpy(&mob->MOB.Carry[SlotId], item, sizeof(STRUCT_ITEM));
			SendItem(ItSlot::Carry, SlotId, &mob->MOB.Carry[SlotId]);
		}
	}

	return true;
}

void CSend::SendCreateItem(CUser* user, int item, int bSend)
{
	if (user)
	{
		MSG_CreateItem sm;
		memset(&sm, 0, sizeof(MSG_CreateItem));

		cGet.GetCreateItem(item, &sm);

		user->cSock.AddMessage((BYTE*)&sm, sizeof(MSG_CreateItem));

		if (bSend)
			user->cSock.SendMessageA();
	}
}

void CSend::SendGridMob(CUser* user)
{
	if (!user)
		return;

	int posX = cSrv.pMob[user->conn].TargetX;
	int posY = cSrv.pMob[user->conn].TargetY;

	int SizeY = VIEWGRIDY;
	int SizeX = VIEWGRIDX;
	int StartX = posX - HALFGRIDX;
	int StartY = posY - HALFGRIDY;

	if ((posX - HALFGRIDX + VIEWGRIDX) >= MAX_GRIDX)
		SizeX -= (StartX + SizeX - MAX_GRIDX);

	if ((posY - HALFGRIDY + VIEWGRIDY) >= MAX_GRIDY)
		SizeY -= (StartY + SizeY - MAX_GRIDY);

	if (StartX < 0)
		StartX = 0;

	if (StartY < 0)
		StartY = 0;

	int sx1 = StartX;
	int sy1 = StartY;
	int sx2 = StartX + SizeX;
	int sy2 = StartY + SizeY;

	for (int y = sy1; y < sy2; y++)
	{
		for (int x = sx1; x < sx2; x++)
		{
			int tmob = cSrv.pMobGrid[y][x];
			int titem = cSrv.pItemGrid[y][x];

			if (tmob > 0 && tmob < MAX_MOB && tmob != user->conn)
			{
				if (cSrv.pMob[tmob].Mode == MOB_EMPTY) { cSrv.pMobGrid[y][x] = 0; }
				else
				{
					this->SendCreateMob(user, tmob, 0);
					this->SendPKInfo(user, tmob);
				}
			}

			if (titem > 0 && titem < MAX_ITEM)
			{
				if (cSrv.pItem[titem].Mode)
					SendCreateItem(user, titem, 0);
				else
					cSrv.pItemGrid[y][x] = 0;
			}
		}
	}
}

#pragma message("-----CHECAR O targetUser AQUI--------")
void CSend::SendPKInfo(CUser* user, int target)
{
	MSG_STANDARDPARM sm;
	memset(&sm, 0, sizeof(MSG_STANDARDPARM));

	sm.Size = sizeof(MSG_STANDARDPARM);
	sm.Type = _MSG_PKInfo;
	sm.ID = target;

	/*auto targetUser = cSrv.getUser(target);
	if (!targetUser)
		return;*/

	if (cSrv.NewbieEventServer == 0)
	{
		int guilty = cGet.GetGuilty(target);

		int state = 0;

		if (guilty /*|| targetUser->PKMode || g_pRvrWar.Status || CastleState || GTorreState */)
			state = 1;

		sm.Parm = state;
	}
	else
		sm.Parm = 1;

	user->cSock.AddMessage((BYTE*)&sm, sizeof(MSG_STANDARDPARM));
}

void CSend::SendHpMp(CUser* user)
{
	if (user)
	{
		if (user->Mode != USER_PLAY)
			return;

		if (user->cSock.Sock == 0)
			return;

		if (auto mob = cSrv.getMob(user->conn))
		{
			MSG_SetHpMp sm_shm;
			memset(&sm_shm, 0, sizeof(MSG_SetHpMp));

			sm_shm.Type = _MSG_SetHpMp;
			sm_shm.Size = sizeof(MSG_SetHpMp);
			sm_shm.ID = user->conn;

			sm_shm.Hp = mob->MOB.CurrentScore.Hp;
			sm_shm.Mp = mob->MOB.CurrentScore.Mp;

			this->GridMulticast(mob->TargetX, mob->TargetY, (MSG_HEADER*)&sm_shm, 0);
		}
	}
}

void CSend::SendSetHpMp(CUser* user)
{
	if (user)
	{
		if (user->Mode != USER_PLAY)
			return;

		if (user->cSock.Sock == 0)
			return;

		if (auto mob = cSrv.getMob(user->conn))
		{
			MSG_SetHpMp sm_shm;
			memset(&sm_shm, 0, sizeof(MSG_SetHpMp));

			sm_shm.Type = _MSG_SetHpMp;
			sm_shm.Size = sizeof(MSG_SetHpMp);
			sm_shm.ID = user->conn;

			sm_shm.Hp = mob->MOB.CurrentScore.Hp;
			sm_shm.Mp = mob->MOB.CurrentScore.Mp;

			cSrv.SetReqHp(mob);
			cSrv.SetReqMp(mob);

			if (!user->cSock.AddMessage((BYTE*)&sm_shm, sizeof(MSG_SetHpMp)))
				cSrv.CloseUser(user->conn);
		}
	}
}

void CSend::SendHpMode(CUser* user)
{
	if (user)
	{
		if (user->Mode != USER_PLAY)
			return;

		if (user->cSock.Sock == 0)
			return;

		if (auto mob = cSrv.getMob(user->conn))
		{
			MSG_SetHpMode sm_shmd;
			memset(&sm_shmd, 0, sizeof(MSG_SetHpMode));

			sm_shmd.Type = _MSG_SetHpMode;
			sm_shmd.Size = sizeof(MSG_SetHpMode);
			sm_shmd.ID = user->conn;

			sm_shmd.Hp = mob->MOB.CurrentScore.Hp;
			sm_shmd.Mode = user->Mode;

			if (!user->cSock.AddMessage((BYTE*)&sm_shmd, sizeof(MSG_SetHpMode)))
				cSrv.CloseUser(user->conn);
		}
	}
}

void CUser::SendRemoveParty(int16 connExit)
{
	if (this->Mode != USER_PLAY)
		return;

	if (this->cSock.Sock == 0)
		return;

	MSG_RemoveParty sm_rp;
	memset(&sm_rp, 0, sizeof(MSG_RemoveParty));

	sm_rp.ID = ESCENE_FIELD;
	sm_rp.Type = _MSG_RemoveParty;
	sm_rp.Size = sizeof(MSG_RemoveParty);

	sm_rp.Leaderconn = connExit;
	sm_rp.unk = 0;

	if (!this->cSock.AddMessage((BYTE*)&sm_rp, sizeof(MSG_RemoveParty)))
		cSrv.CloseUser(this->conn);
}

void CUser::SendEtc()
{
	if (this->Mode != USER_PLAY)
		return;

	if (this->cSock.Sock == 0)
		return;

	MSG_UpdateEtc sm;
	memset(&sm, 0, sizeof(MSG_UpdateEtc));

	sm.Type = _MSG_UpdateEtc;
	sm.Size = sizeof(MSG_UpdateEtc);
	sm.ID = this->conn;

	if (auto mob = cSrv.getMob(this->conn))
	{
		sm.Exp = mob->MOB.Exp;

		sm.SpecialBonus = mob->MOB.SpecialBonus;

		sm.Learn = mob->MOB.LearnedSkill;
		sm.SecLearn = mob->Extra.SecLearnedSkill;

		sm.ScoreBonus = mob->MOB.ScoreBonus;
		sm.SkillBonus = mob->MOB.SkillBonus;

		sm.Coin = mob->MOB.Coin;

		sm.Hold = mob->Extra.Hold;
		sm.Magic = mob->MOB.Magic;
	}

	if (!this->cSock.AddMessage((BYTE*)&sm, sizeof(MSG_UpdateEtc)))
		cSrv.CloseUser(this->conn);
}

void CUser::SendCargoCoin()
{
	if (this->Mode != USER_PLAY)
		return;

	if (this->cSock.Sock == 0)
		return;

	MSG_STANDARDPARM sm_ucc;
	memset(&sm_ucc, 0, sizeof(MSG_STANDARDPARM));

	sm_ucc.Type = _MSG_UpdateCargoCoin;
	sm_ucc.Size = sizeof(MSG_STANDARDPARM);
	sm_ucc.ID = ESCENE_FIELD;
	sm_ucc.Parm = this->Coin;

	if (!this->cSock.AddMessage((BYTE*)&sm_ucc, sizeof(MSG_STANDARDPARM)))
		cSrv.CloseUser(this->conn);
}

void CUser::SendAffect()
{
	if (auto mob = cSrv.getMob(this->conn))
	{
		MSG_SendAffect sm;
		memset(&sm, 0, sizeof(MSG_SendAffect));

		sm.Type = _MSG_SendAffect;
		sm.Size = sizeof(MSG_SendAffect);
		sm.ID = conn;

		for (int i = 0; i < MAX_AFFECT; i++) // i = 0; 
		{
			if (mob->Affect[i].Type == 34 && mob->Affect[i].Time >= 32000000)
			{
				time_t now;
				time(&now);

				if ((this->uExtra.TimeDivina - now) <= 7200) // divina todos char, trocado
				{
					this->uExtra.TimeDivina = 0;

					sm.Affect[i].Type = mob->Affect[i].Type;
					sm.Affect[i].Value = mob->Affect[i].Value;
					sm.Affect[i].Level = mob->Affect[i].Level;
					sm.Affect[i].Time = mob->Affect[i].Time;
					continue;
				}

				sm.Affect[i].Type = mob->Affect[i].Type;
				sm.Affect[i].Value = mob->Affect[i].Value;
				sm.Affect[i].Level = mob->Affect[i].Level;
				sm.Affect[i].Time = (unsigned int)(((this->uExtra.TimeDivina - now) / 60 / 60 * AFFECT_1H) - 60); // divina todos char, trocado
				//sm.Affect[i].Time = (unsigned int)(((pUser[conn].uExtra.TimeDivina - now) / 3600 * 450) - 60); // divina todos char, trocado			
			}
			else if (mob->Affect[i].Type >= 1)
			{
				sm.Affect[i].Type = mob->Affect[i].Type;
				sm.Affect[i].Value = mob->Affect[i].Value;
				sm.Affect[i].Level = mob->Affect[i].Level;
				sm.Affect[i].Time = mob->Affect[i].Time;
			}
		}

		this->cSock.AddMessage((BYTE*)&sm, sizeof(MSG_SendAffect));
	}
}

void CUser::SendShopList(int MobIndex, int ShopType)
{
	if (this->Mode != USER_PLAY)
		return;

	if (this->cSock.Sock == 0)
		return;

	MSG_ShopList sm_sl;
	memset(&sm_sl, 0, sizeof(MSG_ShopList));

	sm_sl.Type = _MSG_ShopList;
	sm_sl.Size = sizeof(MSG_ShopList);
	sm_sl.ID = ESCENE_FIELD;
	sm_sl.ShopType = ShopType;

	if (auto npcID = cSrv.getMob(MobIndex))
	{
		for (int i = 0; i < MAX_SHOPLIST; i++)
		{
			int invpos = (i % 9) + ((i / 9) * MAX_SHOPLIST);

			*(int*)&sm_sl.List[i] = *(int*)&npcID->MOB.Carry[invpos].sIndex;
			*(int*)((int)&sm_sl.List[i] + 4) = *(int*)((int)&npcID->MOB.Carry[invpos] + 4);
		}

		int village = BASE_GetVillage(npcID->TargetX, npcID->TargetY);

		if (village >= 0 && village < MAX_GUILDZONE)
			sm_sl.Tax = g_pGuildZone[village].CityTax;
		else
			sm_sl.Tax = 0;
	}
	else
		cLog.Write(0, TL::Err, "erro na função [SendShopList]");

	if (!this->cSock.AddMessage((BYTE*)&sm_sl, sizeof(MSG_ShopList)))
		cSrv.CloseUser(this->conn);
}

void CSend::SendScore(int conn)
{
	if (auto mob = cSrv.getMob(conn))
	{
		MSG_UpdateScore sm_vus;
		memset(&sm_vus, 0, sizeof(MSG_UpdateScore));

		sm_vus.Type = _MSG_UpdateScore;
		sm_vus.Size = sizeof(MSG_UpdateScore);
		sm_vus.ID = conn;

		if (conn >= MAX_USER && mob->MOB.Coin != 1232) // evocações
		{
			memcpy(&sm_vus.Score, &mob->MOB.CurrentScore, sizeof(STRUCT_SCORE));
			cGet.GetAffect(sm_vus.Affect, mob->Affect);
			this->GridMulticast(mob->TargetX, mob->TargetY, (MSG_HEADER*)&sm_vus, 0); // esse 0 é o skip
			return;
		}

		memcpy(&sm_vus.Score, &mob->MOB.CurrentScore, sizeof(STRUCT_SCORE));

		sm_vus.CurrHp = mob->MOB.CurrentScore.Hp;
		sm_vus.CurrMp = mob->MOB.CurrentScore.Mp;

		sm_vus.Critical = mob->MOB.Critical;
		sm_vus.SaveMana = mob->MOB.SaveMana;
		sm_vus.Guild = mob->MOB.Guild;
		sm_vus.GuildLevel = mob->MOB.GuildLevel;
		sm_vus.Resist[0] = mob->MOB.Resist[0];
		sm_vus.Resist[1] = mob->MOB.Resist[1];
		sm_vus.Resist[2] = mob->MOB.Resist[2];
		sm_vus.Resist[3] = mob->MOB.Resist[3];
		sm_vus.Special[0] = 0xCC;
		sm_vus.Special[1] = 0xCC;
		sm_vus.Special[2] = 0xCC;
		sm_vus.Special[3] = 0xCC;
		sm_vus.Magic = mob->MOB.Magic;
		cGet.GetAffect(sm_vus.Affect, mob->Affect);

		if (mob->GuildDisable)
			sm_vus.Guild = 0;

		/*if (BrState != 0)
		{
			if (conn < MAX_USER)
			{
				int posX = pMob[conn].TargetX;
				int posY = pMob[conn].TargetY;

				if (posX >= 2604 && posY >= 1708 && posX <= 2648 && posY <= 1744)
				{
					sm_vus.Guild = 0;
					sm_vus.GuildLevel = 0;
				}
			}
		}*/

		this->GridMulticast(mob->TargetX, mob->TargetY, (MSG_HEADER*)&sm_vus, 0);
		if (auto user = cSrv.getUser(conn))
			user->SendAffect();
	}
}