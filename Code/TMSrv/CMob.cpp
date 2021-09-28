#include "pch.h"

CMob::CMob()
{
	Mode = MOB_EMPTY;
	LastX = 0;//0xFFFFFFFF;
	LastY = 0;//0xFFFFFFFF;
	Leader = 0;
	WaitSec = 0;
	PotionCount = 0;
	Summoner = 0;
	GuildDisable = 0;
	LastReqParty = 0;
	ProcessorCounter = 0;
	ClientIndex = 0;
	CheckQuest = 0;

	memset(&MOB, 0, sizeof(STRUCT_MOB));
	memset(&Tab, 0, sizeof(Tab));
	memset(&Snd, 0, sizeof(Snd));
	memset(&Extra, 0, sizeof(STRUCT_MOBExtra));
}

CMob::~CMob()
{
}

void CMob::GetCurrentScore(int idx)
{
	if (idx < MAX_USER)
	{
		this->MOB.Clan = 0;

		this->MOB.Resist[0] = BASE_GetMobAbility(&MOB, EF_RESIST1) >= 100 ? 100 : BASE_GetMobAbility(&MOB, EF_RESIST1);
		this->MOB.Resist[1] = BASE_GetMobAbility(&MOB, EF_RESIST2) >= 100 ? 100 : BASE_GetMobAbility(&MOB, EF_RESIST2);
		this->MOB.Resist[2] = BASE_GetMobAbility(&MOB, EF_RESIST3) >= 100 ? 100 : BASE_GetMobAbility(&MOB, EF_RESIST3);
		this->MOB.Resist[3] = BASE_GetMobAbility(&MOB, EF_RESIST4) >= 100 ? 100 : BASE_GetMobAbility(&MOB, EF_RESIST4);

		this->MOB.Equip[0].stEffect[0].cEffect = 0;
		this->MOB.Equip[0].stEffect[0].cValue = 0;

		this->MOB.Rsv = 0;

		this->Parry = BASE_GetMobAbility(&MOB, EF_PARRY); // mob nao tem evasao?
		//pUser[idx].Range = BASE_GetMobAbility(&MOB, EF_RANGE);

		cSrv.getUser(idx)->Range = 23;

		this->ExpBonus = 0;
		this->DropBonus = 0;
		this->ForceDamage = 0;
		this->ReflectDamage = 0;
		this->ForceMobDamage = 0;
		this->HpAbs = 0;

		switch (this->MOB.Equip[13].sIndex)
		{// mob nao tem necessidade de checar se ta usando fada pra q ele usaria os bonus?
		case 3900: this->ExpBonus += 16; break; //Fada Verde 3D
		case 3901: this->DropBonus += 32; break; //Fada Azul 3D
		case 3902: case 3905: case 3908: //Fada Vermelha
		{
			this->ExpBonus += 32;
			this->DropBonus += 16;
		}break;
		case 3903: case 3906: case 3911: case 3912: this->ExpBonus += 16; break; //Fada Verde
		case 3904: case 3907: this->ExpBonus += 32; break; //Fada Verde Azul 
		}

		for (int resist = 0; resist < 4; resist++)
		{//check de resistencia < 0 loop pra array toda
			if (this->MOB.Resist[resist] < 0)
				this->MOB.Resist[resist] = 0;
		}

#pragma region >> ARMAS CHECK
		int w1 = BASE_GetItemAbility(&MOB.Equip[6], EF_DAMAGE);
		int w2 = BASE_GetItemAbility(&MOB.Equip[7], EF_DAMAGE);

		int fw1 = (w1 / 2);
		int fw2 = (w2 / 2);

		if (this->MOB.Class == 0 && this->MOB.LearnedSkill & (1 << 9))
		{//Mestre das Armas - TK
			fw1 = w1;
			fw2 = w2;
		}

		if (this->MOB.Class == 2)
		{//BM
			if (MOB.LearnedSkill & (1 << 17)) ReflectDamage += ((MOB.CurrentScore.Special[3] + 1) / 6);
			if (MOB.LearnedSkill & (1 << 19) && g_pItemList[MOB.Equip[7].sIndex].nPos == 128) MOB.CurrentScore.Ac += (BASE_GetItemAbility(&MOB.Equip[7], EF_AC) + 1) / 7;
		}

		if (w1 >= w2)
			this->WeaponDamage = w1 + fw2;
		else
			this->WeaponDamage = w2 + fw1;

		if (this->MOB.Equip[6].sIndex >= 0 && this->MOB.Equip[6].sIndex < MAX_ITEMLIST)
		{//check para adicionar 40 de dano para armas fisicas +9 na primeira mão
			if (g_pItemList[this->MOB.Equip[6].sIndex].nPos == 64 || g_pItemList[this->MOB.Equip[6].sIndex].nPos == 192)
			{
				if (BASE_GetItemSanc(&MOB.Equip[6]) >= 9)
					this->WeaponDamage += 40;
			}
		}

		if (this->MOB.Equip[7].sIndex >= 0 && this->MOB.Equip[7].sIndex < MAX_ITEMLIST)
		{//check para adicionar 40 de dano para armas fisicas +9 na segunda mão
			if (g_pItemList[MOB.Equip[7].sIndex].nPos == 64 || g_pItemList[MOB.Equip[7].sIndex].nPos == 192)
			{
				if (BASE_GetItemSanc(&MOB.Equip[7]) >= 9)
					this->WeaponDamage += 40;
			}
		}
#pragma endregion

		for (int i = 1; i <= 7; i++)
		{//check do slot 1 (elmo) ao 7 (arma da segunda mão) pois são os unicos q recebem os atributos checados no loop
			if (!this->MOB.Equip[i].sIndex)
				continue; // se não tiver item no slot continua no proximo indice

			int ItemId = this->MOB.Equip[i].sIndex;
			int itemGem = BASE_GetItemGem(&MOB.Equip[i]);
			int itemSanc = BASE_GetItemSanc(&MOB.Equip[i]);

			if (ItemId <= 0 || ItemId >= MAX_ITEMLIST)
				continue;// checando >= pq só tava >

			switch (g_pItemList[ItemId].Grade)
			{
			case 5: this->DropBonus += 8; break;
			case 6: this->ForceDamage += i == 20; break;
			case 7: this->ExpBonus += 2; break;
			case 8: this->ReflectDamage += 20; break;
			}

			int isanc = 0;

			switch (itemSanc)
			{
			case REF_10: isanc = 1; break;
			case REF_11: isanc = 2; break;
			case REF_12: isanc = 3; break;
			case REF_13: isanc = 4; break;
			case REF_14: isanc = 5; break;
			case REF_15: isanc = 6; break;
			}

			switch (itemGem)
			{
			case 0: this->DropBonus += 8; break;
			case 1: this->ForceDamage += (g_pItemList[ItemId].Grade == 6 ? 80 : 40) * isanc; break;
			case 2: this->ExpBonus += 2; break;
			case 3: this->ReflectDamage += (g_pItemList[ItemId].Grade == 8 ? 80 : 40) * isanc; break;
			}
		}

		for (int j = 0; j < MAX_AFFECT; ++j)
		{//check pra joia da precisão
			auto affectType = Affect[j].Type;
			if (affectType == 0)
				continue;

			if (affectType == 8)
				this->MOB.Jewel = Affect[j].Level;
		}

		this->PvPDamage = 0;
		this->ReflectPvP = 0;

		int AtaquePvP = BASE_GetMobAbility(&MOB, EF_HWORDGUILD);
		AtaquePvP = (AtaquePvP + 1) / 10;
		this->PvPDamage = AtaquePvP;

		int DefesaPvP = BASE_GetMobAbility(&MOB, EF_LWORDGUILD);
		DefesaPvP = (DefesaPvP + 1) / 10;
		this->ReflectPvP = DefesaPvP;
	}
	else
	{//checagem de mobs
		if (this->GenerateIndex > 0 && this->GenerateIndex < MAX_NPCGENERATOR)
		{
			this->MOB.Resist[0] = cSrv.mNPCGen.pList[this->GenerateIndex].Leader.Resist[0];
			this->MOB.Resist[1] = cSrv.mNPCGen.pList[this->GenerateIndex].Leader.Resist[1];
			this->MOB.Resist[2] = cSrv.mNPCGen.pList[this->GenerateIndex].Leader.Resist[2];
			this->MOB.Resist[3] = cSrv.mNPCGen.pList[this->GenerateIndex].Leader.Resist[3];

			for (int i = 0; i < 4; i++)
			{//check resist dos mob, como tava checando se é maior q 100 e passando pra 50 dps? lol
				if (this->MOB.Resist[i] > 50)
					this->MOB.Resist[i] = 50;
			}
		}

		this->MOB.Rsv = 0;
	}

	BASE_GetCurrentScore(MOB, Affect, &Extra, &ExpBonus, &ForceMobDamage, idx >= MAX_USER ? 1 : 0, &HpAbs, &ForceDamage);

	if (this->MOB.CurrentScore.Hp > this->MOB.CurrentScore.MaxHp)
		this->MOB.CurrentScore.Hp = this->MOB.CurrentScore.MaxHp;

	if (this->MOB.CurrentScore.Mp > this->MOB.CurrentScore.MaxMp)
		this->MOB.CurrentScore.Mp = this->MOB.CurrentScore.MaxMp;
}