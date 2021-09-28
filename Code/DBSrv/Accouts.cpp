#include "pch.h"

bool CFileDBSRV::Exec_MSG_DBAccountLogin(int conn, char* Msg)
{
	MSG_AccountLogin* m = (MSG_AccountLogin*)Msg;

	_strupr(m->AccountName);

	char* ac = m->AccountName;

	if ((ac[0] == 'C' && ac[1] == 'O' && ac[2] == 'M' && ac[3] >= '0' && ac[3] <= '9' && ac[4] == 0) ||
		(ac[0] == 'L' && ac[1] == 'P' && ac[2] == 'T' && ac[3] >= '0' && ac[3] <= '9' && ac[4] == 0))
	{
		SendDBSignal(conn, m->ID, _MSG_DBAccountLoginFail_Account);
		return false;
	}

	int Idx = GetIndex(conn, m->ID);
	int IdxName = GetIndex(m->AccountName);

	STRUCT_ACCOUNTFILE file;
	memset(&file, 0, sizeof(file));
	memcpy(&file.Info.AccountName, m->AccountName, ACCOUNTNAME_LENGTH);

	int ret = DBReadAccount(&file);

	if (ret == 0)
	{//criando conta automatica por enquanto
		file.Info.NumericToken[0] = -1;//os memcpy tem q ficar ates do dbwrite
		memcpy(&file.Info.AccountName, m->AccountName, ACCOUNTNAME_LENGTH);
		memcpy(&file.Info.AccountPass, m->AccountPassword, ACCOUNTPASS_LENGTH);
		DBWriteAccount(&file);
	}

	if (file.Coin < 0)
		file.Coin = 0;

	if (file.Info.Year != 0 && file.Info.YearDay != 0)
	{
		auto when = getNow();

		if (file.Info.Year >= when.tm_year || file.Info.Year >= when.tm_year && file.Info.YearDay >= when.tm_yday)
		{
			SendDBSignalParm(conn, m->ID, _MSG_DBAccountLoginFail_Block, 0);
			return false;
		}
	}

	int ChangeServer = 0;

	if (file.TempKey[0] != 0 && m->Zero[0] != 0)
	{
		if (memcmp(file.TempKey, m->Zero, sizeof(file.TempKey)) == 0)
		{
			memset(file.TempKey, 0, sizeof(file.TempKey));
			ChangeServer = 1;
			goto lb_sucess;
		}
		memset(file.TempKey, 0, sizeof(file.TempKey));
		DBWriteAccount(&file);
		return TRUE;
	}

	if (strcmp(file.Info.AccountPass, m->AccountPassword) != 0)
	{
		SendDBSignal(conn, m->ID, _MSG_DBAccountLoginFail_Pass);
		return false;
	}
lb_sucess:

	if (IdxName == Idx)
		return true;

	if (IdxName != 0)
	{
		cLog.Write(0, TL::Err, std::format("err, desconectado. conexão anterior finalizada {}", m->AccountName));

		if (m->DBNeedSave == 0)
		{
			SendDBSignal(conn, m->ID, _MSG_DBAlreadyPlaying);
			return false;
		}

		SendDBSignal(conn, m->ID, _MSG_DBStillPlaying);
		SendDBSavingQuit(IdxName, 0);
		return false;
	}

	_strupr(file.Info.AccountName);

	int right = -1;
	int left = -1;

	for (int q = 0; q < 4; q++)
	{// loop checar persoangem com as pedras da troca
		if (file.Char[q].MobName[0] != 0)
		{
			if (file.Char[q].Equip[13].sIndex == 774)
				left = q;
		}

		if (file.Char[q].MobName[0] != 0)
		{
			if (file.Char[q].Equip[13].sIndex == 775)
				right = q;
		}
	}

	if (left != -1 && right != -1)
	{// mudar nome
		char temp[NAME_LENGTH];

		memcpy(temp, file.Char[left].MobName, NAME_LENGTH);
		memcpy(file.Char[left].MobName, file.Char[right].MobName, NAME_LENGTH);
		memcpy(file.Char[right].MobName, temp, NAME_LENGTH);

		file.Char[left].Equip[13].sIndex = 0;
		file.Char[right].Equip[13].sIndex = 0;

		cLog.Write(0, TL::Sys, std::format("etc,name swap {} {} {}", file.Char[left].MobName, file.Char[right].MobName, m->AccountName));

		DBWriteAccount(&file);
	}

	STRUCT_ACCOUNTFILE* pFile = &pAccountList[Idx].File;
	memcpy(pFile, &file, sizeof(STRUCT_ACCOUNTFILE));

	pAccountList[Idx].Mac[0] = m->AdapterName[0];
	pAccountList[Idx].Mac[1] = m->AdapterName[1];
	pAccountList[Idx].Mac[2] = m->AdapterName[2];
	pAccountList[Idx].Mac[3] = m->AdapterName[3];

	AddAccountList(Idx);

	STRUCT_SELCHAR selchar;
	memset(&selchar, 0, sizeof(STRUCT_SELCHAR)); // add

	DBGetSelChar(&selchar, &pAccountList[Idx].File);

	pAccountList[Idx].SecurePass = -1;

	MSG_DBCNFAccountLogin sm;
	memset(&sm, 0, sizeof(MSG_DBCNFAccountLogin));

	sm.Type = _MSG_DBCNFAccountLogin;
	sm.ID = m->ID;

	strncpy(sm.AccountName, file.Info.AccountName, ACCOUNTNAME_LENGTH);
	memcpy(sm.Cargo, pAccountList[Idx].File.Cargo, sizeof(pAccountList[Idx].File.Cargo));

	sm.Coin = pAccountList[Idx].File.Coin;

	sm.sel = selchar;

	sm.IsBlocked = file.IsBlocked;
	strcpy(sm.BlockPass, file.BlockPass);

	memcpy(&sm.uExtra, &pAccountList[Idx].File.uExtra, sizeof(stUserExtra)); // eee

	auto userDB = cSrv.getUserDB(conn);
	if (!userDB)
	{
		cLog.Write(0, TL::Err, "err, [Exec_MSG_DBAccountLogin] - userDB nullo");
		return false;
	}

	userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_DBCNFAccountLogin));

	if (GetAccountsByMac(m->AdapterName) <= 1)
	{
		MSG_DBCheckPrimaryAccount sm_pa;
		memset(&sm_pa, 0, sizeof(MSG_DBCheckPrimaryAccount));

		sm_pa.Size = sizeof(MSG_DBCheckPrimaryAccount);
		sm_pa.Type = _MSG_DBCheckPrimaryAccount;

		memcpy(&sm_pa.Mac, m->AdapterName, sizeof(sm_pa.Mac));

		strncpy(sm_pa.AccountName, pAccountList[Idx].File.Info.AccountName, ACCOUNTNAME_LENGTH);

		for (int i = 0; i < MAX_SERVER; i++)
		{
			if (auto puserDB = cSrv.getUserDB(i))
			{
				if (puserDB->cSock.Sock == 0 || puserDB->Mode == USER_EMPTY)
					continue;

				puserDB->cSock.SendOneMessage((BYTE*)&sm_pa, sizeof(MSG_DBCheckPrimaryAccount));
			}
		}
	}

	if (ChangeServer == 1)
	{
		DBWriteAccount(&file);

		pAccountList[Idx].SecurePass = 1;

		int Slot;
		int Server;
		int Enc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		//sepa isso aqui nem usa
		sscanf(m->Zero, "*%d %d %d %d %d %d %d %d %d %d", &Server, &Enc[0], &Enc[1], &Enc[2], &Enc[3], &Enc[4], &Enc[5], &Enc[6], &Enc[7], &Slot);

		pAccountList[Idx].Slot = Slot;

		MSG_CNFCharacterLogin sm;
		memset(&sm, 0, sizeof(MSG_CNFCharacterLogin));

		sm.Type = _MSG_DBCNFCharacterLogin;
		sm.ID = m->ID;
		sm.Size = sizeof(MSG_CNFCharacterLogin);

		sm.Slot = Slot;
		sm.mob = pAccountList[Idx].File.Char[Slot];
		sm.Donate = pAccountList[Idx].File.Donate;
		memcpy(sm.ShortSkill, pAccountList[Idx].File.ShortSkill[Slot], 16);
		memcpy(sm.affect, pAccountList[Idx].File.affect[Slot], sizeof(pAccountList[Idx].File.affect[Slot])); // ver
		memcpy(&sm.mobExtra, &pAccountList[Idx].File.mobExtra[Slot], sizeof(STRUCT_MOBExtra));

		char* mob = pAccountList[Idx].File.Char[Slot].MobName;

		if (mob[0] == 0)
		{
			cLog.Write(0, TL::Err, std::format("err,charlogin mobname empty {}", pAccountList[Idx].File.Info.AccountName));
			return false;
		}

		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFCharacterLogin));
	}

	return true;
}

bool CFileDBSRV::Exec_MSG_DBCreateCharacter(int conn, char* Msg)
{
	MSG_CreateCharacter* m = (MSG_CreateCharacter*)Msg;

	int Slot = m->Slot;
	int cls = m->MobClass;
	int Idx = GetIndex(conn, m->ID);

	if (Slot < 0 || Slot >= MOB_PER_ACCOUNT)
	{
		cLog.Write(0, TL::Err, std::format("err,newchar  slot out of range {}", pAccountList[Idx].File.Info.AccountName));
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	if (cls < 0 || cls >= 4)
	{
		cLog.Write(0, TL::Err, std::format("err,newchar - class out of range {}", pAccountList[Idx].File.Info.AccountName));
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	int Secure = pAccountList[Idx].SecurePass;

	if (Secure == -1)
	{
		cLog.Write(0, TL::Err, std::format("err,newchar secure illegal {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	char check[NAME_LENGTH];

	strncpy(check, m->MobName, NAME_LENGTH);
	_strupr(check);

	if (strcmp(check, "KING") == 0 || strcmp(check, "KINGDOM") == 0 || strcmp(check, "GRITAR") == 0 || strcmp(check, "RELO") == 0)
	{
		cLog.Write(0, TL::Err, std::format("err,newchar - cmd name {}", pAccountList[Idx].File.Info.AccountName));
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	if ((check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0) ||
		(check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0))
	{
		cLog.Write(0, TL::Err, std::format("err,newchar - com {}", pAccountList[Idx].File.Info.AccountName));
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	STRUCT_MOB* mob = &pAccountList[Idx].File.Char[Slot];

	mob->MobName[NAME_LENGTH - 1] = 0;
	mob->MobName[NAME_LENGTH - 2] = 0;

	m->MobName[NAME_LENGTH - 1] = 0;
	m->MobName[NAME_LENGTH - 2] = 0;

	if (mob->MobName[0] != 0)
	{
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,newchar already charged {} {} {}", pAccountList[Idx].File.Info.AccountName, mob->MobName, m->MobName));
		return false;
	}

	m->MobName[NAME_LENGTH - 1] = 0;
	m->MobName[NAME_LENGTH - 2] = 0;

	int len = strlen(m->MobName);

	for (int i = 0; i < len; i++)
	{
		if (m->MobName[i] == 'í' && m->MobName[i + 1] == 'í')
		{
			SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
			return false;
		}
	}

	int ret = CreateCharacter(pAccountList[Idx].File.Info.AccountName, m->MobName);

	if (ret == 0)
	{
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	STRUCT_MOBExtra* Extra = &pAccountList[Idx].File.mobExtra[Slot];

	BASE_ClearMob(mob);
	BASE_ClearMobExtra(Extra);

	memset(&pAccountList[Idx].File.affect[Slot], 0, sizeof(pAccountList[Idx].File.affect[Slot]));
	memset(&pAccountList[Idx].File.ShortSkill[Slot], -1, 16);
	Extra->ClassMaster = MORTAL;

	switch (cls)
	{
	case 0: memcpy(mob, &g_pBaseSet[0], sizeof(STRUCT_MOB)); break;
	case 1: memcpy(mob, &g_pBaseSet[1], sizeof(STRUCT_MOB)); break;
	case 2: memcpy(mob, &g_pBaseSet[2], sizeof(STRUCT_MOB)); break;
	case 3: memcpy(mob, &g_pBaseSet[3], sizeof(STRUCT_MOB)); break;
	default:
	{
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,newchar fail - undefined class {}", m->MobName));
		return false;
	}
	}

	Extra->MortalFace = mob->Equip[0].sIndex;
	memcpy(mob->MobName, m->MobName, NAME_LENGTH);
	ret = DBWriteAccount(&pAccountList[Idx].File);

	if (ret == 0)
	{
		SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,newchar fail - create file {}", m->MobName));
		return false;
	}

	cLog.Write(0, TL::Sys, std::format("create character {} {}", mob->MobName, pAccountList[Idx].File.Info.AccountName));

	MSG_CNFNewCharacter sm;
	memset(&sm, 0, sizeof(MSG_CNFNewCharacter));
	sm.Type = _MSG_DBCNFNewCharacter;

	DBGetSelChar(&sm.sel, &pAccountList[Idx].File);

	sm.ID = m->ID;
	if (auto userDB = cSrv.getUserDB(conn))
		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFNewCharacter));

	return true;
}

bool CFileDBSRV::Exec_MSG_DBCharacterLogin(int conn, char* Msg)
{
	MSG_CharacterLogin* m = (MSG_CharacterLogin*)Msg;

	int Slot = m->Slot;
	int Idx = GetIndex(conn, m->ID);

	if (Slot < 0 || Slot >= MOB_PER_ACCOUNT)
	{
		cLog.Write(0, TL::Err, std::format("err,charlogin slot illegal {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	int Secure = pAccountList[Idx].SecurePass;
	if (Secure == -1)
	{
		cLog.Write(0, TL::Err, std::format("err,charlogin secure illegal {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	pAccountList[Idx].Slot = Slot;

	MSG_CNFCharacterLogin sm;
	memset(&sm, 0, sizeof(MSG_CNFCharacterLogin)); // add

	sm.Type = _MSG_DBCNFCharacterLogin;
	sm.ID = m->ID;
	sm.Size = sizeof(MSG_CNFCharacterLogin);

	sm.Slot = Slot;

	sm.mob = pAccountList[Idx].File.Char[Slot];
	sm.Donate = pAccountList[Idx].File.Donate;

	memcpy(sm.ShortSkill, pAccountList[Idx].File.ShortSkill[Slot], 16);
	memcpy(sm.affect, pAccountList[Idx].File.affect[Slot], sizeof(pAccountList[Idx].File.affect[Slot]));
	memcpy(&sm.mobExtra, &pAccountList[Idx].File.mobExtra[Slot], sizeof(STRUCT_MOBExtra));

	char* mob = pAccountList[Idx].File.Char[Slot].MobName;

	if (mob[0] == 0)
	{
		cLog.Write(0, TL::Err, std::format("err,charlogin mobname empty {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFCharacterLogin));

	return true;
}

///////////////////////// SAVE ///////////////////////////

bool CFileDBSRV::Exec_MSG_DBNoNeedSave(int conn, char* Msg)
{
	MSG_STANDARD* m = (MSG_STANDARD*)Msg;

	int Idx = this->GetIndex(conn, m->ID);
	char acc[ACCOUNTNAME_LENGTH];
	strncpy(acc, pAccountList[Idx].File.Info.AccountName, ACCOUNTNAME_LENGTH); // desnecessario isso aqui
	this->RemoveAccountList(Idx);

	/*for (int i = 0; i < MAX_ADMIN; i++)
	{
		if (pAdmin[i].Level <= 2)
			continue;

		if (pAdmin[i].DisableID == 0)
			continue;

		if (pAdmin[i].DisableID != Idx)
			continue;

		DisableAccount(i, acc, pAdmin[i].Year, pAdmin[i].YearDay);
	}*/

	return true;
}

bool CFileDBSRV::Exec_MSG_DBSaveMob(int conn, char* Msg)
{
	MSG_DBSaveMob* m = (MSG_DBSaveMob*)Msg;

	int Idx = GetIndex(conn, m->ID);

	pAccountList[Idx].File.Info.AccountName[ACCOUNTNAME_LENGTH - 1] = 0;
	pAccountList[Idx].File.Info.AccountName[ACCOUNTNAME_LENGTH - 2] = 0;

	char acc[ACCOUNTNAME_LENGTH];
	strncpy(acc, pAccountList[Idx].File.Info.AccountName, ACCOUNTNAME_LENGTH);

	m->AccountName[ACCOUNTNAME_LENGTH - 1] = 0;
	m->AccountName[ACCOUNTNAME_LENGTH - 2] = 0;

	int Slot = pAccountList[Idx].Slot;

	if (Slot < 0 || Slot >= MOB_PER_ACCOUNT)
	{
		cLog.Write(0, TL::Err, std::format("-system err,savemob1 {} {} {} {} {}", m->ID, Slot, m->Slot, acc, m->AccountName));
		return false;
	}

	if (Slot != m->Slot)
	{
		cLog.Write(0, TL::Err, std::format("-system err,savemob2 {} {} {} {} {}", m->ID, Slot, m->Slot, acc, m->AccountName));
		return false; //precisa de retorno aqui? estava sem retorno, é um erro nao é?	
	}

	if (pAccountList[Idx].Login == 0)
	{
		cLog.Write(0, TL::Err, std::format("-system err,savemob3 {} {} {} {} {}", m->ID, Slot, m->Slot, acc, m->AccountName));
		return false;
	}

	pAccountList[Idx].File.Donate = m->Donate;
	pAccountList[Idx].File.Char[Slot] = m->MOB;
	pAccountList[Idx].File.mobExtra[Slot] = m->Extra;

	memcpy(&pAccountList[Idx].File.affect[Slot], m->affect, sizeof(pAccountList[Idx].File.affect[Slot]));
	memcpy(&pAccountList[Idx].File.Cargo, m->Cargo, sizeof(pAccountList[Idx].File.Cargo));

	memcpy(&pAccountList[Idx].File.ShortSkill[Slot], m->ShortSkill, 16);

	pAccountList[Idx].File.Coin = m->Coin;
	memcpy(&pAccountList[Idx].File.uExtra, &m->uExtra, sizeof(stUserExtra)); // eee adicionado

	DBWriteAccount(&pAccountList[Idx].File);

	if (m->Export != 0)
		DBExportAccount(&pAccountList[Idx].File);

	return true;
}

bool CFileDBSRV::Exec_MSG_SavingQuit(int conn, char* Msg)
{
	MSG_SavingQuit* m = (MSG_SavingQuit*)Msg;

	int Idx = this->GetIndex(conn, m->ID);
	int Slot = pAccountList[Idx].Slot;

	pAccountList[Idx].File.Char[Slot].MobName[NAME_LENGTH - 1] = 0;
	pAccountList[Idx].File.Char[Slot].MobName[NAME_LENGTH - 2] = 0;

	char acc[ACCOUNTNAME_LENGTH];
	strncpy(acc, pAccountList[Idx].File.Info.AccountName, ACCOUNTNAME_LENGTH);

	m->AccountName[ACCOUNTNAME_LENGTH - 1] = 0;
	m->AccountName[ACCOUNTNAME_LENGTH - 2] = 0;

	if (strncmp(m->AccountName, acc, ACCOUNTNAME_LENGTH))
	{
		cLog.Write(0, TL::Err, std::format("-system err,savenquit1 {} {} {} {} {}", m->ID, Slot, m->Slot, acc, m->AccountName));
		this->RemoveAccountList(Idx);
		return false;
	}

	if ((Slot < 0 || Slot >= MOB_PER_ACCOUNT) || (Slot != m->Slot))
	{
		cLog.Write(0, TL::Err, std::format("-system err,savenquit2 {} {} {} {} {}", m->ID, Slot, m->Slot, acc, m->AccountName));
		this->RemoveAccountList(Idx);
		return false;
	}

	if (pAccountList[Idx].Login == 0)
	{
		cLog.Write(0, TL::Err, std::format("-system err,savenquit3 {} {} {} {} {}", m->ID, Slot, m->Slot, acc, m->AccountName));
		//this->RemoveAccountList(Idx); // não precisa disso aqui?
		return false;
	}

	pAccountList[Idx].File.Char[Slot] = m->MOB;

	memcpy(pAccountList[Idx].File.Cargo, m->Cargo, sizeof(STRUCT_ITEM) * MAX_CARGO);
	memcpy(pAccountList[Idx].File.ShortSkill[Slot], m->ShortSkill, 16);

	pAccountList[Idx].File.mobExtra[Slot] = m->Extra;

	memcpy(&pAccountList[Idx].File.affect[Slot], m->affect, sizeof(pAccountList[Idx].File.affect[Slot]));
	memcpy(&pAccountList[Idx].File.uExtra, &m->uExtra, sizeof(stUserExtra)); // eee

	pAccountList[Idx].File.Coin = m->Coin;
	pAccountList[Idx].File.Donate = m->Donate;

	this->DBWriteAccount(&pAccountList[Idx].File);
	DBExportAccount(&pAccountList[Idx].File);
	RemoveAccountList(Idx);

	strcpy(pAccountList[Idx].File.BlockPass, m->BlockPass);
	pAccountList[Idx].File.IsBlocked = m->IsBlocked;

	this->SendDBSignal(conn, m->ID, _MSG_DBCNFAccountLogOut);

	/*for (int i = 0; i < MAX_ADMIN; i++)
	{
		if (pAdmin[i].Level <= 2 || pAdmin[i].DisableID == 0)
			continue;

		if (pAdmin[i].DisableID == Idx)
		{
			DisableAccount(i, acc, pAdmin[i].Year, pAdmin[i].YearDay);
			break;
		}
	}*/
	return true;
}

bool CFileDBSRV::Exec_MSG_AccountSecure(int conn, char* Msg)
{
	MSG_AccountSecure* m = (MSG_AccountSecure*)Msg;

	int Idx = GetIndex(conn, m->ID);
	int Secure = pAccountList[Idx].SecurePass;
	int Change = m->ChangeNumeric;

	if (Change && Secure == -1)
	{
		cLog.Write(0, TL::Err, std::format("err,secureverify change error 1 {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	if (pAccountList[Idx].File.Info.NumericToken[0] == -1)
	{
		strncpy(pAccountList[Idx].File.Info.NumericToken, m->NumericToken, 6);

		int ret = DBWriteAccount(&pAccountList[Idx].File);

		if (ret == 0)
		{
			cLog.Write(0, TL::Err, std::format("err,save secure - create file {}", pAccountList[Idx].File.Info.AccountName));
			return false;
		}

		pAccountList[Idx].SecurePass = 1;
		SendDBSignal(conn, m->ID, _MSG_AccountSecure);

		MSG_DBClientMessage sm2;
		memset(&sm2, 0, sizeof(MSG_DBClientMessage)); //add

		sm2.Type = _MSG_DBClientMessage;
		sm2.ID = m->ID;
		sm2.Size = sizeof(MSG_DBClientMessage);

		strncpy(sm2.String, "Senha definida com sucesso.", 28);

		if (auto userDB = cSrv.getUserDB(conn))
			userDB->cSock.SendOneMessage((BYTE*)&sm2, sizeof(MSG_DBClientMessage));
		return true;
	}

	if (Change == 0 && Secure == -1 && strncmp(pAccountList[Idx].File.Info.NumericToken, m->NumericToken, 6) == 0)
	{
		pAccountList[Idx].SecurePass = 1;
		SendDBSignal(conn, m->ID, _MSG_AccountSecure);
		return true;
	}

	else if (Change == 1 && Secure == 1)
	{
		strncpy(pAccountList[Idx].File.Info.NumericToken, m->NumericToken, 6);
		int ret = DBWriteAccount(&pAccountList[Idx].File);

		if (ret == 0)
		{
			cLog.Write(0, TL::Err, std::format("err,save secure - create file {}", pAccountList[Idx].File.Info.AccountName));
			return false;
		}

		pAccountList[Idx].SecurePass = 1;
		this->SendDBSignal(conn, m->ID, _MSG_AccountSecure);
		return true;
	}

	pAccountList[Idx].SecurePass = -1;
	this->SendDBSignal(conn, m->ID, _MSG_AccountSecureFail);
	return true;
}

bool CFileDBSRV::Exec_MSG_DBCreateArchCharacter(int conn, char* Msg)
{
	MSG_DBCreateArchCharacter* m = (MSG_DBCreateArchCharacter*)Msg;

	int cls = m->MobClass;
	int MortalFace = m->MortalFace;
	int Idx = this->GetIndex(conn, m->ID);
	int Slot = 0;

	for (Slot = 0; Slot < MOB_PER_ACCOUNT; Slot++)
	{
		if (pAccountList[Idx].File.Char[Slot].MobName[0] == 0)
			break;
	}

	if (Slot < 0 || Slot >= MOB_PER_ACCOUNT)
	{
		cLog.Write(0, TL::Err, std::format("err,newchar  slot out of range {}", pAccountList[Idx].File.Info.AccountName));
		this->SendDBSignal(conn, m->ID, _MSG_DBCNFArchCharacterFail);
		return false;
	}

	if (cls < 0 || cls >= MAX_CLASS)
	{
		cLog.Write(0, TL::Err, std::format("err,newchar - class out of range {}", pAccountList[Idx].File.Info.AccountName));
		this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	char check[NAME_LENGTH];

	strncpy(check, m->MobName, NAME_LENGTH);
	_strupr(check);

	if (strcmp(check, "KING") == 0 || strcmp(check, "KINGDOM") == 0 || strcmp(check, "GRITAR") == 0 || strcmp(check, "RELO") == 0)
	{
		cLog.Write(0, TL::Err, std::format("err,newchar - cmd name {}", pAccountList[Idx].File.Info.AccountName));
		this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	if ((check[0] == 'C' && check[1] == 'O' && check[2] == 'M' && check[3] >= '0' && check[3] <= '9' && check[4] == 0) ||
		(check[0] == 'L' && check[1] == 'P' && check[2] == 'T' && check[3] >= '0' && check[3] <= '9' && check[4] == 0))
	{
		cLog.Write(0, TL::Err, std::format("err,newchar - com {}", pAccountList[Idx].File.Info.AccountName));
		this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		return false;
	}

	STRUCT_MOB* mob = &pAccountList[Idx].File.Char[Slot];

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

	int len = strlen(m->MobName);

	for (int i = 0; i < len; i++)
	{
		if (m->MobName[i] == 'í' && m->MobName[i + 1] == 'í')
		{
			SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
			return false;
		}
	}

	STRUCT_MOBExtra* Extra = &pAccountList[Idx].File.mobExtra[Slot];

	BASE_ClearMob(mob);
	BASE_ClearMobExtra(Extra);

	memset(&pAccountList[Idx].File.affect[Slot], 0, sizeof(pAccountList[Idx].File.affect[Slot]));
	memset(&pAccountList[Idx].File.ShortSkill[Slot], -1, 16);

	Extra->ClassMaster = ARCH;
	Extra->QuestInfo.Arch.MortalSlot = m->MortalSlot;

	switch (cls)
	{
	case 0: memcpy(mob, &g_pBaseSet[0], sizeof(STRUCT_MOB)); break;
	case 1: memcpy(mob, &g_pBaseSet[1], sizeof(STRUCT_MOB)); break;
	case 2: memcpy(mob, &g_pBaseSet[2], sizeof(STRUCT_MOB)); break;
	case 3: memcpy(mob, &g_pBaseSet[3], sizeof(STRUCT_MOB)); break;
	default:
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,newchar fail - undefined class {}", m->MobName));
		return false;
	}
	}

	mob->Equip[0].sIndex = MortalFace + 5 + cls;
	mob->BaseScore.Ac = 230;

	memset(&mob->Equip[1], 0, sizeof(STRUCT_ITEM) * (MAX_EQUIP - 1));
	memset(&mob->Carry[0], 0, sizeof(STRUCT_ITEM) * (MAX_CARRY - 4));

	Extra->MortalFace = MortalFace;
	memcpy(mob->MobName, m->MobName, NAME_LENGTH);

	int ret = DBWriteAccount(&pAccountList[Idx].File);

	if (ret == 0)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBNewCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,newchar fail - create file {}", m->MobName));
		return false;
	}

	cLog.Write(0, TL::Sys, std::format("create character {} {}", m->MobName, pAccountList[Idx].File.Info.AccountName));

	MSG_CNFNewCharacter sm;
	memset(&sm, 0, sizeof(MSG_CNFNewCharacter)); // aDD
	sm.Type = _MSG_DBCNFNewCharacter;
	this->DBGetSelChar(&sm.sel, &pAccountList[Idx].File);
	sm.ID = m->ID;

	this->SendDBSignalParm(conn, m->ID, _MSG_DBCNFArchCharacterSucess, Slot);

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFNewCharacter));
	return true;
}

bool CFileDBSRV::Exec_MSG_DBDeleteCharacter(int conn, char* Msg)
{
	MSG_DeleteCharacter* m = (MSG_DeleteCharacter*)Msg;

	int Idx = GetIndex(conn, m->ID);
	int Slot = m->Slot;

	if (Slot < 0 || Slot >= MOB_PER_ACCOUNT)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBDeleteCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,deletechar slot {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	int Secure = pAccountList[Idx].SecurePass;

	if (Secure == -1)
	{
		cLog.Write(0, TL::Err, std::format("err,deletechar secure illegal {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	if (strncmp(m->Password, pAccountList[Idx].File.Info.AccountPass, ACCOUNTPASS_LENGTH) != 0)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBDeleteCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,deletechar password {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	STRUCT_MOB* mob = &pAccountList[Idx].File.Char[Slot];

	if (pAccountList[Idx].File.mobExtra[Slot].ClassMaster != MORTAL && pAccountList[Idx].File.mobExtra[Slot].ClassMaster != ARCH)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBDeleteCharacterFail);
		return false;
	}

	/*alterado para que não seja possivel deletar personagem arch*/
	if (mob->BaseScore.Level >= 219 || pAccountList[Idx].File.mobExtra[Slot].ClassMaster > MORTAL)
	{
		this->SendDBSignal(conn, m->ID, _MSG_DBDeleteCharacterFail);
		cLog.Write(0, TL::Err, std::format("err,deletechar level 219 {}", pAccountList[Idx].File.Info.AccountName));
		return false;
	}

	memset(pAccountList[Idx].File.ShortSkill[Slot], 0, 16);
	this->DeleteCharacter(mob->MobName, pAccountList[Idx].File.Info.AccountName);

	cLog.Write(0, TL::Sys, std::format("delete char {} {}", mob->MobName, pAccountList[Idx].File.Info.AccountName));

	BASE_ClearMob(mob);
	BASE_ClearMobExtra(&pAccountList[Idx].File.mobExtra[Slot]);

	DBWriteAccount(&pAccountList[Idx].File);

	MSG_CNFDeleteCharacter sm;
	memset(&sm, 0, sizeof(MSG_CNFDeleteCharacter)); // add

	sm.Type = _MSG_DBCNFDeleteCharacter;
	DBGetSelChar(&sm.sel, &pAccountList[Idx].File);
	sm.ID = m->ID;

	if (auto userDB = cSrv.getUserDB(conn))
		userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_CNFDeleteCharacter));
	return true;
}

bool CFileDBSRV::Exec_MSG_DBPrimaryAccount(int conn, char* Msg)
{
	MSG_DBPrimaryAccount* m = (MSG_DBPrimaryAccount*)Msg;

	int Idx = this->GetIndex(conn, m->ID);

	MSG_DBCheckPrimaryAccount sm;
	memset(&sm, 0, sizeof(MSG_DBCheckPrimaryAccount));

	sm.Size = sizeof(MSG_DBCheckPrimaryAccount);
	sm.Type = _MSG_DBCheckPrimaryAccount;

	memcpy(&sm.Mac, m->Mac, sizeof(sm.Mac));
	strncpy(sm.AccountName, pAccountList[Idx].File.Info.AccountName, ACCOUNTNAME_LENGTH);

	for (int i = 0; i < MAX_SERVER; i++)
	{
		if (auto userDB = cSrv.getUserDB(i))
		{
			if (userDB->cSock.Sock == 0 || userDB->Mode == USER_EMPTY)
				continue;

			userDB->cSock.SendOneMessage((BYTE*)&sm, sizeof(MSG_DBCheckPrimaryAccount));
		}
	}

	MSG_DBClientMessage sm2;
	memset(&sm2, 0, sizeof(MSG_DBClientMessage)); //add

	sm2.Type = _MSG_DBClientMessage;
	sm2.ID = m->ID;
	sm2.Size = sizeof(MSG_DBClientMessage);

	strncpy(sm2.String, "Sua conta agora é a primária.", 30);

	if (auto dbUser = cSrv.getUserDB(conn))
		dbUser->cSock.SendOneMessage((BYTE*)&sm2, sizeof(MSG_DBClientMessage));
	return true;
}

