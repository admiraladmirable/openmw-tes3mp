
#include "weapon.hpp"

#include <components/esm/loadweap.hpp>

#include <components/esm_store/cell_store.hpp>

#include "../mwworld/ptr.hpp"
#include "../mwworld/actiontake.hpp"
#include "../mwworld/environment.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/world.hpp"

#include "../mwgui/window_manager.hpp"
#include "../mwgui/tooltips.hpp"

#include "../mwrender/objects.hpp"

#include "../mwsound/soundmanager.hpp"

namespace MWClass
{
    void Weapon::insertObjectRendering (const MWWorld::Ptr& ptr, MWRender::RenderingInterface& renderingInterface) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        assert (ref->base != NULL);
        const std::string &model = ref->base->model;

        if (!model.empty())
        {
            MWRender::Objects& objects = renderingInterface.getObjects();
            objects.insertBegin(ptr, ptr.getRefData().isEnabled(), false);
            objects.insertMesh(ptr, "meshes\\" + model);
        }
    }

    void Weapon::insertObject(const MWWorld::Ptr& ptr, MWWorld::PhysicsSystem& physics, MWWorld::Environment& environment) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();


        const std::string &model = ref->base->model;
        assert (ref->base != NULL);
        if(!model.empty()){
            physics.insertObjectPhysics(ptr, "meshes\\" + model);
        }

    }

    std::string Weapon::getName (const MWWorld::Ptr& ptr) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        return ref->base->name;
    }

    boost::shared_ptr<MWWorld::Action> Weapon::activate (const MWWorld::Ptr& ptr,
        const MWWorld::Ptr& actor, const MWWorld::Environment& environment) const
    {
        environment.mSoundManager->playSound3D (ptr, getUpSoundId(ptr, environment), 1.0, 1.0, MWSound::Play_NoTrack);

        return boost::shared_ptr<MWWorld::Action> (
            new MWWorld::ActionTake (ptr));
    }

    bool Weapon::hasItemHealth (const MWWorld::Ptr& ptr) const
    {
        return true;
    }

    int Weapon::getItemMaxHealth (const MWWorld::Ptr& ptr) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        return ref->base->data.health;
    }

    std::string Weapon::getScript (const MWWorld::Ptr& ptr) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        return ref->base->script;
    }

    std::pair<std::vector<int>, bool> Weapon::getEquipmentSlots (const MWWorld::Ptr& ptr) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        std::vector<int> slots;
        bool stack = false;

        if (ref->base->data.type==ESM::Weapon::Arrow || ref->base->data.type==ESM::Weapon::Bolt)
        {
            slots.push_back (int (MWWorld::InventoryStore::Slot_Ammunition));
            stack = true;
        }
        else if (ref->base->data.type==ESM::Weapon::MarksmanThrown)
        {
            slots.push_back (int (MWWorld::InventoryStore::Slot_CarriedRight));
            stack = true;
        }
        else
            slots.push_back (int (MWWorld::InventoryStore::Slot_CarriedRight));

        return std::make_pair (slots, stack);
    }

    int Weapon::getEquipmentSkill (const MWWorld::Ptr& ptr,
        const MWWorld::Environment& environment) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        const int size = 12;

        static const int sMapping[size][2] =
        {
            { ESM::Weapon::ShortBladeOneHand, ESM::Skill::ShortBlade },
            { ESM::Weapon::LongBladeOneHand, ESM::Skill::LongBlade },
            { ESM::Weapon::LongBladeTwoHand, ESM::Skill::LongBlade },
            { ESM::Weapon::BluntOneHand, ESM::Skill::BluntWeapon },
            { ESM::Weapon::BluntTwoClose, ESM::Skill::BluntWeapon },
            { ESM::Weapon::BluntTwoWide, ESM::Skill::BluntWeapon },
            { ESM::Weapon::SpearTwoWide, ESM::Skill::Spear },
            { ESM::Weapon::AxeOneHand, ESM::Skill::Axe },
            { ESM::Weapon::AxeTwoHand, ESM::Skill::Axe },
            { ESM::Weapon::MarksmanBow, ESM::Skill::Marksman },
            { ESM::Weapon::MarksmanCrossbow, ESM::Skill::Marksman },
            { ESM::Weapon::MarksmanThrown, ESM::Skill::Marksman }
        };

        for (int i=0; i<size; ++i)
            if (sMapping[i][0]==ref->base->data.type)
                return sMapping[i][1];

        return -1;
    }

    int Weapon::getValue (const MWWorld::Ptr& ptr) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        return ref->base->data.value;
    }

    void Weapon::registerSelf()
    {
        boost::shared_ptr<Class> instance (new Weapon);

        registerClass (typeid (ESM::Weapon).name(), instance);
    }

    std::string Weapon::getUpSoundId (const MWWorld::Ptr& ptr, const MWWorld::Environment& environment) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        int type = ref->base->data.type;
        // Ammo
        if (type == 12 || type == 13)
        {
            return std::string("Item Ammo Up");
        }
        // Bow
        if (type == 9)
        {
            return std::string("Item Weapon Bow Up");
        }
        // Crossbow
        if (type == 10)
        {
            return std::string("Item Weapon Crossbow Up");
        }
        // Longblades, One hand and Two
        if (type == 1 || type == 2)
        {
            return std::string("Item Weapon Longblade Up");
        }
        // Shortblade and thrown weapons
        // thrown weapons may not be entirely correct
        if (type == 0 || type == 11)
        {
            return std::string("Item Weapon Shortblade Up");
        }
        // Spear
        if (type == 6)
        {
            return std::string("Item Weapon Spear Up");
        }
        // Blunts and Axes
        if (type == 3 || type == 4 || type == 5 || type == 7 || type == 8)
        {
            return std::string("Item Weapon Blunt Up");
        }

        return std::string("Item Misc Up");
    }

    std::string Weapon::getDownSoundId (const MWWorld::Ptr& ptr, const MWWorld::Environment& environment) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        int type = ref->base->data.type;
        // Ammo
        if (type == 12 || type == 13)
        {
            return std::string("Item Ammo Down");
        }
        // Bow
        if (type == 9)
        {
            return std::string("Item Weapon Bow Down");
        }
        // Crossbow
        if (type == 10)
        {
            return std::string("Item Weapon Crossbow Down");
        }
        // Longblades, One hand and Two
        if (type == 1 || type == 2)
        {
            return std::string("Item Weapon Longblade Down");
        }
        // Shortblade and thrown weapons
        // thrown weapons may not be entirely correct
        if (type == 0 || type == 11)
        {
            return std::string("Item Weapon Shortblade Down");
        }
        // Spear
        if (type == 6)
        {
            return std::string("Item Weapon Spear Down");
        }
        // Blunts and Axes
        if (type == 3 || type == 4 || type == 5 || type == 7 || type == 8)
        {
            return std::string("Item Weapon Blunt Down");
        }

        return std::string("Item Misc Down");
    }

    bool Weapon::hasToolTip (const MWWorld::Ptr& ptr) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        return (ref->base->name != "");
    }

    MWGui::ToolTipInfo Weapon::getToolTipInfo (const MWWorld::Ptr& ptr, MWWorld::Environment& environment) const
    {
        ESMS::LiveCellRef<ESM::Weapon, MWWorld::RefData> *ref =
            ptr.get<ESM::Weapon>();

        MWGui::ToolTipInfo info;
        info.caption = ref->base->name;
        info.icon = ref->base->icon;

        std::string text;

        // weapon type & damage. arrows / bolts don't have his info.
        if (ref->base->data.type < 12)
        {
            text += "\n" + environment.mWorld->getStore().gameSettings.search("sType")->str + " ";

            std::map <int, std::pair <std::string, std::string> > mapping;
            mapping[ESM::Weapon::ShortBladeOneHand] = std::make_pair("sSkillShortblade", "sOneHanded");
            mapping[ESM::Weapon::LongBladeOneHand] = std::make_pair("sSkillLongblade", "sOneHanded");
            mapping[ESM::Weapon::LongBladeTwoHand] = std::make_pair("sSkillLongblade", "sTwoHanded");
            mapping[ESM::Weapon::BluntOneHand] = std::make_pair("sSkillBluntweapon", "sOneHanded");
            mapping[ESM::Weapon::BluntTwoClose] = std::make_pair("sSkillBluntweapon", "sTwoHanded");
            mapping[ESM::Weapon::BluntTwoWide] = std::make_pair("sSkillBluntweapon", "sTwoHanded");
            mapping[ESM::Weapon::SpearTwoWide] = std::make_pair("sSkillSpear", "sTwoHanded");
            mapping[ESM::Weapon::AxeOneHand] = std::make_pair("sSkillAxe", "sOneHanded");
            mapping[ESM::Weapon::AxeTwoHand] = std::make_pair("sSkillAxe", "sTwoHanded");
            mapping[ESM::Weapon::MarksmanBow] = std::make_pair("sSkillMarksman", "");
            mapping[ESM::Weapon::MarksmanCrossbow] = std::make_pair("sSkillMarksman", "");
            mapping[ESM::Weapon::MarksmanThrown] = std::make_pair("sSkillMarksman", "");

            std::string type = mapping[ref->base->data.type].first;
            std::string oneOrTwoHanded = mapping[ref->base->data.type].second;

            text += environment.mWorld->getStore().gameSettings.search(type)->str +
                ((oneOrTwoHanded != "") ? ", " + environment.mWorld->getStore().gameSettings.search(oneOrTwoHanded)->str : "");

            // weapon damage
            if (ref->base->data.type >= 9)
            {
                // marksman
                text += "\n" + environment.mWorld->getStore().gameSettings.search("sAttack")->str + ": "
                    + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.chop[0]))
                    + " - " + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.chop[1]));
            }
            else
            {
                // Chop
                text += "\n" + environment.mWorld->getStore().gameSettings.search("sChop")->str + ": "
                    + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.chop[0]))
                    + " - " + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.chop[1]));
                // Slash
                text += "\n" + environment.mWorld->getStore().gameSettings.search("sSlash")->str + ": "
                    + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.slash[0]))
                    + " - " + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.slash[1]));
                // Thrust
                text += "\n" + environment.mWorld->getStore().gameSettings.search("sThrust")->str + ": "
                    + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.thrust[0]))
                    + " - " + MWGui::ToolTips::toString(static_cast<int>(ref->base->data.thrust[1]));
            }
        }

        /// \todo store the current weapon health somewhere
        if (ref->base->data.type < 11) // thrown weapons and arrows/bolts don't have health, only quantity
            text += "\n" + environment.mWorld->getStore().gameSettings.search("sCondition")->str + ": " + MWGui::ToolTips::toString(ref->base->data.health);

        text += "\n" + environment.mWorld->getStore().gameSettings.search("sWeight")->str + ": " + MWGui::ToolTips::toString(ref->base->data.weight);
        text += MWGui::ToolTips::getValueString(ref->base->data.value, environment.mWorld->getStore().gameSettings.search("sValue")->str);

        if (environment.mWindowManager->getFullHelp()) {
            text += MWGui::ToolTips::getMiscString(ref->ref.owner, "Owner");
            text += MWGui::ToolTips::getMiscString(ref->base->script, "Script");
        }

        info.text = text;

        return info;
    }
}
