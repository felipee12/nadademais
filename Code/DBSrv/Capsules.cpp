#include "pch.h"

bool CFileDBSRV::Exec_MSG_DBCapsuleInfo(int conn, char* Msg)
{
	MSG_STANDARDPARM* m = (MSG_STANDARDPARM*)Msg;

	int Index = m->Parm;
	sprintf(cSrv.temp, "./capsule/%d", Index);
	int Handle = _open(cSrv.temp, O_RDONLY | O_BINARY);

	if (Handle == -1)
	{
		return false;
	}

	STRUCT_CAPSULE file;
	memset(&file, 0, sizeof(STRUCT_CAPSULE)); //add

	_read(Handle, &file, sizeof(STRUCT_CAPSULE));
	_close(Handle);

	MSG_CNFDBCapsuleInfo sm;
	memset(&sm, 0, sizeof(MSG_CNFDBCapsuleInfo)); //add

	sm.Size = sizeof(MSG_CNFDBCapsuleInfo);
	sm.Type = _MSG_CNFDBCapsuleInfo;
	sm.ID = m->ID;

	sm.Index = Index;

	sm.Capsule.Read = 1;
	sm.Capsule.MortalClass = file.Extra.MortalFace / 10;
	sm.Capsule.ClassCele = file.Extra.SaveCelestial[0].Class;
	sm.Capsule.SubClass = file.MOB.Class;
	sm.Capsule.LevelCele = file.Extra.SaveCelestial[0].BaseScore.Level;
	sm.Capsule.LevelSub = file.MOB.BaseScore.Level;

	sm.Capsule.For = file.MOB.BaseScore.Str;
	sm.Capsule.Int = file.MOB.BaseScore.Int;
	sm.Capsule.Dex = file.MOB.BaseScore.Dex;
	sm.Capsule.Con = file.MOB.BaseScore.Con;

	sm.Capsule.ScoreBonus = file.MOB.ScoreBonus;
	sm.Capsule.SkillPoint = file.MOB.SkillBonus;

	sm.Capsule.ArchQuest = file.Extra.QuestInfo.Arch.Cristal;
	sm.Capsule.CelestialQuest = file.Extra.QuestInfo.Celestial.Reset;
	sm.Capsule.ArcanaQuest = file.Extra.QuestInfo.Circle;

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFDBCapsuleInfo));
	return true;
}

bool CFileDBSRV::Exec_MSG_DBPutInCapsule(int conn, char* Msg)
{
	MSG_STANDARDPARM* m = (MSG_STANDARDPARM*)Msg;

	int Slot = m->Parm;
	int Idx = GetIndex(conn, m->ID);

	if (Slot < 0 || Slot >= MOB_PER_ACCOUNT)
	{
		cLog.Write(0, TL::Err, std::format("err,putincapsule  slot out of range {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	cSrv.LastCapsule++;
	sprintf(cSrv.temp, "./capsule/%d", cSrv.LastCapsule);

	int Handle = _open(cSrv.temp, O_RDWR | O_CREAT | O_BINARY, _S_IREAD | _S_IWRITE);

	if (Handle == -1)
	{
		if (errno == EEXIST)
			cLog.Write(0, TL::Err, "err putincapsulewrite EEXIST");
		if (errno == EACCES)
			cLog.Write(0, TL::Err, "err putincapsulewrite EACCES");
		if (errno == EINVAL)
			cLog.Write(0, TL::Err, "err putincapsulewrite EINVAL");
		if (errno == EMFILE)
			cLog.Write(0, TL::Err, "err putincapsulewrite EMFILE");
		if (errno == ENOENT)
			cLog.Write(0, TL::Err, "err putincapsulewrite ENOENT");
		else
			cLog.Write(0, TL::Err, "err putincapsulewrite UNKNOWN");

		return false;
	}

	STRUCT_CAPSULE file;
	memset(&file, 0, sizeof(STRUCT_CAPSULE)); //add

	file.MOB = pAccountList[Idx].File.Char[Slot];
	file.Extra = pAccountList[Idx].File.mobExtra[Slot];

	file.Extra.QuestInfo.Arch.MortalSlot = -1;

	_write(Handle, &file, sizeof(STRUCT_CAPSULE));
	_close(Handle);

	memset(pAccountList[Idx].File.ShortSkill[Slot], 0, 16);

	cLog.Write(0, TL::Sys, std::format("putchar in capsule {} {}", file.MOB.MobName, pAccountList[Idx].File.Info.AccountName));

	STRUCT_MOB* mob = &pAccountList[Idx].File.Char[Slot];

	cSrv.WriteConfig();

	BASE_ClearMob(mob);
	BASE_ClearMobExtra(&pAccountList[Idx].File.mobExtra[Slot]);

	DBWriteAccount(&pAccountList[Idx].File);

	SendDBSignal(conn, m->ID, _MSG_DBCNFCapsuleSucess);

	MSG_CNFDeleteCharacter sm;
	memset(&sm, 0, sizeof(MSG_CNFDeleteCharacter)); // add

	sm.Type = _MSG_DBCNFDeleteCharacter;

	DBGetSelChar(&sm.sel, &pAccountList[Idx].File);

	sm.ID = m->ID;

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFDeleteCharacter));

	sprintf(cSrv.temp, "../../Common/ImportItem/%d%d%d%d%d", cSrv.LastCapsule, rand() % 255, rand() % 255, rand() % 255, rand() % 255);

	FILE* fp = fopen(cSrv.temp, "a+");
	fprintf(fp, "%s %d %d %d %d %d %d %d", pAccountList[Idx].File.Info.AccountName, 3443, 59, cSrv.LastCapsule / 256, 59, (cSrv.LastCapsule - ((cSrv.LastCapsule / 256) * 256)), 0, 0);
	fclose(fp);
	return true;
}

bool CFileDBSRV::Exec_MSG_DBOutCapsule(int conn, char* Msg)
{
	MSG_DBOutCapsule* m = (MSG_DBOutCapsule*)Msg;

	int Slot = m->Slot;
	int NovoSlot = 0;
	int Idx = this->GetIndex(conn, m->ID);

	if (Slot < 0 || Slot >= MOB_PER_ACCOUNT)
	{
		cLog.Write(0, TL::Err, std::format("err,outcapsule  slot out of range {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	for (NovoSlot = 0; NovoSlot < MOB_PER_ACCOUNT; NovoSlot++)
	{
		if (pAccountList[Idx].File.Char[NovoSlot].MobName[0] == 0)
			break;
	}

	if (NovoSlot < 0 || NovoSlot >= MOB_PER_ACCOUNT)
	{
		cLog.Write(0, TL::Err, std::format("err,newcharcapsule  slot out of range {}", pAccountList[Idx].File.Info.AccountName));
		this->SendDBSignal(conn, m->ID, _MSG_DBCNFCapsuleCharacterFail);
		return false;
	}

	int ret = this->CreateCharacter(pAccountList[Idx].File.Info.AccountName, m->MobName);

	if (ret == 0)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBCNFCapsuleCharacterFail2);
		return false;
	}

	STRUCT_ITEM* item = GetItemPointer(&pAccountList[Idx].File.Char[Slot], pAccountList[Idx].File.Cargo, m->SourType, m->SourPos);

	if (item == nullptr)
		return false;

	int index = item->stEffect[0].cValue * 256 + item->stEffect[1].cValue;

	snprintf(cSrv.temp, sizeof(cSrv.temp), "./capsule/%d", index);

	int Handle = _open(cSrv.temp, O_RDONLY | O_BINARY);

	if (Handle == -1)
	{
		if (errno == EEXIST)
			cLog.Write(0, TL::Err, "err putincapsuleread EEXIST");
		if (errno == EACCES)
			cLog.Write(0, TL::Err, "err putincapsuleread EACCES");
		if (errno == EINVAL)
			cLog.Write(0, TL::Err, "err putincapsuleread EINVAL");
		if (errno == EMFILE)
			cLog.Write(0, TL::Err, "err putincapsuleread EMFILE");
		if (errno == ENOENT)
			cLog.Write(0, TL::Err, "err putincapsuleread ENOENT");
		else
			cLog.Write(0, TL::Err, "err putincapsuleread UNKNOWN");

		return false;
	}

	STRUCT_CAPSULE file;
	memset(&file, 0, sizeof(STRUCT_CAPSULE)); //add

	_read(Handle, &file, sizeof(STRUCT_CAPSULE));
	_close(Handle);

	strncpy(file.MOB.MobName, m->MobName, 12);

	STRUCT_MOB* mob = &pAccountList[Idx].File.Char[NovoSlot];

	mob->MobName[NAME_LENGTH - 1] = 0;
	mob->MobName[NAME_LENGTH - 2] = 0;

	m->MobName[NAME_LENGTH - 1] = 0;
	m->MobName[NAME_LENGTH - 2] = 0;

	if (mob->MobName[0] != 0)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,newchar already charged {} {} {}", pAccountList[Idx].File.Info.AccountName, mob->MobName, m->MobName));
		return false;
	}

	m->MobName[NAME_LENGTH - 1] = 0;
	m->MobName[NAME_LENGTH - 2] = 0;

	auto len = strlen(m->MobName);

	for (int i = 0; i < len; i++)
	{
		if (m->MobName[i] == 'í' && m->MobName[i + 1] == 'í')
		{
			this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
			return false;
		}
	}

	STRUCT_MOBExtra* Extra = &pAccountList[Idx].File.mobExtra[NovoSlot];

	mob = &file.MOB;
	Extra = &file.Extra;

	memset(item, 0, sizeof(STRUCT_ITEM));

	memset(&pAccountList[Idx].File.affect[NovoSlot], 0, sizeof(pAccountList[Idx].File.affect[NovoSlot]));
	memset(&pAccountList[Idx].File.ShortSkill[NovoSlot], -1, 16);

	memset(&mob->Equip[1], 0, sizeof(STRUCT_ITEM) * (MAX_EQUIP - 2));
	memset(&mob->Carry[0], 0, sizeof(STRUCT_ITEM) * (MAX_CARRY - 4));

	mob->Coin = 0;
	mob->Guild = 0;
	mob->GuildLevel = 0;

	memcpy(mob->MobName, m->MobName, NAME_LENGTH);

	ret = DBWriteAccount(&pAccountList[Idx].File);

	if (ret == 0)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,newchar fail - create file {}", m->MobName));
		return false;
	}

	memcpy(&pAccountList[Idx].File.Char[NovoSlot], mob, sizeof(STRUCT_MOB));
	memcpy(&pAccountList[Idx].File.mobExtra[NovoSlot], Extra, sizeof(STRUCT_MOBExtra));

	cLog.Write(0, TL::Err, std::format("create character {} - capsule {}", mob->MobName, pAccountList[Idx].File.Info.AccountName));

	snprintf(cSrv.temp, sizeof(cSrv.temp), "./capsule/%d", index);
	DeleteFileA(cSrv.temp);

	MSG_CNFNewCharacter sm;
	memset(&sm, 0, sizeof(MSG_CNFNewCharacter)); // add

	sm.Type = _MSG_DBCNFNewCharacter;
	DBGetSelChar(&sm.sel, &pAccountList[Idx].File);
	sm.ID = m->ID;

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFNewCharacter));
	return true;
}
