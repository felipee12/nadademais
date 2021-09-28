#include "pch.h"

void _geral(MSG_MessageChat* m)
{
	//cade as alteração+
	//porraaaaaaaaaaaaaaaaaaa
	m->String[MESSAGE_LENGTH - 1] = 0;
	m->String[MESSAGE_LENGTH - 2] = 0;

	if (auto user = cSrv.getUser(m->ID))
	{
		if (user->MuteChat == 1)
		{
			user->ClientMsg(g_pMessageStringTable[_NN_No_Speak]);
			return;
		}

		auto mob = cSrv.getMob(m->ID);
		if (!mob)
			return;

		if (user->Mode == USER_PLAY) 
		{
			int leader = mob->Leader;
			if (!leader)
				leader = m->ID;

			cSend.GridMulticast(mob->TargetX, mob->TargetY, (MSG_HEADER*)m, m->ID); // checar o cast da msg
		}
		else
		{
			user->ClientMsg("DEBUG:Client send chatting message with wrong status");
			cLog.Write(0, TL::Pakt, std::format("{} err,send chatting message with wrong status", user->AccountName));
		}

		cLog.Write(0, TL::Pakt, std::format("{} chat, {} : {}", user->AccountName, mob->MOB.MobName, m->String));
	}
	else
		cLog.Write(0, TL::Pakt, std::format("fodase"));
}

void CUser::RequestChat(MSG_HEADER* pMsg)
{
	if (pMsg->Size != sizeof(MSG_MessageChat))
	{
		cLog.Write(0, TL::Pakt, "-packet Size Diferente [MSG_MessageChat]");
		return;
	}

	MSG_MessageChat* m = reinterpret_cast<MSG_MessageChat*>(pMsg);

	// Ingame só é possivel enviar 46 caracteres sem manipular pacote então estou copiando apenas 46 caracteres
	if (strlen(m->String) > 46)
		return;

	if (!CheckString(m->String, 50))
		return;

	if (this->Mode != USER_PLAY)
		return;

	m->String[47] = '\0';
	m->ID = conn;

	/*switch (m->Color)
	{
	case chatMSG::guildon:		cMsg._guildon(m);		return;
	case chatMSG::guildoff:		cMsg._guildoff(m);		return;
	case chatMSG::guildtax:		cMsg._guildtax(m);		return;
	case chatMSG::guild:		cMsg._guild(m);			return;
	case chatMSG::whisper:		cMsg._whisper(m);		return;
	case chatMSG::partychat:	cMsg._partychat(m);		return;
	case chatMSG::kingdomchat:	cMsg._kingdomchat(m);	return;
	case chatMSG::guildchat:	cMsg._guildchat(m);		return;
	case chatMSG::chatting:		cMsg._chatting(m);		return;
	}*/

	_geral(m);
}