#include "pch.h"

CGet cGet = CGet();

int16 CGet::GetItemHab(short item, short Type)
{
	for (int i = 0; i < MAX_STATICEFFECT; i++)
	{
		if (g_pItemList[item].stEffect[i].sEffect == Type)
			return g_pItemList[item].stEffect[i].sValue;
	}

	return FALSE;
}

int16 CGet::GetSlot(CMob* mob, short itemID)
{
	for (int i = 0; i < mob->MaxCarry; i++)
	{
		if (mob->MOB.Carry[i].sIndex == itemID)
			return i;
	}
	return -1;
}

bool CGet::GetFreeSlot(CMob* mob, uint8 quant)
{
	uint8 slots = 0;

	for (int i = 0; i < mob->MaxCarry; i++)
	{
		if (mob->MOB.Carry[i].sIndex == 0)
			slots++;

		if (quant == slots)
			return true;
	}

	return false;
}

BOOL CGet::GetEmptyMobGrid(int mob, int* tx, int* ty)
{
	if (*tx < 0 || *ty < 0 || *tx >= MAX_GRIDX || *ty >= MAX_GRIDY)
	{
		cLog.Write(0, TL::Err, "-system GetEmptyMobGridOut of range");
		return FALSE;
	}

	if (cSrv.pMobGrid[*ty][*tx] == mob)
		return TRUE;

	if (cSrv.pMobGrid[*ty][*tx] == 0 && cSrv.pHeightGrid[*ty][*tx] != 127)
		return TRUE;

	int y = 0;
	int x = 0;
	int z = 0;

	for (z = TRUE; z < MAX_SERVER; z++)
	{
		for (y = (*ty) - z; y <= (*ty) + z; y++)
		{
			for (x = (*tx) - z; x <= (*tx) + z; x++)
			{
				if (x < 0 || y < 0 || x >= MAX_GRIDX || y >= MAX_GRIDY)
					continue;

				if (cSrv.pMobGrid[y][x] != 0)
					continue;

				if (cSrv.pHeightGrid[y][x] == 127)
					continue;

				*ty = y;
				*tx = x;

				return TRUE;
			}
		}
	}

	return FALSE;
}

int CGet::GetCurKill(int conn)
{
	auto mob = cSrv.getMob(conn);
	if (!mob)
		return 0;

	unsigned char cv = mob->MOB.Carry[KILL_MARK].stEffect[0].cValue;

	int ret = cv;

	return ret;
}

int CGet::GetTotKill(int conn)
{
	auto mob = cSrv.getMob(conn);
	if (!mob)
		return 0;

	unsigned char cv1 = mob->MOB.Carry[KILL_MARK].stEffect[1].cValue;
	unsigned char cv2 = mob->MOB.Carry[KILL_MARK].stEffect[2].cValue;

	int ret = cv1 + cv2 * 256;

	return ret;
}

int	CGet::GetPKPoint(int conn)
{
	auto mob = cSrv.getMob(conn);
	if (!mob)
		return 0;

	unsigned char cv = mob->MOB.Carry[KILL_MARK].stEffect[0].cEffect;

	int ret = cv;

	return ret;
}

int CGet::GetGuilty(int conn)
{
	auto mob = cSrv.getMob(conn);
	if (!mob)
		return 0;

	unsigned char cv = mob->MOB.Carry[KILL_MARK].stEffect[1].cEffect;

	int ret = cv;

	if (ret > 50)
	{
		mob->MOB.Carry[KILL_MARK].stEffect[1].cEffect = 0;
		ret = 0;
	}

	return ret;
}

void CGet::GetAffect(uint16* out, STRUCT_AFFECT* affect)
{
	for (int i = 0; i < MAX_AFFECT; i++)
	{
		int type = affect[i].Type;
		int value = affect[i].Time;

		value = value;

		if (value > 2550000)
			value = 2550000;

		unsigned short tout = (type << 8) + (value & 0xFF);

		out[i] = tout;
	}
}

int CGet::GetCreateMob(int id, MSG_CreateMob* sm)
{
	memset(sm, 0, sizeof(MSG_CreateMob));

	auto mob = cSrv.getMob(id);
	if (!mob)
		return 0;
	
	sm->Type = _MSG_CreateMob;

	strncpy(sm->MobName, mob->MOB.MobName, NAME_LENGTH);

	if (id < MAX_USER)
	{
		int ck = this->GetCurKill(id);
		int tk = this->GetTotKill(id);

		unsigned char cv = ck;
		unsigned char cv1 = tk % 256;
		unsigned char cv2 = tk / 256;

		sm->MobName[13] = cv;
		sm->MobName[14] = cv1;
		sm->MobName[15] = cv2;

		int chaos = this->GetPKPoint(id);
		int gv = this->GetGuilty(id);

		if (gv > 0)
			chaos = 0;

		cv = chaos;
		sm->MobName[12] = chaos;
	}

	sm->PosX = mob->TargetX;
	sm->PosY = mob->TargetY;
	sm->Size = sizeof(MSG_CreateMob);
	sm->ID = ESCENE_FIELD;
	sm->MobID = id;
	sm->ClientTick = CurrentTime;

	sm->Score = mob->MOB.CurrentScore;

	sm->Guild = mob->MOB.Guild;
	sm->GuildMemberType = mob->MOB.GuildLevel;

	if (mob->GuildDisable == 1)
	{
		sm->Guild = 0;
		sm->GuildMemberType = 0;
	}
	if (id >= MAX_USER)
	{
		if (mob->MOB.Clan == 4)
			sm->Score.Ac = 0;

		else
			sm->Score.Ac = 1;
	}

	sm->CreateType = 0;

	if (mob->MOB.GuildLevel == 9)
		sm->CreateType = sm->CreateType | 0x80;

	else if (mob->MOB.GuildLevel >= 6)
		sm->CreateType = sm->CreateType | 0x40;

	sm->Hold = mob->Extra.Hold;

	strncpy(sm->Tab, mob->Tab, 26);

	int selfdead = 0;

	int xx = mob->TargetX;
	int yy = mob->TargetY;

	for (int i = 0; i < MAX_EQUIP; i++)
	{
		STRUCT_ITEM* item = &mob->MOB.Equip[i];

		sm->Equip[i] = BASE_VisualItemCode(item, i);

		sm->AnctCode[i] = BASE_VisualAnctCode(item);

		if (i == 14 && sm->Equip[i] >= 2360 && sm->Equip[i] < 2390 && mob->MOB.Equip[i].stEffect[0].sValue <= 0)
		{
			sm->Equip[i] = 0;
			selfdead = 1;
			continue;
		}

		if (i == 14 && sm->Equip[i] >= 2360 && sm->Equip[i] < 2390)
		{
			int sanc = mob->MOB.Equip[i].stEffect[1].cEffect;
			sanc = sanc / 10;

			if (sanc > 13)
				sanc = 13;

			if (sanc < 0)
				sanc = 0;

			sanc = sanc << 12;

			sm->Equip[i] = sm->Equip[i] + sanc;
			continue;
		}
	}

	if (strcmp((char*)sm->MobName, "Carbuncle Wind") == 0) snprintf(sm->Tab, sizeof(sm->Tab), "Ajudante para iniciantes");
	else if (strcmp((char*)sm->MobName, "Argus") == 0) snprintf(sm->Tab, sizeof(sm->Tab), "Loja Fame Guild");

	this->GetAffect(sm->Affect, mob->Affect);

	//if (BrState != 0 && xx >= 2604 && yy >= 1708 && xx <= 2648 && yy <= 1744)
	//{
	//	memset(sm->MobName, 0, NAME_LENGTH); // MOBNAME_LEGNTH = 16
	//	strncpy(sm->MobName, "??????", 6);

	//	sm->Equip[15] = 0;
	//	sm->Guild = 0;
	//}

	return selfdead;
}

int CGet::GetCreateMobTrade(int id, MSG_CreateMobTrade* sm)
{
	sm->Type = _MSG_CreateMobTrade;

	auto mob = cSrv.getMob(id);
	if (!mob)
		return 0;

	strncpy(sm->MobName, mob->MOB.MobName, NAME_LENGTH);

	if (id >= MAX_USER)
	{
		cLog.Write(0, TL::Err, std::format("err,getcreatemob request by non player {}", mob->MOB.MobName));
		return 0;
	}

	if (id < MAX_USER)
	{
		int ck = this->GetCurKill(id);
		int tk = this->GetTotKill(id);

		unsigned char cv = ck;
		unsigned char cv1 = tk % 256;
		unsigned char cv2 = tk / 256;

		sm->MobName[13] = cv;
		sm->MobName[14] = cv1;
		sm->MobName[15] = cv2;

		int chaos = this->GetPKPoint(id);

		cv = chaos;
		sm->MobName[12] = cv;
	}

	sm->PosX = mob->TargetX;
	sm->PosY = mob->TargetY;
	sm->Size = sizeof(MSG_CreateMobTrade);
	sm->ID = ESCENE_FIELD;
	sm->MobID = id;
	sm->ClientTick = CurrentTime;

	memcpy(&sm->Score, &mob->MOB.CurrentScore, sizeof(STRUCT_SCORE));

	sm->Guild = mob->MOB.Guild;

	if (mob->GuildDisable == 1)
		sm->Guild = 0;

	if (id >= MAX_USER)
		sm->Score.Ac = mob->MOB.Clan != 4;

	sm->CreateType = 0;

	if (mob->MOB.GuildLevel == 9)
		sm->CreateType = sm->CreateType | 0x80;

	if (mob->MOB.GuildLevel != 0)
		sm->CreateType = sm->CreateType | 0x40;

	int selfdead = 0;

	for (int i = 0; i < MAX_EQUIP; i++)
	{
		STRUCT_ITEM* item = &mob->MOB.Equip[i];

		sm->Equip[i] = BASE_VisualItemCode(item, i);
		sm->AnctCode[i] = BASE_VisualAnctCode(item);

		if (i == 14 && sm->Equip[i] >= 2360 && sm->Equip[i] < 2390 && mob->MOB.Equip[i].stEffect[0].sValue <= 0)
		{
			sm->Equip[i] = 0;
			selfdead = 1;
			continue;
		}

		if (i == 14 && sm->Equip[i] >= 2360 && sm->Equip[i] < 2390)
		{
			int sanc = mob->MOB.Equip[i].stEffect[1].cEffect;
			sanc = sanc / 10;

			if (sanc > 13)
				sanc = 13;

			if (sanc < 0)
				sanc = 0;

			sanc = sanc << 12;

			sm->Equip[i] = sm->Equip[i] + sanc;
			continue;
		}
	}

	auto user = cSrv.getUser(id);
	if (!user)
		return 0;

	strncpy(sm->Tab, mob->Tab, 26);
	strncpy(sm->Desc, user->AutoTrade.Title, MAX_AUTOTRADETITLE - 1);

	this->GetAffect(sm->Affect, mob->Affect);

	return selfdead;
}

void CGet::GetCreateItem(int idx, MSG_CreateItem* sm)
{
	sm->ID = ESCENE_FIELD;
	sm->Size = sizeof(MSG_CreateItem);
	sm->Type = _MSG_CreateItem;

	sm->GridX = cSrv.pItem[idx].PosX;
	sm->GridY = cSrv.pItem[idx].PosY;

	sm->ItemID = idx + 10000;

	sm->Rotate = cSrv.pItem[idx].Rotate;

	sm->Item = cSrv.pItem[idx].ITEM;

	sm->State = cSrv.pItem[idx].State;

	sm->Height = -204;

	if (cSrv.pItem[idx].ITEM.sIndex == 3145)
	{
		int zone = BASE_GetVillage(cSrv.pItem[idx].PosX, cSrv.pItem[idx].PosY);
		sm->Item.sIndex = 3145 + g_pGuildZone[zone].Victory;

		sm->State = STATE_CLOSED;

		sm->Item.stEffect[0].cEffect = 56;
		sm->Item.stEffect[1].cEffect = 57;

		sm->Item.stEffect[0].cValue = (g_pGuildZone[zone].ChargeGuild + (cSrv.ServerIndex << 12)) >> 8;
		sm->Item.stEffect[1].cValue = g_pGuildZone[zone].ChargeGuild;
	}

	if (cSrv.pItem[idx].ITEM.sIndex == 5700)
		return;

	if (sm->State == STATE_OPEN)
	{
		sm->Height = cSrv.pItem[idx].Height;
		sm->Create = 0;
	}
}

void CGet::GetAction(int id, int targetx, int targety, MSG_Action* sm)
{
	if (auto mob = cSrv.getMob(id))
	{
		sm->ID = id;
		sm->ClientTick = CurrentTime;
		sm->Size = sizeof(MSG_Action);
		sm->Type = _MSG_Action;

		sm->TargetX = targetx;
		sm->TargetY = targety;
		sm->Route[0] = 0;

		sm->Speed = 2;
		sm->Effect = 1;
		sm->PosX = mob->TargetX;
		sm->PosY = mob->TargetY;
	}
}

unsigned char CGet::GetAttribute(int x, int y)
{
	if (x < 0 || x > MAX_GRIDX || y < 0 || y > MAX_GRIDY)
		return 0;

	int xx = x / 4;

	xx = xx & 0x3FF;

	int yy = y / 4;

	yy = yy & 0x3FF;

	unsigned char ret = 0;

	if (xx >= 0 && xx < 1024 && yy >= 0 && yy < 1024)
		ret = g_pAttribute[yy][xx];

	return ret;
}

int16 CGet::GetEmptyAffect(int id, uint8 type)
{
	if (auto mob = cSrv.getMob(id))
	{
		for (int i = 0; i < MAX_AFFECT; i++)
		{
			if (mob->Affect[i].Type == type)
				return i;
		}

		for (int i = 0; i < MAX_AFFECT; i++)
		{
			if (mob->Affect[i].Type == 0)
				return i;
		}
	}

	return -1;
}

bool CGet::GetEmptyItemGrid(int* gridx, int* gridy)
{
	if ((cSrv.pItemGrid[*gridy][*gridx] == 0) && (cSrv.pHeightGrid[*gridy][*gridx] != 127))
		return true;

	for (int y = (*gridy) - 1; y <= (*gridy) + 1; y++)
	{
		for (int x = (*gridx) - 1; x <= (*gridx) + 1; x++)
		{
			if (x < 0 || y < 0 || x >= MAX_GRIDX || y >= MAX_GRIDY)
				continue;

			if (cSrv.pItemGrid[y][x] == 0 && cSrv.pHeightGrid[y][x] != 127)
			{
				*gridy = y;
				*gridx = x;
				return true;
			}
		}
	}

	return false;
}

int CGet::GetTeleportPosition(CUser* user, int* x, int* y)
{
	int Charge = 0;
	if (auto mob = cSrv.getMob(user->conn))
	{
		int xv = (*x) & 0xFFFC;
		int yv = (*y) & 0xFFFC;

		if (xv == 2116 && yv == 2100) //Armia para Noatum
		{
			*x = 1044 + rand() % 3;
			*y = 1724 + rand() % 3;

			Charge = 700;
		}
		else if (xv == 2140 && yv == 2068) //Armia para Campo Armia
		{
			*x = 2588 + rand() % 3;
			*y = 2096 + rand() % 3;
		}
		else if (xv == 2480 && yv == 1716) //Arzan para Notaum
		{
			*x = 1044 + rand() % 3;
			*y = 1716 + rand() % 3;

			Charge = 700;
		}
		else if (xv == 2456 && yv == 2016) //Erion para Notaum
		{
			*x = 1044 + rand() % 3;
			*y = 1708 + rand() % 3;

			Charge = 700;
		}
		else if (xv == 1044 && yv == 1724) //Noatum para Armia
		{
			*x = 2116 + rand() % 3;
			*y = 2100 + rand() % 3;
		}
		else if (xv == 1044 && yv == 1716) //Noatum para Arzan
		{
			*x = 2480 + rand() % 3;
			*y = 1716 + rand() % 3;
		}
		else if (xv == 1044 && yv == 1708) //Noatum para Erion
		{
			*x = 2456 + rand() % 3;
			*y = 2016 + rand() % 3;
		}
		else if (xv == 1048 && yv == 1764) // Noatum para Guerra
		{
			*x = 1100 + rand() % 3;
			*y = 1712 + rand() % 3;
		}
		else if (xv == 1052 && yv == 1708) // Noatum para Gelo
		{
			*x = 3650 + rand() % 3;
			*y = 3110 + rand() % 3;
		}
		else if (xv == 2468 && yv == 1716) // Arzan para Campo Arzan
		{
			*x = 2248 + rand() % 3;
			*y = 1556 + rand() % 3;
		}
		else if (xv == 2364 && yv == 2284) //Dungeon para Campo armia 2
		{
			*x = 144 + rand() % 3;
			*y = 3788 + rand() % 3;
		}
		else if (xv == 144 && yv == 3788) // Campo armia para Dungeon 2
		{
			*x = 2364 + rand() % 3;
			*y = 2284 + rand() % 3;
		}
		else if (xv == 2668 && yv == 2156) //Campo armia para Dungeon 1
		{
			*x = 148 + rand() % 2;
			*y = 3774 + rand() % 2;
		}
		else if (xv == 144 && yv == 3772) // Dungeon para Campo armia 1
		{
			*x = 2668 + rand() % 3;
			*y = 2156 + rand() % 3;
		}
		else if (xv == 148 && yv == 3780) // Dungeon 1 para Dungeon 2
		{
			*x = 1004 + rand() % 3;
			*y = 4028 + rand() % 3;
		}
		else if (xv == 744 && yv == 3804) // Dungeon para Dungeon
		{
			*x = 912 + rand() % 3;
			*y = 3811 + rand() % 3;
		}
		else if (xv == 912 && yv == 3808) // Dungeon para Dungeon
		{
			*x = 744 + rand() % 3;
			*y = 3804 + rand() % 3;
		}
		else if (xv == 744 && yv == 3816) //Dungeon para Dungeon
		{
			*x = 1006 + rand() % 3;
			*y = 3993 + rand() % 3;
		}

		else if (xv == 1004 && yv == 4028) //Dungeon para Dungeon
		{
			*x = 148 + rand() % 3;
			*y = 3780 + rand() % 3;
		}
		else if (xv == 408 && yv == 4072) // Dungeon para Dungeon
		{
			*x = 1004 + rand() % 3;
			*y = 4064 + rand() % 3;
		}
		else if (xv == 1004 && yv == 4064) // Dungeon para Dungeon
		{
			*x = 408 + rand() % 3;
			*y = 4072 + rand() % 3;
		}
		else if (xv == 745 && yv == 3816) // Dungeon para Dungeon
		{
			*x = 1004 + rand() % 3;
			*y = 3992 + rand() % 3;
		}
		else if (xv == 1004 && yv == 3992) //Dungeon para Dungeon
		{
			*x = 744 + rand() % 3;
			*y = 3820 + rand() % 3;
		}
		else if (xv == 680 && yv == 4076) // Dungeon 2 para Dungeon 3
		{
			*x = 916 + rand() % 3;
			*y = 3820 + rand() % 3;
		}
		else if (xv == 916 && yv == 3820) //Dungeon 3 para Dungeon 2
		{
			*x = 680 + rand() % 3;
			*y = 4076 + rand() % 3;
		}
		else if (xv == 876 && yv == 3872) // Dungeon 2 para Dungeon 3
		{
			*x = 932 + rand() % 3;
			*y = 3820 + rand() % 3;
		}
		else if (xv == 932 && yv == 3820) // Dungeon 3 para Dungeon 2
		{
			*x = 876 + rand() % 3;
			*y = 3872 + rand() % 3;
		}
		else if (xv == 188 && yv == 188) // War area to Azran
		{
			*x = 2548 + rand() % 3;
			*y = 1740 + rand() % 3;
		}
		else if (xv == 2548 && yv == 1740) // Azran to vale
		{
			if (mob->MOB.Equip[13].sIndex == 3916)
			{
				*x = 2281 + rand() % 3;
				*y = 3688 + rand() % 3;
				Charge = 1000;
			}
		}
		else if (xv == 1824 && yv == 1772) //Submundo para Campo arzan
		{
			*x = 1172 + rand() % 3;
			*y = 4080 + rand() % 3;
		}
		else if (xv == 1172 && yv == 4080) // Submundo para Campo arzan
		{
			*x = 1824 + rand() % 3;
			*y = 1772 + rand() % 3;
		}
		else if (xv == 1516 && yv == 3996) // Submundo para Submundo2
		{
			*x = 1304 + rand() % 3;
			*y = 3816 + rand() % 3;
		}
		else if (xv == 1304 && yv == 3816) // Submundo2 para Submundo
		{
			*x = 1516 + rand() % 3;
			*y = 3996 + rand() % 3;
		}
		else if (xv == 2452 && yv == 1716) // Arzan para Campo Arzan 2
		{
			*x = 1969 + rand() % 3;
			*y = 1711 + rand() % 3;
		}
		else if (xv == 2452 && yv == 1988) // Erion teleport to Azran field
		{
			*x = 1989 + rand() % 3;
			*y = 1755 + rand() % 3;
		}
		else if (xv == 3648 && yv == 3108) //  Gelo para Noatun
		{
			*x = 1054 + rand() % 3;
			*y = 1710 + rand() % 3;
		}
		else if ((xv == 1752 && yv == 1980) || (xv == 1708 && yv == 1980)) // RVR
		{
			*x = 1733 + rand() % 3;
			*y = 1981 + rand() % 3;
		}

		//else if (xv == 1056 && yv == 1724) // Noatum to RvR - Deserto
		//{
		//	if (!g_pRvrWar.Status)
		//	{
		//		SendClientMsg(conn, "Somente às 22:00hrs de segunda à sábado.");
		//		return 0;
		//	}

		//	INT32 cape = pMob[conn].MOB.Clan;
		//	if (cape != 7 && cape != 8)
		//	{
		//		SendClientMsg(conn, "Personagens andarilhos não podem participar.");
		//		return 0;
		//	}

		//	if (cape == 7)
		//	{
		//		*x = 1754 - rand() % 4;
		//		*y = 1971 + rand() % 20;
		//	}
		//	else
		//	{
		//		*x = 1711 + rand() % 4;
		//		*y = 1971 + rand() % 20;
		//	}

		//	SendCounterMob(conn, g_pRvrWar.Points[0], g_pRvrWar.Points[1]);
		//}

		else if (xv == 2364 && yv == 3892 && mob->Extra.KefraTicket > 0) // Kefra para Hall
		{
			mob->Extra.KefraTicket--;
			*x = 2364 + rand() % 3;
			*y = 3906 + rand() % 3;

			user->ClientMsg(std::format(g_pMessageStringTable[_DN_CHANGE_COUNT], mob->Extra.KefraTicket));
		}

		else if (xv == 2364 && yv == 3924 && cSrv.KefraLive != false) //Hall Kefra para Cidade Kefra
		{
			*x = 3250 + rand() % 3;
			*y = 1703 + rand() % 3;
		}

		else if (xv == 1312 && yv == 1900) //Deserto para Kefra
		{
			*x = 2366 + rand() % 2;
			*y = 4073 + rand() % 2;
		}

		else if (xv == 2364 && yv == 4072) //Kefra para Deserto
		{
			*x = 1314 + rand() % 2;
			*y = 1900 + rand() % 2;
		}
	}

	return Charge;
}

int CGet::GetInView(int a, CMob* target)
{
	auto mob = cSrv.getMob(a);
	if (!mob)
		return FALSE;

	if (mob->TargetX < target->TargetX - VIEWGRIDX || mob->TargetX > target->TargetX + VIEWGRIDX ||
		mob->TargetY < target->TargetY - VIEWGRIDY || mob->TargetY > target->TargetY + VIEWGRIDY)
		return FALSE;

	return TRUE;
}