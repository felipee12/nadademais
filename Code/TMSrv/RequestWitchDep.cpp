#include "pch.h"

bool CUser::RequestDeposit(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_STANDARDPARM))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [Exec_MSG_Deposit]");
		return false;
	}

	auto mob = cSrv.getMob(this->conn);
	if (!mob)
		return false;

	if (mob->MOB.CurrentScore.Hp == 0 || this->Mode != USER_PLAY)
	{
		cSend.SendHpMode(this);
		//AddCrackError(conn, 10, 1);
		return false;
	}

	MSG_STANDARDPARM* m = (MSG_STANDARDPARM*)pMsg;

	int coin = m->Parm;

	if (mob->MOB.Coin >= coin && coin >= 0 && coin <= 2000000000)
	{
		int tcoin = coin + this->Coin;

		if (tcoin >= 0 && tcoin <= 2000000000)
		{
			mob->MOB.Coin -= coin;
			this->Coin = tcoin;
			m->ID = ESCENE_FIELD;

			this->cSock.AddMessage((BYTE*)m, sizeof(MSG_STANDARDPARM));
			this->SendCargoCoin();

			cLog.Write(this->conn, TL::Pakt, std::format("{} etc,deposito no bau D:{} R:{}", this->AccountName, coin, tcoin));
		}
		else
			this->ClientMsg(g_pMessageStringTable[_NN_Cant_get_more_than_2G]);
	}
	else
		this->ClientMsg(g_pMessageStringTable[_NN_Cant_Deposit_That_Much]);

	return true;
}

bool CUser::RequestWithdraw(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_STANDARDPARM))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [Exec_MSG_Withdraw]");
		return false;
	}

	MSG_STANDARDPARM* m = (MSG_STANDARDPARM*)pMsg;

	auto mob = cSrv.getMob(this->conn);
	if (!mob)
		return false;

	if (mob->MOB.CurrentScore.Hp == 0 || this->Mode != USER_PLAY)
	{
		cSend.SendHpMode(this);
		//AddCrackError(conn, 10, 2);
		return false;
	}

	int coin = m->Parm;

	if (this->Coin >= coin && coin >= 0 && coin <= 2000000000)
	{
		int tcoin = coin + mob->MOB.Coin;

		if (tcoin >= 0 && tcoin <= 2000000000)
		{
			mob->MOB.Coin = tcoin;
			this->Coin -= coin;

			m->ID = 30000;

			this->cSock.AddMessage((BYTE*)m, sizeof(MSG_STANDARDPARM));
			this->SendCargoCoin();

			cLog.Write(this->conn, TL::Pakt, std::format("{} etc,retirada do bau V:{} R:{}", this->AccountName, coin, this->Coin));
		}
		else
			this->ClientMsg(g_pMessageStringTable[_NN_Cant_get_more_than_2G]);
	}
	else
		this->ClientMsg(g_pMessageStringTable[_NN_Cant_Withdraw_That_Much]);

	return true;
}