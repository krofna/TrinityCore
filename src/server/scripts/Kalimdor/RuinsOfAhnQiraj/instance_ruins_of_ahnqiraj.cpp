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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ruins_of_ahnqiraj.h"

#define NUM_KALDOREI 4
#define ACTION_YELL_KURINAXX 2
// TODO: These may be incorrect. Contact Krofna for details
const Position AndorovSpawnPosition = { -8598.299f, 1459.903f, 32.33472f, 2.741698f };
const Position KaldoreiSpawnPositions[NUM_KALDOREI] =
{
    {},
    {},
    {},
    {}
};

class instance_ruins_of_ahnqiraj : public InstanceMapScript
{
    public:
        instance_ruins_of_ahnqiraj() : InstanceMapScript("instance_ruins_of_ahnqiraj", 509) {}

        struct instance_ruins_of_ahnqiraj_InstanceMapScript : public InstanceScript
        {
            instance_ruins_of_ahnqiraj_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(NUM_ENCOUNTER);

                _kurinaxxGUID   = 0;
                _rajaxxGUID     = 0;
                _moamGUID       = 0;
                _buruGUID       = 0;
                _ayamissGUID    = 0;
                _ossirianGUID   = 0;
                _paralyzedGUID  = 0;
                _andorovGUID    = 0;
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_KURINAXX:
                        _kurinaxxGUID = creature->GetGUID();
                        break;
                    case NPC_RAJAXX:
                        _rajaxxGUID = creature->GetGUID();
                        break;
                    case NPC_MOAM:
                        _moamGUID = creature->GetGUID();
                        break;
                    case NPC_BURU:
                        _buruGUID = creature->GetGUID();
                        break;
                    case NPC_AYAMISS:
                        _ayamissGUID = creature->GetGUID();
                        break;
                    case NPC_OSSIRIAN:
                        _ossirianGUID = creature->GetGUID();
                        break;
                    case NPC_ANDOROV:
                        _andorovGUID = creature->GetGUID();
                        break;
                }
            }

            bool SetBossState(uint32 bossId, EncounterState state)
            {
                if (!InstanceScript::SetBossState(bossId, state))
                    return false;

                return true;
            }

            void SetData64(uint32 type, uint64 data)
            {
                switch (type)
                {
                    case DATA_PARALYZED:
                        _paralyzedGUID = data;
                        break;
                    case DATA_ANDOROV:
                        _andorovGUID = data;
                        break;
                }
            }

            uint64 GetData64(uint32 type) const
            {
                switch (type)
                {
                    case DATA_KURINNAXX:
                        return _kurinaxxGUID;
                    case DATA_RAJAXX:
                        return _rajaxxGUID;
                    case DATA_MOAM:
                        return _moamGUID;
                    case DATA_BURU:
                        return _buruGUID;
                    case DATA_AYAMISS:
                        return _ayamissGUID;
                    case DATA_OSSIRIAN:
                        return _ossirianGUID;
                    case DATA_PARALYZED:
                        return _paralyzedGUID;
                    case DATA_ANDOROV:
                        return _andorovGUID;
                }

                return 0;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "R A" << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* data)
            {
                if (!data)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(data);

                char dataHead1, dataHead2;

                std::istringstream loadStream(data);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'R' && dataHead2 == 'A')
                {
                    for (uint8 i = 0; i < NUM_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > TO_BE_DECIDED)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
            
            void OnUnitDeath(Unit* unit)
            {
                if (unit->GetEntry() == NPC_KURINAXX)
                {
                    if (Creature* ossirian = instance->GetCreature(GetData64(DATA_OSSIRIAN)))
                        ossirian->AI()->DoAction(ACTION_YELL_KURINAXX);

                    instance->SummonCreature(NPC_ANDOROV, AndorovSpawnPosition);
                    
                    for (uint8 i = 0; i < NUM_KALDOREI; ++i)
                        instance->SummonCreature(NPC_KALDOREI_ELITE, KaldoreiSpawnPositions[i]);
                }
            }

        private:
            uint64 _kurinaxxGUID;
            uint64 _rajaxxGUID;
            uint64 _moamGUID;
            uint64 _buruGUID;
            uint64 _ayamissGUID;
            uint64 _ossirianGUID;
            uint64 _paralyzedGUID;
            uint64 _andorovGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_ruins_of_ahnqiraj_InstanceMapScript(map);
        }
};

void AddSC_instance_ruins_of_ahnqiraj()
{
    new instance_ruins_of_ahnqiraj();
}
