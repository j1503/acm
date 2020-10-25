#ifndef ACM_SDK_GAME_HPP
#define ACM_SDK_GAME_HPP
#pragma once

#include "entity.hpp"
#include <cstdint>

// gamemodes
#define m_lms(gamemode)         (gamemode==3 || gamemode==4 || gamemode==20)
#define m_ctf(gamemode)         (gamemode==5)
#define m_pistol(gamemode)      (gamemode==6 || gamemode==16 || gamemode==18)
#define m_lss(gamemode)         (gamemode==9 || gamemode==17 || gamemode==19)
#define m_osok(gamemode)        ((gamemode>=10 && gamemode<=12) || gamemode==21)
#define m_htf(gamemode)         (gamemode==13)
#define m_ktf(gamemode)         (gamemode==14 || gamemode==15)

#define m_teammode(gamemode)    (gamemode==0 || gamemode==4 || gamemode==5 || gamemode==7 || gamemode==11 || gamemode==13 || gamemode==14 || gamemode==16 || gamemode==17 || gamemode==20 || gamemode==21)
#define m_flags       (m_ctf || m_htf || m_ktf)

struct flaginfo
{
    int team;
    entity* flagent;
    int actor_cn;
    playerent* actor;
    vec pos;
    int state; // one of CTFF_*
    bool ack;
    flaginfo() : flagent(0), actor(0), state(CTFF_INBASE), ack(false) {}
};

class game {
public:
	uint32_t nextmode;
	uint32_t gamemode;
	uint32_t nextgamemode;
	uint32_t modeacronyms;

};

#endif
