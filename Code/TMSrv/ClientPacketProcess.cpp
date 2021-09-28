#include "pch.h"

bool CServer::ProcessClientMSG(int conn, char* pMsg, BOOL isServer)
{
	MSG_HEADER* m = (MSG_HEADER*)pMsg;

	if ((m->ID < 0) || (m->ID >= MAX_USER))
	{
		if (conn > 0 && conn < MAX_USER)
		{
			snprintf(cSrv.temp, sizeof(cSrv.temp), "err,packet Type:%d ID:%d Size:%d KeyWord:%d", m->Type, m->ID, m->Size, m->KeyWord);
			cLog.Write(conn, TL::Warn, cSrv.temp);
		}

		return false;
	}

	auto user = cSrv.getUser(conn);
	if (!user)
	{
		cLog.Write(0, TL::Warn, "retornou nullo user func [ProcessClientMSG]");
		return false;
	}

	if (m->Type == _MSG_Ping)
		return false;

	if (!isServer && m->ClientTick == SKIPCHECKTICK)
		return false;

	if (m->ID != conn && m->Type != 0x20D)
		m->ID = conn;

	switch (m->Type)
	{
	case _MSG_AccountLogin: return user->RequestAccountLogin(conn, m);
	case 0x0FDE: return user->RequestSecondPass(m);
	case _MSG_CreateCharacter: return user->RequestCreateChar(m);
	case _MSG_DeleteCharacter: return user->RequestDeleteCharacter(m);
	case _MSG_CharacterLogin: return user->RequestCharacterLogin(m);
	case _MSG_CharacterLogout: return user->RequestLogout(m);
	case _MSG_Action: 
	case _MSG_Action2: 
	case _MSG_Action3: 
		return user->RequestMovement(m);
	case _MSG_ChangeCity: return user->RequestChangeCity(m);
	case _MSG_ReqTeleport: return user->RequestTeleport(m);
	case _MSG_REQShopList: return user->RequestShopList(m);;
	case _MSG_Withdraw: return user->RequestWithdraw(m);
	case _MSG_Deposit: return user->RequestDeposit(m);
	case _MSG_TradingItem: return user->RequestMoveItem(m);
	case _MSG_DeleteItem: return user->RequestDeleteItem(m);
	case _MSG_SplitItem: return user->RequestSplitItem(m);
	case _MSG_MessageWhisper: user->RequestWhisper(m); break;
	case _MSG_MessageChat: user->RequestChat(m); break;

	}

	return true;
}