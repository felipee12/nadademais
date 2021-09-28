#include "pch.h"

CLog cLog = CLog();
CServer cSrv = CServer();

uint32 CurrentTime = 0;

CLog::CLog()
{
	this->SysFile = NULL;
	this->WarnFile = NULL;
	this->ErrFile = NULL;
	this->PacketFile = NULL;
	this->ItemFile = NULL;

	this->SysLogDay = -1;
	this->WarnLogDay = -1;
	this->ErrLogDay = -1;
	this->PacketLogDay = -1;
	this->ItemLogDay = -1;
}

CLog::~CLog()
{
}

void CLog::Start()
{
	if (this->SysFile != NULL)
		fclose(this->SysFile);

	if (this->WarnFile != NULL)
		fclose(this->WarnFile);

	if (this->ErrFile != NULL)
		fclose(this->ErrFile);

	if (this->PacketFile != NULL)
		fclose(this->PacketFile);

	auto when = getNow();

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\System\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->SysFile = fopen(cSrv.temp, "wt");
	this->SysLogDay = when.tm_mday;

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\Warning\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->WarnFile = fopen(cSrv.temp, "wt");
	this->WarnLogDay = when.tm_mday;

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\Error\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->ErrFile = fopen(cSrv.temp, "wt");
	this->ErrLogDay = when.tm_mday;

	snprintf(cSrv.temp, sizeof(cSrv.temp), ".\\Log\\Packet\\System_%02d_%02d_%02d_%02d_%02d.txt", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);
	this->PacketFile = fopen(cSrv.temp, "wt");
	this->PacketLogDay = when.tm_mday;
}

void CLog::Write(int conn, TL Type, const std::string& str)
{
	auto when = getNow();

	if (conn != 0)
	{
		if (auto user = cSrv.getUser(conn))
		{
			uint8* cIP = (uint8*)&user->IP;
			snprintf(cSrv.temp, sizeof(cSrv.temp), "[%02d/%02d][%02d:%02d:%02d] IP: %d.%d.%d.%d >> ", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec, cIP[0], cIP[1], cIP[2], cIP[3]);
		}
		else
			strcpy_s(cSrv.temp, "[LOG Warning] - USER RETORNOU NULLO!!! >>>");
	}
	else
		snprintf(cSrv.temp, sizeof(cSrv.temp), "[%02d/%02d][%02d:%02d:%02d] >>", when.tm_mday, when.tm_mon + 1, when.tm_hour, when.tm_min, when.tm_sec);

	snprintf(cSrv.temp, sizeof(cSrv.temp), "%s %s\n", cSrv.temp, str.c_str());

	switch (Type)
	{
	case TL::Sys:
		if (this->SysFile)
			fprintf_s(this->SysFile, cSrv.temp);
		break;
	case TL::Warn:
		if (this->WarnFile)
			fprintf_s(this->WarnFile, cSrv.temp);
		break;
	case TL::Err:
		if (this->ErrFile)
			fprintf_s(this->ErrFile, cSrv.temp);
		break;
	case TL::Pakt:
		if (this->PacketFile)
			fprintf_s(this->PacketFile, cSrv.temp);
		break;
	}

	SetWindowText(hWndMain, cSrv.temp);
}

CServer::CServer()
{
	this->ServerDown = -1000;
	this->DBServerPort = 0;

	this->ServerGroup = -1;
	this->ServerIndex = -1;
	this->NumServerInGroup = 0;
	this->NewbieEventServer = 0;
	this->CurrentWeather = 0;
	this->SecCounter = 0;
	this->MinCounter = 0;
	this->g_EmptyMob = MAX_USER;
	this->g_dwInitItem = 0;

	this->KefraLive = false;

	memset(this->pItemGrid, 0, sizeof(this->pItemGrid));
	memset(this->pMobGrid, 0, sizeof(this->pMobGrid));
	memset(this->pHeightGrid, 0, sizeof(this->pHeightGrid));
	memset(this->LocalIP, 0, sizeof(this->LocalIP));
	memset(this->DBServerAddress, 0, sizeof(this->DBServerAddress));
	memset(this->pMobMerc, 0, sizeof(this->pMobMerc));
	memset(this->GuildImpostoID, 0, sizeof(this->GuildImpostoID));	
}

CServer::~CServer()
{
}

CUser* CServer::getUser(int id)
{
	if (id > 0 && id < MAX_USER)
		return &this->pUser[id];

	return nullptr;
}

CMob* CServer::getMob(int id)
{
	if (id > 0 && id < MAX_MOB)
		return &this->pMob[id];

	return nullptr;
}

void CServer::MountProcess(int conn, STRUCT_ITEM* Mount)
{
	if (Mount)
	{
		STRUCT_ITEM* MItem = &pMob[conn].MOB.Equip[14];

		int IsEqual = 1;

		if (Mount != NULL)
			IsEqual = memcmp(Mount, MItem, sizeof(STRUCT_ITEM));

		if (IsEqual != 0)
			return;

		int leaderid = pMob[conn].Leader;

		if (leaderid == 0)
			leaderid = conn;

		for (int i = 0; i < MAX_PARTY; i++)
		{
			int partyconn = pMob[leaderid].PartyList[i];

			if (partyconn <= 0 || partyconn > MAX_MOB)
				continue;

			int face = pMob[partyconn].MOB.Equip[0].sIndex;

			if (pMob[partyconn].Summoner == conn)
			{
				if (face >= 315 && face < 345)
					this->DeleteMob(partyconn, 3);
			}
		}

		int MBabyId = MItem->sIndex - 2320;

#pragma message("---FAZER QUANDO TIVER TEMPO----- - EVOCAÇÕES DE CRIAS")
		/*if (MBabyId >= 10 && MBabyId < 40)
		{
			int MHp = BASE_GetItemAbility(MItem, EF_MOUNTHP);

			if (MHp > 0)
				GenerateSummon(conn, MBabyId, MItem, 1);
		}*/
	}
}

void CServer::DoTeleport(int id, int x, int y)
{
	MSG_Action sm;
	memset(&sm, 0, sizeof(MSG_Action));

	int ret = cGet.GetEmptyMobGrid(id, &x, &y);

	if (ret)
	{
		cGet.GetAction(id, x, y, &sm);

		sm.Effect = 1; // teleport effect

		if (id < MAX_USER)
			pUser[id].cSock.AddMessage((BYTE*)&sm, sizeof(MSG_Action));

		cSend.GridMulticast(id, x, y, (MSG_HEADER*)&sm);
	}
}

bool CServer::SetAffect(int conn, int skillnum, int time, int Level)
{
	if (pMob[conn].MOB.Merchant == 1 || conn > MAX_USER)
		return false;

	int sAffectType = g_pSpell[skillnum].AffectType;
	int sAggressive = g_pSpell[skillnum].Aggressive;

	if (pMob[conn].MOB.Rsv & RSV_BLOCK && sAggressive)
		return false;

	int EmptyAffect = cGet.GetEmptyAffect(conn, sAffectType);

	if (sAffectType <= 0 || EmptyAffect < 0 || EmptyAffect >= MAX_AFFECT)
		return false;

	int sType = pMob[conn].Affect[EmptyAffect].Type;

	pMob[conn].Affect[EmptyAffect].Type = sAffectType;
	pMob[conn].Affect[EmptyAffect].Value = g_pSpell[skillnum].AffectValue;

	int sTime = (g_pSpell[skillnum].AffectTime + 1) * time / 100;

	if (sType == 1 || sType == 3 || sType == 10)
		sTime = 4;

	if (sType == sAffectType && (unsigned int)sTime > pMob[conn].Affect[EmptyAffect].Time)
		pMob[conn].Affect[EmptyAffect].Time = sTime;

	else
		pMob[conn].Affect[EmptyAffect].Time = sTime;

	if (sType == sAffectType && Level > pMob[conn].Affect[EmptyAffect].Level)
		pMob[conn].Affect[EmptyAffect].Level = Level;

	else
		pMob[conn].Affect[EmptyAffect].Level = Level;

	if (time >= 2139062143)
		pMob[conn].Affect[EmptyAffect].Time = 2139062143;

	pMob[conn].GetCurrentScore(conn);

	if (sType == CANCEL_SPEEL_INDEX) { pMob[conn].Affect[EmptyAffect].Time = 2; }

	return true;
}

bool CServer::SetTick(int mob, int skillnum, int Delay, int Level)
{
	if (pMob[mob].MOB.Merchant == 1 && mob > MAX_USER)
		return false;

	int sTickType = g_pSpell[skillnum].TickType;
	int sAggressive = g_pSpell[skillnum].Aggressive;

	if ((pMob[mob].MOB.Rsv & RSV_BLOCK) && (sAggressive != 0))
		return false;

	int EmptyAffect = cGet.GetEmptyAffect(mob, sTickType);

	if (sTickType > 0 && EmptyAffect >= 0 && EmptyAffect < MAX_AFFECT)
	{
		pMob[mob].Affect[EmptyAffect].Type = sTickType;
		pMob[mob].Affect[EmptyAffect].Value = g_pSpell[skillnum].TickValue;
		pMob[mob].Affect[EmptyAffect].Time = (Delay * (g_pSpell[skillnum].AffectTime + 1)) / 100;

		if (Delay >= 500000000)
			pMob[mob].Affect[EmptyAffect].Time = 500000000;

		if (pMob[mob].Affect[EmptyAffect].Time >= 3 && (sTickType == 1 || sTickType == 3 || sTickType == 10))
			pMob[mob].Affect[EmptyAffect].Time = 2;

		pMob[mob].Affect[EmptyAffect].Level = Level;


		if (sTickType == CANCEL_SPEEL_INDEX) { pMob[mob].Affect[EmptyAffect].Time = 2; }

		pMob[mob].GetCurrentScore(mob);

		return true;
	}

	return false;
}

int CServer::GetEmptyNPCMob()
{
	int ReadCount = MAX_MOB - (MAX_USER * 3);

	if (g_EmptyMob >= ReadCount) 
		g_EmptyMob = MAX_USER;

	int EmptyMob = g_EmptyMob;

	for (int i = 0; i < MAX_MOB; i++, EmptyMob++)
	{
		if (EmptyMob >= ReadCount)
		{
			EmptyMob = MAX_USER;
			continue;
		}

		if (pMob[EmptyMob].Mode == MOB_EMPTY)
		{
			g_EmptyMob = EmptyMob + TRUE;
			return EmptyMob;
		}
	}

	return 0;
}

void CServer::GenerateMob(int index, int PosX, int PosY)
{
	int GenerateMin = mNPCGen.pList[index].MinuteGenerate;

	int nindex = index;

	for (int i = 0; i < MAX_MOB_MERC; i++)
	{
		if (index != pMobMerc[i].GenerateIndex || index == 0)
			continue;

		memcpy(mNPCGen.pList[index].Leader.Carry, pMobMerc[i].Carry, sizeof(mNPCGen.pList[index].Leader.Carry));
		break;
	}

	if (GenerateMin >= 500)
	{
		int segx = mNPCGen.pList[index].SegmentListX[0];
		int segy = mNPCGen.pList[index].SegmentListY[0];

		if ((segx - segy) <= 0)
		{
			cLog.Write(0, TL::Err, "-system err,end index < start index");
			return;
		}

		int reloc = rand() % segx + segy;
		if (reloc < 0 || reloc >= mNPCGen.NumList)
			cLog.Write(0, TL::Err, "-system err,wrong index relocation");

		index = reloc;
	}

	int Formation = mNPCGen.pList[nindex].Formation;
	int MinGroup = mNPCGen.pList[nindex].MinGroup;

	int qmob = mNPCGen.pList[nindex].MaxGroup - mNPCGen.pList[nindex].MinGroup + 1;

	if (qmob <= 0)
	{
		cLog.Write(0, TL::Err, std::format("-system err,zero divide : {}", index));
		qmob = 1;
	}

	MinGroup += rand() % qmob;

	if (mNPCGen.pList[nindex].CurrentNumMob >= mNPCGen.pList[nindex].MaxNumMob)
		return;

	if (mNPCGen.pList[nindex].CurrentNumMob + MinGroup > mNPCGen.pList[nindex].MaxNumMob)
		MinGroup = mNPCGen.pList[nindex].MaxNumMob - mNPCGen.pList[nindex].CurrentNumMob;

	int tmob = GetEmptyNPCMob();

	if (tmob == 0)
	{
		//snprintf(temp, sizeof(temp), "err,no emptymob leader: %d", index);
		//Log(temp, "-system", 0);
		return;
	}

	if (mNPCGen.pList[index].Leader.Equip[0].sIndex != 219 && mNPCGen.pList[index].SegmentListX[0] >= 2440 && mNPCGen.pList[index].SegmentListX[0] <= 2545 && mNPCGen.pList[index].SegmentListY[0] >= 1845 && mNPCGen.pList[index].SegmentListY[0] <= 1921)
	{
		mNPCGen.pList[index].MinuteGenerate = -1;
		return;
	}

	int tmob2 = tmob;

	memset(&pMob[tmob].PartyList, 0, sizeof(pMob[tmob].PartyList));
	memcpy(&pMob[tmob].MOB, &mNPCGen.pList[nindex].Leader, sizeof(STRUCT_MOB));

	pMob[tmob].MOB.MobName[NAME_LENGTH - 1] = 0;
	pMob[tmob].MOB.BaseScore.Merchant = 0;

	strncpy(pMob[tmob].MOB.MobName, mNPCGen.pList[nindex].Leader.MobName, NAME_LENGTH);

	if (strcmp(pMob[tmob].MOB.MobName, mNPCGen.pList[nindex].Leader.MobName) != 0)
		memcpy(pMob[tmob].MOB.MobName, mNPCGen.pList[nindex].Leader.MobName, NAME_LENGTH);

	for (int i = 0; i < NAME_LENGTH; i++)
	{
		if (pMob[tmob].MOB.MobName[i] == 95)
			pMob[tmob].MOB.MobName[i] = 32;

		if (pMob[tmob].MOB.MobName[i] == 64)
			pMob[tmob].MOB.MobName[i] = 32;
	}

	memset(pMob[tmob].Affect, 0, sizeof(pMob[tmob].Affect));

	for (int i = 0; i < MAX_SEGMENT; i++)
	{
		if (mNPCGen.pList[index].SegmentListX[i])
		{
			if (mNPCGen.pList[index].SegmentRange[i])
			{
				pMob[tmob].SegmentListX[i] = mNPCGen.pList[index].SegmentListX[i]
					- mNPCGen.pList[index].SegmentRange[i] + (rand() % (mNPCGen.pList[index].SegmentRange[i] + 1));

				pMob[tmob].SegmentListY[i] = mNPCGen.pList[index].SegmentListY[i]
					- mNPCGen.pList[index].SegmentRange[i] + (rand() % (mNPCGen.pList[index].SegmentRange[i] + 1));
			}
			else
			{
				pMob[tmob].SegmentListX[i] = mNPCGen.pList[index].SegmentListX[i];
				pMob[tmob].SegmentListY[i] = mNPCGen.pList[index].SegmentListY[i];
			}
			pMob[tmob].SegmentWait[i] = mNPCGen.pList[index].SegmentWait[i];
		}
		else
		{
			pMob[tmob].SegmentListX[i] = 0;
			pMob[tmob].SegmentListY[i] = 0;
		}
	}

	if (PosX && PosY)
	{
		for (int i = 0; i < MAX_SEGMENT; i++)
		{
			pMob[tmob].SegmentListX[i] = PosX;
			pMob[tmob].SegmentListY[i] = PosY;
		}
	}

	/*if (pMob[tmob].MOB.Equip[0].sIndex == 220 || pMob[tmob].MOB.Equip[0].sIndex == 219)
	{
		int br = BrState;
		if (BrState > 0 && br < MAX_USER)
		{
			if (pUser[br].Mode == USER_PLAY && GTorreState == 0)
			{
				*(int*)&pMob[tmob].MOB.Equip[15] = *(int*)&pMob[br].MOB.Equip[15];
				*(int*)(&pMob[tmob].MOB.Equip[15] + 4) = *(int*)(&pMob[br].MOB.Equip[15] + 4);

				if (pMob[tmob].MOB.Equip[0].sIndex == 219)
				{
					pMob[tmob].MOB.Guild = pMob[br].MOB.Guild;

					GetGuild(tmob);

					if (pMob[br].MOB.GuildLevel == 9)
						pMob[tmob].MOB.GuildLevel = 0;
				}
			}
		}
	}*/

	//WarOfTower::CreateTower(index, tmob);

	pMob[tmob].GenerateIndex = index;

	pMob[tmob].Formation = mNPCGen.pList[nindex].Formation;

	pMob[tmob].RouteType = mNPCGen.pList[nindex].RouteType;
	pMob[tmob].Mode = MOB_PEACE;

	pMob[tmob].SegmentDirection = 0;

	pMob[tmob].Leader = 0;
	pMob[tmob].WaitSec = pMob[tmob].SegmentWait[0];
	pMob[tmob].LastTime = CurrentTime;

	//retirado
	//pMob[tmob].GetCurrentScore(MAX_USER);
	pMob[tmob].MOB.CurrentScore.Merchant = (pMob[tmob].MOB.CurrentScore.Merchant & 240) | pMob[tmob].MOB.Merchant;//nobugmer
	pMob[tmob].MOB.CurrentScore.Hp = pMob[tmob].MOB.CurrentScore.MaxHp;

	if (NewbieEventServer && pMob[tmob].MOB.CurrentScore.Level < 120)
		pMob[tmob].MOB.CurrentScore.Hp = 3 * pMob[tmob].MOB.CurrentScore.Hp / 4;

	pMob[tmob].SegmentProgress = 0;

	memset(pMob[tmob].EnemyList, 0, sizeof(pMob[tmob].EnemyList));

	if (pMob[tmob].MOB.Clan == 1 && rand() % 10 == 1)
		pMob[tmob].MOB.Clan = 2;

	int mobslot = cGet.GetEmptyMobGrid(tmob, &pMob[tmob].SegmentListX[0], &pMob[tmob].SegmentListY[0]);

	if (mobslot == 0)
	{
		pMob[tmob].Mode = 0;

		pMob[tmob].MOB.MobName[0] = 0;
		pMob[tmob].GenerateIndex = -1;

		return;
	}

	//Imposto
	/*if (pMob[tmob].MOB.Merchant == 6 && tmob >= MAX_USER)
	{
		int level = pMob[tmob].MOB.CurrentScore.Level;

		GuildImpostoID[level] = tmob;
		pMob[tmob].MOB.Guild = g_pGuildZone[level].ChargeGuild;
	}*/

	pMob[tmob].SegmentX = pMob[tmob].SegmentListX[0];
	pMob[tmob].TargetX = pMob[tmob].SegmentListX[0];
	pMob[tmob].LastX = pMob[tmob].SegmentListX[0];
	pMob[tmob].SegmentY = pMob[tmob].SegmentListY[0];
	pMob[tmob].TargetY = pMob[tmob].SegmentListY[0];
	pMob[tmob].LastY = pMob[tmob].SegmentListY[0];

	if (mNPCGen.pList[nindex].CurrentNumMob < 0)
		mNPCGen.pList[nindex].CurrentNumMob = 0;

	mNPCGen.pList[nindex].CurrentNumMob++;

	int skillnum = mNPCGen.pList[nindex].Leader.BaseScore.MaxMp;

	if (skillnum)
	{
		this->SetAffect(tmob, skillnum, 30000, 200);
		this->SetTick(tmob, skillnum, 30000, 200);
	}

	MSG_CreateMob sm;
	memset(&sm, 0, sizeof(MSG_CreateMob));
	cGet.GetCreateMob(tmob, &sm);

	sm.CreateType |= 2;

	pMobGrid[pMob[tmob].SegmentListY[0]][pMob[tmob].SegmentListX[0]] = tmob;

	cSend.GridMulticast(pMob[tmob].SegmentListX[0], pMob[tmob].SegmentListY[0], (MSG_HEADER*)&sm, 0);

	for (int i = 0; i < MinGroup && i < MAX_PARTY; i++)
	{
		int tempmob = GetEmptyNPCMob();

		if (tempmob == 0)
		{
			pMob[tmob2].PartyList[i] = 0;
			cLog.Write(0, TL::Err, std::format("-system err,no empty mob: {}", index));
			continue;
		}

		memset(&pMob[tempmob].PartyList, 0, sizeof(pMob[tempmob].PartyList));

		pMob[tmob2].PartyList[i] = tempmob;

		memcpy(&pMob[tempmob], &mNPCGen.pList[nindex].Follower, sizeof(STRUCT_MOB));

		pMob[tempmob].MOB.BaseScore.Merchant = 0;

		strncpy(pMob[tempmob].MOB.MobName, mNPCGen.pList[nindex].Follower.MobName, NAME_LENGTH);

		for (int j = 0; j < NAME_LENGTH; ++j)
		{
			if (pMob[tempmob].MOB.MobName[j] == 95)
				pMob[tempmob].MOB.MobName[j] = 32;

			if (pMob[tempmob].MOB.MobName[j] == '@')
				pMob[tempmob].MOB.MobName[j] = 0;
		}
		memset(pMob[tempmob].Affect, 0, sizeof(pMob[tempmob].Affect));

		for (int j = 0; j < MAX_SEGMENT; j++)
		{
			if (mNPCGen.pList[index].SegmentRange[j])
			{
				pMob[tempmob].SegmentListX[j] = g_pFormation[i][j][Formation] + pMob[tmob].SegmentListX[j];

				pMob[tempmob].SegmentListY[j] = g_pFormation[i][j][Formation] + pMob[tmob].SegmentListY[j];


				pMob[tempmob].SegmentWait[j] = mNPCGen.pList[index].SegmentWait[j];
			}
			else
			{
				pMob[tempmob].SegmentListX[j] = mNPCGen.pList[index].SegmentListX[j];
				pMob[tempmob].SegmentListY[j] = mNPCGen.pList[index].SegmentListY[j];
			}
		}
		if (PosX && PosY)
		{
			for (int k = 0; k < MAX_SEGMENT; k++)
			{
				pMob[tempmob].SegmentListX[k] = PosX + rand() % 5 - 2;
				pMob[tempmob].SegmentListY[k] = PosY + rand() % 5 - 2;
			}
		}

		pMob[tempmob].GenerateIndex = index;

		pMob[tempmob].Formation = mNPCGen.pList[nindex].Formation;

		pMob[tempmob].RouteType = mNPCGen.pList[nindex].RouteType;
		pMob[tempmob].Mode = MOB_PEACE;

		pMob[tempmob].SegmentProgress = 0;

		pMob[tempmob].Leader = tmob2;
		pMob[tempmob].LastTime = CurrentTime;

		pMob[tempmob].GetCurrentScore(MAX_USER);
		pMob[tempmob].MOB.CurrentScore.Hp = pMob[tempmob].MOB.CurrentScore.MaxHp;

		if (NewbieEventServer && pMob[tempmob].MOB.CurrentScore.Level < 120)
			pMob[tempmob].MOB.CurrentScore.Hp = 3 * pMob[tempmob].MOB.CurrentScore.Hp / 4;

		pMob[tempmob].WaitSec = pMob[tempmob].SegmentWait[0];
		pMob[tempmob].SegmentDirection = 0;

		memset(pMob[tempmob].EnemyList, 0, sizeof(pMob[tempmob].EnemyList));

		if (pMob[tempmob].MOB.Clan == 1 && rand() % 10 == 1)
			pMob[tempmob].MOB.Clan = 2;

		int mslot = cGet.GetEmptyMobGrid(tempmob, &pMob[tempmob].SegmentListX[0], &pMob[tempmob].SegmentListY[0]);

		if (mslot == 0)
		{
			cLog.Write(0, TL::Err, std::format("-system err,No empty mobgrid: {} {} {}", index, pMob[tempmob].SegmentListX[0], pMob[tempmob].SegmentListY[0]));

			pMob[tempmob].Mode = 0;
			pMob[tempmob].MOB.MobName[0] = 0;
			pMob[tempmob].GenerateIndex = -1;
			pMob[tmob2].PartyList[i] = 0;
			return;
		}

		pMob[tempmob].SegmentX = pMob[tempmob].SegmentListX[0];
		pMob[tempmob].TargetX = pMob[tempmob].SegmentListX[0];
		pMob[tempmob].LastX = pMob[tempmob].SegmentListX[0];
		pMob[tempmob].SegmentY = pMob[tempmob].SegmentListY[0];
		pMob[tempmob].TargetY = pMob[tempmob].SegmentListY[0];
		pMob[tempmob].LastY = pMob[tempmob].SegmentListY[0];

		skillnum = mNPCGen.pList[nindex].Leader.BaseScore.MaxMp;

		if (skillnum)
		{
			this->SetAffect(tempmob, skillnum, 30000, 200);
			this->SetTick(tempmob, skillnum, 30000, 200);
		}

		MSG_CreateMob sm2;
		memset(&sm2, 0, sizeof(MSG_CreateMob));
		cGet.GetCreateMob(tempmob, &sm2);

		sm2.CreateType |= 2;

		pMobGrid[pMob[tempmob].SegmentListY[0]][pMob[tempmob].SegmentListX[0]] = tempmob;

		cSend.GridMulticast(pMob[tempmob].SegmentListX[0], pMob[tempmob].SegmentListY[0], (MSG_HEADER*)&sm2, 0);

		if (mNPCGen.pList[nindex].CurrentNumMob < 0)
			mNPCGen.pList[nindex].CurrentNumMob = 0;

		mNPCGen.pList[nindex].CurrentNumMob++;
	}
}

int CServer::GetEmptyUser()
{
	for (int i = 1; i < MAX_USER; i++)
	{
		if (this->pUser[i].Mode == USER_EMPTY)
			return i;
	}

	return 0;
}

uint32 CServer::GetUserFromSocket(uint32 Sock)
{
	if (Sock == 0)
		return 0;

	for (int i = 1; i < MAX_USER; i++)
	{
		if (this->pUser[i].cSock.Sock == Sock)
			return i;
	}

	return 0;
}

int CServer::GetEmptyItem()
{
	for (int i = 1; i < MAX_ITEM; i++)
	{
		if (pItem[i].Mode == 0)
			return i;
	}

	return 0;
}

int CServer::CreateMob(const char* MobName, int PosX, int PosY, const char* folder, int Type)
{
	int tmob = this->GetEmptyNPCMob();

	if (tmob == 0)
	{
		cLog.Write(0, TL::Err, std::format("-system err,no emptymob create {}", MobName));
		return -1;
	}

	strncpy(pMob[tmob].MOB.MobName, MobName, NAME_LENGTH);

	memset(&pMob[tmob].PartyList, 0, sizeof(pMob[tmob].PartyList));

	int read = ReadMob(&pMob[tmob].MOB, folder);

	if (read == 0)
		return 0;

	pMob[tmob].MOB.MobName[NAME_LENGTH - 1] = 0;
	pMob[tmob].MOB.BaseScore.Merchant = 0;


	for (int i = 0; i < NAME_LENGTH; i++)
	{
		if (pMob[tmob].MOB.MobName[i] == 95)
			pMob[tmob].MOB.MobName[i] = 32;

		if (pMob[tmob].MOB.MobName[i] == '@')
			pMob[tmob].MOB.MobName[i] = 0;
	}
	memset(pMob[tmob].Affect, 0, sizeof(pMob[tmob].Affect));


	for (int i = 0; i < MAX_SEGMENT; i++)
	{
		if (pMob[tmob].MOB.Equip[0].sIndex != 220 && pMob[tmob].MOB.Equip[0].sIndex != 219 && pMob[tmob].MOB.Equip[0].sIndex != 358)
		{
			pMob[tmob].SegmentListX[i] = PosX + rand() % 5 - 2;
			pMob[tmob].SegmentListY[i] = PosY + rand() % 5 - 2;
		}

		else
		{
			pMob[tmob].SegmentListX[i] = PosX;
			pMob[tmob].SegmentListY[i] = PosY;
		}
	}

	if (pMob[tmob].MOB.Equip[0].sIndex == 220 || pMob[tmob].MOB.Equip[0].sIndex == 219)
	{
		if (pMob[tmob].MOB.Equip[0].sIndex == 219)
		{
			pMob[tmob].MOB.Guild = g_pGuildZone[4].ChargeGuild;
			pMob[tmob].MOB.GuildLevel = 0;
		}
	}

	pMob[tmob].GenerateIndex = -1;

	pMob[tmob].Formation = 0;

	pMob[tmob].RouteType = Type;
	pMob[tmob].Mode = MOB_PEACE;

	pMob[tmob].SegmentDirection = 0;

	pMob[tmob].Leader = 0;
	pMob[tmob].WaitSec = Type == 0 ? 10 : 40;
	pMob[tmob].LastTime = CurrentTime;

	pMob[tmob].GetCurrentScore(MAX_USER);
	pMob[tmob].MOB.CurrentScore.Hp = pMob[tmob].MOB.CurrentScore.MaxHp;

	if (NewbieEventServer && pMob[tmob].MOB.CurrentScore.Level < 120)
		pMob[tmob].MOB.CurrentScore.Hp = 3 * pMob[tmob].MOB.CurrentScore.Hp / 4;

	pMob[tmob].SegmentProgress = Type == 0 ? 0 : 4;

	memset(pMob[tmob].EnemyList, 0, sizeof(pMob[tmob].EnemyList));

	int px = pMob[tmob].SegmentListX[0];
	int py = pMob[tmob].SegmentListY[0];

	int mobslot = cGet.GetEmptyMobGrid(tmob, &px, &py);

	if (mobslot == 0)
	{
		pMob[tmob].Mode = 0;

		pMob[tmob].MOB.MobName[0] = 0;
		pMob[tmob].GenerateIndex = -1;

		return -2;
	}

	pMob[tmob].SegmentX = px;
	pMob[tmob].TargetX = px;
	pMob[tmob].LastX = px;
	pMob[tmob].SegmentY = py;
	pMob[tmob].TargetY = py;
	pMob[tmob].LastY = py;

	int skillnum = pMob[tmob].MOB.BaseScore.MaxMp;

	if (skillnum)
	{
		SetAffect(tmob, skillnum, 30000, 200);
		SetTick(tmob, skillnum, 30000, 200);
	}

	MSG_CreateMob sm;
	memset(&sm, 0, sizeof(MSG_CreateMob));

	cGet.GetCreateMob(tmob, &sm);

	sm.CreateType |= 2;

	pMobGrid[py][px] = tmob;

	cSend.GridMulticast(px, py, (MSG_HEADER*)&sm, 0);

	return TRUE;
}

int CServer::UpdateItem(int Gate, int state, int* height)
{
	int rotate = pItem[Gate].Rotate;

	int update = BASE_UpdateItem(pItem[Gate].GridCharge, pItem[Gate].State, state, pItem[Gate].PosX, pItem[Gate].PosY, (char*)&pHeightGrid, rotate, height);

	if (update == 0)
		return FALSE;

	pItem[Gate].State = state;
	pItem[Gate].Height = *height;

	int GridCharge = pItem[Gate].GridCharge;
	int itemstate = pItem[Gate].State;

	pItem[Gate].Delay = 0;

	if (GridCharge >= 6 || GridCharge < 0)
	{
		cLog.Write(0, TL::Err, "-system err,updateItem maskidx illegal");
		return TRUE;
	}

	if (itemstate == state)
	{
		int keyid = BASE_GetItemAbility(&pItem[Gate].ITEM, EF_KEYID);

		if (keyid == 15 && itemstate == 1 && state == 3)
			CreateMob("GATE", pItem[Gate].PosX, pItem[Gate].PosY, "npc", 0);

		pItem[Gate].State = state;

		return TRUE;
	}

	int posX = pItem[Gate].PosX;
	int posY = pItem[Gate].PosY;

	for (int i = 0; i <= MAX_GROUNDMASK - 2; i++)
	{
		for (int j = 0; j <= MAX_GROUNDMASK - 2; j++)
		{
			int val = g_pGroundMask[GridCharge][rotate][i][j];

			int iX = posX + j - 2;
			int jY = posY + i - 2;

			if (iX >= 1 && jY >= 1 && iX <= 4094 && jY <= 4094 && val)
			{
				int tmob = pMobGrid[jY][iX];

				if (tmob != 0)
				{
					if (pMob[tmob].MOB.Equip[0].sIndex == 220)
						DeleteMob(tmob, 3);

					else
					{
						int mobgrid = cGet.GetEmptyMobGrid(tmob, &iX, &jY);

						if (mobgrid != 0)
						{
							pMob[tmob].Route[0] = 0;

							MSG_Action sm;
							memset(&sm, 0, sizeof(MSG_Action));

							cGet.GetAction(tmob, iX, jY, &sm);

							cSend.GridMulticast(iX, jY, (MSG_HEADER*)&sm, 0);

							if (tmob < MAX_USER)
								pUser[tmob].cSock.SendOneMessage((BYTE*)&sm, sizeof(sm));
						}
					}
				}
			}
		}
	}

	return TRUE;
}

int CServer::CreateItem(int x, int y, STRUCT_ITEM* item, int rotate, int Create)
{
	if (item == nullptr)
		return FALSE;

	if (item->sIndex <= 0 || item->sIndex >= MAX_ITEMLIST)
		return FALSE;

	cGet.GetEmptyItemGrid(&x, &y);

	//	if (pHeightGrid[y][x] >= -15 || pHeightGrid[y][x] >= 90)
	//		return FALSE;

	if (pItemGrid[y][x])
		return FALSE;

	int empty = this->GetEmptyItem();

	if (empty == 0)
		return FALSE;

	pItem[empty].Mode = 1;

	pItem[empty].PosX = x;
	pItem[empty].PosY = y;

	memcpy(&pItem[empty].ITEM, item, sizeof(STRUCT_ITEM));

	pItem[empty].Rotate = rotate;
	pItem[empty].State = STATE_OPEN;

	pItem[empty].Delay = 90;
	pItem[empty].Decay = 0;

	pItem[empty].GridCharge = BASE_GetItemAbility(item, EF_GROUND);

	pItemGrid[y][x] = empty;

	pItem[empty].Height = pHeightGrid[y][x];

	MSG_CreateItem sm;
	memset(&sm, 0, sizeof(MSG_CreateItem));

	sm.Type = _MSG_CreateItem;
	sm.Size = sizeof(MSG_CreateItem);
	sm.ID = ESCENE_FIELD;

	sm.ItemID = empty + 10000;

	memcpy(&sm.Item, &pItem[empty].ITEM, sizeof(STRUCT_ITEM));

	sm.GridX = x;
	sm.GridY = y;

	sm.Rotate = rotate;
	sm.State = pItem[empty].State;
	//	sm.Create = Create;

	//	sm.Height = pItem[empty].Height;

	if (Create == 2)//Bau
		sm.Height = -204;

	cSend.GridMulticast(x, y, (MSG_HEADER*)&sm, 0);

	return TRUE;
}

void CServer::Reboot()
{
	for (int y = 0; y < MAX_GRIDY; y++)
	{
		for (int x = 0; x < MAX_GRIDX; x++)
		{
			pMobGrid[y][x] = 0;
			pItemGrid[y][x] = 0;
			pHeightGrid[y][x] = 0;
		}
	}

	/*for (int i = 0; i < 3; i++)
		LiveTower[i] = 0;

	BASE_InitializeMobname("MobName.txt", 0);*/

	this->mNPCGen.ReadNPCGenerator();

	int Handle = _open("./heightmap.dat", _O_RDONLY | _O_BINARY, NULL);

	if (Handle == -1)
	{
		cLog.Write(0, TL::Err, "-system err Load height grid fail");
		return;
	}

	_read(Handle, (void*)pHeightGrid, sizeof(pHeightGrid));
	_close(Handle);

	BASE_InitializeBaseDef();
	BASE_ApplyAttribute((char*)pHeightGrid, MAX_GRIDX);
	BASE_InitializeMessage("Language.txt");
	this->mSummon.Initialize();

	cLog.Write(0, TL::Sys, "-system sys,start server");

	for (int i = 0; i < this->g_dwInitItem; i++) // verificar a necessidade do loop pq inicia com 0 o g_...
	{
		STRUCT_ITEM Item;
		memset(&Item, 0, sizeof(STRUCT_ITEM));

		Item.sIndex = g_pInitItem[i].ItemIndex;

		int ID = CreateItem(g_pInitItem[i].PosX, g_pInitItem[i].PosY, &Item, g_pInitItem[i].Rotate, 1);

		if (ID >= MAX_ITEM || ID <= 0)
			continue;

		int ret = BASE_GetItemAbility(&Item, EF_KEYID);

		if (ret != 0 && ret < 15)
		{
			int Height;

			UpdateItem(ID, STATE_LOCKED, &Height);
		}
	}

}

bool CServer::StartServer() 
{
	BASE_InitModuleDir();
	BASE_InitializeBaseDef();
	cLog.Start();

	int ret = cSrv.ListenSocket.WSAInitialize();

	if (!ret)
	{
		cLog.Write(0, TL::Err, "erro,não foi possivel iniciar a seguinte função : WSAInitialize");
		return false;
	}

	this->Reboot();

	char name[255];

	FILE* fp = fopen("localip.txt", "rt");

	if (fp)
	{
		fscanf(fp, "%s", name);
		fclose(fp);
	}
	else
		MessageBox(hWndMain, "Não foi possivel encontrar o arquivo LocalIP.txt", "Erro ao iniciar", NULL);

	int r1 = 0, r2 = 0, r3 = 0, r4 = 0;

	sscanf(name, "%d.%d.%d.%d", &r1, &r2, &r3, &r4);

	this->LocalIP[0] = r1;
	this->LocalIP[1] = r2;
	this->LocalIP[2] = r3;
	this->LocalIP[3] = r4;
	this->DBServerAddress[0] = 0;

	for (int i = 0; i < MAX_SERVERGROUP; i++)
	{
		for (int j = 1; j < MAX_SERVERNUMBER; j++)
		{
			if (!strcmp(g_pServerList[i][j], name))
			{
				strcpy_s(this->DBServerAddress, g_pServerList[i][0]);

				this->ServerGroup = i;
				this->ServerIndex = j - 1;
				break;
			}
		}

		if (this->DBServerAddress[0] == 0)
			continue;
	}

	if (this->DBServerAddress[0] == 0)
	{
		MessageBox(hWndMain, "Não foi possivel pegar o ServerGroup. LocalIP.txt / ServerList.txt", "Erro ao iniciar", MB_OK | MB_SYSTEMMODAL);
		return false;
	}

	this->DBServerPort = 7514;
	int* pip = (int*)LocalIP;
	ret = this->DBSocket.ConnectServer(this->DBServerAddress, 7514, *pip, WSA_READDB);

	if (ret == NULL)
	{
		cLog.Write(0, TL::Err, "erro, Não foi possivel se conectar a DBServer");
		MessageBox(hWndMain, "Não foi possivel se conectar a DBServer", "Erro ao iniciar", NULL);
		return false;
	}

	for (int i = 1; i < MAX_SERVERNUMBER; i++)
	{
		if (g_pServerList[ServerGroup][i][0] != 0)
			this->NumServerInGroup++;
	}

	if (NumServerInGroup <= 0)
		this->NumServerInGroup = 1;

	if (this->NumServerInGroup > 10)
		this->NumServerInGroup = 10;

	// local da biserver não usado

	CurrentTime = timeGetTime();

	SetTimer(hWndMain, TIMER_SEC, 500, NULL);
	SetTimer(hWndMain, TIMER_MIN, 12000, NULL);

	ListenSocket.StartListen(hWndMain, *pip, GAME_PORT, WSA_ACCEPT);

	return true;
}

void CServer::CloseUser(int conn)
{
	/*if (pUser[conn].Mode == USER_PLAY && pMob[conn].TargetX >= 0 && pMob[conn].TargetX < 4096
		&& pMob[conn].TargetY >= 0 && pMob[conn].TargetY < 4096)
		pMobGrid[pMob[conn].TargetY][pMob[conn].TargetX] = 0;*/	

	pUser[conn].cSock.CloseSocket();

	int Mode = pUser[conn].Mode;

	if (Mode && Mode != USER_ACCEPT)
	{
		if (Mode != USER_PLAY && Mode != USER_SAVING4QUIT)
		{
			MSG_STANDARD sm;
			memset(&sm, 0, sizeof(MSG_STANDARD));

			sm.Type = _MSG_DBNoNeedSave;
			sm.ID = conn;

			cSrv.DBSocket.SendOneMessage((BYTE*)&sm, sizeof(MSG_STANDARD));

			pMob[conn].Mode = USER_EMPTY;

			pUser[conn].Disconnect();
		}

		else
		{
			this->RemoveParty(conn);

			int tradeOpponentID = pUser[conn].Trade.OpponentID;

			if (tradeOpponentID > 0 && tradeOpponentID < MAX_USER && pUser[tradeOpponentID].Mode == USER_PLAY && pUser[tradeOpponentID].Trade.OpponentID == conn)
				this->RemoveTrade(tradeOpponentID);

			pUser[conn].Trade.OpponentID = 0;

			MSG_SavingQuit sm;
			memset(&sm, 0, sizeof(MSG_SavingQuit));

			sm.Type = _MSG_SavingQuit;

			if (conn < 0 || conn > MAX_MOB)
			{
				//CrackLog(conn, " SaveAndQuitMobidx");
				return;
			}

			int Slot = pUser[conn].Slot;

			if (Slot < 0 || Slot >= MAX_CLASS)
			{
				//CrackLog(conn, " SaveAndQuitWrongSlot");
				return;
			}

			memcpy(&sm.MOB, &pMob[conn], sizeof(STRUCT_MOB));
			memcpy(sm.Cargo, &pUser[conn].Cargo, sizeof(pUser[conn].Cargo));
			memcpy(sm.ShortSkill, &pUser[conn].CharShortSkill, 16);
	
			memcpy(sm.affect, pMob[conn].Affect, sizeof(sm.affect));
			sm.Extra = pMob[conn].Extra;

			strncpy((char*)&sm.AccountName, pUser[conn].AccountName, ACCOUNTNAME_LENGTH);

			sm.Coin = pUser[conn].Coin;
		
			sm.ID = conn;

			cSrv.DBSocket.SendOneMessage((BYTE*)&sm, sizeof(MSG_SavingQuit));

			pUser[conn].Mode = USER_SAVING4QUIT;

			this->DeleteMob(conn, 2);
		}
	}

	else
		pUser[conn].Disconnect();
}

void CServer::DeleteMob(int conn, int Type)
{
	MSG_RemoveMob sm;
	memset(&sm, 0, sizeof(MSG_RemoveMob));

	sm.Type = _MSG_RemoveMob;
	sm.Size = sizeof(MSG_RemoveMob);
	sm.ID = conn;
	sm.RemoveType = Type;

	cSend.GridMulticast(pMob[conn].TargetX, pMob[conn].TargetY, (MSG_HEADER*)&sm, conn < MAX_USER ? conn : 0);

	if (Type != 0)
	{
		int X = pMob[conn].TargetX;
		int Y = pMob[conn].TargetY;

		int geneidx = pMob[conn].GenerateIndex;

		if (conn != pMobGrid[Y][X])
		{
			bool Flag = false;

			for (int y = 0; y < MAX_GRIDY; y++)
			{
				for (int x = 0; x < MAX_GRIDX; x++)
				{
					if (pMobGrid[y][x] == conn)
					{
						Flag = true;

						cSend.GridMulticast(x, y, (MSG_HEADER*)&sm, 0);

						pMobGrid[y][x] = 0;

						break;
					}
				}

				if (Flag) break;
			}
		}
		else
		{
			pMobGrid[Y][X] = 0;
		}

		pMob[conn].MOB.CurrentScore.Hp = 0;  //4

		pMob[conn].Mode = MOB_EMPTY;

		this->RemoveParty(conn);

		if (conn >= MAX_USER)
		{
			if (geneidx >= 0 && geneidx < MAX_NPCGENERATOR)
			{
				mNPCGen.pList[geneidx].CurrentNumMob--;

				if (mNPCGen.pList[geneidx].CurrentNumMob < 0)
					mNPCGen.pList[geneidx].CurrentNumMob = 0;
			}
		}
	}
}

void CServer::RemoveParty(int conn) // revisar
{
	if (conn <= 0 || conn >= MAX_USER)
		return;

	int leader = pMob[conn].Leader;

	if (leader < 0 || leader >= MAX_MOB)
	{
		cLog.Write(0, TL::Err, "err,RemoveParty but worng leader");
		return;
	}

	if (leader && leader < MAX_USER && pUser[leader].Mode != USER_PLAY)
	{
		cLog.Write(0, TL::Err, "err,removeparty");
		pMob[conn].Leader = 0;
	}
	else
	{
		if (conn > 0 && conn < MAX_USER)
			pUser[conn].SendRemoveParty(0);

		if (leader)
		{
			if (leader > 0 && leader < MAX_USER)
				pUser[leader].SendRemoveParty(conn);

			pMob[conn].Leader = 0;

			for (int i = 0; i < MAX_PARTY; i++)
			{
				int partyconn = pMob[leader].PartyList[i];

				if (partyconn)
				{
					if (partyconn == conn)
						pMob[leader].PartyList[i] = 0;

					if (pMob[partyconn].Summoner && pMob[partyconn].Summoner == conn)
						DeleteMob(partyconn, 3);

					if (!pMob[partyconn].Summoner && partyconn >= MAX_USER && leader < MAX_USER)
						DeleteMob(partyconn, 3);

					if (partyconn > 0 && partyconn <= MAX_USER && pUser[partyconn].Mode == USER_PLAY)
						pUser[partyconn].SendRemoveParty(conn);
				}
			}
		}

		else
		{
			int novolider = 0;

			for (int i = 0; i < MAX_PARTY; i++)
			{
				int partyconn = pMob[conn].PartyList[i];

				if (partyconn != 0 && partyconn < MAX_USER)
				{
					novolider = partyconn;

					/*for (int s = 0; s < 7; s++)
					{
						for (int t = 0; t < 3; t++)
						{
							if (Pista[s].Party[t].LeaderID == conn && strcmp(Pista[s].Party[t].LeaderName, pMob[conn].MOB.MobName) == 0)
							{
								strncpy(Pista[s].Party[t].LeaderName, pMob[novolider].MOB.MobName, NAME_LENGTH);
								Pista[s].Party[t].LeaderID = novolider;
							}
						}
					}*/

					break;
				}
			}

			for (int i = 0; i < MAX_PARTY; i++)
			{
				int partyconn = pMob[conn].PartyList[i];

				if (partyconn > 0 && partyconn <= MAX_MOB)
				{
					pMob[conn].PartyList[i] = 0;
					pMob[partyconn].Leader = 0;

					if (partyconn > 0 && partyconn < MAX_USER && pUser[partyconn].Mode == USER_PLAY)
						pUser[partyconn].SendRemoveParty(0);

					if (partyconn > MAX_USER)
					{
						if (conn < MAX_USER && pMob[partyconn].Summoner && pMob[partyconn].Summoner == conn)
							DeleteMob(partyconn, 3);

						pMob[partyconn].Summoner = 0;
					}

					if (novolider != 0 && novolider != partyconn && partyconn < MAX_USER && novolider < MAX_USER)
					{
						MSG_AcceptParty sm;
						memset(&sm, 0, sizeof(MSG_AcceptParty));

						sm.Size = sizeof(MSG_AcceptParty);
						sm.Type = _MSG_AcceptParty;
						sm.ID = 0;

						sm.LeaderID = novolider;

						strncpy(sm.MobName, pMob[novolider].MOB.MobName, NAME_LENGTH);

						pMob[partyconn].LastReqParty = novolider;

						this->ProcessClientMSG(partyconn, (char*)&sm, TRUE);
					}
				}
			}
		}
	}
}

void CServer::SaveUser(int conn, int Export)
{
	MSG_DBSaveMob sm;
	memset(&sm, 0, sizeof(MSG_DBSaveMob));

	sm.Type = _MSG_DBSaveMob;

	memcpy(&sm.MOB, &pMob[conn].MOB, sizeof(STRUCT_MOB));

	memcpy(&sm.Cargo, pUser[conn].Cargo, sizeof(STRUCT_ITEM) * MAX_CARGO);

	sm.Coin = pUser[conn].Coin;

	sm.ID = conn;
	sm.Export = Export;

	sm.Slot = pUser[conn].Slot;
	//sm.Donate = pUser[conn].Donate;

	memcpy(&sm.uExtra, &pUser[conn].uExtra, sizeof(stUserExtra)); // eee save da struct, antes tava sm.uExtra = ...

	strncpy(sm.AccountName, pUser[conn].AccountName, ACCOUNTNAME_LENGTH);

	memcpy(sm.ShortSkill, pUser[conn].CharShortSkill, 16);

	memcpy(sm.affect, pMob[conn].Affect, sizeof(sm.affect));
	sm.Extra = pMob[conn].Extra;

	this->DBSocket.SendOneMessage((BYTE*)&sm, sizeof(MSG_DBSaveMob));
}

void CServer::CharLogOut(int conn)
{
	if (conn <= 0 || conn >= MAX_USER)
	{
		cLog.Write(0, TL::Err, "-system err,char logout - not conn valid");
		return;
	}

	if (pUser[conn].Mode != USER_PLAY)
	{
		cLog.Write(conn, TL::Err, std::format("{} err,char logout - not user_play", pUser[conn].AccountName));
		pUser[conn].SendClientSignal(conn, _MSG_CNFCharacterLogout);
		return;
	}

	//pMob[conn].Extra.CheckTimeKersef = 0;

	/*if (pUser[conn].IsBillConnect && CHARSELBILL == 0)
		SendBilling(conn, pUser[conn].AccountName, 2, 0);*/

	int trade = pUser[conn].Trade.OpponentID;

	if (trade > 0 && trade < MAX_USER)
	{
		if (pUser[trade].Mode == USER_PLAY && pUser[trade].Trade.OpponentID == conn)
			RemoveTrade(trade);
	}

	pUser[conn].SelChar.Score[pUser[conn].Slot].Level = pMob[conn].MOB.BaseScore.Level;

	this->SaveUser(conn, 1);
	this->DeleteMob(conn, 2);

	pMob[conn].TargetX = pMob[conn].MOB.SPX;
	pMob[conn].TargetY = pMob[conn].MOB.SPY;

	pUser[conn].Mode = USER_SELCHAR;

	pMob[conn].Mode = 0;

	pUser[conn].SendClientSignal(conn, _MSG_CNFCharacterLogout);
}

void CServer::RemoveTrade(int conn)
{
	if (conn <= 0 || conn >= MAX_USER)
		return;

	memset(&pUser[conn].Trade, 0, sizeof(MSG_Trade));

	for (int i = 0; i < MAX_TRADE; i++)
		pUser[conn].Trade.InvenPos[i] = -1;

	memset(&pUser[conn].AutoTrade, 0, sizeof(MSG_SendAutoTrade));

	for (int i = 0; i < MAX_AUTOTRADE; i++)
		pUser[conn].AutoTrade.CarryPos[i] = -1;

	if (pUser[conn].Mode != USER_PLAY)
		return;

	pUser[conn].SendClientSignal(conn, 900);

	if (pUser[conn].TradeMode)
	{
		int posX = pMob[conn].TargetX;
		int posY = pMob[conn].TargetY;

		MSG_CreateMob sm;
		memset(&sm, 0, sizeof(MSG_CreateMob));

		cGet.GetCreateMob(conn, &sm);
		cSend.GridMulticast(posX, posY, (MSG_HEADER*)&sm, 0);

		pUser[conn].TradeMode = 0;
	}
}

int CServer::ReadMob(STRUCT_MOB* mob, const char* dir)
{
	snprintf(temp, sizeof(temp), "./%s/%s", dir, mob->MobName);

	int Handle = _open(temp, O_RDONLY | O_BINARY);

	if (Handle == -1)
	{
		if (errno == EINVAL)
			cLog.Write(0, TL::Err, std::format("-system err,ReadMob EEXIST {}", mob->MobName));
		else if (errno == EMFILE)
			cLog.Write(0, TL::Err, std::format("-system err,ReadMob EMFILE {}", mob->MobName));
		else if (errno == ENOENT)
			cLog.Write(0, TL::Err, std::format("-system err,ReadMob ENOENTN {}", mob->MobName));
		else
			cLog.Write(0, TL::Err, std::format("-system err,ReadMob UNKNOW {}", mob->MobName));

		return FALSE;
	}

	strncpy(this->temp, mob->MobName, NAME_LENGTH);

	int ret = _read(Handle, mob, sizeof(STRUCT_MOB));

	if (ret == -1)
	{
		_close(Handle);
		return FALSE;
	}

	_close(Handle);

	strncpy(mob->MobName, temp, NAME_LENGTH);

	BASE_GetEnglish(mob->MobName);

	mob->BaseScore = mob->CurrentScore;

	return TRUE;
}

void CServer::SetReqHp(CMob* mob)
{
	if (mob)
	{
		if (mob->MOB.CurrentScore.Hp > mob->MOB.CurrentScore.MaxHp)
			mob->MOB.CurrentScore.Hp = mob->MOB.CurrentScore.MaxHp;
	}
}

void CServer::SetReqMp(CMob* mob)
{
	if (mob)
	{
		if (mob->MOB.CurrentScore.Mp > mob->MOB.CurrentScore.MaxMp)
			mob->MOB.CurrentScore.Mp = mob->MOB.CurrentScore.MaxMp;
	}
}