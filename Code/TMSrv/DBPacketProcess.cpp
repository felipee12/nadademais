#include "pch.h"

void m_MSG_DBClientMessage(CUser* user, char* Msg)
{
	if (user)
	{
		MSG_DBClientMessage* m = (MSG_DBClientMessage*)Msg;
		user->ClientMsg(m->String);
	}
}

void m_MSG_DBAlreadyPlaying(int conn, char* Msg)
{//não sei
	MSG_HEADER* m = (MSG_HEADER*)Msg;
	if (auto user = cSrv.getUser(m->ID))
	{
		user->SendClientSignal(ESCENE_FIELD + 2, _MSG_AlreadyPlaying);
		user->cSock.SendMessageA();
	}
	cSrv.CloseUser(conn);
}

void m_MSG_DBStillPlaying(int conn, char* Msg)
{//conexão simultanea
	MSG_HEADER* m = (MSG_HEADER*)Msg;
	if (auto user = cSrv.getUser(m->ID))
	{
		user->SendClientSignal(ESCENE_FIELD + 2, _MSG_StillPlaying);
		user->cSock.SendMessageA();
	}
	cSrv.CloseUser(conn);
}

#pragma message("testar bem essa func")
void m_MSG_DBSavingQuit(CUser* otherUser, char* Msg)
{
	MSG_DBSavingQuit* m = (MSG_DBSavingQuit*)Msg;

	auto user = cSrv.getUser(m->ID);
	if (!user)
	{
		cLog.Write(0, TL::Err, "-system err,DBsavingquit - id range");
		return;
	}

	if (user->Mode != USER_PLAY && user->Mode != USER_SAVING4QUIT)
	{
		MSG_HEADER sm;
		memset(&sm, 0, sizeof(MSG_HEADER)); //add

		sm.Type = _MSG_DBNoNeedSave;
		sm.ID = otherUser->conn;
		cSrv.DBSocket.SendOneMessage((BYTE*)&sm, sizeof(MSG_HEADER));
	}

	if (user->Mode == USER_PLAY || user->Mode == USER_SELCHAR)
	{
		if (m->Mode == 0)
			user->ClientMsg(g_pMessageStringTable[_NN_Your_Account_From_Others]);
		else if (m->Mode == 1)
			user->ClientMsg(g_pMessageStringTable[_NN_Disabled_Account]);

		otherUser->cSock.SendMessageA();
	}

	cSrv.CloseUser(m->ID);
}

bool m_MSG_DBCNFAccountLogin(CUser* user, char* Msg)
{
	if (user)
	{
		MSG_DBCNFAccountLogin* m = (MSG_DBCNFAccountLogin*)Msg;

		if (strcmp(m->AccountName, user->AccountName) != 0)
		{
			user->ClientMsg(g_pMessageStringTable[_NN_Try_Reconnect]);
			user->cSock.SendMessageA();
			cSrv.CloseUser(user->conn);
			return false;
		}

		/*for (int x = 0; x < MAX_MAC; x++)
		{
			if (pMac[x].Mac[0] == 0 && pMac[x].Mac[1] == 0 && pMac[x].Mac[2] == 0 && pMac[x].Mac[3] == 0)
				continue;

			if (pMac[x].Mac[0] == pUser[conn].Mac[0] && pMac[x].Mac[1] == pUser[conn].Mac[1] && pMac[x].Mac[2] == pUser[conn].Mac[2] && pMac[x].Mac[3] == pUser[conn].Mac[3])
			{
				SendClientMsg(conn, g_pMessageStringTable[_NN_MAC_Block]);

				pUser[conn].cSock.SendMessageA();

				CloseUser(conn);
				break;
			}
		}*/

		memcpy(&user->uExtra, &m->uExtra, sizeof(stUserExtra)); // eee
		//memset(&user->inGame, 0, sizeof(user->inGame)); // eee

		user->Whisper = false;
		user->Guildchat = false;
		user->PartyChat = false;
		user->KingChat = false;
		user->Chatting = false;
		user->Primary = true;

		m->ID = ESCENE_FIELD + 2;
		m->Type = _MSG_CNFAccountLogin;

		for (int i = 0; i < MAX_CARGO; i++)
		{
			STRUCT_ITEM* tempsour = &m->Cargo[i];

			if (tempsour->sIndex > 0 && tempsour->sIndex < MAX_ITEMLIST)
			{
				int nPos = g_pItemList[tempsour->sIndex].nPos;

				if (nPos == 64 || nPos == 192)
				{
					if (tempsour->stEffect[0].cEffect == EF_DAMAGEADD || tempsour->stEffect[0].cEffect == EF_DAMAGE2)
						tempsour->stEffect[0].cEffect = EF_DAMAGE;

					if (tempsour->stEffect[1].cEffect == EF_DAMAGEADD || tempsour->stEffect[1].cEffect == EF_DAMAGE2)
						tempsour->stEffect[1].cEffect = EF_DAMAGE;

					if (tempsour->stEffect[2].cEffect == EF_DAMAGEADD || tempsour->stEffect[2].cEffect == EF_DAMAGE2)
						tempsour->stEffect[2].cEffect = EF_DAMAGE;
				}
			}
		}

		user->cSock.SendOneMessage((BYTE*)m, sizeof(MSG_DBCNFAccountLogin));

		memcpy(user->Cargo, m->Cargo, sizeof(STRUCT_ITEM) * MAX_CARGO);

		user->Coin = m->Coin;
		//user->Unk_1816 = 0;
		user->SelChar = m->sel;

		user->Mode = USER_SELCHAR;

		/*if (BILLING > 0 && IsFree(&m->sel) != 0)
		{
			if (CHARSELBILL == 0)
				SendBilling(conn, m->AccountName, 8, 1);
			else
				SendBilling(conn, m->AccountName, 1, 1);

			pUser[conn].Unk_2732 = SecCounter;
		}*/

		//user->LastClientTick = 0;

		/*snprintf(temp, sizeof(temp), "CNFAccountLogin Mac: %d.%d.%d.%d", pUser[conn].Mac[0], pUser[conn].Mac[1], pUser[conn].Mac[2], pUser[conn].Mac[3]);
		Log(temp, pUser[conn].AccountName, pUser[conn].IP);*/
	}

	return true;
}

void m_MSG_DBAccountLoginFail_Pass(CUser* user) // retirado argumento char* Msg
{
	user->ClientMsg("Senha Incorreta");
	user->cSock.SendMessageA();
	cSrv.CloseUser(user->conn);
}

void m_MSG_DBCNFNewCharacter(CUser* user, char* Msg)
{
	MSG_CNFNewCharacter* m = (MSG_CNFNewCharacter*)Msg;
	m->Type = _MSG_CNFNewCharacter;
	m->ID = ESCENE_FIELD + 1;
	user->cSock.SendOneMessage((BYTE*)m, sizeof(MSG_CNFNewCharacter));
	user->Mode = USER_SELCHAR;
}

void m_MSG_DBNewCharacterFail(CUser* user, char* Msg)
{
	MSG_STANDARD* m = (MSG_STANDARD*)Msg;
	m->ID = ESCENE_FIELD + 1;
	user->SendClientSignal(0, _MSG_NewCharacterFail);
	user->Mode = USER_SELCHAR;
}

void m_MSG_DBCNFDeleteCharacter(CUser* user, char* Msg)
{
	MSG_CNFDeleteCharacter* m = (MSG_CNFDeleteCharacter*)Msg;
	m->Type = _MSG_CNFDeleteCharacter;
	m->ID = ESCENE_FIELD + 1;
	user->cSock.SendOneMessage((BYTE*)m, sizeof(MSG_CNFDeleteCharacter));
	user->Mode = USER_SELCHAR;
}

void m_MSG_DBDeleteCharacterFail(CUser* user, char* Msg)
{
	MSG_STANDARD* m = (MSG_STANDARD*)Msg;
	m->ID = ESCENE_FIELD + 1;
	user->SendClientSignal(0, _MSG_DeleteCharacterFail);
	user->Mode = USER_SELCHAR;
}

void m_MSG_DBCNFCharacterLogin(CUser* user, char* Msg)
{
	MSG_CNFCharacterLogin* m = (MSG_CNFCharacterLogin*)Msg;
	MSG_CNFClientCharacterLogin sm;
	memcpy(&sm, m, sizeof(MSG_CNFClientCharacterLogin));

	auto pMob = &cSrv.pMob[user->conn];

	auto setaddsFace = [m](uint8 thisValue) -> void
	{
		m->mob.Equip[0].stEffect[1].cEffect = 98;
		m->mob.Equip[0].stEffect[1].cValue = thisValue;
		m->mob.Equip[0].stEffect[2].cEffect = 106;
		m->mob.Equip[0].stEffect[2].cValue = static_cast<uint8>(m->mob.Equip[0].sIndex);
	};

	if (m->mobExtra.ClassMaster == MORTAL || m->mobExtra.ClassMaster == ARCH)
		setaddsFace(0);
	else
		setaddsFace(3);

	m->mob.Equip[0].stEffect[2].cEffect = 28;
	m->mob.Equip[0].stEffect[2].cValue = m->mobExtra.Citizen;

	if (m->mob.CurrentScore.Hp <= 0)
		m->mob.CurrentScore.Hp = 2;

	pMob->MOB = m->mob;

	pMob->LastReqParty = 0;
	pMob->ProcessorCounter = 1;

	memcpy(&pMob->Extra, &m->mobExtra, sizeof(STRUCT_MOBExtra));

	/*if (m->mob.Guild)
		pMob->Extra.Citizen = GuildInfo[m->mob.Guild].Citizen;*/

	memcpy(user->CharShortSkill, m->ShortSkill, 16);

	memcpy(pMob->Affect, m->affect, sizeof(m->affect));

	pMob->MaxCarry = 30;

	if (pMob->MOB.Carry[60].sIndex == 3467)
		pMob->MaxCarry += 15;

	if (pMob->MOB.Carry[61].sIndex == 3467)
		pMob->MaxCarry += 15;

	strncpy(pMob->Tab, "", sizeof(pMob->Tab));
	strncpy(pMob->Snd, "", sizeof(pMob->Snd));

	pMob->MOB.BaseScore.Merchant = 0;

	if (pMob->Extra.ClassMaster == ARCH && pMob->Extra.QuestInfo.Arch.MortalSlot >= 0 && pMob->Extra.QuestInfo.Arch.MortalSlot < 3)
		pMob->Extra.QuestInfo.Arch.MortalLevel = user->SelChar.Score[pMob->Extra.QuestInfo.Arch.MortalSlot].Level - 299;
	else
		pMob->Extra.QuestInfo.Arch.MortalLevel = 99;

	BASE_GetHpMp(&pMob->MOB, &pMob->Extra);

	//GetGuild(conn);

	BASE_GetBonusSkillPoint(&pMob->MOB, &pMob->Extra);
	BASE_GetBonusScorePoint(&pMob->MOB, &pMob->Extra);

	pMob->Mode = MOB_USER;

	sm.mob = pMob->MOB;
	sm.mob.SPX = sm.mob.SPX;
	sm.mob.SPY = sm.mob.SPY;
	sm.Type = _MSG_CNFCharacterLogin;

	if (cSrv.NewbieEventServer == 1)
		sm.ID = ESCENE_FIELD + 1;
	else
		sm.ID = ESCENE_FIELD;

	sm.PosX = sm.mob.SPX;
	sm.PosY = sm.mob.SPY;
	sm.ClientID = user->conn;
	sm.Weather = cSrv.CurrentWeather;

	pMob->LastTime = CurrentTime;
	pMob->LastX = pMob->TargetX = sm.mob.SPX;
	pMob->LastY = pMob->TargetY = sm.mob.SPY;

	if (pMob->MOB.Carry[KILL_MARK].sIndex == 0)
	{
		memset(&pMob->MOB.Carry[KILL_MARK], 0, sizeof(STRUCT_ITEM));

		pMob->MOB.Carry[KILL_MARK].sIndex = 547;
		pMob->MOB.Carry[KILL_MARK].stEffect[0].cEffect = EF_CURKILL;
		pMob->MOB.Carry[KILL_MARK].stEffect[1].cEffect = EF_LTOTKILL;
		pMob->MOB.Carry[KILL_MARK].stEffect[2].cEffect = EF_HTOTKILL;
	}

	user->LastChat[0] = 0;
	user->Slot = m->Slot;
	/*user->NumError = 0;
	user->LastMove = 0;
	user->LastAction = _MSG_Action;
	user->LastActionTick = SKIPCHECKTICK;
	user->LastAttack = 0;
	user->LastAttackTick = SKIPCHECKTICK;
	user->LastIllusionTick = SKIPCHECKTICK;
	user->RankingTarget = 0;
	user->RankingType = 0;*/
	user->CastleStatus = 0;

	memset(&user->Trade, 0, sizeof(user->Trade));

	int k;

	for (k = 0; k < MAX_TRADE; k++)
		user->Trade.InvenPos[k] = -1;

	for (k = 0; k < MAX_AUTOTRADE; k++)
		user->AutoTrade.CarryPos[k] = -1;

	user->TradeMode = 0;
	user->PKMode = 0;

	int tx = sm.PosX;
	int ty = sm.PosY;

	int CityID = (pMob->MOB.Merchant & 0xC0) >> 6;

	tx = g_pGuildZone[CityID].CitySpawnX + rand() % 15;
	ty = g_pGuildZone[CityID].CitySpawnY + rand() % 15;

	int MobGuild = pMob->MOB.Guild;
	int MobCLS = pMob->MOB.Class;

	if (MobCLS < 0 || MobCLS > MAX_CLASS - 1)
	{
		cLog.Write(0, TL::Err, std::format("{} err,login Undefined class", user->AccountName));
		cSrv.CloseUser(user->conn);
		return;
	}

	int n;

	for (n = 0; n < MAX_GUILDZONE; n++)
	{
		if (MobGuild != 0 && MobGuild == g_pGuildZone[n].ChargeGuild)
		{
			tx = g_pGuildZone[n].GuildSpawnX;
			ty = g_pGuildZone[n].GuildSpawnY;

			break;
		}
	}

	if (pMob->MOB.BaseScore.Level < 35 && pMob->Extra.ClassMaster == MORTAL)
	{
		tx = 2112 + rand() % 5 - 2;
		ty = 2042 + rand() % 5 - 2;
	}

	int ret = cGet.GetEmptyMobGrid(user->conn, &tx, &ty);

	if (ret == 0)
	{
		cLog.Write(0, TL::Err, std::format("{} Can't start can't get mobgrid", user->AccountName));
		cSrv.CloseUser(user->conn);
		return;
	}

	sm.PosX = tx;
	sm.PosY = ty;

	pMob->TargetX = tx;
	pMob->LastX = tx;
	pMob->TargetY = ty;
	pMob->LastY = ty;

	user->Mode = USER_PLAY;
	user->Trade.OpponentID = 0;
	pMob->GuildDisable = false;

	/*if (pMob->MOB.Guild)
	{
		int usGuild = pMob->MOB.Guild & MAX_GUILD - 1;

		if (pMob->MOB.Clan != GuildInfo[usGuild].Clan)
		{
			int mantle = pMob[conn].MOB.Equip[15].sIndex;

			if (GuildInfo[usGuild].Clan == 7)
			{
				if (m->mobExtra.ClassMaster == CELESTIAL || m->mobExtra.ClassMaster == SCELESTIAL || m->mobExtra.ClassMaster == CELESTIALCS)
					pMob[conn].MOB.Equip[15].sIndex = 3197;

				else if (mantle >= 543 && mantle <= 544)
					pMob[conn].MOB.Equip[15].sIndex = 543;

				else if (mantle >= 545 && mantle <= 546)
					pMob[conn].MOB.Equip[15].sIndex = 545;

				else if (mantle == 548)
					pMob[conn].MOB.Equip[15].sIndex = 543;

				else if (mantle == 549)
					pMob[conn].MOB.Equip[15].sIndex = 545;

				else if (mantle >= 3191 && mantle <= 3193)
					pMob[conn].MOB.Equip[15].sIndex = 3191;

				else if (mantle >= 3194 && mantle <= 3196)
					pMob[conn].MOB.Equip[15].sIndex = 3194;

			}
			else if (GuildInfo[usGuild].Clan == 8)
			{
				if (m->mobExtra.ClassMaster == CELESTIAL || m->mobExtra.ClassMaster == SCELESTIAL || m->mobExtra.ClassMaster == CELESTIALCS)
					pMob[conn].MOB.Equip[15].sIndex = 3198;

				else if (mantle >= 543 && mantle <= 544)
					pMob[conn].MOB.Equip[15].sIndex = 544;

				else if (mantle >= 545 && mantle <= 546)
					pMob[conn].MOB.Equip[15].sIndex = 546;

				else if (mantle == 548)
					pMob[conn].MOB.Equip[15].sIndex = 544;

				else if (mantle == 549)
					pMob[conn].MOB.Equip[15].sIndex = 546;

				else if (mantle >= 3191 && mantle <= 3193)
					pMob[conn].MOB.Equip[15].sIndex = 3192;

				else if (mantle >= 3194 && mantle <= 3196)
					pMob[conn].MOB.Equip[15].sIndex = 3195;
			}

			memcpy(&sm.mob.Equip[15], &pMob[conn].MOB.Equip[15], sizeof(STRUCT_ITEM));
		}
	}*/

	user->cProgress = 0;

	user->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFClientCharacterLogin));

	MSG_CreateMob sm2;

	cGet.GetCreateMob(user->conn, &sm2);

	sm2.CreateType = 2;

	STRUCT_ITEM* Mount = &pMob->MOB.Equip[14];

	cSrv.pMobGrid[sm.PosY][sm.PosX] = user->conn;

	cSend.GridMulticast(sm.PosX, sm.PosY, (MSG_HEADER*)&sm2, 0);

	cSend.SendPKInfo(user, user->conn);

	cSend.SendGridMob(user);

	pMob->ClientIndex = user->conn;

	//MountProcess(conn, 0);

	/*SendWarInfo(conn, g_pGuildZone[4].Clan);

	if (CastleState != 0)
		SendClientSignalParm(conn, ESCENE_FIELD, _MSG_SendCastleState, CastleState);

	ClearCrown(conn);*/

	user->SendEtc();
	pMob->GetCurrentScore(user->conn);
	cSend.SendScore(user->conn);

	user->ClientMsg("Você chegou, parabéns!");
}

void m_MSG_DBCNFAccountLogOut(CUser* user)
{
	if (auto mob = cSrv.getMob(user->conn))
	{
		cLog.Write(user->conn, TL::Sys, std::format("{} etc,charlogout conn: {} name: {}", user->AccountName, user->conn, mob->MOB.MobName));
		mob->Mode = MOB_EMPTY;
		user->Mode = USER_ACCEPT;
		cSrv.CloseUser(user->conn);
	}
}

bool CServer::ProcessDBMessage(char* Msg)
{
	MSG_HEADER* m = (MSG_HEADER*)Msg;

	if (!(m->Type & FLAG_DB2GAME) || (m->ID < 0) || (m->ID >= MAX_USER))
	{
		snprintf(cSrv.temp, sizeof(cSrv.temp), "err,packet Type:%d ID:%d Size:%d KeyWord:%d", m->Type, m->ID, m->Size, m->KeyWord);
		cLog.Write(0, TL::Warn, cSrv.temp);
		return false;
	}

	auto conn = m->ID;

	if (conn == 0)
	{
		switch (m->Type)
		{
		}
	}

	else
	{
		if (auto user = cSrv.getUser(conn))
		{
			if ((conn > 0 && conn < MAX_USER) && user->Mode == 0)
			{
				MSG_HEADER sm;
				memset(&sm, 0, sizeof(MSG_HEADER)); //add

				sm.Type = _MSG_DBNoNeedSave;
				sm.ID = conn;

				cSrv.DBSocket.SendOneMessage((BYTE*)&sm, sizeof(MSG_HEADER));
				return true;
			}

			if (conn <= 0 || conn >= MAX_USER)
			{
				snprintf(this->temp, sizeof(this->temp), "Unknown DB Message - conn:%d - type:%d", conn, m->Type);
				cLog.Write(0, TL::Warn, this->temp);
				return false;
			}

			switch (m->Type)
			{
			case _MSG_DBCNFAccountLogin: return m_MSG_DBCNFAccountLogin(user, Msg);
			case _MSG_AccountSecure: user->SendClientSignal(ESCENE_FIELD, _MSG_AccountSecure); break;
			case _MSG_DBClientMessage: m_MSG_DBClientMessage(user, Msg); break;
			case _MSG_DBAccountLoginFail_Pass: m_MSG_DBAccountLoginFail_Pass(user); break;
			case _MSG_DBCNFNewCharacter: m_MSG_DBCNFNewCharacter(user, Msg); break;
			case _MSG_DBNewCharacterFail: m_MSG_DBNewCharacterFail(user, Msg); break;
			case _MSG_DBCNFDeleteCharacter: m_MSG_DBCNFDeleteCharacter(user, Msg); break;
			case _MSG_DBDeleteCharacterFail: m_MSG_DBDeleteCharacterFail(user, Msg); break;
			case _MSG_DBCNFCharacterLogin: m_MSG_DBCNFCharacterLogin(user, Msg); break;
			case _MSG_DBCNFAccountLogOut: m_MSG_DBCNFAccountLogOut(user); break; // ver se precisa disso aqui
			case _MSG_DBAlreadyPlaying: m_MSG_DBAlreadyPlaying(conn, Msg); break;
			case _MSG_DBStillPlaying: m_MSG_DBStillPlaying(conn, Msg); break;
			case _MSG_DBSavingQuit: m_MSG_DBSavingQuit(user, Msg); break;
			}
		}
	}

	return true;
}