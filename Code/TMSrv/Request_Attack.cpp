#include "pch.h"


//void CUser::RequestAttack(MSG_HEADER* pMsg)
//{
//	if (pMsg->Size != sizeof(MSG_Attack) && pMsg->Size != sizeof(MSG_AttackOne) && pMsg->Size != sizeof(MSG_AttackTwo))
//	{
//		cSrv.CloseUser(this->conn);
//		return;
//	}
//
//	if (this->Mode != USER_PLAY)
//	{
//		cSend.SendHpMode(this);
//		return;
//	}
//
//	MSG_Attack* m = (MSG_Attack*)pMsg;
//
//	auto mob = cSrv.getMob(this->conn);
//	if (!mob)
//		return;
//
//	m->ID = ESCENE_FIELD;
//
//	short TargetKilled[MAX_TARGET] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//	unsigned int ClientTick = m->ClientTick;
//	unsigned int lastAttackTik = 0;
//
//	if (this->TradeMode)
//	{
//		this->ClientMsg(g_pMessageStringTable[_NN_CantWhenAutoTrade]);
//		return;
//	}
//
//	if (pMob[conn].MOB.CurrentScore.Hp == 0 && m->SkillIndex != 99)
//	{
//		SendHpMode(conn);
//		AddCrackError(conn, 1, 8);
//		return;
//	}
//
//	if (ClientTick != SKIPCHECKTICK && this->LastAttackTick != SKIPCHECKTICK && m->ClientTick < (unsigned int)this->LastAttackTick + 1000)
//	{
//		snprintf(temp, sizeof(temp), "err,attack %d %d 800ms limit", m->ClientTick, this->LastAttackTick);
//		Log(temp, pUser[conn].AccountName, pUser[conn].IP);
//		//AddCrackError(conn, 1, 107);
//		return;
//	}
//
//	if (ClientTick != SKIPCHECKTICK && this->LastAttackTick != SKIPCHECKTICK)
//	{
//		/*if (ClientTick < (unsigned int)this->LastAttackTick - 100)
//			AddCrackError(conn, 4, 7);*/
//
//		lastAttackTik = this->LastAttackTick;
//
//		this->LastAttackTick = ClientTick;
//		this->LastAttack = m->SkillIndex;
//	}
//
//	if (ClientTick != SKIPCHECKTICK)
//	{
//		int ClientTick15sec = 0;
//
//		if (CurrentTime <= 0x1D4C0)
//			ClientTick15sec = 0;
//		else
//			ClientTick15sec = CurrentTime - 120000;
//
//		if (ClientTick > CurrentTime + 15000 || ClientTick < (unsigned int)ClientTick15sec)
//		{
//			Log("etc,clienttime faster than 15 sec - MSG_ATTACK", pUser[conn].AccountName, pUser[conn].IP);
//			AddCrackError(conn, 1, 107);
//
//			return;
//		}
//
//		this->LastAttackTick = ClientTick;
//	}
//
//	int skillnum = m->SkillIndex;
//
//	int tx = m->TargetX;
//	int ty = m->TargetY;
//
//	if (ClientTick != SKIPCHECKTICK && skillnum != 42 && (tx != m->TargetX || ty != m->TargetY))
//	{
//		snprintf(temp, sizeof(temp), "err,hitposition %d,%d - %d,%d - %d,%d", m->PosX, m->PosY, m->TargetX, m->TargetY, tx, ty);
//		MyLog(LogType::Attack, pUser[conn].AccountName, temp, 0, pUser[conn].IP);
//		//AddCrackError(conn, 2, 9);
//
//		return;
//	}
//
//
//	if (skillnum >= 0 && skillnum < MAX_SKILLINDEX && ClientTick != SKIPCHECKTICK)
//	{
//		if (skillnum < -1 || skillnum > MAX_SKILLINDEX)
//		{
//			MyLog(LogType::Attack, pUser[conn].AccountName, "MSG_Attack, skillnum out of range", 0, pUser[conn].IP);
//			return;
//		}
//
//		if (g_pSpell[skillnum].Passive == 1)
//			return;
//
//		/* Proteção Absoluta */
//		if (pMob[conn].MOB.Class == 1)
//		{
//			for (int i = 0; i < MAX_AFFECT; i++)
//			{
//				if (pMob[conn].Affect[i].Type != 6)
//					continue;
//
//				pMob[conn].Affect[i].Type = 0;
//				pMob[conn].Affect[i].Value = 0;
//				pMob[conn].Affect[i].Level = 0;
//				pMob[conn].Affect[i].Time = 0;
//
//				mob->GetCurrentScore(this->conn);
//				cSend.SendScore(this->conn);
//
//				break;
//			}
//		}
//		if (lastAttackTik != -1 && ClientTick != SKIPCHECKTICK)
//		{
//			int isTime = ClientTick - lastAttackTik;
//
//			int skilldelay = g_pSpell[skillnum].Delay;
//
//			if ((pMob[conn].MOB.Rsv & RSV_CAST) != 0 && skilldelay >= 2)
//				skilldelay--;
//
//			skilldelay = skilldelay * 1000;
//
//			if (isTime < 700)
//			{
//				AddCrackError(conn, 10, 30);
//				snprintf(temp, sizeof(temp), "skill delay skillnum: %d-%d", ClientTick, isTime);
//				MyLog(LogType::Attack, pUser[conn].AccountName, temp, 0, pUser[conn].IP);
//
//				return;
//			}
//		}
//
//		this->LastAttackTick = m->ClientTick;
//
//		if ((skillnum / 24) != pMob[conn].MOB.Class && (skillnum / 24) <= 3)
//		{
//			MyLog(LogType::Attack, pUser[conn].AccountName, "MSG_Attack, Request other class skill", 0, pUser[conn].IP);
//			return;
//		}
//	}
//
//	int Level = 0;
//	int Delay = 100;
//	int Special = 0;
//
//	if (skillnum >= 0 && skillnum < 96)
//	{
//		int getSkillMaster = ((skillnum % 24) / 8) + 1;
//
//		if (ClientTick != SKIPCHECKTICK)
//		{
//			int learn = skillnum % 24;
//			int learnedSkill = 1 << learn;
//
//			if (m->ClientTick != SKIPCHECKTICK && (pMob[conn].MOB.LearnedSkill & learnedSkill) == FALSE && AddCrackError(conn, 8, 10) != FALSE)
//				return;
//
//			if (getSkillMaster <= 0 || getSkillMaster > 3)
//			{
//				MyLog(LogType::Attack, pUser[conn].AccountName, "Skill kind out of bound - MSG_ATTACK", pUser[conn].AccountName, pUser[conn].IP);
//				return;
//			}
//		}
//
//		Level = pMob[conn].MOB.CurrentScore.Special[getSkillMaster];
//		Delay = Delay + Level;
//		Special = Level;
//	}
//	else
//	{
//		if (skillnum >= 96 && skillnum <= 100)
//		{
//			if (skillnum == 97)
//			{
//				if (m->PosX > 0 && m->PosX < 4096 && m->PosY > 0 && m->PosY < 4096)
//				{
//					int itemId = cSrv.pItemGrid[m->PosX][m->PosY];
//					if (itemId > 0 && itemId < MAX_ITEMLIST && cSrv.pItem[itemId].ITEM.sIndex == 746)
//						m->Motion = 1;
//					else
//					{
//						MyLog(LogType::Attack, pUser[conn].AccountName, "err, mortar invalid", pUser[conn].AccountName, pUser[conn].IP);
//						return;
//					}
//				}
//				else
//				{
//					MyLog(LogType::Attack, pUser[conn].AccountName, "err, mortar invalid", pUser[conn].AccountName, pUser[conn].IP);
//					return;
//				}
//			}
//			else
//			{
//				int learn = skillnum - 72;
//				int learnedSkill = 1 << learn;
//				if (m->ClientTick != SKIPCHECKTICK && (pMob[conn].MOB.LearnedSkill & learnedSkill) == FALSE && AddCrackError(conn, 208, 1) != FALSE)
//					return;
//			}
//
//			Level = pMob[conn].MOB.CurrentScore.Level;
//			Delay = Delay + Level;
//			Special = Level;
//		}
//	}
//
//#pragma region >> anti range
//	int alvo = m->Dam[0].TargetID;
//	if (conn != alvo)
//	{
//		int Spectral = -1;
//		int WeaponRange = -1;
//		int dist = 2;
//		int Distance = -1;
//
//		if (pMob[conn].MOB.LearnedSkill & (1 << 29))
//			Spectral = 1;
//
//		if (pMob[conn].MOB.Equip[6].sIndex != 0)
//		{
//			for (int i = 0; i < MAX_STATICEFFECT; i++)
//			{
//				if (g_pItemList[pMob[conn].MOB.Equip[6].sIndex].stEffect[i].sEffect == EF_RANGE)
//					WeaponRange = g_pItemList[pMob[conn].MOB.Equip[6].sIndex].stEffect[i].sValue;
//			}
//		}
//
//		if (Spectral != -1)
//			dist = dist + 1;
//
//		if (alvo > 0 && alvo < MAX_MOB)
//			Distance = BASE_GetDistance(pMob[conn].TargetX, pMob[conn].TargetY, pMob[alvo].TargetX, pMob[alvo].TargetY);
//
//		if (Distance <= 0 || Distance >= 150)
//			Distance = -1;
//
//		// Astaroth transformation
//		if (pMob[conn].MOB.Equip[0].sIndex == 24 && Distance > dist + 5)
//			return;
//		// Magic range attack
//		else if (m->Motion == 255)
//		{
//			if (Distance > dist + g_pSpell[m->SkillIndex].Range + 4) // 6 de distancia
//				return;
//		}
//		else
//		{
//			// Attacked without possessing any weapon
//			if (WeaponRange == -1)
//			{
//				if (Distance > dist + 4)
//					return;
//			}
//			// Attacked too far
//			else
//			{
//				if (Distance > dist + WeaponRange + 4)
//					return;
//			}
//		}
//	}
//#pragma endregion
//
//#pragma region RESFRIAMENTO
//	// Skill Hack
//	if (skillnum >= 0 && skillnum < 103)
//	{
//		int curTime = GetTickCount64(); // ou timeGetTime();
//
//		int skilldelay = g_pSpell[skillnum].Delay;
//
//		if (pUser[conn].inGame.Skill[skillnum].IntervalTime)
//		{
//			int PreviousAttackTime = curTime - pUser[conn].inGame.Skill[skillnum].IntervalTime;
//
//			if (&pMob[conn].MOB.Equip[4] && BASE_GetItemSanc(&pMob[conn].MOB.Equip[4]) >= 9) // Verificar se tem Manopla +9 = -1 Delay Skill
//				skilldelay--;
//
//			for (int i = 0; i < MAX_AFFECT; i++) {
//				int index = pMob[conn].Affect[i].Type;
//				if (index == NULL)
//					continue;
//
//				int master = pMob[conn].Affect[i].Level;
//				if (index == 8) { // Sagacidade
//					if ((master & (1 << 0)) != 0)
//						skilldelay--;
//				}
//			}
//
//			if (skillnum == 102) skilldelay = 1;
//			skilldelay = skilldelay * 1000;
//			if (skilldelay <= 0) skilldelay = 400;
//
//			pUser[conn].inGame.Skill[skillnum].IntervalTime = curTime;
//
//			// Skill Delay Hack
//			if (PreviousAttackTime < (skilldelay - 300)) {
//				for (int i = 0; i < 13; i++) {
//
//					int target = m->Dam[i].TargetID;
//
//					if (target <= 0 || target >= MAX_MOB)
//						continue;
//
//					//SendClientMessage(conn, "esse");
//					m->Dam[i].TargetID = 0;
//					m->Dam[i].Damage = 0;
//					return;
//				}
//
//				memset(pMsg, 0, m->Size);
//				return;
//			}
//		}
//
//		pUser[conn].inGame.Skill[skillnum].IntervalTime = curTime;
//	}
//#pragma endregion
//
//#pragma region Escudo dourado
//	if (skillnum == 85)
//	{
//		int coin = 100 * Level;
//
//		if (pMob[conn].MOB.Coin < coin)
//			return;
//
//		pMob[conn].MOB.Coin -= coin;
//		this->SendEtc();
//	}
//#pragma endregion
//#pragma region Consumo de mana
//	int Mp = pMob[conn].MOB.CurrentScore.Mp;
//	//int ReqMp = pUser[conn].ReqMp;
//
//	if (skillnum >= 0 && skillnum < MAX_SKILLINDEX)
//	{
//		int ManaSpent = BASE_GetManaSpent(skillnum, pMob[conn].MOB.SaveMana, Special);
//
//		if ((pMob[conn].MOB.CurrentScore.Mp - ManaSpent) < 0)
//		{
//			cSend.SendSetHpMp(this);
//			return;
//		}
//
//		pMob[conn].MOB.CurrentScore.Mp = pMob[conn].MOB.CurrentScore.Mp - ManaSpent;
//		//pUser[conn].ReqMp = pUser[conn].ReqMp - ManaSpent;
//		cSend.SetReqMp(conn);
//
//		/* Espelho Mágico */
//		if (pMob[conn].MOB.Class == 0 && (pMob[conn].Extra.SecLearnedSkill & 0x100))
//		{
//			pMob[conn].MOB.CurrentScore.Mp = pMob[conn].MOB.CurrentScore.Mp - ManaSpent / 2;
//			SetReqMp(conn);
//		}
//	}
//
//	m->CurrentMp = pMob[conn].MOB.CurrentScore.Mp;
//	//m->ReqMp = pUser[conn].ReqMp;
//#pragma endregion
//
//	int master = 0;
//
//	if (!pMob[conn].MOB.Class && pMob[conn].MOB.LearnedSkill & 0x4000)
//	{
//		master = pMob[conn].MOB.CurrentScore.Special[2] / 20;
//
//		if (master < 0)
//			master = 0;
//
//		if (master > 15)
//			master = 15;
//	}
//
//	/*if (pMob[conn].MOB.Class == 3)
//		DoRemoveHide(conn);*/
//
//	//DoRemoveSamaritano(conn);
//
//	int Exp = 0;
//
//	unsigned char map_att = cGet.GetAttribute(pMob[conn].TargetX, pMob[conn].TargetY) & 0x40;
//
//	int hp = pMob[conn].MOB.CurrentScore.Hp;
//
//	int UNK2 = 0;
//	unsigned char DoubleCritical = 0;
//
//	if (skillnum == 79)
//	{
//		m->ClientTick = 0xE0A1ACA;
//		m->Motion = 254;
//
//		m->TargetX = pMob[conn].TargetX;
//		m->TargetY = pMob[conn].TargetY;
//
//		m->AttackerID = conn;
//		m->SkillIndex = 79;
//
//		for (int x = 1; x < 6; x++)
//		{
//			m->Dam[x].Damage = m->Dam[0].Damage;
//			m->Dam[x].TargetID = m->Dam[0].TargetID;
//		}
//	}
//
//#pragma region Loop Target
//	for (int i = 0; i < MAX_TARGET; i++)
//	{
//		if (i >= MAX_TARGET && m->Size <= sizeof(MSG_Attack))
//			break;
//
//		else if (i >= 1 && m->Size <= sizeof(MSG_AttackOne))
//			break;
//
//		else if (i >= 2 && m->Size <= sizeof(MSG_AttackTwo))
//			break;
//
//
//		int idx = m->Dam[i].TargetID;
//
//		if (idx <= 0 || idx >= MAX_MOB)
//			continue;
//
//		/*if (pMob[idx].MOB.Class == 3)
//			DoRemoveHide(idx);*/
//
//		if (pMob[idx].Mode == MOB_EMPTY)
//		{
//			cSend.SendRemoveMob(this, idx, 1, 0);
//			continue;
//		}
//
//		if (idx >= MAX_USER && (pMob[idx].MOB.Equip[0].sIndex == 219 || pMob[idx].MOB.Equip[0].sIndex == 220) && ((pMob[idx].TargetX / 128) == 26 && (pMob[idx].TargetY / 128) == 12))
//		{
//			m->Dam[i].TargetID = 0;
//			m->Dam[i].Damage = 0;
//			continue;
//		}
//
//		if (pMob[idx].MOB.CurrentScore.Hp <= 0 && m->SkillIndex != 31 && m->SkillIndex != 99)
//		{
//			m->Dam[i].TargetID = 0;
//			m->Dam[i].Damage = 0;
//
//			// SendRemoveMob(conn, idx, 1, 0);
//			continue;
//		}
//
//		if (pMob[idx].MOB.Merchant == 4 || pMob[idx].MOB.Merchant == 43 || pMob[idx].MOB.Merchant == 1 || pMob[idx].MOB.Merchant == 100)
//		{
//			m->Dam[i].TargetID = 0;
//			m->Dam[i].Damage = 0;
//			continue;
//		}
//
//		if (skillnum != 42 && (pMob[conn].TargetX < pMob[idx].TargetX - VIEWGRIDX || pMob[conn].TargetX > pMob[idx].TargetX + VIEWGRIDX || pMob[conn].TargetY < pMob[idx].TargetY - VIEWGRIDY || pMob[conn].TargetY > pMob[idx].TargetY + VIEWGRIDY))
//		{
//			cSend.SendRemoveMob(this, idx, 1, 0);
//			continue;
//		}
//
//		int dam = m->Dam[i].Damage;
//
//		if (dam != -2 && dam != -1 && dam)
//		{
//			m->Dam[i].Damage = 0;
//			//AddCrackError(conn, 10, 77);
//			continue;
//		}
//
//		int leader = pMob[conn].Leader;
//
//		if (leader == 0)
//			leader = conn;
//
//		int mobleader = pMob[idx].Leader;
//
//		if (mobleader == 0)
//			mobleader = idx;
//
//		int Guild = pMob[conn].MOB.Guild;
//
//		if (pMob[conn].GuildDisable)
//			Guild = 0;
//
//		int MobGuild = pMob[idx].MOB.Guild;
//
//		if (pMob[idx].GuildDisable)
//			MobGuild = 0;
//
//		if (Guild == 0 && MobGuild == 0)
//			Guild = -1;
//
//		/*if (BrState && BRItem > 0 && pMob[conn].TargetX >= 2604 && pMob[conn].TargetY >= 1708 && pMob[conn].TargetX <= 2648 && pMob[conn].TargetY <= 1744)
//			Guild = -1;*/
//
//		int Clan = pMob[conn].MOB.Clan;
//		int MobClan = pMob[idx].MOB.Clan;
//
//		int isFrag = 0;
//
//		if (Clan == 7 && MobClan == 7 || Clan == 8 && MobClan == 8)
//			isFrag = 1;
//
//		/*if (WarOfTower::AttackTower(conn, idx) == FALSE)
//		{
//			m->Dam[i].TargetID = 0;
//			m->Dam[i].Damage = 0;
//			continue;
//		}*/
//
//		/*if (conn != idx && conn > 0 && conn < MAX_USER && idx > 0 && idx < MAX_USER && CastleState == 0 && GTorreState == 0 && NewbieEventServer == 0)
//		{
//			int TargetPK = pUser[idx].PKMode;
//			int TargetGuilty = GetGuilty(idx);
//
//			unsigned char attributemap = GetAttribute(pMob[conn].TargetX, pMob[conn].TargetY) & 64;
//
//			if (dam == -2 && !TargetPK && !TargetGuilty && attributemap || dam == -1 && skillnum >= 0 && skillnum <= MAX_SKILLINDEX && g_pSpell[skillnum].Aggressive && attributemap && !TargetPK && !TargetGuilty)
//			{
//				m->Dam[i].TargetID = 0;
//				m->Dam[i].Damage = 0;
//				continue;
//			}
//		}*/
//
//#pragma region Ataque físico
//		if (dam == -2)
//		{
//			int dis = BASE_GetDistance(m->PosX, m->PosY, m->TargetX, m->TargetY);
//
//			if (dis > this->Range || dis > 23)
//				return;
//
//			dam = 0;
//
//			if (i > 0 && m->Size < sizeof(MSG_AttackTwo) && pMob[conn].MOB.Class != 3 && (pMob[conn].MOB.LearnedSkill & 0x40) == 0)
//			{
//				//AddCrackError(conn, 10, 24);
//				continue;
//			}
//
//			int CriticalDouble = 0;
//
//			if (i == 0)
//				CriticalDouble = BASE_GetDoubleCritical(&pMob[conn].MOB, &this->cProgress, &m->Progress, &DoubleCritical);
//
//			dam = pMob[conn].MOB.CurrentScore.Damage;
//
//			if (DoubleCritical & 2)
//				dam = 15 * dam / 10; // trocado
//
//			/*if ((DoubleCritical & 2) != 0)
//			{
//				if (idx < MAX_USER)
//					dam = (((rand() % 2) + 13) * dam) / 10;
//				else
//					dam = (((rand() % 2) + 15) * dam) / 10;
//			}*/
//
//			int Ac = pMob[idx].MOB.CurrentScore.Ac;
//
//			/*if (idx < MAX_USER)
//				Ac *= 3;*/ // retirado
//
//			dam = BASE_GetDamage((int)dam, Ac, master);
//
//			if (i == 0 && m->Size >= sizeof(MSG_AttackTwo) && pMob[conn].MOB.Class == 3 && (pMob[conn].MOB.LearnedSkill & 0x200000) && (rand() % 4) == 0)
//			{
//				int skilldam = (pMob[conn].MOB.CurrentScore.Special[3] + pMob[conn].MOB.CurrentScore.Str);
//
//				skilldam = BASE_GetDamage(skilldam, Ac, master);
//
//				if (skilldam > 0)
//					skilldam /= 2;
//
//				if (skilldam < 60)
//					skilldam = rand() % 60 + 10;
//
//				m->Dam[1].TargetID = MOB_EMPTY;
//				m->Dam[1].Damage = skilldam;
//
//				DoubleCritical |= 4;
//				dam += skilldam;
//			}
//
//			if (DoubleCritical & 1 && idx >= MAX_USER)
//				dam *= 2;
//
//			m->DoubleCritical = DoubleCritical;
//		}
//#pragma endregion
//#pragma region Ataque Mágico
//		else if (dam == -1 && skillnum >= 0 && skillnum <= MAX_SKILLINDEX)
//		{
//			auto userIDX = cSrv.getUser(idx);
//			if (!userIDX)
//				continue;
//
//			dam = 0;
//
//			int maxtarget = g_pSpell[skillnum].MaxTarget;
//
//			if (ClientTick != SKIPCHECKTICK && i > maxtarget)
//			{
//				//AddCrackError(conn, 10, 28);
//
//				snprintf(temp, sizeof(temp), "skill target max num %d", ClientTick);
//				MyLog(LogType::Attack, pUser[conn].AccountName, temp, 0, pUser[conn].IP);
//
//				continue;
//			}
//
//			if (g_pSpell[skillnum].bParty != 0 && leader != mobleader && Guild != MobGuild)
//			{
//				//AddCrackError(conn, 10, 27);
//
//				snprintf(temp, sizeof(temp), "party skill - skill:%d leader:%d tleader:%d", ClientTick, leader, mobleader);
//				MyLog(LogType::Attack, pUser[conn].AccountName, temp, 0, pUser[conn].IP);
//
//				continue;
//			}
//
//			int InstanceType = g_pSpell[skillnum].InstanceType;
//
//#pragma region Ataques com elementos
//			if (InstanceType >= 1 && InstanceType <= 5)
//			{
//				int Weather = cSrv.CurrentWeather;
//				int ApplyWeather = 0;
//
//				if ((pMob[conn].TargetX / 128) < 12 && (pMob[conn].TargetY / 128) > 25)
//					ApplyWeather = 1;
//
//				else
//					ApplyWeather = 0;
//
//				if (ApplyWeather != 0)
//					Weather = 0;
//#pragma region Ataque de Fada
//				if (ClientTick == SKIPCHECKTICK && m->Motion == 254 && (m->SkillIndex == 32 || m->SkillIndex == 34 || m->SkillIndex == 36))
//				{
//					int lvl = pMob[conn].MOB.CurrentScore.Level;
//					int familiarsanc = BASE_GetItemSanc(&pMob[conn].MOB.Equip[13]);
//
//					switch (m->SkillIndex)
//					{
//					case 32:
//						dam = (familiarsanc * 200) + (lvl * 8);
//						break;
//					case 34:
//						dam = (familiarsanc * 300) + (lvl * 8);
//						break;
//					case 36:
//						dam = (familiarsanc * 350) + (lvl * 8);
//						break;
//					}
//
//					if (dam > 1000)
//						dam = 1000;
//				}
//#pragma endregion
//				else
//					dam = BASE_GetSkillDamage(skillnum, &pMob[conn].MOB, Weather, pMob[conn].WeaponDamage);
//
//				int def = pMob[idx].MOB.CurrentScore.Ac;
//
//				if (idx < MAX_USER)
//					def *= 2;
//
//				if (pMob[idx].MOB.Class == 1)
//					def = (def * 3) / 2;
//
//				dam = BASE_GetSkillDamage(dam, def, master);
//
//				if (InstanceType == 1)
//				{
//					int ResistPoint = 0;
//
//					if (idx < MAX_USER)
//					{
//						ResistPoint = 10;
//
//						if (pMob[idx].MOB.CurrentScore.Level > 354)
//						{
//							if (pMob[idx].Extra.ClassMaster >= CELESTIAL)
//							{
//								ResistPoint += 32;
//							}
//							else if (pMob[idx].Extra.ClassMaster == ARCH)
//							{
//								ResistPoint += 16;
//							}
//							else
//							{
//								ResistPoint += 15;
//							}
//						}
//					}
//					else
//					{
//						ResistPoint = pMob[idx].MOB.LearnedSkill / 1000;
//					}
//
//					dam = ((100 - ResistPoint) * dam) / 100;
//				}
//				else if (InstanceType >= 2 && InstanceType <= 5 && skillnum != 97)
//				{
//					int skind = InstanceType - 2;
//
//					int MobResist = pMob[idx].MOB.Resist[skind];
//
//					if (idx < MAX_USER)
//						MobResist /= 2;
//
//					dam = dam * (200 - MobResist) / 100;
//				}
//
//			}
//#pragma endregion
//#pragma region All Skills
//#pragma region Tempestade de Raios
//			if (skillnum == 79)
//			{
//				dam = BASE_GetSkillDamage(skillnum, &pMob[conn].MOB, cSrv.CurrentWeather, pMob[conn].WeaponDamage);
//
//				int Ac = pMob[idx].MOB.CurrentScore.Ac;
//
//				if (idx < MAX_USER)
//					Ac *= 2;
//
//				dam = pMob[idx].MOB.CurrentScore.Damage;
//
//				cSend.SendSetHpMp(this);
//			}
//#pragma endregion
//#pragma region Bênção Divina
//			if (skillnum == 201) // Bênção Divina
//			{
//				if (pMob[conn].Extra.SecLearnedSkill & 0x2)
//				{
//					int Hp = pMob[conn].MOB.CurrentScore.Hp;
//					int MaxHp = pMob[conn].MOB.CurrentScore.MaxHp;
//					int HpTax = MaxHp - Hp;
//
//					pMob[conn].DivineBlessing = TRUE;
//					pMob[conn].RateRegen = HpTax;
//
//					MSG_Attack sm;
//
//					memset(&sm, 0, sizeof(MSG_AttackOne));
//
//					sm.Type = _MSG_AttackOne;
//					sm.ID = conn;
//					sm.Size = sizeof(MSG_AttackOne);
//
//					sm.ClientTick = 0xE0A1ACA;
//					sm.Motion = 255;
//
//					sm.TargetX = pMob[conn].TargetX;
//					sm.TargetY = pMob[conn].TargetY;
//
//					sm.CurrentMp = -2;
//
//					sm.AttackerID = conn;
//					sm.SkillIndex = 29;
//
//					sm.Dam[0].TargetID = conn;
//					sm.Dam[0].Damage = -1;
//
//					if (conn < MAX_USER)
//						cSrv.ProcessClientMSG(this->conn, (char*)&sm, TRUE);
//				}
//			}
//			else if (InstanceType == 6 && pMob[conn].DivineBlessing == TRUE)
//			{
//				if (pMob[idx].MOB.Clan == 4)
//					continue;
//
//				unsigned char getAttribute = cGet.GetAttribute(pMob[idx].TargetX, pMob[idx].TargetY) & 0x40;
//
//				if (map_att == 0 && getAttribute)
//				{
//					m->Dam[i].TargetID = MOB_EMPTY;
//					m->Dam[i].Damage = 0;
//
//					continue;
//				}
//
//				if (skillnum == 27)
//					dam = 0;
//				else
//					dam = pMob[conn].RateRegen;
//
//				dam = -dam;
//
//				if (dam < 0 && dam > -6)
//					dam = -6;
//
//				int MobHP = pMob[idx].MOB.CurrentScore.Hp;
//
//				if (pMob[idx].MOB.Equip[13].sIndex == 786)
//				{
//					int sanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//					if (sanc < 2)
//						sanc = 2;
//
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - dam / sanc;
//
//				}
//				else  if (pMob[idx].MOB.Equip[13].sIndex == 1936)
//				{
//					int sanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//					if (sanc < 2)
//						sanc = 2;
//
//					sanc *= 10;
//
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - dam / sanc;
//
//				}
//
//				else  if (pMob[idx].MOB.Equip[13].sIndex == 1937)
//				{
//					int sanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//					if (sanc < 2)
//						sanc = 2;
//
//					sanc *= 1000;
//
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - dam / sanc;
//
//				}
//
//				else
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - dam;
//
//
//				if (pMob[idx].MOB.CurrentScore.Hp > pMob[idx].MOB.CurrentScore.MaxHp)
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.MaxHp;
//
//
//				if (idx > 0 && idx < MAX_USER)
//					SetReqHp(idx);
//
//
//				int calc_exp = pMob[idx].MOB.CurrentScore.Hp - MobHP >> 3;
//
//				if (calc_exp > 120)
//					calc_exp = 120;
//
//				if (calc_exp < 0)
//					calc_exp = 0;
//
//
//				if (calc_exp > 0 && conn != idx && conn > 0 && conn < MAX_USER)
//				{
//					int village = BASE_GetVillage(pMob[idx].TargetX, pMob[idx].TargetY);
//
//					if (village < 0 || village >= 5)
//					{
//						if (UNK2 < calc_exp)
//							UNK2 = calc_exp;
//					}
//				}
//				pMob[conn].DivineBlessing = FALSE;
//				pMob[conn].RateRegen = 0;
//			}
//#pragma endregion
//#pragma region Cura / Recuperação
//			else if (InstanceType == 6)
//			{
//				if (pMob[idx].MOB.Clan == 4)
//					continue;
//
//				unsigned char getAttribute = cGet.GetAttribute(pMob[idx].TargetX, pMob[idx].TargetY) & 0x40;
//
//				if (map_att == 0 && getAttribute || pMob[idx].MOB.CurrentScore.Hp <= 0)
//				{
//					m->Dam[i].TargetID = MOB_EMPTY;
//					m->Dam[i].Damage = 0;
//
//					continue;
//				}
//
//				if (skillnum == 27)
//					dam = (Special * 2) + g_pSpell[skillnum].InstanceValue;
//				else
//					dam = (Special * 3) + g_pSpell[skillnum].InstanceValue;
//
//
//				if (pMob[conn].Extra.ClassMaster != MORTAL && pMob[conn].Extra.ClassMaster != ARCH)
//					dam *= 2;
//
//				if (pMob[conn].Extra.ClassMaster != MORTAL && pMob[conn].Extra.ClassMaster != ARCH)
//				{
//					if (dam >= 1090)
//						dam = 1100;
//				}
//				else
//				{
//					if (dam >= 638)
//						dam = 638;
//				}
//
//				dam = -dam;
//
//				if (dam < 0 && dam > -6)
//					dam = -6;
//
//				int MobHP = pMob[idx].MOB.CurrentScore.Hp;
//
//				if (pMob[idx].MOB.Equip[13].sIndex == 786)
//				{
//					int sanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//					if (sanc < 2)
//						sanc = 2;
//
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - ((int)dam / sanc);
//
//				}
//				else  if (pMob[idx].MOB.Equip[13].sIndex == 1936)
//				{
//					int sanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//					if (sanc < 2)
//						sanc = 2;
//
//					sanc *= 10;
//
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - ((int)dam / (sanc * 10));
//
//				}
//
//				else  if (pMob[idx].MOB.Equip[13].sIndex == 1937)
//				{
//					int sanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//					if (sanc < 2)
//						sanc = 2;
//
//					sanc *= 1000;
//
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - ((int)dam / (sanc * 20));
//
//				}
//
//				else
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - (int)dam;
//
//
//				if (pMob[idx].MOB.CurrentScore.Hp > pMob[idx].MOB.CurrentScore.MaxHp)
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.MaxHp;
//
//				if (idx > 0 && idx < MAX_USER)
//					SendHpMp(idx);
//
//				int calc_exp = (pMob[idx].MOB.CurrentScore.Hp - MobHP) >> 3;
//
//				if (calc_exp > 120)
//					calc_exp = 120;
//
//				if (calc_exp < 0)
//					calc_exp = 0;
//
//
//				if (calc_exp > 0 && conn != idx && conn > 0 && conn < MAX_USER)
//				{
//					int village = BASE_GetVillage(pMob[idx].TargetX, pMob[idx].TargetY);
//
//					if (village < 0 || village >= 5)
//					{
//						if (UNK2 < calc_exp)
//							UNK2 = calc_exp;
//					}
//				}
//			}
//#pragma endregion
//#pragma region Flash
//			else if (InstanceType == 7)
//			{
//				pMob[idx].Mode = MOB_PEACE;
//				pMob[idx].CurrentTarget = MOB_EMPTY;
//
//				for (int j = 0; j < MAX_ENEMY; j++)
//					pMob[idx].EnemyList[j] = MOB_EMPTY;
//			}
//#pragma endregion
//#pragma region Desintoxicar
//			else if (InstanceType == 8)
//			{
//				int NeedUpdate = 0;
//
//				for (int k = 0; k < MAX_AFFECT; k++)
//				{
//					int Type = pMob[idx].Affect[k].Type;
//
//					if (Type == 1 || Type == 3 || Type == 5 || Type == 7 || Type == 10 || Type == 12 || Type == 20 || (Type == 32 && (1 << 7) & pMob[conn].MOB.LearnedSkill))
//					{
//						pMob[idx].Affect[k].Type = 0;
//						NeedUpdate = 1;
//					}
//				}
//
//				if (NeedUpdate)
//				{
//					pMob[idx].GetCurrentScore(idx);
//					cSend.SendScore(idx);
//				}
//			}
//#pragma endregion
//#pragma region Teleporte
//			else if (InstanceType == 9)
//			{
//				if (pMob[idx].MOB.CurrentScore.Hp <= 0)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Cant_Summon_Dead_Person]);
//					break;
//				}
//
//				unsigned char getAtt = cGet.GetAttribute(pMob[conn].TargetX, pMob[conn].TargetY);
//
//				if (getAtt & 0x4 && pMob[conn].MOB.CurrentScore.Level < 1000)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Summon_Not_Allowed_Here]);
//					continue;
//				}
//
//				if (pMob[idx].MOB.CurrentScore.Hp > pMob[conn].MOB.CurrentScore.Hp + Special + 30)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Too_High_Level_To_Summon]);
//					continue;
//				}
//
//				if ((pMob[idx].TargetX & 0xFF00) == 0 && (pMob[idx].TargetY & 0xFF00) == 0)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Cant_Use_That_Here]);
//					return;
//				}
//
//				/* Zone Kefra*/
//				if (pMob[idx].TargetX >= 2335 && pMob[idx].TargetY >= 3896 && pMob[idx].TargetX <= 2395 && pMob[idx].TargetY <= 3955)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Cant_Use_That_Here]);
//					return;
//				}
//
//				/* Zone RvR*/
//				if (pMob[idx].TargetX >= 1710 && pMob[idx].TargetY >= 1968 && pMob[idx].TargetX <= 1755 && pMob[idx].TargetY <= 1995)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Cant_Use_That_Here]);
//					return;
//				}
//
//				/* Zone Calabouço Zumbi */
//				if ((pMob[idx].TargetX / 128) == 13 || (pMob[idx].TargetY / 128) == 16)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Cant_Use_That_Here]);
//					return;
//				}
//
//				/* Zone Coliseu N */
//				if ((pMob[idx].TargetX / 128) == 27 || (pMob[idx].TargetY / 128) == 11)
//				{
//					this->ClientMsg(g_pMessageStringTable[_NN_Cant_Use_That_Here]);
//					return;
//				}
//
//				if (auto alvotele = cSrv.getUser(idx))
//				{
//					if (alvotele->Mode == USER_PLAY)
//					{
//						DoSummon(idx, pMob[conn].TargetX, pMob[conn].TargetY);
//
//						snprintf(temp, sizeof(temp), g_pMessageStringTable[_SN_Summoned_By_S], pMob[conn].MOB.MobName);
//						alvotele->ClientMsg(temp);
//					}
//				}
//			}
//#pragma endregion
//#pragma region Invisibilidade
//			else if (InstanceType == 10 && idx < MAX_USER)
//			{
//				for (int l = MAX_USER; l < MAX_MOB; l++)
//				{
//					if (pMob[l].Mode == MOB_COMBAT && pMob[l].CurrentTarget == idx)
//					{
//						for (int mm = 0; mm < MAX_ENEMY; mm++)
//						{
//							if (pMob[l].EnemyList[mm] == idx)
//								pMob[l].EnemyList[mm] = conn;
//						}
//
//						pMob[l].CurrentTarget = conn;
//					}
//				}
//			}
//#pragma endregion
//#pragma region Evocação
//			else if (InstanceType == 11)
//			{
//
//				int instancevalue = g_pSpell[skillnum].InstanceValue;
//
//				if (instancevalue >= 1 && instancevalue <= 50)
//				{
//					int summons = 0;
//
//					if (instancevalue == 1 || instancevalue == 2)
//						summons = pMob[conn].MOB.CurrentScore.Special[2] / 30;
//
//					else if (instancevalue == 3 || instancevalue == 4 || instancevalue == 5)
//						summons = pMob[conn].MOB.CurrentScore.Special[2] / 40;
//
//					else if (instancevalue == 6 || instancevalue == 7)
//						summons = pMob[conn].MOB.CurrentScore.Special[2] / 80;
//
//					else if (instancevalue == 8)
//						summons = 1;
//
//					if (GenerateSummon(conn, instancevalue - 1, 0, summons) == 0)
//					{
//						pMob[conn].MOB.CurrentScore.Mp = Mp;
//						//pUser[conn].ReqMp = ReqMp;
//					}
//				}
//				dam = 0;
//			}
//#pragma endregion
//#pragma region Chamas Etéreas
//			else if (InstanceType == 12)
//			{
//				int Leader = pMob[conn].Leader, slot = 0;
//
//				if (Leader <= 0)
//					Leader = conn;
//
//				// Montros
//				if (idx > MAX_USER && pMob[idx].MOB.Merchant == 0)
//				{
//					if (pMob[idx].TargetX >= 3725 && pMob[idx].TargetY >= 2833 && pMob[idx].TargetX <= 3833 && pMob[idx].TargetY <= 2908)
//						continue;
//
//					if (pMob[idx].TargetX >= 3837 && pMob[idx].TargetY >= 2873 && pMob[idx].TargetX <= 3942 && pMob[idx].TargetY <= 2894)
//						continue;
//
//					if (pMob[idx].TargetX >= 1676 && pMob[idx].TargetY >= 1556 && pMob[idx].TargetX <= 1776 && pMob[idx].TargetY <= 1636)
//						continue;
//
//					if (pMob[idx].TargetX >= 1676 && pMob[idx].TargetY >= 1816 && pMob[idx].TargetX <= 1776 && pMob[idx].TargetY <= 1892)
//						continue;
//
//					if (pMob[idx].TargetX >= 2445 && pMob[idx].TargetY >= 1850 && pMob[idx].TargetX <= 2546 && pMob[idx].TargetY <= 1920)
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Cav. Lugefer"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "FrenzyDemonLord"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Lich Crunt"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Dragao Lich"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Hezling"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Golem Anciao"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "LichCrunt"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Lich Batama"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Espelho RainhaG"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "VeridRainhaG"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Verid"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Sombra Negra"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "LichVatama"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Torre Guardia"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Torre Guardia "))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Lich Infernal"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Lugefer"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Torre"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Shama Amald"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Ranger Amald"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Mago Amald"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Templario Amald"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Ent Anciao"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Guerreiro Amon"))
//						continue;
//
//					if (!strcmp(pMob[idx].MOB.MobName, "Simio Anciao"))
//						continue;
//
//					int targetLevel = pMob[idx].MOB.CurrentScore.Level; //378
//					int level = pMob[conn].MOB.CurrentScore.Level; //37c
//
//					int calcLevel = 100; //380
//					if (targetLevel + 10 > level)
//					{
//						int getCalcLevel = (targetLevel + 10) - level; //384
//						calcLevel = (getCalcLevel << 2) - calcLevel;
//					}
//
//					//00432E8F
//					int Rand = rand() % 100; //388
//					if (Rand > calcLevel || idx < MAX_USER || pMob[idx].MOB.CurrentScore.Hp < pMob[idx].MOB.CurrentScore.MaxHp || pMob[idx].MOB.Clan == 6)
//						continue;
//
//					int j = 0;
//
//					for (j = 0; j < MAX_PARTY; j++)
//					{
//						if (pMob[Leader].PartyList[j])
//						{
//							slot++;
//							continue;
//						}
//
//						else
//							break;
//					}
//
//					if (j >= MAX_PARTY)
//					{
//						this->ClientMsg(g_pMessageStringTable[_NN_Party_Full_Cant_Summon]);
//						return;
//					}
//
//					cSrv.RemoveParty(idx);
//
//					memset(pMob[idx].PartyList, 0, sizeof(pMob[idx].PartyList));
//					pMob[idx].MOB.BaseScore.Level = pMob[conn].MOB.BaseScore.Level <= MAX_LEVEL ? pMob[conn].MOB.BaseScore.Level : MAX_LEVEL;
//
//					strcat(pMob[idx].MOB.MobName, "^");
//
//					for (int k = 0; k < NAME_LENGTH; k++)
//					{
//						if (pMob[idx].MOB.MobName[k] == '_')
//							pMob[idx].MOB.MobName[k] = ' ';
//					}
//
//					memset(pMob[idx].Affect, 0, sizeof(pMob[idx].Affect));
//
//					pMob[idx].Formation = 5;
//
//					pMob[idx].RouteType = 5;
//					pMob[idx].Mode = MOB_PEACE;
//
//					memset(&pMob[idx].SegmentX, 0, sizeof(pMob[idx].SegmentX));
//					memset(&pMob[idx].SegmentY, 0, sizeof(pMob[idx].SegmentY));
//
//					pMob[idx].Leader = Leader;
//
//					pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.MaxHp;
//					pMob[idx].MOB.Clan = 4;
//
//					pMob[idx].GetCurrentScore(MAX_USER);
//
//					memset(pMob[idx].EnemyList, 0, sizeof(pMob[idx].EnemyList));
//
//					MSG_CreateMob sm_tb;
//					memset(&sm_tb, 0, sizeof(MSG_CreateMob));
//					cGet.GetCreateMob(idx, &sm_tb);
//
//					cSrv.pMobGrid[pMob[idx].TargetY][pMob[idx].TargetX] = idx;
//
//					cSend.GridMulticast(pMob[idx].TargetX, pMob[idx].TargetY, (MSG_HEADER*)&sm_tb, 0);
//
//					pMob[Leader].PartyList[j] = idx;
//					pMob[idx].Leader = Leader;
//
//					if (slot == 0)
//						cSend.SendAddParty(Leader, Leader, 0);
//
//					cSend.SendAddParty(idx, Leader, 0);
//
//					if (slot == 0)
//						cSend.SendAddParty(idx, idx, j + 1);
//
//					cSend.SendAddParty(Leader, idx, j + 1);
//
//					for (int l = 0; l < MAX_PARTY; l++)
//					{
//						if (pMob[Leader].PartyList[l] == 0)
//							continue;
//
//						if (pMob[Leader].PartyList[l] != idx)
//							cSend.SendAddParty(pMob[Leader].PartyList[l], idx, l + 1);
//
//						cSend.SendAddParty(idx, pMob[Leader].PartyList[l], l + 1);
//					}
//
//					pMob[idx].Summoner = conn;
//				}
//
//
//				// Personagem
//				else if (idx < MAX_USER)
//				{
//					auto userIDX = cSrv.getUser(idx);
//					if (!userIDX)
//						continue;
//
//					int chanceA = (pMob[conn].MOB.BaseScore.Special[1] + 1) / 7;
//					int Rand = rand() % 100;
//
//					if (Rand > chanceA)
//					{
//						int burnmp = ((pMob[idx].MOB.CurrentScore.Mp + 1) / 100) * (10 + rand() % 10);
//
//						if (burnmp > pMob[idx].MOB.CurrentScore.Mp)
//							pMob[idx].MOB.CurrentScore.Mp = 0;
//
//						else
//							pMob[idx].MOB.CurrentScore.Mp -= burnmp;
//
//						cSend.SendSetHpMp(userIDX);
//
//						cSend.SendScore(idx);
//						continue;
//					}
//
//					for (int k = 0; k < MAX_AFFECT; k++)
//					{
//						int Type = pMob[idx].Affect[k].Type;
//
//						if (Type == 18 || Type == 16 || Type == 14 || Type == 19 || Type == 32)
//						{
//							pMob[idx].Affect[k].Type = 0;
//							pMob[idx].Affect[k].Value = 0;
//							pMob[idx].Affect[k].Level = 0;
//							pMob[idx].Affect[k].Time = 0;
//
//							pMob[idx].GetCurrentScore(idx);
//							cSend.SendScore(idx);
//						}
//					}
//
//					userIDX->SendEquip(0);
//					pMob[idx].GetCurrentScore(idx);
//					cSend.SendScore(idx);
//					continue;
//				}
//			}
//
//#pragma endregion
//#pragma region Furia Divina
//			if (skillnum == 6)
//			{
//				if (idx >= MAX_USER && pMob[idx].MOB.Merchant != 0)
//					break;
//
//				if (pMob[idx].MOB.Equip[0].sIndex == 219 || pMob[idx].MOB.Equip[0].sIndex == 220 || pMob[idx].MOB.Equip[0].sIndex == 362)
//					break;
//
//				if (pMob[idx].GenerateIndex == 8 || pMob[idx].GenerateIndex == 9)
//					break;
//
//				if (pMob[idx].MOB.Clan == 6)
//					break;
//
//				dam = 1;
//
//				pMob[idx].Route[0] = 0;
//
//				int PosX = pMob[conn].TargetX;
//
//				if (PosX < pMob[idx].TargetX)
//					PosX++;
//
//				if (PosX > pMob[idx].TargetX)
//					PosX--;
//
//				int PosY = pMob[conn].TargetY;
//
//				if (PosY < pMob[idx].TargetY)
//					PosY++;
//
//				if (PosY > pMob[idx].TargetY)
//					PosY--;
//
//				if (cGet.GetEmptyMobGrid(idx, &PosX, &PosY) == FALSE)
//					break;
//
//				int kind2 = pMob[conn].MOB.CurrentScore.Special[1];
//
//				int kindvalue = (kind2 / 10) + 20;
//
//				if (idx > MAX_USER)
//					kindvalue = (kind2 / 5) + 40;
//
//				int leveldif = pMob[conn].Extra.ClassMaster != MORTAL && pMob[conn].Extra.ClassMaster != ARCH ? pMob[conn].MOB.CurrentScore.Level : pMob[conn].MOB.CurrentScore.Level + MAX_LEVEL - (pMob[idx].Extra.ClassMaster != MORTAL && pMob[idx].Extra.ClassMaster != ARCH ? pMob[idx].MOB.CurrentScore.Level + MAX_LEVEL : pMob[idx].MOB.CurrentScore.Level);
//
//				leveldif = leveldif >> 2;
//
//				int _chance = kindvalue + leveldif;
//
//				if (_chance > 50)
//					_chance = 50;
//
//				if (rand() % 100 < _chance)
//				{
//					MSG_Action sm_act;
//					memset(&sm_act, 0, sizeof(MSG_Action));
//
//					cGet.GetAction(idx, PosX, PosY, &sm_act);
//
//					sm_act.Effect = 2;
//					sm_act.Speed = 6;
//
//					cSend.GridMulticast(idx, PosX, PosY, (MSG_HEADER*)&sm_act);
//
//					if (idx < MAX_USER)
//					{
//						memset(&sm_act, 0, sizeof(MSG_Action));
//						cGet.GetAction(idx, PosX, PosY, &sm_act);
//
//						sm_act.Effect = 2;
//						sm_act.Speed = 6;
//						userIDX->cSock.AddMessage((char*)&sm_act, sizeof(MSG_Action));
//					}
//
//					if (idx >= MAX_USER)
//						SetBattle(idx, conn);
//				}
//			}
//#pragma endregion
//#pragma region Exterminar
//			else if (skillnum == 22)
//			{
//				int CurrentMp = pMob[conn].MOB.CurrentScore.Mp;
//
//				pMob[conn].MOB.CurrentScore.Mp = 0;
//				//pUser[conn].ReqMp = 0;
//
//				m->CurrentMp = 0;
//
//				int _INT = pMob[conn].MOB.CurrentScore.Int;
//				dam = (dam + CurrentMp) + (_INT / 2);
//
//				int PosX = pMob[idx].TargetX;
//				int PosY = pMob[idx].TargetY;
//
//				if (cGet.GetEmptyMobGrid(conn, &PosX, &PosY) != 0 && pMob[idx].MOB.Equip[0].sIndex != 219 && pMob[idx].MOB.Equip[0].sIndex != 220)
//				{
//					MSG_Action sm_act;
//					memset(&sm_act, 0, sizeof(MSG_Action));
//
//					cGet.GetAction(conn, PosX, PosY, &sm_act);
//
//					sm_act.Type = 2;
//					sm_act.Speed = 2;
//
//					cSend.GridMulticast(conn, PosX, PosY, (MSG_HEADER*)&sm_act);
//					this->cSock.AddMessage((BYTE*)&sm_act, sizeof(MSG_Action));
//				}
//			}
//#pragma endregion
//#pragma region Julgamento Divino
//			else if (skillnum == 30)
//			{
//				dam = dam + hp;
//
//				pMob[conn].MOB.CurrentScore.Hp = ((pMob[conn].MOB.CurrentScore.Hp) / 6) + 1;
//				//pUser[conn].ReqHp = pMob[conn].MOB.CurrentScore.Hp;
//			}
//#pragma endregion
//#pragma region Renascimento 
//			else if (skillnum == 31)
//			{
//				int hP = (rand() % 10 + 10) * ((pMob[conn].MOB.CurrentScore.MaxHp + 1) / 100);
//
//				pMob[conn].MOB.CurrentScore.Mp = 0;
//				//pUser[conn].ReqMp = 0;
//
//				m->CurrentMp = 0;
//
//				SendSetHpMp(conn);
//
//				if ((rand() % 100) < 70)
//				{
//					pMob[idx].MOB.CurrentScore.Hp = hP;
//					pUser[idx].NumError = 0;
//					SendScore(idx);
//					SendSetHpMp(idx);
//					SendEtc(idx);
//
//					int sAffect = GetEmptyAffect(idx, 55);
//
//					if (sAffect == -1 || pMob[idx].Affect[sAffect].Type == 55)
//						continue;
//
//					pMob[idx].Affect[sAffect].Type = 55;
//					pMob[idx].Affect[sAffect].Value = 0;
//					pMob[idx].Affect[sAffect].Level = 0;
//					pMob[idx].Affect[sAffect].Time = 2;
//
//					pMob[idx].GetCurrentScore(idx);
//					SendScore(idx);
//
//					MSG_CreateMob CreateMob;
//					GetCreateMob(idx, &CreateMob);
//					GridMulticast(pMob[idx].TargetX, pMob[idx].TargetY, (MSG_STANDARD*)&CreateMob, 0);
//				}
//			}
//#pragma endregion
//#pragma region Velocidade
//			else if (skillnum == 41)
//			{
//				int skill_target = (Special / 25) + 2;
//
//				if (skill_target >= MAX_TARGET)
//					skill_target = MAX_TARGET;
//
//				if (skill_target <= 1)
//					skill_target = 2;
//
//				for (int q = 0; q < skill_target && ((q < MAX_TARGET && m->Size >= sizeof(MSG_Attack)) || (q < 1 && m->Size >= sizeof(MSG_AttackOne)) || (q < 2 && m->Size >= sizeof(MSG_AttackTwo))); q++)
//				{
//					int targetidx = m->Dam[q].TargetID;
//
//					if (targetidx <= MOB_EMPTY || targetidx >= MAX_USER)
//						continue;
//
//					if (pUser[targetidx].Mode != USER_PLAY)
//						continue;
//
//					if (SetAffect(targetidx, skillnum, Delay, Level) == 0)
//						continue;
//
//					SendScore(targetidx);
//				}
//
//				break;
//			}
//#pragma endregion
//#pragma region Arma Mágica
//			else if (skillnum == 44)
//			{
//				int skill_target = (Special / 25) + 2;
//
//				if (skill_target >= MAX_TARGET)
//					skill_target = MAX_TARGET;
//
//				if (skill_target <= 1)
//					skill_target = 2;
//
//				for (int q = 0; q < skill_target && ((q < MAX_TARGET && m->Size >= sizeof(MSG_Attack)) || (q < 1 && m->Size >= sizeof(MSG_AttackOne)) || (q < 2 && m->Size >= sizeof(MSG_AttackTwo))); q++)
//				{
//					int targetidx = m->Dam[q].TargetID;
//					if (targetidx <= MOB_EMPTY || targetidx >= MAX_USER)
//						continue;
//
//					if (pUser[targetidx].Mode != USER_PLAY)
//						continue;
//
//					if (SetAffect(targetidx, skillnum, Delay, Level) == 0)
//						continue;
//
//					SendScore(targetidx);
//				}
//
//				break;
//			}
//#pragma endregion
//#pragma region Cancelamento
//			else if (skillnum == 47)
//			{
//				int CancelContinue = 0;
//
//				for (int k = 0; k < MAX_AFFECT; k++)
//				{
//					int Type = pMob[idx].Affect[k].Type;
//
//					if (Type == 19)
//					{
//						pMob[idx].Affect[k].Type = 0;
//						pMob[idx].Affect[k].Value = 0;
//						pMob[idx].Affect[k].Level = 0;
//						pMob[idx].Affect[k].Time = 0;
//
//						pMob[idx].GetCurrentScore(idx);
//						SendScore(idx);
//						CancelContinue = 1;
//						break;
//					}
//				}
//
//				if (CancelContinue == 1)
//					break;
//			}
//#pragma endregion
//#pragma region Book Vinha
//			else if (skillnum == 98)
//			{
//				if (m->TargetX < 0 || m->TargetX >= MAX_GRIDX || m->TargetY < 0 || m->TargetY >= MAX_GRIDY)
//					break;
//
//				unsigned char getAttribute = GetAttribute(m->TargetX, m->TargetY);
//				getAttribute = getAttribute & 0x40;
//
//				if (map_att == 0 && getAttribute == 1)
//					continue;
//
//				CreateMob("Vinha", m->TargetX, m->TargetY, "npc", 3);
//				break;
//			}
//#pragma endregion
//#pragma region Book Ressureição
//			if (pMob[conn].MOB.CurrentScore.Hp == 0 && skillnum == 99)
//			{
//				int rev = rand() % 115;
//
//				if (rev > 100)
//					rev -= 15;
//
//				if (rev >= 40)
//				{
//					pMob[conn].MOB.CurrentScore.Hp = 2;
//					pUser[conn].NumError = 0;
//					SendScore(conn);
//					SendSetHpMp(conn);
//					DoRecall(conn);
//					SendEtc(conn);
//				}
//
//				int tHP = (rand() % 50 + 1) * ((pMob[conn].MOB.CurrentScore.MaxHp + 1) / 100);
//
//				pMob[conn].MOB.CurrentScore.Hp = tHP;
//				pUser[conn].NumError = 0;
//				SendScore(conn);
//				SendSetHpMp(conn);
//				SendEtc(conn);
//
//				MSG_CreateMob CreateMob;
//				GetCreateMob(conn, &CreateMob);
//				GridMulticast(pMob[conn].TargetX, pMob[conn].TargetY, (MSG_STANDARD*)&CreateMob, 0);
//			}
//#pragma endregion
//#pragma region Buffs Time
//			int Aggressive = g_pSpell[skillnum].Aggressive;
//
//			int SameLeaderGuild = 1;
//
//			if (Aggressive != 0)
//			{
//				if (leader == mobleader || Guild == MobGuild)
//					SameLeaderGuild = 0;
//
//				int affectresist = g_pSpell[skillnum].AffectResist;
//
//				int idxlevel = pMob[idx].Extra.ClassMaster == MORTAL || pMob[idx].Extra.ClassMaster == ARCH ? pMob[idx].MOB.CurrentScore.Level : pMob[idx].MOB.CurrentScore.Level + MAX_LEVEL;
//				int connlevel = pMob[conn].Extra.ClassMaster == MORTAL || pMob[idx].Extra.ClassMaster == ARCH ? pMob[conn].MOB.CurrentScore.Level : pMob[conn].MOB.CurrentScore.Level + MAX_LEVEL;
//
//				int difLevel = (idxlevel - connlevel) / 2;
//
//				difLevel = -difLevel;
//
//				if (affectresist >= 1 && affectresist <= 4)
//				{
//					int random = rand() % 100;
//
//					if (random > pMob[idx].MOB.RegenMP + affectresist + difLevel)
//						SameLeaderGuild = 0;
//				}
//
//				if ((pMob[idx].MOB.Rsv & 0x80) != FALSE)
//					SameLeaderGuild = 0;
//
//				if (conn < MAX_USER && pMob[idx].MOB.Clan == 6)
//					SameLeaderGuild = 0;
//			}
//
//#pragma region Time
//			if (SameLeaderGuild)
//			{
//				if (skillnum == 102)
//				{
//					if (pMob[conn].Extra.ClassMaster == MORTAL)
//						Delay = 200;
//
//					else if (pMob[conn].Extra.ClassMaster != MORTAL && pMob[conn].Extra.ClassMaster != ARCH)
//					{
//						Delay = 200;
//
//						if (pMob[conn].MOB.CurrentScore.Level < 39)
//							Delay -= (39 - (pMob[conn].MOB.CurrentScore.Level)) * 4;
//
//						else if (pMob[conn].MOB.CurrentScore.Level >= 199)
//							Delay = 400;
//
//						else
//							Delay = 200;
//					}
//				}
//
//				if (SetAffect(idx, skillnum, Delay, Special) != 0)
//					UNK2 = TRUE;
//
//				if (SetTick(idx, skillnum, Delay, Special) != 0)
//					UNK2 = TRUE;
//
//				if (UNK2 != FALSE)
//				{
//					pMob[idx].GetCurrentScore(idx);
//					SendScore(idx);
//				}
//			}
//#pragma endregion
//#pragma endregion
//#pragma region Transformações
//			if (skillnum == 64 || skillnum == 66 || skillnum == 68 || skillnum == 70 || skillnum == 71)
//			{
//				pMob[conn].GetCurrentScore(conn);
//				SendScore(conn);
//				SendEquip(conn, 0);
//			}
//#pragma endregion
//#pragma endregion
//
//		}
//#pragma endregion
//
//		else
//		{
//			MyLog(LogType::Attack, pUser[conn].AccountName, "err,dam not -2 -1 MSG_ATTACK", 0, pUser[conn].IP);
//			return;
//		}
//
//		m->Dam[i].Damage = (int)dam;
//
//		if (dam <= 0)
//			continue;
//
//#pragma region Perfuração		   
//		if (idx < MAX_USER || pMob[idx].MOB.Clan == 4)
//		{
//			if ((m->DoubleCritical & 4) != 0)
//				dam = m->Dam[1].Damage + (dam >> 2);
//
//			else
//				dam = dam >> 2;
//		}
//
//		if (pMob[conn].ForceDamage != 0)
//		{
//			if (dam <= 1)
//				dam = pMob[conn].ForceDamage;
//
//			else if (dam > 0)
//				dam = dam + pMob[conn].ForceDamage;
//
//			m->Dam[i].Damage = (int)dam;
//		}
//#pragma endregion
//#pragma region Ataque PvP
//		if (pMob[conn].PvPDamage != 0 && idx < MAX_USER)
//		{
//			if (dam <= 1)
//				dam += (dam * pMob[conn].PvPDamage / 100);
//
//			else if (dam > 0)
//				dam += (dam / 100 * pMob[conn].PvPDamage);
//
//			m->Dam[i].Damage = (int)dam;
//		}
//#pragma endregion
//#pragma region PK
//		if (leader == mobleader || Guild == MobGuild)
//			dam = 0;
//
//		if (idx >= MAX_USER && isFrag != FALSE)
//			dam = 0;
//
//		int Summoner = idx;
//
//		if (idx >= MAX_USER && pMob[idx].MOB.Clan == 4 && pMob[idx].Summoner > 0 && pMob[idx].Summoner < MAX_USER && pUser[pMob[idx].Summoner].Mode == USER_PLAY)
//			Summoner = pMob[idx].Summoner;
//
//		if (Summoner < MAX_USER)
//		{
//			int pointPK = GetPKPoint(conn);
//			int SummonerPointPK = GetPKPoint(Summoner);
//
//			int arena = BASE_GetArena(pMob[conn].TargetX, pMob[conn].TargetY);
//			int village = BASE_GetVillage(pMob[conn].TargetX, pMob[conn].TargetY);
//
//			int mapX = pMob[conn].TargetX >> 7;
//			int mapY = pMob[conn].TargetY >> 7;
//
//			int MapPK = 0;
//			int isWar = 0;
//
//			int connGuild = pMob[conn].MOB.Guild;
//			int SummonerGuild = pMob[Summoner].MOB.Guild;
//			int maxguild = 65536;
//
//			if (connGuild > 0 && connGuild < maxguild && SummonerGuild > 0 && SummonerGuild < maxguild && g_pGuildWar[connGuild] == SummonerGuild && g_pGuildWar[SummonerGuild] == connGuild)
//				isWar = 1;
//
//			if (CastleState != 0 && mapX == 8 && mapY == 13)
//				isWar = 1;
//
//			if (mapX == 1 && mapY == 31)
//				MapPK = 1;
//
//
//			if (arena == MAX_GUILDZONE && village == MAX_GUILDZONE && MapPK == FALSE && isWar == FALSE)
//			{
//				if (pointPK <= 10 && SummonerPointPK > 10)
//				{
//					snprintf(temp, sizeof(temp), g_pMessageStringTable[_DN_CantKillUser], pointPK);
//					SendClientMsg(conn, temp);
//					dam = 0;
//				}
//				else if (SummonerPointPK > 10 && dam > 0 || skillnum == 6)
//				{
//					int Guilty = GetGuilty(conn);
//					SetGuilty(conn, 8);
//
//					if (Guilty == 0)
//					{
//						MSG_CreateMob mob;
//						GetCreateMob(conn, &mob);
//						GridMulticast(pMob[conn].TargetX, pMob[conn].TargetY, (MSG_STANDARD*)&mob, 0);
//					}
//				}
//			}
//#pragma endregion
//#pragma region Miss
//			if (idx != this->conn)
//			{
//				int parryretn = YParryRate(&pMob[conn].MOB, pMob[idx].MOB.CurrentScore.Dex, pMob[idx].MOB.Rsv, pMob[idx].Parry);
//
//				if (skillnum == 79 || skillnum == 22)
//					parryretn = 30 * parryretn / 100;
//
//				int rd = rand() % 1000 + 1;
//
//				if (rd < parryretn)
//				{
//					dam = -3;
//
//					if ((pMob[idx].MOB.Rsv & 0x200) != 0 && rd < 100)
//						dam = -4;
//				}
//			}
//#pragma endregion
//		}
//
//		if (pMob[idx].MOB.Clan == 6 && Level < MAX_LEVEL)
//			dam = 0;
//
//		if (idx > MAX_USER && dam >= 1)
//			dam += pMob[conn].ForceMobDamage;
//
//		if (dam >= MAX_DAMAGE)
//			dam = MAX_DAMAGE;
//
//		m->Dam[i].Damage = (int)dam;
//
//		if (dam <= 0)
//			continue;
//
//		if ((pMob[conn].MOB.Rsv & RSV_FROST) != 0)
//		{
//			if (rand() % 2 == 0)
//			{
//				int skind2 = pMob[conn].MOB.CurrentScore.Special[1];
//
//				if (SetAffect(idx, 36, skind2 + 150, skind2) != 0)
//					SendScore(idx);
//
//				if (SetTick(idx, 36, skind2 + 150, skind2) != 0)
//					SendScore(idx);
//			}
//		}
//
//		if ((pMob[conn].MOB.Rsv & RSV_DRAIN) != 0)
//		{
//			if (rand() % 2 == 0)
//			{
//				int skind2 = pMob[conn].MOB.CurrentScore.Special[1];
//
//				if (SetAffect(idx, 40, skind2 + 150, skind2) != 0)
//					SendScore(idx);
//
//				if (SetTick(idx, 40, skind2 + 150, skind2) != 0)
//					SendScore(idx);
//			}
//		}
//
//		if (idx > 0 && idx < MAX_USER && dam > 0)
//		{
//			if (pMob[idx].ReflectDamage > 0)
//				dam = dam - pMob[idx].ReflectDamage;
//
//			if (dam <= 0)
//				dam = 1;
//
//
//			if (pMob[idx].ReflectPvP > 0)
//				dam = dam - (dam / 100 * pMob[idx].ReflectPvP);
//
//			if (dam <= 0)
//				dam = 1;
//
//			m->Dam[i].Damage = (int)dam;
//		}
//
//		if (pMob[conn].TargetX >= 1017 && pMob[conn].TargetX <= 1290 && pMob[conn].TargetY >= 1911 && pMob[conn].TargetY <= 2183 && dam > 0 && pMob[conn].MOB.Clan == pMob[idx].MOB.Clan)
//		{
//			m->Dam[i].Damage = 0;
//			m->Dam[i].TargetID = 0;
//
//			continue;
//		}
//
//		if (idx < MAX_USER && conn < MAX_USER)
//		{
//			if (pMob[idx].TargetX >= AREA_RVR_MINX && pMob[idx].TargetX <= AREA_RVR_MAXX &&
//				pMob[idx].TargetY >= AREA_RVR_MINY && pMob[idx].TargetY <= AREA_RVR_MAXY && g_pRvrWar.Status == 1)
//			{
//				if (pMob[idx].MOB.Clan == pMob[conn].MOB.Clan)
//				{
//					dam = 0;
//					m->Dam[i].TargetID = 0;
//					m->Dam[i].Damage = 0;
//				}
//			}
//		}
//
//		int _pDamage = (int)dam;
//		int _calcDamage = 0;
//		int pTargetMountId = pMob[idx].MOB.Equip[14].sIndex;
//
//		if (idx < MAX_USER && pTargetMountId >= 2360 && pTargetMountId < 2390 && pMob[idx].MOB.Equip[14].stEffect[0].sValue > 0)
//		{
//
//			_pDamage = (int)((dam * 3) >> 2);
//			_calcDamage = (int)(dam - _pDamage);
//			if (_pDamage <= 0)
//				_pDamage = 1;
//
//			m->Dam[i].Damage = _pDamage;
//		}
//
//#pragma region Controle de Mana
//		int tDamage = _pDamage;
//
//		for (int c = 0; c < MAX_AFFECT; c++)
//		{
//			if (pMob[idx].Affect[c].Type == 18)
//			{
//				if (pMob[idx].MOB.CurrentScore.Mp > ((pMob[idx].MOB.CurrentScore.MaxMp / 100) * 20))
//				{
//					int mana = pMob[idx].MOB.CurrentScore.Mp - tDamage;
//
//					if (mana < 0)
//					{
//						tDamage -= mana;
//						mana = 0;
//					}
//
//					pMob[idx].MOB.CurrentScore.Mp = mana;
//
//					SendSetHpMp(idx);
//
//					int abs = (1 << 23) & pMob[conn].MOB.LearnedSkill ? 55 : 60;
//
//					tDamage = ((tDamage >> 1) + (tDamage << 4)) / abs;
//					m->Dam[i].Damage = tDamage;
//
//					if ((DoubleCritical & 4) != FALSE)
//						m->Dam[1].Damage = tDamage;
//					break;
//				}
//			}
//		}
//#pragma endregion
//
//		if (!strcmp(pMob[idx].MOB.MobName, "Zard"))
//		{
//			pMob[conn].MOB.Equip[12].EF1 = 51;
//			pMob[conn].MOB.Equip[12].EFV1 += 1;
//
//			SendClientSignalParm(conn, ESCENE_FIELD, _MSG_MobLeft, pMob[conn].MOB.Equip[12].EFV1);
//
//			if (pMob[conn].MOB.Equip[12].EFV1 == 80)
//			{
//				STRUCT_ITEM Runa;
//				memset(&Runa, 0, sizeof(STRUCT_ITEM));
//
//				Runa.sIndex = 475;
//				PutItem(conn, &Runa);
//				pMob[conn].MOB.Equip[12].EFV1 = 0;
//			}
//
//			dam = 0;
//		}
//
//		if (pMob[conn].MOB.Class == 1 || pMob[conn].MOB.Class == 3)
//		{
//			bool isFlag = false;
//
//			//Coveiro
//			if (pMob[conn].TargetX >= 2379 && pMob[conn].TargetY >= 2076 && pMob[conn].TargetX <= 2426 && pMob[conn].TargetY <= 2133)
//				isFlag = true;
//
//			//Jardin
//			if (pMob[conn].TargetX > 2228 && pMob[conn].TargetY > 1700 && pMob[conn].TargetX < 2257 && pMob[conn].TargetY < 1728)
//				isFlag = true;
//
//			//Kaizen
//			if (pMob[conn].TargetX > 459 && pMob[conn].TargetY > 3887 && pMob[conn].TargetX < 497 && pMob[conn].TargetY < 3916)
//				isFlag = true;
//
//			//Hidra
//			if (pMob[conn].TargetX > 658 && pMob[conn].TargetY > 3728 && pMob[conn].TargetX < 703 && pMob[conn].TargetY < 3762)
//				isFlag = true;
//
//			//Elfos
//			if (pMob[conn].TargetX > 1312 && pMob[conn].TargetY > 4027 && pMob[conn].TargetX < 1348 && pMob[conn].TargetY < 4055)
//				isFlag = true;
//
//			if (isFlag == true)
//			{
//				if (g_pItemList[pMob[conn].MOB.Equip[6].sIndex].nUnique == 42)
//				{
//					for (i = 0; i < pMob[conn].MaxCarry; i++)
//					{
//						if (pMob[conn].MOB.Carry[i].sIndex == 5572)
//							break;
//					}
//
//					if (i != pMob[conn].MaxCarry)
//					{
//						int amount = BASE_GetItemAmount(&pMob[conn].MOB.Carry[i]);
//
//						if (amount > 1)
//							BASE_SetItemAmount(&pMob[conn].MOB.Carry[i], amount - 1);
//						else
//							BASE_ClearItem(&pMob[conn].MOB.Carry[i]);
//
//						SendItem(conn, ITEM_PLACE_CARRY, i, &pMob[conn].MOB.Carry[i]);
//					}
//
//					else
//					{
//						SendClientMsg(conn, "Não possui flexas suficientes.");
//						return;
//					}
//				}
//			}
//		}
//
//
//		if (pMob[idx].MOB.Equip[13].sIndex == 786)
//		{
//
//			int itemSanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//			if (itemSanc < 2)
//				itemSanc = 2;
//
//
//			tDamage = _pDamage / itemSanc;
//			if (tDamage > pMob[idx].MOB.CurrentScore.Hp)
//				tDamage = pMob[idx].MOB.CurrentScore.Hp;
//
//			pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - tDamage;
//		}
//
//		else if (pMob[idx].MOB.Equip[13].sIndex == 1936)//10X HP MONSTRO
//		{
//
//			int itemSanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//			if (itemSanc < 2)
//				itemSanc = 2;
//
//			itemSanc *= 10;
//
//			tDamage = _pDamage / itemSanc;
//
//			if (tDamage > pMob[idx].MOB.CurrentScore.Hp)
//				tDamage = pMob[idx].MOB.CurrentScore.Hp;
//
//			pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - tDamage;
//		}
//
//		else if (pMob[idx].MOB.Equip[13].sIndex == 1937)//20X HP MONSTRO
//		{
//			int itemSanc = pMob[idx].MOB.Equip[13].stEffect[0].cValue;
//			if (itemSanc < 2)
//				itemSanc = 2;
//
//			itemSanc *= 1000;
//
//			tDamage = _pDamage / itemSanc;
//
//			if (tDamage > pMob[idx].MOB.CurrentScore.Hp)
//				tDamage = pMob[idx].MOB.CurrentScore.Hp;
//
//			pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - tDamage;
//		}
//
//		else
//		{
//			if (tDamage > pMob[idx].MOB.CurrentScore.Hp)
//				tDamage = pMob[idx].MOB.CurrentScore.Hp;
//
//
//			pMob[idx].MOB.CurrentScore.Hp = pMob[idx].MOB.CurrentScore.Hp - tDamage;
//		}
//
//		if (_calcDamage > 0)
//			ProcessAdultMount(idx, _calcDamage / 2);
//
//		if (LOCALSERVER != 0)
//		{
//			snprintf(temp, sizeof(temp), "%s: %d(%d) - %d", pMob[idx].MOB.MobName, _pDamage, tDamage, pMob[idx].MOB.CurrentScore.Hp);
//			SendSay(idx, temp);
//		}
//
//		if (idx > 0 && idx < MAX_USER)
//		{
//			SetReqHp(idx);
//		}
//		else if (pMob[idx].MOB.CurrentScore.Hp > MAX_HP)
//			SendScore(idx);// atualiza o hp dos mob com hp muito alto
//
//		if (idx > MAX_USER && pMob[idx].MOB.Clan == 4)
//			LinkMountHp(idx);
//
//		//#pragma region Joia Abs
//		//		if (pMob[conn].HpAbs != 0 && (rand() % 2) == 0 && dam >= 1) // desativado até acertar
//		//		{
//		//			int RecHP = (int)((dam * pMob[conn].HpAbs + 1) / 100);
//		//
//		//			if (RecHP > 350)
//		//				RecHP = 350;
//		//		}
//		//#pragma endregion
//
//				/*if (skillnum == 79)
//				{
//					SendSetHpMp(idx);
//				}*/
//		if (pMob[idx].MOB.CurrentScore.Hp <= 0)
//		{
//			pMob[idx].MOB.CurrentScore.Hp = 0;
//			TargetKilled[i] = idx;
//			continue;
//		}
//
//		if (pMob[idx].Mode != MOB_EMPTY && pMob[idx].MOB.CurrentScore.Hp > 0)
//		{
//			SetBattle(idx, conn);
//
//			pMob[conn].CurrentTarget = idx;
//
//			int mleader = pMob[conn].Leader;
//			if (mleader <= MOB_EMPTY)
//				mleader = conn;
//
//			int r = 0;
//			for (r = MOB_EMPTY; r < MAX_PARTY; r++)
//			{
//				int partyMemberId = pMob[mleader].PartyList[r];
//				if (partyMemberId <= MAX_USER)
//					continue;
//
//				if (pMob[partyMemberId].Mode == MOB_EMPTY || pMob[partyMemberId].MOB.CurrentScore.Hp <= 0)
//				{
//					if (pMob[partyMemberId].Mode != MOB_EMPTY)
//						DeleteMob(partyMemberId, 1);
//
//					pMob[mleader].PartyList[r] = MOB_EMPTY;
//				}
//				else
//					SetBattle(partyMemberId, idx);
//			}
//
//
//			mleader = pMob[idx].Leader;
//			if (mleader <= 0)
//				mleader = idx;
//
//			for (r = 0; r < MAX_PARTY; r++)
//			{
//				int ptMemberId = pMob[mleader].PartyList[r];
//				if (ptMemberId <= MAX_USER)
//					continue;
//
//				if (pMob[ptMemberId].Mode == MOB_EMPTY || pMob[ptMemberId].MOB.CurrentScore.Hp <= 0)
//				{
//					if (pMob[ptMemberId].Mode != MOB_EMPTY)
//						DeleteMob(ptMemberId, 1);
//
//					pMob[mleader].PartyList[r] = MOB_EMPTY;
//				}
//				else
//					SetBattle(ptMemberId, conn);
//			}
//		}
//
//	}
//#pragma endregion
//
//	if (UNK2 > 0)
//		Exp = UNK2;
//
//	if (DOUBLEMODE)
//		Exp = Exp * 2;
//
//	if (Exp <= 0)
//		Exp = 0;
//
//	if (Exp > 200)
//		Exp = 200;
//
//	//Espectral
//	if ((pMob[conn].MOB.LearnedSkill & (1 << 29)) != 0)
//		m->DoubleCritical = m->DoubleCritical | 8;
//
//	m->CurrentExp = pMob[conn].MOB.Exp;
//	m->CurrentHp = pMob[conn].MOB.CurrentScore.Hp;
//	m->CurrentMp = pMob[conn].MOB.CurrentScore.Mp;
//	m->ReqMp = m->CurrentMp;
//
//	if (ClientTick == SKIPCHECKTICK)
//		m->ClientTick = CurrentTime;
//
//	if (skillnum != 102)
//		GridMulticast(pMob[conn].TargetX, pMob[conn].TargetY, (MSG_STANDARD*)m, 0);
//
//	if (skillnum == 30)
//		SendSetHpMp(conn);
//
//	if (skillnum == 79 && alvo < MAX_USER) // pra nao bugar a 8 de ht
//		SendHpMp(alvo);
//
//#pragma region MobKilled Check
//	for (int i = 0; i < MAX_TARGET; i++)
//	{
//		if (TargetKilled[i] > 0 && TargetKilled[i] < MAX_MOB && pMob[TargetKilled[i]].Mode != MOB_EMPTY)
//			MobKilled(TargetKilled[i], conn, 0, 0);
//	}
//#pragma endregion
//#pragma region Level Check
//
//	int Segment = pMob[conn].CheckGetLevel();
//
//	if (Segment >= 1 && Segment <= 4)
//	{
//		if (Segment == 4)
//		{
//			SendClientMsg(conn, g_pMessageStringTable[_NN_Level_Up]);
//			SetCircletSubGod(conn);
//
//			if (pMob[conn].Extra.ClassMaster == MORTAL)
//				DoItemLevel(conn);
//		}
//
//		if (Segment == 3)
//			SendClientMsg(conn, g_pMessageStringTable[_NN_3_Quarters_Bonus]);
//
//		if (Segment == 2)
//			SendClientMsg(conn, g_pMessageStringTable[_NN_2_Quarters_Bonus]);
//
//		if (Segment == 1)
//			SendClientMsg(conn, g_pMessageStringTable[_NN_1_Quarters_Bonus]);
//
//		SendScore(conn);
//		SendEmotion(conn, 14, 3);
//
//		if (Segment == 4)
//		{
//			SendEtc(conn);
//
//			int PKPoint = GetPKPoint(conn) + 5;
//			SetPKPoint(conn, PKPoint);
//
//			MSG_CreateMob sm_lup;
//			memset(&sm_lup, 0, sizeof(MSG_CreateMob));
//			GetCreateMob(conn, &sm_lup);
//
//			GridMulticast(pMob[conn].TargetX, pMob[conn].TargetY, (MSG_STANDARD*)&sm_lup, 0);
//
//			snprintf(temp, sizeof(temp), "lvl %s level up to %d", pMob[conn].MOB.MobName, pMob[conn].MOB.BaseScore.Level);
//			MyLog(LogType::Attack, pUser[conn].AccountName, temp, 0, pUser[conn].IP);
//
//		}
//	}
//#pragma endregion
//
//}
