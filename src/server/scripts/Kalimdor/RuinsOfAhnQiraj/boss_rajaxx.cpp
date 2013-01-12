/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * RESEARCH:
(15471,0,0,14,100,"andorov SAY_ANDOROV_INTRO","They come now. Try not to get yourself killed, young blood."),
(15471,1,0,14,100,"andorov SAY_ANDOROV_ATTACK","Remember, Rajaxx, when I said I\'d kill you last? I lied..."),
(15341,0,8612,14,100,"rajaxx SAY_WAVE3","The time of our retribution is at hand! Let darkness reign in the hearts of our enemies!"),
(15341,1,8610,14,100,"rajaxx SAY_WAVE4","No longer will we wait behind barred doors and walls of stone! No longer will our vengeance be denied! The dragons themselves will tremble before our wrath!"),
(15341,2,8608,14,100,"rajaxx SAY_WAVE5","Fear is for the enemy! Fear and death!"),
(15341,3,8611,14,100,"rajaxx SAY_WAVE6","Staghelm will whimper and beg for his life, just as his whelp of a son did! One thousand years of injustice will end this day!"),
(15341,4,8607,14,100,"rajaxx SAY_WAVE7","Fandral! Your time has come! Go and hide in the Emerald Dream and pray we never find you!"),
(15341,5,8609,14,100,"rajaxx SAY_INTRO","Impudent fool! I will kill you myself!"),
(15341,6,8603,14,100,"rajaxx SAY_UNK1","Attack and make them pay dearly!"),
(15341,7,8605,14,100,"rajaxx SAY_UNK2","Crush them! Drive them out!"),
(15341,8,8606,14,100,"rajaxx SAY_UNK3","Do not hesitate! Destroy them!"),
(15341,9,8613,14,100,"rajaxx SAY_UNK4","Warriors! Captains! Continue the fight!"),
(15341,10,8614,14,100,"rajaxx SAY_DEAGGRO","You are not worth my time $N!"),
(15341,11,8604,14,100,"rajaxx SAY_KILLS_ANDOROV","Breath your last!"),
(15341,12,0,14,100,"rajaxx SAY_COMPLETE_QUEST","Soon you will know the price of your meddling, mortals... The master is nearly whole... And when he rises, your world will be cease!");
 * */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ruins_of_ahnqiraj.h"

enum Yells
{
    // The time of our retribution is at hand! Let darkness reign in the hearts of our enemies! Sound: 8645 Emote: 35
    SAY_ANDOROV_INTRO         = 0,
    SAY_ANDOROV_ATTACK        = 1,

    SAY_WAVE3                 = 0,
    SAY_WAVE4                 = 1,
    SAY_WAVE5                 = 2,
    SAY_WAVE6                 = 3,
    SAY_WAVE7                 = 4,
    SAY_AGGRO                 = 5,
    SAY_UNK1                  = 6,
    SAY_UNK2                  = 7,
    SAY_UNK3                  = 8,
    SAY_DEATH                 = 9,
    SAY_DEAGGRO               = 10,
    SAY_KILLS_ANDOROV         = 11,
    SAY_COMPLETE_QUEST        = 12    // Yell when realm complete quest 8743 for world event
    // Warriors, Captains, continue the fight! Sound: 8640
};

enum Spells
{
    // Rajaxx
    SPELL_DISARM            = 6713,
    SPELL_FRENZY            = 8269,
    SPELL_THUNDERCRASH      = 25599,

    // Andorov
    SPELL_AURA_OF_COMMAND   = 25516,
    SPELL_BASH              = 25515,
    SPELL_STRIKE            = 22591,

    // Kaldorei
    SPELL_CLEAVE            = 26350,
    SPELL_MORTAL_STRIKE     = 16856
};

enum GossipItems
{
    GOSSIP_TEXT_ID_INTRO = 7883,
    GOSSIP_TEXT_ID_TRADE = 8305
};

enum Events
{
    // Rajaxx
    EVENT_DISARM            = 1,
    EVENT_THUNDERCRASH      = 2,
    EVENT_CHANGE_AGGRO      = 3,
    
    // Andorov
    EVENT_BASH              = 4,
    EVENT_COMMAND_AURA      = 5,
    EVENT_STRIKE            = 6,

    // Kaldorei
    EVENT_MORTAL_STRIKE     = 7,
    EVENT_CLEAVE            = 8
};

enum Points
{
    POINT_
};

enum Actions
{
    ACTION_START_EVENT      = 0
};

// From spawn position to Rajaxx event position
const uint32 NumWaypoints = 2;
const Position Waypoints[NumWaypoints]
{
    {},
    {}
};

class boss_rajaxx : public CreatureScript
{
    public:
        boss_rajaxx() : CreatureScript("boss_rajaxx") { }

        struct boss_rajaxxAI : public BossAI
        {
            boss_rajaxxAI(Creature* creature) : BossAI(creature, DATA_RAJAXX)
            {
            }

            void Reset()
            {
                _Reset();
                _enraged = false;
            }
            
            void EnterCombat(Unit* /*victim*/)
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_DISARM, 10000);
                events.ScheduleEvent(EVENT_THUNDERCRASH, 12000);
                events.ScheduleEvent(EVENT_CHANGE_AGGRO, 0); // TODO
                _EnterCombat();
            }
            
            void KilledUnit(Unit* victim)
            {
                if (victim->GetEntry() == NPC_ANDOROV)
                    Talk(SAY_KILLS_ANDOROV);
            }

            void JustDied(Unit* /*killer*/)
            {
                Talk(SAY_DEATH);
                _JustDied();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                if (me->GetHealthPct() < 30 && !_enraged)
                {
                    _enraged = true;
                    DoCast(me, SPELL_FRENZY);
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DISARM:
                            DoCastVictim(SPELL_DISARM);
                            events.ScheduleEvent(EVENT_DISARM, 22000);
                            break;
                        case EVENT_THUNDERCRASH:
                            DoCast(me, SPELL_THUNDERCRASH);
                            DoResetThreat();
                            events.ScheduleEvent(EVENT_THUNDERCRASH, 21000);
                            break;
                        case EVENT_CHANGE_AGGRO:// TODO
                            Talk(SAY_DEAGGRO);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
            private:
                bool _enraged;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_rajaxxAI(creature);
        }
};

class npc_andorov : public CreatureScript
{
    public:
        npc_andorov() : CreatureScript("npc_andorov") { }

        struct npc_andorovAI : public ScriptedAI
        {
            npc_andorovAI(Creature* creature) : ScriptedAI(creature)
            {
            }
            
            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_START_EVENT:
                        me->GetMotionMaster()->MovePoint(0, Waypoints[0]);
                        Talk(SAY_ANDOROV_ATTACK);
                        break;
                }
            }
            
            void EnterCombat(Unit* /*victim*/)
            {
                _events.ScheduleEvent(EVENT_BASH, urand(8000, 11000));
                _events.ScheduleEvent(EVENT_COMMAND_AURA, urand(1000, 3000));
                _events.ScheduleEvent(EVENT_STRIKE, urand(2000, 5000));
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (who->GetEntry() == NPC_RAJAXX && me->IsWithinDistInMap(who, 50.0f))
                    AttackStart(who);
                else
                    ScriptedAI::MoveInLineOfSight(who);
            }
            
            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE)
                {
                    if (id < NumWaypoints)
                        me->GetMotionMaster()->MovePoint(id+1, Waypoints[id+1]);

                    if (id == NumWaypoints-1)
                        Talk(SAY_ANDOROV_INTRO);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BASH:
                            DoCastVictim(SPELL_BASH);
                            _events.ScheduleEvent(EVENT_BASH, urand(12000, 15000));
                            break;
                        case EVENT_COMMAND_AURA:
                            DoCast(me, SPELL_AURA_OF_COMMAND);
                            _events.ScheduleEvent(EVENT_COMMAND_AURA, urand(30000, 45000));
                            break;
                        case EVENT_STRIKE:
                            DoCastVictim(SPELL_STRIKE);
                            _events.ScheduleEvent(EVENT_STRIKE, urand(4000, 6000));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
            private:
                EventMap _events;
        };

        bool OnGossipHello(Player* player, Creature* creature)
        {
            InstanceScript* instance = creature->GetInstanceScript();

            if (!instance || instance->GetBossState(DATA_RAJAXX) == IN_PROGRESS)
                return true;

            if (instance->GetBossState(DATA_RAJAXX) == NOT_STARTED || instance->GetBossState(DATA_RAJAXX) == FAIL)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let's find out.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Let's see what you have.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_ID_INTRO, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    creature->AI()->DoAction(ACTION_START_EVENT);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_TRADE:
                    player->GetSession()->SendListInventory(creature->GetGUID());
                    break;
            }
            return true;
        }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_andorovAI(creature);
        }
};

class npc_kaldorei_elite : public CreatureScript
{
    public:
        npc_kaldorei_elite() : CreatureScript("npc_kaldorei_elite") { }

        struct npc_kaldorei_eliteAI : public ScriptedAI
        {
            npc_kaldorei_eliteAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = me->GetInstanceScript();
            }

            void EnterCombat(Unit* /*victim*/)
            {
                _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(8000, 11000));
                _events.ScheduleEvent(EVENT_CLEAVE, urand(2000, 4000));
            }
            
            void EnterEvadeMode()
            {
                _EnterEvadeMode();
                // TODO: no me->GetStuff, but instead make it nicer with some constant formation
                if (Creature* andorov = me->GetMap()->GetCreature(_instance->GetData64(DATA_ANDOROV)))
                    me->GetMotionMaster()->MoveFollow(andorov, me->GetDistance(andorov), me->GetAngle(andorov));
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(9000, 13000));
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(SPELL_CLEAVE);
                            _events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 7000));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
            private:
                EventMap _events;
                InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_kaldorei_eliteAI(creature);
        }
};

void AddSC_boss_rajaxx()
{
    new boss_rajaxx();
    new npc_andorov();
    new npc_kaldorei_elite();
}
