#include "pch.h"

bool CUser::RequestMoveItem(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_TradingItem))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [Exec_MSG_MoveItem]");
		return false;
	}

	auto mob = cSrv.getMob(this->conn);
	if (!mob)
		return false;

	if (mob->MOB.CurrentScore.Hp == 0 || this->Mode != USER_PLAY)
	{
		cSend.SendHpMode(this);
		//AddCrackError(conn, 1, 19);
		return false;
	}

	MSG_TradingItem* m = (MSG_TradingItem*)pMsg;

	if (this->Trade.OpponentID)
	{
		cSrv.RemoveTrade(this->Trade.OpponentID);
		cSrv.RemoveTrade(this->conn);
		return false;
	}

	if (this->TradeMode)
	{
		this->ClientMsg(g_pMessageStringTable[_NN_CantWhenAutoTrade]);
		return false;
	}

	if (m->SrcSlot == m->DestSlot && m->DestPlace == m->SrcPlace) // anti dup
	{
		cLog.Write(0, TL::Pakt, std::format("{} -packet TENTATIVA DE DUPAR", this->AccountName));
		cSrv.CloseUser(this->conn);
		return false;
	}

	if ((m->DestPlace == ITEM_PLACE_CARGO || m->SrcPlace == ITEM_PLACE_CARGO))
	{
		int WarpID = m->WarpID;
		if (auto pMob = cSrv.getMob(WarpID))
		{
			if (WarpID <= MAX_USER && WarpID >= MAX_MOB)
			{
				this->SendClientSignal(ESCENE_FIELD, 412);
				return false;
			}

			if (pMob[WarpID].Mode != MOB_PEACE || pMob[WarpID].MOB.Merchant != 2)
			{
				this->SendClientSignal(ESCENE_FIELD, 412);
				return false;
			}

			int PosX = pMob[conn].TargetX;
			int PosY = pMob[conn].TargetY;
			int tx = pMob[WarpID].TargetX;
			int ty = pMob[WarpID].TargetY;

			if (PosX < tx - VIEWGRIDX || PosX > tx + VIEWGRIDX || PosY < ty - VIEWGRIDY || PosY > ty + VIEWGRIDY)
			{
				this->SendClientSignal(ESCENE_FIELD, 412);
				return false;
			}
		}
	}
	STRUCT_ITEM* Mount = &mob->MOB.Equip[14];

	int DestPlace = 0;
	int SrcPlace = 0;

	STRUCT_ITEM* DestItem = GetItemPointer(&mob->MOB, this->Cargo, m->DestPlace, m->DestSlot);
	STRUCT_ITEM* SrcItem = GetItemPointer(&mob->MOB, this->Cargo, m->SrcPlace, m->SrcSlot);

	STRUCT_ITEM* dItem = GetItemPointer(&mob->MOB, this->Cargo, m->DestPlace, m->DestSlot);
	STRUCT_ITEM* sItem = GetItemPointer(&mob->MOB, this->Cargo, m->SrcPlace, m->SrcSlot);

	if (m->DestPlace)
	{
		if (m->DestPlace == ITEM_PLACE_CARRY)
		{
			if (m->DestSlot >= MAX_CARRY - 4)
			{
				cLog.Write(0, TL::Pakt, std::format("{} err,trading fails.SwapItem-Carry", this->AccountName));
				return false;
			}
		}
		else
		{
			if (m->DestPlace != ITEM_PLACE_CARGO)
			{
				cLog.Write(0, TL::Pakt, std::format("{} trading fails.SwapItem source type", this->AccountName));
				return false;
			}
			if (m->DestSlot > MAX_CARGO)
			{
				cLog.Write(0, TL::Pakt, std::format("{} err,trading fails.SwapItem-Cargo", this->AccountName));
				return false;
			}
		}
	}
	else
	{
		if (m->DestSlot < 0 || m->DestSlot >= MAX_EQUIP - 1)
		{
			cLog.Write(0, TL::Pakt, std::format("{} err,trading fails.SwapItem-Equip", this->AccountName));
			return false;
		}
		DestPlace = 1;
		SrcPlace = 1;
	}
	if (m->SrcPlace)
	{
		if (m->SrcPlace == ITEM_PLACE_CARRY)
		{
			if (m->SrcSlot >= MAX_CARRY - 4)
			{
				cLog.Write(0, TL::Pakt, std::format("{} DEBUG:Trading Fails.Swap-Carr", this->AccountName));
				return false;
			}
		}
		else
		{
			if (m->SrcPlace != ITEM_PLACE_CARGO)
			{
				cLog.Write(0, TL::Pakt, std::format("{} DEBUG:Trading Fails.SwapItem dest type", this->AccountName));
				return false;
			}
			if (m->SrcSlot > MAX_CARGO)
			{
				cLog.Write(0, TL::Pakt, std::format("{} DEBUG:Trading Fails.Swap-Cargo", this->AccountName));
				return false;
			}
		}
	}
	else
	{
		if (m->SrcSlot <= 0 || m->SrcSlot >= MAX_EQUIP - 1)
		{
			cLog.Write(0, TL::Pakt, std::format("{} DEBUG:Trading Fails.Swap-Equip", this->AccountName));
			return false;
		}

		DestPlace = 1;
		SrcPlace = 1;
	}

	if (m->DestPlace == ITEM_PLACE_CARRY && m->DestSlot >= mob->MaxCarry)
		return false;

	if (DestItem == NULL || SrcItem == NULL)
	{
		cLog.Write(0, TL::Pakt, std::format("{} DEBUG:Trading Fails.Swap-Equip", this->AccountName));
		return false;
	}

	if (DestItem->sIndex != 747 && SrcItem->sIndex != 747 || m->DestPlace != ITEM_PLACE_CARGO && m->SrcPlace != ITEM_PLACE_CARGO)
	{
		int isGuild = 0;

		if ((DestItem->sIndex == 508 || DestItem->sIndex == 522 || DestItem->sIndex >= 526 && DestItem->sIndex <= 537)
			&& !m->DestPlace && m->SrcPlace)
			isGuild = 1;

		int isGuildAgain = 0;

		if ((SrcItem->sIndex == 508 || SrcItem->sIndex == 522 || SrcItem->sIndex >= 526 && SrcItem->sIndex <= 537)
			&& !m->DestPlace && m->SrcPlace)
			isGuildAgain = 1;

		/*if ((isGuild == 1 || isGuildAgain == 1) && g_Week && pMob[conn].MOB.BaseScore.Level < 1000)
		{
			SendClientMsg(conn, g_pMessageStringTable[_NN_Only_Sunday]);
			return;
		}*/

		/*GetGuild(SrcItem);
		GetGuild(DestItem);*/

		int Can = 1;
		int YouCan = 1;

		if (dItem->sIndex)
		{
			int error = -2;

			if (m->SrcPlace == ITEM_PLACE_EQUIP)
				Can = BASE_CanEquip(dItem, &mob->MOB.CurrentScore, m->SrcSlot, mob->MOB.Class, mob->MOB.Equip, &mob->Extra);

			if (m->SrcPlace == ITEM_PLACE_CARRY)
			{
				if (!Can)
				{
					if (error > 0 && error <= mob->MaxCarry)
					{
						error--;
						this->SendItem(ITEM_PLACE_CARRY, error, &mob->MOB.Carry[error]);
					}
				}
			}
		}

		if (sItem->sIndex)
		{
			int error = -2;

			if (m->DestPlace == ITEM_PLACE_EQUIP)

				YouCan = BASE_CanEquip(sItem, &mob->MOB.CurrentScore, m->DestSlot, mob->MOB.Class, mob->MOB.Equip, &mob->Extra);

			if (m->DestPlace == ITEM_PLACE_CARRY)
			{
				if (!YouCan)
				{
					if (error > 0 && error <= mob->MaxCarry)
					{
						error--;
						this->SendItem(ITEM_PLACE_CARRY, error, &mob->MOB.Carry[error]);
					}
				}
			}
		}
		if (Can && YouCan)
		{

			STRUCT_ITEM save1;
			STRUCT_ITEM save2;

			save1.sIndex = SrcItem->sIndex;
			save1.stEffect[0].cEffect = SrcItem->stEffect[0].cEffect;
			save1.stEffect[0].cValue = SrcItem->stEffect[0].cValue;
			save1.stEffect[1].cEffect = SrcItem->stEffect[1].cEffect;
			save1.stEffect[1].cValue = SrcItem->stEffect[1].cValue;
			save1.stEffect[2].cEffect = SrcItem->stEffect[2].cEffect;
			save1.stEffect[2].cValue = SrcItem->stEffect[2].cValue;

			save2.sIndex = DestItem->sIndex;
			save2.stEffect[0].cEffect = DestItem->stEffect[0].cEffect;
			save2.stEffect[0].cValue = DestItem->stEffect[0].cValue;
			save2.stEffect[1].cEffect = DestItem->stEffect[1].cEffect;
			save2.stEffect[1].cValue = DestItem->stEffect[1].cValue;
			save2.stEffect[2].cEffect = DestItem->stEffect[2].cEffect;
			save2.stEffect[2].cValue = DestItem->stEffect[2].cValue;

			if (save1.sIndex == save2.sIndex && cGet.GetItemHab(save2.sIndex, EF_JOIN) && BASE_GetItemAmount(&save1) < 120 && BASE_GetItemAmount(&save2) < 120)
			{
				int amount1 = BASE_GetItemAmount(&save1);
				int amount2 = BASE_GetItemAmount(&save2);
				int tamount = (amount1 <= 0 ? 1 : amount1) + (amount2 <= 0 ? 1 : amount2);

				if (tamount <= 119)
				{
					save1.stEffect[0].cEffect = 61;
					save1.stEffect[0].cValue = tamount;

					BASE_ClearItem(&save2);

					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}
				else if (tamount >= 120)
				{
					save1.stEffect[0].cEffect = 61;
					save1.stEffect[0].cValue = 120;

					tamount -= 120;

					if (tamount >= 1)
						BASE_SetItemAmount(&save2, tamount > 120 ? 120 : tamount);

					if (tamount <= 0)
						BASE_ClearItem(&save2);

					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}
			}

			if (m->DestPlace == ITEM_PLACE_EQUIP)
			{
				if (save1.sIndex >= 3900 && save1.sIndex <= 3902 && save1.stEffect[0].cEffect == 0 && save1.stEffect[1].cEffect == 0 && save1.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save1, 2);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}
				if (save2.sIndex >= 3900 && save2.sIndex <= 3902 && save2.stEffect[0].cEffect == 0 && save2.stEffect[1].cEffect == 0 && save2.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save2, 2);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}
				if (save1.sIndex >= 3903 && save1.sIndex <= 3905 && save1.stEffect[0].cEffect == 0 && save1.stEffect[1].cEffect == 0 && save1.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save1, 4);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}
				if (save2.sIndex >= 3903 && save2.sIndex <= 3905 && save2.stEffect[0].cEffect == 0 && save2.stEffect[1].cEffect == 0 && save2.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save2, 4);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}
				if (save1.sIndex >= 3906 && save1.sIndex <= 3908 && save1.stEffect[0].cEffect == 0 && save1.stEffect[1].cEffect == 0 && save1.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save1, 6);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}
				if (save2.sIndex >= 3906 && save2.sIndex <= 3908 && save2.stEffect[0].cEffect == 0 && save2.stEffect[1].cEffect == 0 && save2.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save2, 6);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}
				if (save1.sIndex == 3913 && save1.stEffect[0].cEffect == 0 && save1.stEffect[1].cEffect == 0 && save1.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save1, 0);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}
				if (save2.sIndex == 3913 && save2.stEffect[0].cEffect == 0 && save2.stEffect[1].cEffect == 0 && save2.stEffect[2].cEffect == 0)
				{
					BASE_SetDateFairy(&save2, 0);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}
				if (save1.sIndex >= 4150 && save1.sIndex <= 4188 && save1.stEffect[0].cEffect == 0 && save1.stEffect[1].cEffect == 0 && save1.stEffect[2].cEffect == 0)
				{
					BASE_SetItemDate(&save1, 30);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}
				if (save2.sIndex >= 4150 && save2.sIndex <= 4188 && save2.stEffect[0].cEffect == 0 && save2.stEffect[1].cEffect == 0 && save2.stEffect[2].cEffect == 0)
				{
					BASE_SetItemDate(&save2, 30);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}

				if (save1.sIndex >= 3980 && save1.sIndex <= 3982 && save1.stEffect[0].cEffect == 0 && save1.stEffect[1].cEffect == 0 && save1.stEffect[2].cEffect == 0)
				{
					BASE_SetItemDate(&save1, 3);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}

				if (save2.sIndex >= 3980 && save2.sIndex <= 3982 && save2.stEffect[0].cEffect == 0 && save2.stEffect[1].cEffect == 0 && save2.stEffect[2].cEffect == 0)
				{
					BASE_SetItemDate(&save2, 3);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}

				if (save1.sIndex >= 3983 && save1.sIndex <= 3985 && save1.stEffect[0].cEffect == 0 && save1.stEffect[1].cEffect == 0 && save1.stEffect[2].cEffect == 0)
				{
					BASE_SetItemDate(&save1, 15);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}

				if (save2.sIndex >= 3983 && save2.sIndex <= 3985 && save2.stEffect[0].cEffect == 0 && save2.stEffect[1].cEffect == 0 && save2.stEffect[2].cEffect == 0)
				{
					BASE_SetItemDate(&save2, 15);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}

				if (save1.sIndex >= 3980 && save1.sIndex <= 3989 && BASE_CheckItemDate(&save1) || save1.sIndex >= 4150 && save1.sIndex <= 4188 && BASE_CheckItemDate(&save1))
				{
					BASE_ClearItem(&save1);
					this->SendItem(m->SrcPlace, m->SrcSlot, &save1);
				}

				if (save2.sIndex >= 3980 && save2.sIndex <= 3989 && BASE_CheckItemDate(&save2) || save2.sIndex >= 4150 && save2.sIndex <= 4188 && BASE_CheckItemDate(&save2))
				{
					BASE_ClearItem(&save2);
					this->SendItem(m->DestPlace, m->DestSlot, &save2);
				}
			}

			char itemLog[2048];
			char itemLog2[2048];

			BASE_GetItemCode(&save1, itemLog);
			BASE_GetItemCode(&save2, itemLog2);

			cLog.Write(this->conn, TL::Item, std::format("{} moveitem, SrcType:{} SrcSlot:{} Item:{} DstType:{} DstSlot:{} Item:{}", mob->MOB.MobName, m->SrcPlace, m->SrcSlot, itemLog, m->DestPlace, m->DestSlot, itemLog2));

			memcpy(SrcItem, &save2, sizeof(STRUCT_ITEM));
			memcpy(DestItem, &save1, sizeof(STRUCT_ITEM));


			/*if (save2.sIndex <= 40)
				GetGuild(SrcItem);

			if (save1.sIndex <= 40)
				GetGuild(DestItem);*/

			mob->GetCurrentScore(conn);

			this->cSock.AddMessage((BYTE*)m, sizeof(MSG_TradingItem));

			if (!mob->MOB.Equip[6].sIndex)
			{
				if (mob->MOB.Equip[7].sIndex)
				{
					int hab = BASE_GetItemAbility(&mob->MOB.Equip[7], EF_POS);

					if (hab != 128)
					{
						m->DestPlace = ITEM_PLACE_EQUIP;
						m->DestSlot = 6;
						m->SrcPlace = ITEM_PLACE_EQUIP;
						m->SrcSlot = 7;

						this->cSock.AddMessage((BYTE*)m, sizeof(MSG_TradingItem));

						mob->MOB.Equip[6] = mob->MOB.Equip[7];

						memset(&mob->MOB.Equip[7], 0, sizeof(STRUCT_ITEM));
					}
				}
			}

			if (DestPlace == ITEM_PLACE_CARRY)
				this->SendEquip(this->conn);

			if (SrcPlace == ITEM_PLACE_CARRY)
			{
				//GetGuild(conn);
				cSend.SendScore(this->conn);
			}

			if (m->DestPlace == ITEM_PLACE_EQUIP || m->SrcPlace == ITEM_PLACE_EQUIP) { // Correção para poder movimentar as crias com ela summonada e não ficar fazendo ela nascer toda hora
				if (DestItem->sIndex >= 2330 && DestItem->sIndex < 2360 || SrcItem->sIndex >= 2330 && SrcItem->sIndex < 2360)
					cSrv.MountProcess(conn, Mount);
			}

			STRUCT_ITEM* Storage2 = &this->Cargo[MAX_CARGO - 2];
			STRUCT_ITEM* LastStorage = &this->Cargo[MAX_CARGO - 1];

			if (m->DestPlace == ITEM_PLACE_CARGO || m->SrcPlace == ITEM_PLACE_CARGO)
			{
				if (Storage2->sIndex || LastStorage->sIndex)
				{
					for (int i = 0; i < 126; ++i)
					{
						if (Storage2->sIndex)
						{
							int cancargo = BASE_CanCargo(Storage2, this->Cargo, i % 9, i / 9);
							if (cancargo)
							{
								memcpy(&this->Cargo[i], Storage2, sizeof(STRUCT_ITEM));

								memset(Storage2, 0, sizeof(STRUCT_ITEM));
								this->SendItem(ITEM_PLACE_CARGO, i, &this->Cargo[i]);
							}
						}

						else
						{
							if (LastStorage->sIndex)
							{
								int cancargo = BASE_CanCargo(LastStorage, this->Cargo, i % 9, i / 9);
								if (cancargo)
								{
									memcpy(&this->Cargo[i], LastStorage, sizeof(STRUCT_ITEM));

									memset(LastStorage, 0, sizeof(STRUCT_ITEM));
									this->SendItem(ITEM_PLACE_CARGO, i, &this->Cargo[i]);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			STRUCT_ITEM save1;
			STRUCT_ITEM save2;

			save1.sIndex = SrcItem->sIndex;
			save1.stEffect[0].cEffect = SrcItem->stEffect[0].cEffect;
			save1.stEffect[0].cValue = SrcItem->stEffect[0].cValue;
			save1.stEffect[1].cEffect = SrcItem->stEffect[1].cEffect;
			save1.stEffect[1].cValue = SrcItem->stEffect[1].cValue;
			save1.stEffect[2].cEffect = SrcItem->stEffect[2].cEffect;
			save1.stEffect[2].cValue = SrcItem->stEffect[2].cValue;

			save2.sIndex = DestItem->sIndex;
			save2.stEffect[0].cEffect = DestItem->stEffect[0].cEffect;
			save2.stEffect[0].cValue = DestItem->stEffect[0].cValue;
			save2.stEffect[1].cEffect = DestItem->stEffect[1].cEffect;
			save2.stEffect[1].cValue = DestItem->stEffect[1].cValue;
			save2.stEffect[2].cEffect = DestItem->stEffect[2].cEffect;
			save2.stEffect[2].cValue = DestItem->stEffect[2].cValue;

			memcpy(SrcItem, &save1, sizeof(STRUCT_ITEM));
			memcpy(DestItem, &save2, sizeof(STRUCT_ITEM));
		}
	}

	return true;
}
