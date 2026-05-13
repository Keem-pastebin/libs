#include "Includes/Logger.h"
#include "Includes/Macros.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "ImGui/Call_ImGui.h"
#include "IL2CppSDKGenerator/BasicStructs/Call_BasicStructs.h"
#include "IL2CppSDKGenerator/IL2Cpp/Call_IL2Cpp.h"
#include "Hacks/Hacks.h"
#include "IL2CppSDKGenerator/KittyMemory/MemoryPatch.h"

#include "foxcheats/include/ScanEngine.hpp"
#include "ctorHook/ConstructorHook.hpp"
#include "FTools/Iconcpp.h"
#include "FTools/Font.h"
#include "FTools/Icon.h"
//#include "FTools/Battery.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <array>
#include <unordered_map>

#include "Fields.h"

#include "oxorany/source/oxorany.h"
#include "oxorany/source/oxorany.cpp"
#include "oxorany/source/oxorany_include.h"

#include "Noctivagous/StrEnc.h"
#include "Noctivagous/json.hpp"
#include "Noctivagous/md5.h"
#include "curl/curl.h"
#include "HideProcess/ProcessHide.h"

class _BYTE;
class _BOOL4;
class _BOOL8;
class _WORD;
class _DWORD;
class _QWORD;

bool ClearDisplay = false;
bool SnowB = false;
float SnowBsize = 0.0f;
bool range3P = false;
float range3PValue = 0.0f;
bool view3PEnabled = false;
bool isSpeedHackEnabled = false;
float speedHackMultiplier = 0.0f;
bool isJumpAdjustmentEnabled = false;
float jumpHeightMultiplier = 1.0f;

std::unordered_map<std::string, bool> sBool;
std::unordered_map<int, int> activeKillEffects;

int (*orig_GetCurrentWeaponKillEffect)(Weapon);
int _GetCurrentWeaponKillEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!localPawn) return 0;

    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!currentWeapon) return 0;

    int currentID = currentWeapon->get_WeaponID();

    auto it = activeKillEffects.find(currentID);
    if (it != activeKillEffects.end()) {
        return it->second;
    }

    return 0;
}

std::vector<void *> itemInventoryInstance;
std::vector<void *> weaponExtraInstance;
std::vector<void *> weaponFireEffectInstance;
std::vector<void *> weaponConfInstance;
std::vector<void *> weaponAssetGroupInstance;
std::vector<void *> mythicArmorInstance;
std::vector<void *> mythicSightInstance;
std::vector<void *> killEffectItemInstance;
std::vector<void *> weaponSkinConfigInstance;
std::vector<void *> itemResourceConfigInstance;
std::vector<void *> CharacterModelConfigInstance;
std::vector<void *> RoleConfConfigInstance;
std::vector<void *> RoleSkinConfigInstance;
std::vector<void *> RolePackConfConfigInstance;
std::vector<void *> BRDeadboxSkinConfigInstance;
std::vector<void *> BRDropPlaneSkinConfigInstance;

uintptr_t Item2InventoryAddress = 0x4FEC2D4;
uintptr_t WeaponConfAddress = 0x505CCF4;
uintptr_t WeaponConfExtraAddress = 0x505E2C0;
uintptr_t WeaponFireEffectAddress = 0x505FB00;
uintptr_t CharacterModelAddress = 0x4F9EB88;
uintptr_t BRDeadboxSkinAddress = 0x4F89E90;
uintptr_t BRDropPlaneSkinAddress = 0x4F8C688;
uintptr_t WeaponAssetGroupAddress = 0x505883C;
uintptr_t MythicArmorConfigAddress = 0x5017840;
uintptr_t MythicSightConfigAddress = 0x5017D10;
uintptr_t KillEffectItemConfConfigAddress = 0x5002640;
uintptr_t WeaponSkinAddress = 0x5068C30;
uintptr_t ItemResourceAddress = 0x50002A0;
uintptr_t RoleConfAddress = 0x502A644;
uintptr_t RoleSkinAddress = 0x5030568;
uintptr_t RolePackConfAddress = 0x502EE64;
uintptr_t WeaponConfName = 0x4FEEFF4;
uintptr_t GetDropPlaneName = 0x4F8D130;
uintptr_t GetNameRoleSkin = 0x5030690;
uintptr_t GetRoleConfName = 0x502BF4C;
uintptr_t GetDeadBoxIDAddress = 0x502B170;
uintptr_t GetCurrentWeaponAddress = 0x8FDF99C;

#define ReadInt(base, offset) (*(int*)((uintptr_t)(base) + (offset)))
#define ReadBool(base, offset) (*(bool*)((uintptr_t)(base) + (offset)))
#define ReadFloat(base, offset) (*(float*)((uintptr_t)(base) + (offset)))
#define ReadByte(base, offset) (*(uint8_t*)((uintptr_t)(base) + (offset)))
#define READ_PTR(type, base, offset) (*(type**)((uintptr_t)(base) + (offset)))

std::unordered_map<std::string, bool> getplane;
std::unordered_map<std::string, bool> getguns;

void *GameUpdate(void *) {
    sleep(60);
    while (true) {
        if (!loadCharacter && CharacterModelConfigInstance.size() > 0 && itemResourceConfigInstance.size() > 0
        && RoleConfConfigInstance.size() > 0 && RoleSkinConfigInstance.size() > 0
        && RolePackConfConfigInstance.size() > 0 && BRDeadboxSkinConfigInstance.size() > 0) {
        
            for (auto &a : RoleSkinConfigInstance) {
                if (!a) continue;
                auto *fx = (RoleSkinFields *) ((uintptr_t) a + 0x18);
                if ((fx->FxAssetID_1P ^ 0) != 0) {
                    std::string n = GetNameString(GetNameRoleSkin, (void *) a);
                    watch.push_back({n, fx->FxAssetID_1P});
                }
            }

            std::string _g = "";
            for (auto &z : BRDeadboxSkinConfigInstance) {
                if (!z) continue;
                auto *y = (BRDeadboxSkinFields *) ((uintptr_t) z + 0x10);
                bool __b = false;

                for (auto &q : RoleConfConfigInstance) {
                    if (!q) continue;
                    auto *f = (RoleConfFields *) ((uintptr_t) q + 0x14);
                    std::string s = GetNameString(GetRoleConfName, q);

                    if ((y->ID & 0xFFFFFFF) != 0) {
                        auto fx = (int (*)(void *))(getRealOffset(GetDeadBoxIDAddress));
                        int k = fx(q);

                        if (y->ID == k && !__b) {
                            auto dump = [&](const std::string &nm) {
                                deadboxF.push_back({nm, {
                                    y->ColorID,
                                    y->DeadBoxEffectAsset,
                                    y->Flag,
                                    y->FlagAsset,
                                    y->ModelAsset3P,
                                    y->ModelAssetUI
                                }});
                            };
                            if (y->ColorID == 5) {
                                _g = s;
                                dump(s);
                                __b = true;
                            } else {
                                dump(s);
                            }
                        }
                        if (__b) break;
                        if (k + 1 == y->ID) {
                            if (y->ColorID == 5 && y->FlagAsset != 0 && !_g.empty()) {
                                std::string v = _g + " (Variant)";
                                deadboxF.push_back({v, {
                                    y->ColorID,
                                    y->DeadBoxEffectAsset,
                                    y->Flag,
                                    y->FlagAsset,
                                    y->ModelAsset3P,
                                    y->ModelAssetUI
                                }});
                            }
                        }
                    }
                }
            }

            for (auto X0 : CharacterModelConfigInstance) {
                if (!X0) continue;
                auto *X1 = (CharacterModelFields *)((uintptr_t)X0 + 0x10);
                auto A = X1->ItemID;
                auto B = X1->BRBagModel;
                auto C = X1->BRHeadModel;
                auto D = X1->BRLobby;
                auto E = X1->BRModel;
                auto F = X1->BindEffect1P;
                auto G = X1->ChangeClipEffect1P;
                auto H = X1->DefaultModelID;
                auto I = X1->Guarder1P;
                auto J = X1->Guarder3P;
                auto K = X1->GuarderBagModel;
                auto L = X1->GuarderHeadModel;
                auto M = X1->GuarderLobby;
                auto N = X1->Traitor1P;

                for (auto Y0 : itemResourceConfigInstance) {
                    if (!Y0) continue;
                    auto *Y1 = (ItemResourceFields *)((uintptr_t)Y0 + 0x10);
                    auto A_ = Y1->AvatarModelID;
                    if (A != A_) continue;
                    auto P = Y1->FxAssetID;
                    auto Q = Y1->InventoryModelID;
                    auto R = Y1->ModelAssetIDRaw;
                    auto S = Y1->UIMiniSpriteName;
                    auto T = Y1->UISmallSpriteName;
                    auto U = Y1->UISpriteName;
                    auto V = Y1->UISquareSpriteName;
                    auto W = Y1->ID;

                    for (auto Z0 : RoleConfConfigInstance) {
                        if (!Z0) continue;
                        auto *Z1 = (RoleConfFields *)((uintptr_t)Z0 + 0x14);
                        if (Z1->ID != W) continue;
                        auto a = Z1->roleLeftArmID;
                        auto b = Z1->roleFinalSuitID;
                        auto c = Z1->roleBasicHologramID;
                        auto d = Z1->ColorID;
                        auto e = Z1->ColorSubID;
                        auto f = Z1->ShowRare;
                        auto g = Z1->RoleLvGroupID;
                        auto h = Z1->RolePackID;
                        int i = 0, j = 0;
                        for (auto RP : RolePackConfConfigInstance) {
                            if (!RP) continue;
                            auto *rpF = (RolePackFields *)((uintptr_t)RP + 0x10);
                            if (rpF->RolePackID != h) continue;
                            if (rpF->LoadingFrame && rpF->LobbySceneType == 0) {
                                i = rpF->RolePackID;
                                j = rpF->LoadingFrame;
                            }
                        }
                        std::string Zz, n0 = GetNameString(GetRoleConfName, Z0);
                        if (n0.empty()) {
                            if (!lastKnownName.empty()) {
                                char c_ = 'A' + emptyNameCount++;
                                n0 = lastKnownName + " " + c_ + "+";
                            } else n0 = "UnknownName";
                        } else {
                            lastKnownName = n0;
                            emptyNameCount = 0;
                        }
                        std::string key = n0 + "_" + std::to_string(A);
                        if (nameCountChar.count(key)) {
                            nameCountChar[key]++;
                            n0 += " +" + std::to_string(nameCountChar[key]);
                        } else nameCountChar[key] = 0;
                        if (d == 5) Zz = "[M] " + n0;
                        else if (d == 4) Zz = "[L] " + n0;
                        else if (d == 3) Zz = "[E] " + n0;
                        else Zz = "[C] " + n0;
                        if (Zz.find("A+") != std::string::npos) h--;
                        charData.push_back({
                            Zz,
                            {B, C, D, E, F, G, H, I, J, K, L, M},
                            {P, Q, R},
                            {S, T, U, V},
                            {a, b, c, d, e, f, g, h},
                            {i, j}
                        });
                    }
                    break;
                }
                loadCharacter = true;
            }
        }

        if (!loadskinhack && weaponConfInstance.size() > 0 && itemInventoryInstance.size() > 0 && weaponAssetGroupInstance.size() > 0 && weaponFireEffectInstance.size() > 0 && weaponExtraInstance.size() > 0 && killEffectItemInstance.size() > 0 && mythicArmorInstance.size() > 0 && mythicSightInstance.size() > 0 && itemResourceConfigInstance.size() > 0) {
            for (auto conf : weaponConfInstance) {
                if (!conf) continue;
                auto baseID = *(int *) ((uintptr_t) conf + 0x34);
                auto confID = *(int *) ((uintptr_t) conf + 0x40);
                for (auto item : itemInventoryInstance) {
                    if (!item) continue;
                    auto itemIDbase = *(int *) ((uintptr_t) item + 0x20);
                    int itemBaseModified;
                    int itemBase;
                    if (baseID == itemIDbase) {
                        itemBase = *(int *) ((uintptr_t) item + 0x20);
                        itemBaseModified = itemBase + 200;
                    }
                    if (confID == itemIDbase) {
                        auto confcolorID = *(uint8_t *) ((uintptr_t) conf + 0x22);
                        auto itemIDskin2 = *(int *) ((uintptr_t) item + 0x24);
                        auto itemIDskin3 = *(int *) ((uintptr_t) item + 0x28);
                        std::string AName;
                        std::string getName = GetNameString(WeaponConfName, conf);
                        if (confcolorID == 5) {
                            AName += "[M] " + getName;
                        } else if (confcolorID == 4) {
                            AName += "[L] " + getName;
                        } else if (confcolorID == 3) {
                            AName += "[E] " + getName;
                        } else {
                            AName += "[C] " + getName;
                        }
                        if (nameCountMap.find(AName) != nameCountMap.end()) {
                            nameCountMap[AName]++;
                            AName += " +" + std::to_string(nameCountMap[AName]);
                        } else {
                            nameCountMap[AName] = 0;
                        }
                        if (!AName.empty()) {
                            int fireIds = 0, fireIds2 = 0, assetIds = 0;
                            int originalFireID = 0;
                            for (auto asset : weaponAssetGroupInstance) {
                                if (!asset) continue;
                                auto assetID = *(int *) ((uintptr_t) asset + 0x44);
                                if (itemIDskin2 == assetID) {
                                    auto fireEffectID = *(int *) ((uintptr_t) asset + 0x40);
                                    for (auto fireConf : weaponFireEffectInstance) {
                                        if (!fireConf) continue;
                                        int fireID = *(int *) ((uintptr_t) fireConf + 0x80);
                                        int assetIdBulletSmoke = *(int *) ((uintptr_t) fireConf + 0x1C);
                                        if (AName.find("[M]") != std::string::npos) {
                                            if (fireEffectID == fireID) {
                                                if (assetIdBulletSmoke != 0) {
                                                    fireIds = fireID;
                                                    originalFireID = fireID;
                                                    break;
                                                }
                                                int nextFireID = fireID + 1;
                                                bool found = false;
                                                for (int i = 0; i < 10; i++) {
                                                    for (auto nextFireConf : weaponFireEffectInstance) {
                                                        if (!nextFireConf) continue;
                                                        int nextFireIDCheck = *(int *) ((uintptr_t) nextFireConf + 0x80);
                                                        int nextAssetIdBulletSmoke = *(int *) ((uintptr_t) nextFireConf + 0x1C);
                                                        if (nextFireIDCheck == nextFireID && nextAssetIdBulletSmoke != 0) {
                                                            fireIds = nextFireIDCheck;
                                                            found = true;
                                                            break;
                                                        }
                                                    }
                                                    if (found) break;
                                                    nextFireID++;
                                                }
                                                if (fireIds == 0) fireIds = fireID;
                                                fireIds2 = fireID;
                                                assetIds = assetID;
                                            }
                                        }
                                    }
                                }
                            }
                            int confbaseskin = 0, confSkinID = 0, confBrocastID = 0, confColorID = 0;
                            if (confID == itemIDbase) {
                                confbaseskin = ReadInt(conf, 0x34);
                                confColorID = ReadByte(conf, 0x22);
                                confSkinID = ReadInt(conf, 0x38);
                                confBrocastID = ReadInt(conf, 0x3C);
                            }
                            int mythicArmor = 0, deadReplay = 0, killEffect = 0, extraOrig = 0;
                            for (auto extra : weaponExtraInstance) {
                                if (!extra) continue;
                                weaponextraFields = (WeaponConfExtraFields*) ((uintptr_t) extra + 0x10);
                                auto extraID = weaponextraFields->ID;
                                if (baseID == extraID) {
                                    extraOrig = weaponextraFields->ID;
                                }
                                if (confID == extraID) {
                                    deadReplay = weaponextraFields->DefaultDeadReplayEffectId;
                                    killEffect = weaponextraFields->DefaultKillEffectId;
                                }
                            }
                            for (auto armor : mythicArmorInstance) {
                                if (!armor) continue;
                                mythicarmorFields = (MythicArmorFields*) ((uintptr_t) armor + 0x14);
                                auto armorassetID = mythicarmorFields->AssetID;
                                auto secondTab = mythicarmorFields->SecondTab;
                                auto thirdTab = mythicarmorFields->ThirdTab;
                                if (itemIDskin3 == secondTab) {
                                    if (thirdTab == 5) {
                                        if (AName.find("[M]") != std::string::npos) {
                                            mythicArmor = armorassetID;
                                        }
                                    }
                                }
                            }
                            int sightMythic = 0;
                            for (auto sight : mythicSightInstance) {
                                if (!sight) continue;
                                mythicsightFields = (MythicSightFields*) ((uintptr_t) sight + 0x10);
                                auto sightID = *(int *) ((uintptr_t) sight + 0x14);
                                auto equipArray = *(Array<int>**) ((uintptr_t) sight + 0x38);
                                if (equipArray && equipArray->getLength() > 0) {
                                    if (itemIDskin3 == equipArray->m_Items[equipArray->getLength()]) {
                                        if (AName.find("[M]") != std::string::npos) {
                                            sightMythic = sightID;
                                        }
                                    }
                                }
                            }
                            for (auto wKilling : killEffectItemInstance) {
                                if (!wKilling) continue;
                                killeffectFields = (KillEffectItemFields*) ((uintptr_t) wKilling + 0x10);
                                auto equipArray = *(Array<int> **) ((uintptr_t) wKilling + 0x90);
                                int canEquip = 0;
                                if (equipArray && equipArray->getLength() > 0) {
                                    canEquip = equipArray->m_Items[equipArray->getLength()];
                                }
                                auto realAssetIDs_1 = *(Array<int> **) ((uintptr_t) wKilling + 0x10);
                                int lastAssetID = 0;
                                if (realAssetIDs_1 && realAssetIDs_1->getLength() > 0) {
                                    lastAssetID = realAssetIDs_1->m_Items[realAssetIDs_1->getLength()];
                                }
                                if (itemIDskin3 == canEquip) {
                                    if (AName.find("[M]") != std::string::npos) {
                                        killEffect = lastAssetID;
                                    }
                                }
                            }
                            void *spr1 = NULL;
                            void *spr2 = NULL;
                            void *spr3 = NULL;
                            void *spr4 = NULL;
                            int xItem1 = 0, xItem2 = 0, xItem3 = 0;
                            for (auto itemResource : itemResourceConfigInstance) {
                                if (!itemResource) continue;
                                itemFields = (ItemResourceFields*) ((uintptr_t) itemResource + 0x10);
                                auto checkItem = itemFields->ID;
                                auto sItem1 = itemFields->FxAssetID;
                                auto sItem2 = itemFields->InventoryModelID;
                                auto sItem3 = itemFields->ModelAssetIDRaw;
                                auto sprite1 = itemFields->UISmallSpriteName;
                                auto sprite2 = itemFields->UIMiniSpriteName;
                                auto sprite3 = itemFields->UISpriteName;
                                auto sprite4 = itemFields->UISquareSpriteName;
                                if (checkItem == confID) {
                                    xItem1 = sItem1;
                                    xItem2 = sItem2;
                                    xItem3 = sItem3;
                                    spr1 = sprite1;
                                    spr2 = sprite2;
                                    spr3 = sprite3;
                                    spr4 = sprite4;
                                }
                            }
                            itemData.push_back({AName,
                                {itemBase, itemIDskin2, itemIDskin3, itemBaseModified},
                                {confbaseskin, confColorID, confID, confBrocastID},
                                {extraOrig, mythicArmor, sightMythic, deadReplay, killEffect},
                                {assetIds, fireIds, fireIds2},
                                {xItem1, xItem2, xItem3},
                                {spr1, spr2, spr3, spr4}
                            });
                        }
                    }
                }
            }
            loadskinhack = true;
        }

        if (BRDropPlaneSkinConfigInstance.size() > 0) {
            for (auto planedrop : BRDropPlaneSkinConfigInstance) {
                if (!planedrop) continue;
                dropplaneFields = (BRDropPlaneSkinFields*)((uintptr_t)planedrop + 0x10);
                std::string plane = GetNameString(GetDropPlaneName, planedrop);
                if (dropplaneFields->ModelAsset1P != 0 && !getplane[plane]) {
                    getplane[plane] = true;
                    dropplane.push_back({plane, {
                        dropplaneFields->ColorID,
                        dropplaneFields->ModelAsset1P,
                        dropplaneFields->ModelAsset3P,
                        dropplaneFields->ModelAssetCutScene,
                        dropplaneFields->ModelAssetUI,
                        dropplaneFields->Priority
                    }});
                    if (getplane.size() == BRDropPlaneSkinConfigInstance.size()) {
                        break;
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
    return NULL;
}

struct ConfigData {
    // Tab 1 - ESP Settings
    bool Esp;
    bool PlayerLine;
    bool Box;
    bool Health;
    bool Name;
    bool Distance;
    bool Count;
    bool Skeleton;
    int CrosshairType;
    float AimSize;
    bool Aimline;

    // Tab 2 - Memory Settings
    bool AimAssist;
    float AimAssistSize;
    bool Kinetic;
    bool Recoil;
    bool Spread;
    bool Reload;
    bool Scope;
    bool Switch;
    bool Shake;
    bool Hit;

    // Tab 3 - Ability Settings
    bool SnowB;
    float SnowBsize;
    bool SlideTackle;
    float SlideRange;
    bool SpeedHack;
    float SpeedMultiplier;
    bool JumpAdjustment;
    float JumpMultiplier;
    float RedWallhack;    
    bool Parachute;
    bool Diving;

    // Tab 4 - Aimbot Settings
    bool Aimbot360;
    bool AimSilent;
    int Target;
    int Trigger;
    int By;

    // Tab 6 - Events (No settings to save)

    // Tab 7 - Performance Settings
    bool FpsLevel;
    float FpsLevel1;
    bool showFPSLevelSlider;
    bool FpsLevelUltra;
    float FpsLevelUltra1;
    bool showFPSLevelUltraSlider;
};

void saveAllConfig() {
    ConfigData config;
    
    config.Esp = Config.ESPMenu.Esp;
    config.PlayerLine = Config.ESPMenu.isPlayerLine;
    config.Box = Config.ESPMenu.Box;
    config.Health = Config.ESPMenu.Health;
    config.Name = Config.ESPMenu.Name;
    config.Distance = Config.ESPMenu.Distance;
    config.Count = Config.ESPMenu.Count;
    config.Skeleton = Config.ESPMenu.Skeleton;
    config.CrosshairType = static_cast<int>(Config.ESPMenu.CrosshairType);
    config.AimSize = Config.Aim.size;
    config.Aimline = Config.ESPMenu.Aimline;

    config.AimAssist = Config.Aim.AimAssist;
    config.AimAssistSize = Config.Aim.AimAssistSize;
    config.Kinetic = Config.ExtraMenu.Kinetic;
    config.Recoil = Config.ExtraMenu.Recoil;
    config.Spread = Config.ExtraMenu.Spread;
    config.Reload = Config.ExtraMenu.Reload;
    config.Scope = Config.ExtraMenu.Scope;
    config.Switch = Config.ExtraMenu.Switch;
    config.Shake = Config.ExtraMenu.Shake;
    config.Hit = Config.ExtraMenu.Hit;

    config.SnowB = SnowB;
    config.SnowBsize = SnowBsize;
    config.SlideTackle = SlideTackle;
    config.SlideRange = SlideRange;
    config.SpeedHack = isSpeedHackEnabled;
    config.SpeedMultiplier = speedHackMultiplier;
    config.JumpAdjustment = isJumpAdjustmentEnabled;
    config.JumpMultiplier = jumpHeightMultiplier;
    config.RedWallhack = Config.ExtraMenu.RedWallhack;    
    config.Parachute = Config.ExtraMenu.Parachute;
    config.Diving = Config.ExtraMenu.Diving;

    config.Aimbot360 = Config.Aim.Aimbot360;
    config.AimSilent = Config.Aim.AimSilent;
    config.Target = static_cast<int>(Config.Aim.Target);
    config.Trigger = static_cast<int>(Config.Aim.Trigger);
    config.By = static_cast<int>(Config.Aim.By);

    config.FpsLevel1 = Config.Aim.FpsLevel1;

    std::ofstream file("/storage/emulated/0/Download/Hex_config.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char*>(&config), sizeof(ConfigData));
        file.close();
        ImGui::OpenPopup("Config Saved");
    } else {
        ImGui::OpenPopup("Config Save Error");
    }
}

bool loadAllConfig() {
    ConfigData config;
    std::ifstream file("/storage/emulated/0/Download/Hex_config.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&config), sizeof(ConfigData));
        file.close();

        Config.ESPMenu.Esp = config.Esp;
        Config.ESPMenu.isPlayerLine = config.PlayerLine;
        Config.ESPMenu.Box = config.Box;
        Config.ESPMenu.Health = config.Health;
        Config.ESPMenu.Name = config.Name;
        Config.ESPMenu.Distance = config.Distance;
        Config.ESPMenu.Count = config.Count;
        Config.ESPMenu.Skeleton = config.Skeleton;
        Config.ESPMenu.CrosshairType = static_cast<decltype(Config.ESPMenu.CrosshairType)>(config.CrosshairType);
        Config.Aim.size = config.AimSize;
        Config.ESPMenu.Aimline = config.Aimline;

        Config.Aim.AimAssist = config.AimAssist;
        Config.Aim.AimAssistSize = config.AimAssistSize;
        Config.Aim.showAimAssistSlider = config.AimAssist;
        Config.ExtraMenu.Kinetic = config.Kinetic;
        Config.ExtraMenu.Recoil = config.Recoil;
        Config.ExtraMenu.Spread = config.Spread;
        Config.ExtraMenu.Reload = config.Reload;
        Config.ExtraMenu.Scope = config.Scope;
        Config.ExtraMenu.Switch = config.Switch;
        Config.ExtraMenu.Shake = config.Shake;
        Config.ExtraMenu.Hit = config.Hit;

        SnowB = config.SnowB;
        SnowBshow = config.SnowB;
        SnowBsize = config.SnowBsize;
        SlideTackle = config.SlideTackle;
        SlideShow = config.SlideTackle;
        SlideRange = config.SlideRange;
        isSpeedHackEnabled = config.SpeedHack;
        showSpeedHackSlider = config.SpeedHack;
        speedHackMultiplier = config.SpeedMultiplier;
        isJumpAdjustmentEnabled = config.JumpAdjustment;
        jumpHeightMultiplier = config.JumpMultiplier;
        Config.ExtraMenu.RedWallhack = config.RedWallhack;                   
        Config.ExtraMenu.Parachute = config.Parachute;
        Config.ExtraMenu.Diving = config.Diving;

        Config.Aim.Aimbot360 = config.Aimbot360;
        Config.Aim.AimSilent = config.AimSilent;
        Config.Aim.Target = static_cast<decltype(Config.Aim.Target)>(config.Target);
        Config.Aim.Trigger = static_cast<decltype(Config.Aim.Trigger)>(config.Trigger);
        Config.Aim.By = static_cast<decltype(Config.Aim.By)>(config.By);

        Config.Aim.showFPSLevelSlider = config.FpsLevel;
        Config.Aim.FpsLevel1 = config.FpsLevel1;

        return true;
    }
    return false;
}

#define _BYTE uint8_t
#define _WORD  uint8_t
#define _DWORD uint64_t
#define _QWORD uint64_t
#define _BOOL4 uint8_t

#define SPAMSINT1() int __xbypass_any = __LINE__; int any_xbypass = __LINE__; if (!strstr(oxorany("stoler"), oxorany("relots"))) { __xbypass_any--; any_xbypass++; if (__xbypass_any != any_xbypass && strstr(oxorany("better"), oxorany("better"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("learn"), oxorany("nrael"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("first"), oxorany("tsrif"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("you"), oxorany("you"))) { __xbypass_any++; any_xbypass--; if (!strstr(oxorany("are"), oxorany("era"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("DUMB"), oxorany("DUMB"))) { __xbypass_any = __xbypass_any - any_xbypass; any_xbypass = any_xbypass + __xbypass_any; } else { __xbypass_any = __xbypass_any + any_xbypass; any_xbypass = any_xbypass - __xbypass_any; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; }  } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } __xbypass_any++; return 1
#define SPAMS() int __xbypass_any = __LINE__; int any_xbypass = __LINE__; if (!strstr(oxorany("stoler"), oxorany("relots"))) { __xbypass_any--; any_xbypass++; if (__xbypass_any != any_xbypass && strstr(oxorany("better"), oxorany("better"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("learn"), oxorany("nrael"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("first"), oxorany("tsrif"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("you"), oxorany("you"))) { __xbypass_any++; any_xbypass--; if (!strstr(oxorany("are"), oxorany("era"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("DUMB"), oxorany("DUMB"))) { __xbypass_any = __xbypass_any - any_xbypass; any_xbypass = any_xbypass + __xbypass_any; } else { __xbypass_any = __xbypass_any + any_xbypass; any_xbypass = any_xbypass - __xbypass_any; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; }  } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } any_xbypass = any_xbypass + __xbypass_any
#define SPAMSINT0() int __xbypass_any = __LINE__; int any_xbypass = __LINE__; if (!strstr(oxorany("stoler"), oxorany("relots"))) { __xbypass_any--; any_xbypass++; if (__xbypass_any != any_xbypass && strstr(oxorany("better"), oxorany("better"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("learn"), oxorany("nrael"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("first"), oxorany("tsrif"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("you"), oxorany("you"))) { __xbypass_any++; any_xbypass--; if (!strstr(oxorany("are"), oxorany("era"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("DUMB"), oxorany("DUMB"))) { __xbypass_any = __xbypass_any - any_xbypass; any_xbypass = any_xbypass + __xbypass_any; } else { __xbypass_any = __xbypass_any + any_xbypass; any_xbypass = any_xbypass - __xbypass_any; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; }  } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } return (__xbypass_any & any_xbypass)
#define SPAMSINTTYPE(typex, any) int __xbypass_any = __LINE__; int any_xbypass = __LINE__; if (!strstr(oxorany("stoler"), oxorany("relots"))) { __xbypass_any--; any_xbypass++; if (__xbypass_any != any_xbypass && strstr(oxorany("better"), oxorany("better"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("learn"), oxorany("nrael"))) { __xbypass_any--; any_xbypass++; if (!strstr(oxorany("first"), oxorany("tsrif"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("you"), oxorany("you"))) { __xbypass_any++; any_xbypass--; if (!strstr(oxorany("are"), oxorany("era"))) { __xbypass_any++; any_xbypass--; if (strstr(oxorany("DUMB"), oxorany("DUMB"))) { __xbypass_any = __xbypass_any - any_xbypass; any_xbypass = any_xbypass + __xbypass_any; } else { __xbypass_any = __xbypass_any + any_xbypass; any_xbypass = any_xbypass - __xbypass_any; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; } } else { __xbypass_any--; any_xbypass++; }  } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } } else { __xbypass_any++; any_xbypass--; } return (typex)(__xbypass_any & any_xbypass)

#include <fstream>
using namespace std;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

ImFont* medium;
ImFont* bold;
ImFont* tab_icons;
ImFont* logo;
ImFont* tab_title;
ImFont* tab_title_icon;
ImFont* subtab_title;
ImFont* combo_arrow;

struct My_Patches
{
    MemoryPatch A1, smk, Waller, chute, wing, tutorial, kinetic, RedWallhack;
} Patches;

float AVIWA = 119.167f;

struct sRegion
{
    uintptr_t start, end;
};

std::chrono::steady_clock::time_point appStartTime = std::chrono::steady_clock::now();

std::string FormatTimeDuration(std::chrono::steady_clock::duration duration) {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1)).count();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1)).count();

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);
    return std::string(buffer);
}

static std::string EXP ="";
static std::string storedKey ="";
static char s[256];

void RenderFloatingInfo(ImDrawList* draw, float screenWidth, float screenHeight) {
    if (Config.ExtraMenu.ClearDisplay) return;

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);
    
    char dateStr[64], timeStr[64];
    strftime(dateStr, sizeof(dateStr), "%B %d, %Y", localTime);
    strftime(timeStr, sizeof(timeStr), "%I:%M %p", localTime);
    
    for (char* p = dateStr; *p; p++) *p = toupper(*p);
    for (char* p = timeStr; *p; p++) *p = toupper(*p);

    auto currentDuration = std::chrono::steady_clock::now() - appStartTime;
    std::string playTime = FormatTimeDuration(currentDuration);
    for (char& c : playTime) c = toupper(c);

    float padding = 30.0f;
    float lineSpacing = 28.0f;
    float startY = screenHeight - padding - (lineSpacing * 5);

    auto drawTextWithSpacing = [&](const char* text, ImVec2 pos, ImColor color) {
        float xOffset = 0.0f;
        float charSpacing = 1.5f;
        for (int i = 0; text[i] != '\0'; i++) {
            char c[2] = { text[i], '\0' };
            draw->AddText(ImVec2(pos.x + xOffset, pos.y), color, c);
            xOffset += ImGui::CalcTextSize(c).x + charSpacing;
        }
    };

    char line2[128], line3[64], line4[64];
    snprintf(line2, sizeof(line2), "DATE : %s", dateStr);
    snprintf(line3, sizeof(line3), "TIME : %s", timeStr);
    snprintf(line4, sizeof(line4), "PLAYING TIME : %s", playTime.c_str());

    ImColor gold(255, 215, 0, 255);
    ImColor white(255, 255, 255, 255);

    drawTextWithSpacing("HEXPHANTOM | GARENA | 1.6.55", ImVec2(padding, startY), gold);
    drawTextWithSpacing(line2, ImVec2(padding, startY + lineSpacing), white);
    drawTextWithSpacing(line3, ImVec2(padding, startY + lineSpacing * 2), white);
    drawTextWithSpacing(line4, ImVec2(padding, startY + lineSpacing * 3), white);
    drawTextWithSpacing("DEVELOPER: @Hexxx22", ImVec2(padding, startY + lineSpacing * 4), white);
}

void saveKeyToFile(const std::string& key) {
    std::ofstream outFile("/storage/emulated/0/Download/yoshi.dat");
    if (outFile) {
        outFile << key;
        outFile.close();
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "ImGui", "Error: Could not save key to file.");
    }
}

std::string loadKeyFromFile() {
    std::ifstream inFile("/storage/emulated/0/Download/yoshi.dat");
    std::string key;
    if (inFile) {
        std::getline(inFile, key);
        inFile.close();
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "ImGui", "Error: Could not load key from file.");
    }
    return key;
}

std::vector<sRegion> trapRegions;
uintptr_t address = 0;
JavaVM *VM;
std::string md5(std::string s);
using json = nlohmann::json;
uintptr_t g_il2cpp;
std::string g_Token, g_Auth;
bool bValid = false;

std::string getClipboard() {
    std::string result;
    JNIEnv *env;
    VM->AttachCurrentThread(&env, NULL);
    auto looperClass = env->FindClass(OBFUSCATE("android/os/Looper"));
    auto prepareMethod = env->GetStaticMethodID(looperClass, OBFUSCATE("prepare"), OBFUSCATE("()V"));
    env->CallStaticVoidMethod(looperClass, prepareMethod);
    jclass activityThreadClass = env->FindClass(OBFUSCATE("android/app/ActivityThread"));
    jfieldID sCurrentActivityThreadField = env->GetStaticFieldID(activityThreadClass, OBFUSCATE("sCurrentActivityThread"), OBFUSCATE("Landroid/app/ActivityThread;"));
    jobject sCurrentActivityThread = env->GetStaticObjectField(activityThreadClass, sCurrentActivityThreadField);
    jfieldID mInitialApplicationField = env->GetFieldID(activityThreadClass, OBFUSCATE("mInitialApplication"), OBFUSCATE("Landroid/app/Application;"));
    jobject mInitialApplication = env->GetObjectField(sCurrentActivityThread, mInitialApplicationField);
    auto contextClass = env->FindClass(OBFUSCATE("android/content/Context"));
    auto getSystemServiceMethod = env->GetMethodID(contextClass, OBFUSCATE("getSystemService"), OBFUSCATE("(Ljava/lang/String;)Ljava/lang/Object;"));
    auto str = env->NewStringUTF(OBFUSCATE("clipboard"));
    auto clipboardManager = env->CallObjectMethod(mInitialApplication, getSystemServiceMethod, str);
    env->DeleteLocalRef(str);
    jclass ClipboardManagerClass = env->FindClass(OBFUSCATE("android/content/ClipboardManager"));
    auto getText = env->GetMethodID(ClipboardManagerClass, OBFUSCATE("getText"), OBFUSCATE("()Ljava/lang/CharSequence;"));
    jclass CharSequenceClass = env->FindClass(OBFUSCATE("java/lang/CharSequence"));
    auto toStringMethod = env->GetMethodID(CharSequenceClass, OBFUSCATE("toString"), OBFUSCATE("()Ljava/lang/String;"));
    auto text = env->CallObjectMethod(clipboardManager, getText);
    if (text) {
        str = (jstring) env->CallObjectMethod(text, toStringMethod);
        result = env->GetStringUTFChars(str, 0);
        env->DeleteLocalRef(str);
        env->DeleteLocalRef(text);
    }
    env->DeleteLocalRef(CharSequenceClass);
    env->DeleteLocalRef(ClipboardManagerClass);
    env->DeleteLocalRef(clipboardManager);
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(mInitialApplication);
    env->DeleteLocalRef(activityThreadClass);
    VM->DetachCurrentThread();
    return result.c_str();
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    auto *mem = (struct MemoryStruct *) userp;
    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == nullptr) {
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

const char *GetAndroidID(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass(OBFUSCATE("android/content/Context"));
    jmethodID getContentResolverMethod = env->GetMethodID(contextClass, OBFUSCATE("getContentResolver"), OBFUSCATE("()Landroid/content/ContentResolver;"));
    jclass settingSecureClass = env->FindClass(OBFUSCATE("android/provider/Settings$Secure"));
    jmethodID getStringMethod = env->GetStaticMethodID(settingSecureClass, OBFUSCATE("getString"), OBFUSCATE("(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;"));
    auto obj = env->CallObjectMethod(context, getContentResolverMethod);
    auto str = (jstring) env->CallStaticObjectMethod(settingSecureClass, getStringMethod, obj, env->NewStringUTF(OBFUSCATE("android_id")));
    return env->GetStringUTFChars(str, nullptr);
}

const char *GetDeviceModel(JNIEnv *env) {
    jclass buildClass = env->FindClass(OBFUSCATE("android/os/Build"));
    jfieldID modelId = env->GetStaticFieldID(buildClass, OBFUSCATE("MODEL"), OBFUSCATE("Ljava/lang/String;"));
    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, nullptr);
}

const char *GetDeviceBrand(JNIEnv *env) {
    jclass buildClass = env->FindClass(OBFUSCATE("android/os/Build"));
    jfieldID modelId = env->GetStaticFieldID(buildClass, OBFUSCATE("BRAND"), OBFUSCATE("Ljava/lang/String;"));
    auto str = (jstring) env->GetStaticObjectField(buildClass, modelId);
    return env->GetStringUTFChars(str, nullptr);
}

const char *GetPackageName(JNIEnv *env, jobject context) {
    jclass contextClass = env->FindClass(OBFUSCATE("android/content/Context"));
    jmethodID getPackageNameId = env->GetMethodID(contextClass, OBFUSCATE("getPackageName"), OBFUSCATE("()Ljava/lang/String;"));
    auto str = (jstring) env->CallObjectMethod(context, getPackageNameId);
    return env->GetStringUTFChars(str, nullptr);
}

const char *GetDeviceUniqueIdentifier(JNIEnv *env, const char *uuid) {
    jclass uuidClass = env->FindClass(OBFUSCATE("java/util/UUID"));
    auto len = strlen(uuid);
    jbyteArray myJByteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(myJByteArray, 0, len, (jbyte *) uuid);
    jmethodID nameUUIDFromBytesMethod = env->GetStaticMethodID(uuidClass, OBFUSCATE("nameUUIDFromBytes"), OBFUSCATE("([B)Ljava/util/UUID;"));
    jmethodID toStringMethod = env->GetMethodID(uuidClass, OBFUSCATE("toString"), OBFUSCATE("()Ljava/lang/String;"));
    auto obj = env->CallStaticObjectMethod(uuidClass, nameUUIDFromBytesMethod, myJByteArray);
    auto str = (jstring) env->CallObjectMethod(obj, toStringMethod);
    return env->GetStringUTFChars(str, nullptr);
}

std::string Login(const char *user_key) {
    JNIEnv *env;
    VM->AttachCurrentThread(&env, nullptr);
    std::string hwid = user_key;
    jobject context = getGlobalContext(env);
    hwid += GetAndroidID(env, context);
    hwid += GetDeviceModel(env);
    hwid += GetDeviceBrand(env);
    std::string UUID = GetDeviceUniqueIdentifier(env, hwid.c_str());
    VM->DetachCurrentThread();
    std::string errMsg;
    struct MemoryStruct chunk {};
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        std::string api_key = oxorany("https://kupalpanel.x10.mx/connect");  
        curl_easy_setopt(curl, CURLOPT_URL, (api_key.c_str()));  
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");  
        struct curl_slist *headers = NULL;  
        headers = curl_slist_append(headers, oxorany("Content-Type: application/x-www-form-urlencoded"));  
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);  
        char data[4096];  
        sprintf(data, oxorany("game=CODM&user_key=%s&serial=%s"), user_key, UUID.c_str());  
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);  
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);  
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);  
        res = curl_easy_perform(curl);  
        if (res == CURLE_OK) {  
            try {  
                json result = json::parse(chunk.memory);  
                if (result[std::string(OBFUSCATE("status"))] == true) {  
                    std::string token = result[std::string(OBFUSCATE("data"))][std::string(OBFUSCATE("token"))].get<std::string>();  
                    time_t rng = result[std::string(OBFUSCATE("data"))][std::string(OBFUSCATE("rng"))].get<time_t>();  
                    EXP = result["data"]["EXP"].get<std::string>();  
                    if (rng + 30 > time(0)) {  
                        std::string auth = "CODM";  
                        auth += "-";  
                        auth += user_key;  
                        auth += "-";  
                        auth += hwid;  
                        auth += "-";          
                        auth += "Vm8Lk7Uj2JmsjCPVPVjrLa7zgfx3uz9E";  
                        bValid = g_Token == g_Auth;  
                        if (g_Token == g_Auth) {  
                        }
                    }
                } else {
                    auto REASON = std::string{"reason"};
                    errMsg = result[REASON].get<std::string>();
                }
            } catch (nlohmann::json::exception &e) {
                errMsg = "{";
                errMsg += e.what();
                errMsg += "}\n{";
                errMsg += chunk.memory;
                errMsg += "}";
            }
        } else {
            errMsg = curl_easy_strerror(res);
        }
    }
    curl_easy_cleanup(curl);
    return bValid ? OBFUSCATE("OK") : errMsg;
}

static bool isMenuVisible = true;
#define CREATE_COLOR(r, g, b, a) new float[4]{(float)r, (float)g, (float)b, (float)a};

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    if (!Config.ImGuiMenu.g_Initialized) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGuiStyle *style = &ImGui::GetStyle();
        
        // Modern Style Settings
        style->WindowPadding = ImVec2(10, 10);
        style->FrameBorderSize = 6.0f;
        style->WindowBorderSize = 6.0f;
        style->FramePadding = ImVec2(2, 2);
        style->ItemSpacing = ImVec2(3, 2);
        style->IndentSpacing = 12;
        style->ScrollbarSize = 6.5;
        style->WindowRounding = 2.0f;
        style->FrameRounding = 1.5f;
        style->ScrollbarRounding = 2;
        style->WindowBorderSize = 4;
        style->FrameBorderSize = 4;
        style->WindowTitleAlign = ImVec2(0.5, 0.5);
        style->ButtonTextAlign = ImVec2(0.5, 0.5);
        style->WindowPadding = ImVec2(6, 8);
        style->FramePadding = ImVec2(2, 2);

        // Colors - Dark Mode with Cyan/Blue accents
        style->Colors[ImGuiCol_Text]                 = ImVec4(0.95f, 0.95f, 1.00f, 1.00f);
        style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.60f, 1.00f);
        style->Colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.06f, 0.10f, 0.98f);
        style->Colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.10f, 0.18f, 1.00f);
        style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.12f, 0.12f, 0.22f, 1.00f);
        style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.08f, 0.08f, 0.14f, 1.00f);
        style->Colors[ImGuiCol_Border]               = ImVec4(0.20f, 0.20f, 0.30f, 0.80f);
        style->Colors[ImGuiCol_FrameBg]              = ImVec4(0.10f, 0.10f, 0.15f, 0.95f);
        style->Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.15f, 0.15f, 0.22f, 0.95f);
        style->Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.18f, 0.18f, 0.25f, 0.95f);
        style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
        style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.20f, 0.40f, 0.70f, 0.95f);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.50f, 0.85f, 0.95f);
        style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.30f, 0.60f, 1.00f, 0.95f);
        style->Colors[ImGuiCol_Separator]            = ImVec4(0.20f, 0.40f, 0.70f, 0.95f);
        style->Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.25f, 0.50f, 0.85f, 0.95f);
        style->Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.30f, 0.60f, 1.00f, 0.95f);
        style->Colors[ImGuiCol_Button]               = ImVec4(0.12f, 0.12f, 0.18f, 0.95f);
        style->Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.18f, 0.18f, 0.25f, 0.95f);
        style->Colors[ImGuiCol_ButtonActive]         = ImVec4(0.15f, 0.15f, 0.22f, 0.95f);
        style->Colors[ImGuiCol_Tab]                  = ImVec4(0.08f, 0.08f, 0.14f, 0.95f);
        style->Colors[ImGuiCol_TabHovered]           = ImVec4(0.15f, 0.15f, 0.22f, 0.95f);
        style->Colors[ImGuiCol_TabActive]            = ImVec4(0.12f, 0.12f, 0.20f, 0.95f);
        style->Colors[ImGuiCol_Header]               = ImVec4(0.10f, 0.10f, 0.16f, 0.95f);
        style->Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.15f, 0.15f, 0.22f, 0.95f);
        style->Colors[ImGuiCol_HeaderActive]         = ImVec4(0.12f, 0.12f, 0.20f, 0.95f);
        style->Colors[ImGuiCol_CheckMark]            = ImVec4(0.20f, 0.60f, 1.00f, 1.00f);
        style->Colors[ImGuiCol_SliderGrab]           = ImVec4(0.20f, 0.50f, 0.90f, 0.95f);
        style->Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.30f, 0.60f, 1.00f, 0.95f);
        style->Colors[ImGuiCol_TableRowBg]           = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
        style->Colors[ImGuiCol_TableRowBgAlt]        = ImVec4(0.10f, 0.10f, 0.15f, 0.95f);
        style->Colors[ImGuiCol_NavHighlight]         = ImVec4(0.20f, 0.60f, 1.00f, 0.95f);

        style->ScrollbarSize *= 1.0f;

        ImGui_ImplAndroid_Init();
        ImGui_ImplOpenGL3_Init("#version 300 es");

        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.IniFilename = NULL;
        static const ImWchar icons_ranges[] = {
            0x0020, 0x00FF,
            0x3000, 0x30FF,
            0x31F0, 0x31FF,
            0xFF00, 0xFFEF,
            0x4e00, 0x9FAF,
            0xf000, 0xf3ff,
            0xe000, 0xf8ff,
            0
        };

        ImFontConfig CustomFont;
        CustomFont.FontDataOwnedByAtlas = true;
        ImFontConfig icons_config;
        CustomFont.FontDataOwnedByAtlas = false;

        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.OversampleH = 2.5;
        icons_config.OversampleV = 2.5;

        io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 22.f, &CustomFont);
        io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data1, font_awesome_size1, 28.0f, &icons_config, icons_ranges);
        io.Fonts->AddFontFromMemoryCompressedTTF(Custom_data, Custom_size, 26.0f, &CustomFont, io.Fonts->GetGlyphRangesJapanese());

        ImGui::GetStyle().ScaleAllSizes(3.0f);
        memset(&Config, 0, sizeof(sConfig));

        Config.sColorsESPPLAYER.LinePLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.BoxPLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.NamePLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.DistancePLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.HealthPLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.SkeletonPLAYER = CREATE_COLOR(255, 255, 255, 255);
        Config.sColorsESPBOT.LineBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.BoxBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.NameBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.HealthBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.DistanceBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.SkeletonBOT = CREATE_COLOR(255, 255, 255, 255);
        Config.sColorsESPOTHERS.PovOTHERS = CREATE_COLOR(225, 0, 0, 180);
        
        Config.PskelLine = 2.0f;
        Config.BskelLine = 2.0f;
        Config.ImGuiMenu.g_Initialized = true;
    }

    ImGuiIO *io = &ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(get_width(), get_height());
    ImGui::NewFrame();

    if (!isMenuVisible) {
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImVec2 buttonSize = ImVec2(200, 50);
        ImVec2 buttonPos = ImVec2((displaySize.x - buttonSize.x) / 2, 10);
        ImGui::SetNextWindowPos(buttonPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(buttonSize);
        ImGui::Begin("InvisibleButtonWindow", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::InvisibleButton("##ShowMenu", buttonSize)) {
            isMenuVisible = true;
        }
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    static const ImWchar icons_ranges[] = {0xe000, 0xf8ff, 0};
    ImFontConfig icons_config;
    ImFontConfig CustomFont;
    CustomFont.FontDataOwnedByAtlas = false;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.OversampleH = 2.5;
    icons_config.OversampleV = 2.5;

    static bool ic;
    static bool show;
    static bool HideWindow = true;

    DrawESP(ImGui::GetBackgroundDrawList(), get_width(), get_height(), get_density());
    AimBot_Func();

    // Floating Info Panel
    RenderFloatingInfo(ImGui::GetBackgroundDrawList(), get_width(), get_height());

    if (isMenuVisible) {
        ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(950, 650), ImGuiCond_Once);

        char title[256];
        sprintf(title, "HEXPHANTOM | GARENA | 1.6.55 | FPS: %.0d ###FPSTitle", static_cast<int>(io->Framerate), ImGui::GetFrameCount());

        if (ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            static bool isLogin = false;
            static std::string err;
            static char s[64] = "";
            static std::string storedKey = loadKeyFromFile();

            static float t = 0.0f;
            t += ImGui::GetIO().DeltaTime * 2.0f;
            ImVec4 rgbColor = ImVec4(
                (sin(t) * 0.5f + 0.5f),
                (sin(t + 2.094f) * 0.5f + 0.5f),
                (sin(t + 4.188f) * 0.5f + 0.5f),
                1.0f
            );

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Developer: @Hexxx22").x) * 0.5f);
            ImGui::TextColored(rgbColor, "Developer: @Hexxx22");
            ImGui::Separator();

            if (isLogin) {
                ImGui::Spacing();
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Ianhax Login").x) / 2);
                ImGui::Text("Enter Your Key:");
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
                ImGui::PushItemWidth(-1);
                if (ImGui::InputTextWithHint("##key", "Paste your key here...", s, sizeof(s), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    err = Login(s);
                    if (err == "OK") {
                        isLogin = false;
                        storedKey = s; 
                        saveKeyToFile(storedKey); 
                    }
                }
                ImGui::PopItemWidth();
                ImGui::PopStyleVar();
                ImGui::Spacing();

                float buttonWidth = (ImGui::GetContentRegionAvail().x / 2) - 5;
                if (ImGui::Button("Paste License", ImVec2(buttonWidth, 40))) {
                    auto key = getClipboard();
                    strncpy(s, key.c_str(), sizeof(s));
                    s[sizeof(s) - 1] = '\0'; 
                }
                ImGui::SameLine();
                if (ImGui::Button("Load License", ImVec2(buttonWidth, 40))) {
                    if (!storedKey.empty()) {
                        strncpy(s, storedKey.c_str(), sizeof(s));
                        s[sizeof(s) - 1] = '\0'; 
                    } else {
                        err = "No key has been stored yet.";
                    }
                }
                ImGui::Spacing();

                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
                if (ImGui::Button("Login", ImVec2(-1, 50))) {
                    if (strlen(s) == 0) {
                        err = "Please enter a valid license.";
                    } else {
                        err = Login(s);
                        if (err == "OK") {
                            isLogin = false;
                            storedKey = s; 
                            saveKeyToFile(storedKey); 
                        }
                    }
                }
                ImGui::PopStyleVar();
                ImGui::Spacing();

                if (!err.empty()) {
                    ImVec4 msgColor = (err == "OK") ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                    ImGui::TextColored(msgColor, err == "OK" ? "Login successful! Welcome to Ianhax Mod." : ("Error: " + err).c_str());
                }
            } else {
                static int tab = 1;
                const float navWidth = 190.0f;
                const float childHeight = ImGui::GetContentRegionAvail().y;

                ImGui::BeginChild("Tabs", ImVec2(navWidth, 0), true);
                if (ImGui::Button("  " ICON_FA_EYE "  VISUAL", ImVec2(-1, 73))) tab = 1;
                if (ImGui::Button("  " ICON_FA_DATABASE "  MEMORY", ImVec2(-1, 73))) tab = 2;
                if (ImGui::Button("  " ICON_FA_GAMEPAD "  ABILITY", ImVec2(-1, 73))) tab = 3;
                if (ImGui::Button("  " ICON_FA_CROSSHAIRS "  AIMING", ImVec2(-1, 73))) tab = 4;
                if (ImGui::Button("  " ICON_FA_USERS "  WEAPON", ImVec2(-1, 73))) tab = 5;
                if (ImGui::Button("  " ICON_FA_COG "  SETTING", ImVec2(-1, 73))) tab = 6;
                ImGui::EndChild();
                ImGui::SameLine();

                const float contentWidth = ImGui::GetContentRegionAvail().x;
                const float leftPanelWidth = contentWidth * 0.40f;
                const float rightPanelWidth = contentWidth - leftPanelWidth - ImGui::GetStyle().ItemSpacing.x;

                if (tab == 1) {
                    const float espLeftWidth = ImGui::GetContentRegionAvail().x * 0.5f;
                    const float espRightWidth = ImGui::GetContentRegionAvail().x - espLeftWidth - ImGui::GetStyle().ItemSpacing.x;

                    static float rgb_timer = 0.0f;
                    rgb_timer += ImGui::GetIO().DeltaTime * 0.5f;
                    ImVec4 rgb_color = ImVec4(
                        (sin(rgb_timer) * 0.5f + 0.5f),
                        (sin(rgb_timer + 2.0f) * 0.5f + 0.5f),
                        (sin(rgb_timer + 4.0f) * 0.5f + 0.5f),
                        1.0f
                    );
                    
                    ImGui::BeginChild("ESP Nav##1", ImVec2(espLeftWidth, childHeight), true);
                    {
                        ImGui::TextColored(rgb_color, "Extra Sensory Perception");
                        ImGui::Spacing();
                        
                        if (Config.ESPMenu.Esp) {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
                            if (ImGui::Button("ESP (ON)", ImVec2(-1, 40))) Config.ESPMenu.Esp = false;
                            ImGui::PopStyleColor();
                        } else {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
                            if (ImGui::Button("ESP (OFF)", ImVec2(-1, 40))) Config.ESPMenu.Esp = true;
                            ImGui::PopStyleColor();
                        }
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::TextColored(rgb_color, "Advanced Visuals");
                        ImGui::Spacing();
                        ImGui::Checkbox("ESP Line", &Config.ESPMenu.isPlayerLine);
                        ImGui::Checkbox("ESP Box", &Config.ESPMenu.Box);
                        ImGui::Checkbox("ESP Health", &Config.ESPMenu.Health);
                        ImGui::Checkbox("ESP Name", &Config.ESPMenu.Name);
                        ImGui::Checkbox("ESP Distance", &Config.ESPMenu.Distance);
                        ImGui::Checkbox("ESP Counter", &Config.ESPMenu.Count);
                        ImGui::Checkbox("ESP Skeleton", &Config.ESPMenu.Skeleton);
                        ImGui::Separator();
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("ESP Content##1", ImVec2(espRightWidth, childHeight), true);
                    {
                        ImGui::TextColored(rgb_color, "Extra Visuals");
                        ImGui::Spacing();
                        
                        if (ImGui::CollapsingHeader("Player ESP Colors")) {
                            static ImVec4 playerLineColor, playerBoxColor, playerNameColor, playerHealthColor, playerDistanceColor, playerSkeletonColor;
                            
                            if (Config.sColorsESPPLAYER.LinePLAYER) {
                                playerLineColor = ImVec4(Config.sColorsESPPLAYER.LinePLAYER[0], Config.sColorsESPPLAYER.LinePLAYER[1], 
                                                       Config.sColorsESPPLAYER.LinePLAYER[2], Config.sColorsESPPLAYER.LinePLAYER[3]);
                                if (ImGui::ColorEdit4("Player Line", (float*)&playerLineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPPLAYER.LinePLAYER[0] = playerLineColor.x;
                                    Config.sColorsESPPLAYER.LinePLAYER[1] = playerLineColor.y;
                                    Config.sColorsESPPLAYER.LinePLAYER[2] = playerLineColor.z;
                                    Config.sColorsESPPLAYER.LinePLAYER[3] = playerLineColor.w;
                                }
                            }
                            if (Config.sColorsESPPLAYER.BoxPLAYER) {
                                playerBoxColor = ImVec4(Config.sColorsESPPLAYER.BoxPLAYER[0], Config.sColorsESPPLAYER.BoxPLAYER[1], 
                                                      Config.sColorsESPPLAYER.BoxPLAYER[2], Config.sColorsESPPLAYER.BoxPLAYER[3]);
                                if (ImGui::ColorEdit4("Player Box", (float*)&playerBoxColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPPLAYER.BoxPLAYER[0] = playerBoxColor.x;
                                    Config.sColorsESPPLAYER.BoxPLAYER[1] = playerBoxColor.y;
                                    Config.sColorsESPPLAYER.BoxPLAYER[2] = playerBoxColor.z;
                                    Config.sColorsESPPLAYER.BoxPLAYER[3] = playerBoxColor.w;
                                }
                            }
                            if (Config.sColorsESPPLAYER.NamePLAYER) {
                                playerNameColor = ImVec4(Config.sColorsESPPLAYER.NamePLAYER[0], Config.sColorsESPPLAYER.NamePLAYER[1], 
                                                       Config.sColorsESPPLAYER.NamePLAYER[2], Config.sColorsESPPLAYER.NamePLAYER[3]);
                                if (ImGui::ColorEdit4("Player Name", (float*)&playerNameColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPPLAYER.NamePLAYER[0] = playerNameColor.x;
                                    Config.sColorsESPPLAYER.NamePLAYER[1] = playerNameColor.y;
                                    Config.sColorsESPPLAYER.NamePLAYER[2] = playerNameColor.z;
                                    Config.sColorsESPPLAYER.NamePLAYER[3] = playerNameColor.w;
                                }
                            }
                            if (Config.sColorsESPPLAYER.HealthPLAYER) {
                                playerHealthColor = ImVec4(Config.sColorsESPPLAYER.HealthPLAYER[0], Config.sColorsESPPLAYER.HealthPLAYER[1], 
                                                          Config.sColorsESPPLAYER.HealthPLAYER[2], Config.sColorsESPPLAYER.HealthPLAYER[3]);
                                if (ImGui::ColorEdit4("Player Health", (float*)&playerHealthColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPPLAYER.HealthPLAYER[0] = playerHealthColor.x;
                                    Config.sColorsESPPLAYER.HealthPLAYER[1] = playerHealthColor.y;
                                    Config.sColorsESPPLAYER.HealthPLAYER[2] = playerHealthColor.z;
                                    Config.sColorsESPPLAYER.HealthPLAYER[3] = playerHealthColor.w;
                                }
                            }
                            if (Config.sColorsESPPLAYER.DistancePLAYER) {
                                playerDistanceColor = ImVec4(Config.sColorsESPPLAYER.DistancePLAYER[0], Config.sColorsESPPLAYER.DistancePLAYER[1], 
                                                           Config.sColorsESPPLAYER.DistancePLAYER[2], Config.sColorsESPPLAYER.DistancePLAYER[3]);
                                if (ImGui::ColorEdit4("Player Distance", (float*)&playerDistanceColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPPLAYER.DistancePLAYER[0] = playerDistanceColor.x;
                                    Config.sColorsESPPLAYER.DistancePLAYER[1] = playerDistanceColor.y;
                                    Config.sColorsESPPLAYER.DistancePLAYER[2] = playerDistanceColor.z;
                                    Config.sColorsESPPLAYER.DistancePLAYER[3] = playerDistanceColor.w;
                                }
                            }
                            // ====== SKELETON COLOR FOR PLAYER ======
                            if (Config.sColorsESPPLAYER.SkeletonPLAYER) {
                                playerSkeletonColor = ImVec4(Config.sColorsESPPLAYER.SkeletonPLAYER[0], Config.sColorsESPPLAYER.SkeletonPLAYER[1],
                                                             Config.sColorsESPPLAYER.SkeletonPLAYER[2], Config.sColorsESPPLAYER.SkeletonPLAYER[3]);
                                if (ImGui::ColorEdit4("Player Skeleton", (float*)&playerSkeletonColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPPLAYER.SkeletonPLAYER[0] = playerSkeletonColor.x;
                                    Config.sColorsESPPLAYER.SkeletonPLAYER[1] = playerSkeletonColor.y;
                                    Config.sColorsESPPLAYER.SkeletonPLAYER[2] = playerSkeletonColor.z;
                                    Config.sColorsESPPLAYER.SkeletonPLAYER[3] = playerSkeletonColor.w;
                                }
                            }
                            // =======================================
                            ImGui::Separator();
                        }

                        if (ImGui::CollapsingHeader("Bot ESP Colors")) {
                            static ImVec4 botLineColor, botBoxColor, botNameColor, botHealthColor, botDistanceColor, botSkeletonColor;
                            
                            if (Config.sColorsESPBOT.LineBOT) {
                                botLineColor = ImVec4(Config.sColorsESPBOT.LineBOT[0], Config.sColorsESPBOT.LineBOT[1], 
                                                    Config.sColorsESPBOT.LineBOT[2], Config.sColorsESPBOT.LineBOT[3]);
                                if (ImGui::ColorEdit4("Bot Line", (float*)&botLineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPBOT.LineBOT[0] = botLineColor.x;
                                    Config.sColorsESPBOT.LineBOT[1] = botLineColor.y;
                                    Config.sColorsESPBOT.LineBOT[2] = botLineColor.z;
                                    Config.sColorsESPBOT.LineBOT[3] = botLineColor.w;
                                }
                            }
                            if (Config.sColorsESPBOT.BoxBOT) {
                                botBoxColor = ImVec4(Config.sColorsESPBOT.BoxBOT[0], Config.sColorsESPBOT.BoxBOT[1], 
                                                   Config.sColorsESPBOT.BoxBOT[2], Config.sColorsESPBOT.BoxBOT[3]);
                                if (ImGui::ColorEdit4("Bot Box", (float*)&botBoxColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPBOT.BoxBOT[0] = botBoxColor.x;
                                    Config.sColorsESPBOT.BoxBOT[1] = botBoxColor.y;
                                    Config.sColorsESPBOT.BoxBOT[2] = botBoxColor.z;
                                    Config.sColorsESPBOT.BoxBOT[3] = botBoxColor.w;
                                }
                            }
                            if (Config.sColorsESPBOT.NameBOT) {
                                botNameColor = ImVec4(Config.sColorsESPBOT.NameBOT[0], Config.sColorsESPBOT.NameBOT[1], 
                                                    Config.sColorsESPBOT.NameBOT[2], Config.sColorsESPBOT.NameBOT[3]);
                                if (ImGui::ColorEdit4("Bot Name", (float*)&botNameColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPBOT.NameBOT[0] = botNameColor.x;
                                    Config.sColorsESPBOT.NameBOT[1] = botNameColor.y;
                                    Config.sColorsESPBOT.NameBOT[2] = botNameColor.z;
                                    Config.sColorsESPBOT.NameBOT[3] = botNameColor.w;
                                }
                            }
                            if (Config.sColorsESPBOT.HealthBOT) {
                                botHealthColor = ImVec4(Config.sColorsESPBOT.HealthBOT[0], Config.sColorsESPBOT.HealthBOT[1], 
                                                      Config.sColorsESPBOT.HealthBOT[2], Config.sColorsESPBOT.HealthBOT[3]);
                                if (ImGui::ColorEdit4("Bot Health", (float*)&botHealthColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPBOT.HealthBOT[0] = botHealthColor.x;
                                    Config.sColorsESPBOT.HealthBOT[1] = botHealthColor.y;
                                    Config.sColorsESPBOT.HealthBOT[2] = botHealthColor.z;
                                    Config.sColorsESPBOT.HealthBOT[3] = botHealthColor.w;
                                }
                            }
                            if (Config.sColorsESPBOT.DistanceBOT) {
                                botDistanceColor = ImVec4(Config.sColorsESPBOT.DistanceBOT[0], Config.sColorsESPBOT.DistanceBOT[1], 
                                                        Config.sColorsESPBOT.DistanceBOT[2], Config.sColorsESPBOT.DistanceBOT[3]);
                                if (ImGui::ColorEdit4("Bot Distance", (float*)&botDistanceColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPBOT.DistanceBOT[0] = botDistanceColor.x;
                                    Config.sColorsESPBOT.DistanceBOT[1] = botDistanceColor.y;
                                    Config.sColorsESPBOT.DistanceBOT[2] = botDistanceColor.z;
                                    Config.sColorsESPBOT.DistanceBOT[3] = botDistanceColor.w;
                                }
                            }
                            // ====== SKELETON COLOR FOR BOT ======
                            if (Config.sColorsESPBOT.SkeletonBOT) {
                                botSkeletonColor = ImVec4(Config.sColorsESPBOT.SkeletonBOT[0], Config.sColorsESPBOT.SkeletonBOT[1],
                                                          Config.sColorsESPBOT.SkeletonBOT[2], Config.sColorsESPBOT.SkeletonBOT[3]);
                                if (ImGui::ColorEdit4("Bot Skeleton", (float*)&botSkeletonColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPBOT.SkeletonBOT[0] = botSkeletonColor.x;
                                    Config.sColorsESPBOT.SkeletonBOT[1] = botSkeletonColor.y;
                                    Config.sColorsESPBOT.SkeletonBOT[2] = botSkeletonColor.z;
                                    Config.sColorsESPBOT.SkeletonBOT[3] = botSkeletonColor.w;
                                }
                            }
                            // ====================================
                            ImGui::Separator();
                        }


                        if (ImGui::CollapsingHeader("Other ESP Colors")) {
                            if (Config.sColorsESPOTHERS.PovOTHERS) {
                                static ImVec4 povColor = ImVec4(Config.sColorsESPOTHERS.PovOTHERS[0], Config.sColorsESPOTHERS.PovOTHERS[1],
                                                               Config.sColorsESPOTHERS.PovOTHERS[2], Config.sColorsESPOTHERS.PovOTHERS[3]);
                                if (ImGui::ColorEdit4("POV Color", (float*)&povColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                                    Config.sColorsESPOTHERS.PovOTHERS[0] = povColor.x;
                                    Config.sColorsESPOTHERS.PovOTHERS[1] = povColor.y;
                                    Config.sColorsESPOTHERS.PovOTHERS[2] = povColor.z;
                                    Config.sColorsESPOTHERS.PovOTHERS[3] = povColor.w;
                                }
                            }
                            ImGui::Separator();
                        }

                        if (ImGui::Button("Reset All Colors to Default", ImVec2(-1, 30))) {
                            if (Config.sColorsESPPLAYER.LinePLAYER) {
                                Config.sColorsESPPLAYER.LinePLAYER[0] = 1.0f;
                                Config.sColorsESPPLAYER.LinePLAYER[1] = 0.0f;
                                Config.sColorsESPPLAYER.LinePLAYER[2] = 0.0f;
                                Config.sColorsESPPLAYER.LinePLAYER[3] = 1.0f;
                            }
                            if (Config.sColorsESPPLAYER.BoxPLAYER) {
                                Config.sColorsESPPLAYER.BoxPLAYER[0] = 1.0f;
                                Config.sColorsESPPLAYER.BoxPLAYER[1] = 0.0f;
                                Config.sColorsESPPLAYER.BoxPLAYER[2] = 0.0f;
                                Config.sColorsESPPLAYER.BoxPLAYER[3] = 1.0f;
                            }
                            if (Config.sColorsESPPLAYER.NamePLAYER) {
                                Config.sColorsESPPLAYER.NamePLAYER[0] = 1.0f;
                                Config.sColorsESPPLAYER.NamePLAYER[1] = 0.0f;
                                Config.sColorsESPPLAYER.NamePLAYER[2] = 0.0f;
                                Config.sColorsESPPLAYER.NamePLAYER[3] = 1.0f;
                            }
                            if (Config.sColorsESPPLAYER.HealthPLAYER) {
                                Config.sColorsESPPLAYER.HealthPLAYER[0] = 1.0f;
                                Config.sColorsESPPLAYER.HealthPLAYER[1] = 0.0f;
                                Config.sColorsESPPLAYER.HealthPLAYER[2] = 0.0f;
                                Config.sColorsESPPLAYER.HealthPLAYER[3] = 1.0f;
                            }
                            if (Config.sColorsESPPLAYER.DistancePLAYER) {
                                Config.sColorsESPPLAYER.DistancePLAYER[0] = 1.0f;
                                Config.sColorsESPPLAYER.DistancePLAYER[1] = 0.0f;
                                Config.sColorsESPPLAYER.DistancePLAYER[2] = 0.0f;
                                Config.sColorsESPPLAYER.DistancePLAYER[3] = 1.0f;
                            }
                            if (Config.sColorsESPBOT.LineBOT) {
                                Config.sColorsESPBOT.LineBOT[0] = 1.0f;
                                Config.sColorsESPBOT.LineBOT[1] = 1.0f;
                                Config.sColorsESPBOT.LineBOT[2] = 1.0f;
                                Config.sColorsESPBOT.LineBOT[3] = 0.7f;
                            }
                            if (Config.sColorsESPBOT.BoxBOT) {
                                Config.sColorsESPBOT.BoxBOT[0] = 1.0f;
                                Config.sColorsESPBOT.BoxBOT[1] = 1.0f;
                                Config.sColorsESPBOT.BoxBOT[2] = 1.0f;
                                Config.sColorsESPBOT.BoxBOT[3] = 0.7f;
                            }
                            if (Config.sColorsESPBOT.NameBOT) {
                                Config.sColorsESPBOT.NameBOT[0] = 1.0f;
                                Config.sColorsESPBOT.NameBOT[1] = 1.0f;
                                Config.sColorsESPBOT.NameBOT[2] = 1.0f;
                                Config.sColorsESPBOT.NameBOT[3] = 0.7f;
                            }
                            if (Config.sColorsESPBOT.HealthBOT) {
                                Config.sColorsESPBOT.HealthBOT[0] = 1.0f;
                                Config.sColorsESPBOT.HealthBOT[1] = 1.0f;
                                Config.sColorsESPBOT.HealthBOT[2] = 1.0f;
                                Config.sColorsESPBOT.HealthBOT[3] = 0.7f;
                            }
                            if (Config.sColorsESPBOT.DistanceBOT) {
                                Config.sColorsESPBOT.DistanceBOT[0] = 1.0f;
                                Config.sColorsESPBOT.DistanceBOT[1] = 1.0f;
                                Config.sColorsESPBOT.DistanceBOT[2] = 1.0f;
                                Config.sColorsESPBOT.DistanceBOT[3] = 0.7f;
                            }
                            if (Config.sColorsESPOTHERS.PovOTHERS) {
                                Config.sColorsESPOTHERS.PovOTHERS[0] = 0.88f;
                                Config.sColorsESPOTHERS.PovOTHERS[1] = 0.0f;
                                Config.sColorsESPOTHERS.PovOTHERS[2] = 0.0f;
                                Config.sColorsESPOTHERS.PovOTHERS[3] = 0.7f;
                            }
                            if (Config.sColorsESPPLAYER.SkeletonPLAYER) {
                                Config.sColorsESPPLAYER.SkeletonPLAYER[0] = 1.0f;
                                Config.sColorsESPPLAYER.SkeletonPLAYER[1] = 1.0f;
                                Config.sColorsESPPLAYER.SkeletonPLAYER[2] = 1.0f;
                                Config.sColorsESPPLAYER.SkeletonPLAYER[3] = 1.0f;
                            }
                            if (Config.sColorsESPBOT.SkeletonBOT) {
                                Config.sColorsESPBOT.SkeletonBOT[0] = 1.0f;
                                Config.sColorsESPBOT.SkeletonBOT[1] = 1.0f;
                                Config.sColorsESPBOT.SkeletonBOT[2] = 1.0f;
                                Config.sColorsESPBOT.SkeletonBOT[3] = 1.0f;
                            }
                        }
                        
                        ImGui::Text("Crosshair Style");
                        const char* crosshairStyles[] = { "Circle", "Plus", "X" };
                        for (int i = 0; i < IM_ARRAYSIZE(crosshairStyles); i++) {
                            if (ImGui::RadioButton(crosshairStyles[i], static_cast<int>(Config.ESPMenu.CrosshairType) == i))
                                Config.ESPMenu.CrosshairType = static_cast<decltype(Config.ESPMenu.CrosshairType)>(i);
                        }
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        
                        ImGui::Text("FOV Size:");
                        ImGui::SliderFloat("##FOV", &Config.Aim.size, 0, 500, "%.0f");
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        
                        ImGui::Text("Custom Settings:");
                        ImGui::Checkbox("Custom Aim Line", &Config.ESPMenu.Aimline);
                    }
                    ImGui::EndChild();
                }

                if (tab == 2) {
                    const float leftPanelWidth = ImGui::GetContentRegionAvail().x * 0.5f;
                    const float rightPanelWidth = ImGui::GetContentRegionAvail().x - leftPanelWidth - ImGui::GetStyle().ItemSpacing.x;

                    static float rgb_timer = 0.0f;
                    rgb_timer += ImGui::GetIO().DeltaTime * 0.5f;
                    ImVec4 rgb_color = ImVec4(
                        (sin(rgb_timer) * 0.5f + 0.5f),
                        (sin(rgb_timer + 2.0f) * 0.5f + 0.5f),
                        (sin(rgb_timer + 4.0f) * 0.5f + 0.5f),
                        1.0f
                    );

                    ImGui::BeginChild("Memory Nav##2", ImVec2(leftPanelWidth, childHeight), true);
                    {
                        ImGui::TextColored(rgb_color, "Custom Aimlock");
                        ImGui::Spacing();
                        
                        if (ImGui::Checkbox("Aimlock", &Config.Aim.AimAssist)) {
                            Config.Aim.showAimAssistSlider = Config.Aim.AimAssist;
                        }
                        
                        if (Config.Aim.showAimAssistSlider) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                            ImGui::Text("Aimlock Settings");
                            ImGui::PopStyleColor();
                            ImGui::SliderFloat("Locking", &Config.Aim.AimAssistSize, 0, 100, "%.0f");
                            ImGui::Spacing();
                            ImGui::Separator();
                        }
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::TextColored(rgb_color, "Weapon Memory V1");
                        ImGui::Spacing();
                        ImGui::Checkbox("Weapon Kinetic", &Config.ExtraMenu.Kinetic);
                        ImGui::Checkbox("No Recoil", &Config.ExtraMenu.Recoil);
                        ImGui::Checkbox("No Spread", &Config.ExtraMenu.Spread);
                        ImGui::Checkbox("No Reload", &Config.ExtraMenu.Reload);
                        ImGui::Checkbox("No Scope", &Config.ExtraMenu.Scope);
                        ImGui::Checkbox("No Switch", &Config.ExtraMenu.Switch);
                        ImGui::Checkbox("No Shake", &Config.ExtraMenu.Shake);
                        ImGui::Separator();
                    }
                    ImGui::EndChild();
                    ImGui::SameLine();

                    ImGui::BeginChild("Memory Content##2", ImVec2(rightPanelWidth, childHeight), true);
                    {
                        ImGui::TextColored(rgb_color, "Weapon Memory V2");
                        ImGui::Spacing();
                        ImGui::Checkbox("Unlock Blueprints", &Config.ExtraMenu.Blueprints);
                        ImGui::Checkbox("Hitbox++", &Config.ExtraMenu.Hit);
                        ImGui::Separator();
                    }
                    ImGui::EndChild();
                }

                if (tab == 3) {
                    const float leftPanelWidth = ImGui::GetContentRegionAvail().x * 0.5f;
                    const float rightPanelWidth = ImGui::GetContentRegionAvail().x - leftPanelWidth - ImGui::GetStyle().ItemSpacing.x;

                    static float rgb_timer = 0.0f;
                    rgb_timer += ImGui::GetIO().DeltaTime * 0.5f;
                    ImVec4 rgb_color = ImVec4(
                        (sin(rgb_timer) * 0.5f + 0.5f),
                        (sin(rgb_timer + 2.0f) * 0.5f + 0.5f),
                        (sin(rgb_timer + 4.0f) * 0.5f + 0.5f),
                        1.0f
                    );

                    ImGui::BeginChild("Ability Nav##3", ImVec2(leftPanelWidth, childHeight), true);
                    {
                        ImGui::TextColored(rgb_color, "Movement Abilities");
                        ImGui::Spacing();

                        if (ImGui::Checkbox("Snowboard Boost", &SnowB)) SnowBshow = SnowB;
                        if (ImGui::Checkbox("Long Slide", &SlideTackle)) SlideShow = SlideTackle;
                        if (ImGui::Checkbox("Speed Hack", &isSpeedHackEnabled)) showSpeedHackSlider = isSpeedHackEnabled;
                        if (ImGui::Checkbox("Ipad View", &range3P)) view3PEnabled = range3P;
                        if (ImGui::Checkbox("High Jump", &isJumpAdjustmentEnabled)) {
                            if (!isJumpAdjustmentEnabled) jumpHeightMultiplier = 1.0f;
                        }

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::TextColored(rgb_color, "Visual Abilities");
                        ImGui::Spacing();

                        ImGui::Checkbox("Wallhack Red", &Config.ExtraMenu.RedWallhack);           
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("Ability Content##3", ImVec2(rightPanelWidth, childHeight), true);
                    {
                        ImGui::TextColored(rgb_color, "Ability Settings");
                        ImGui::Spacing();

                        if (SnowBshow) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                            ImGui::Text("Snowboard Settings:");
                            ImGui::PopStyleColor();
                            ImGui::SliderFloat("Boost", &SnowBsize, 0.0f, 100.0f, "%.1f");
                            ImGui::Spacing();
                        }

                        if (SlideShow) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                            ImGui::Text("Slide Settings:");
                            ImGui::PopStyleColor();
                            ImGui::SliderFloat("Slide", &SlideRange, 0.0f, 30.0f, "%.1f");
                            ImGui::Spacing();
                        }

                        if (showSpeedHackSlider) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                            ImGui::Text("Speed Settings:");
                            ImGui::PopStyleColor();
                            ImGui::SliderFloat("Speed", &speedHackMultiplier, 0.5f, 10.0f, "%.1f");
                            ImGui::Spacing();
                        }
                        
                        if (view3PEnabled) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                            ImGui::Text("3p View:");
                            ImGui::PopStyleColor();
                            ImGui::SliderFloat("View", &range3PValue, 0.5f, 150.0f, "%.1f");
                            ImGui::Spacing();
                        }

                        if (isJumpAdjustmentEnabled) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
                            ImGui::Text("Jump Settings:");
                            ImGui::PopStyleColor();
                            ImGui::SliderFloat("Height", &jumpHeightMultiplier, 0.5f, 5.0f, "%.1f");
                            ImGui::Spacing();
                        }

                        ImGui::Separator();
                        ImGui::TextColored(rgb_color, "Additional Features");
                        ImGui::Spacing();

                        ImGui::Checkbox("No Parachute", &Config.ExtraMenu.Parachute);
                        ImGui::Checkbox("Sky Diving Speed", &Config.ExtraMenu.Diving);
                        ImGui::Checkbox("Walk Underwater", &Config.ExtraMenu.Water);
                    }
                    ImGui::EndChild();
                }

                if (tab == 4) {
                    ImGui::BeginChild("Aim Nav##4", ImVec2(leftPanelWidth, childHeight), true);
                    ImGui::Checkbox("Aimbot 360", &Config.Aim.Aimbot360);
                    ImGui::Checkbox("Bullet Track", &Config.Aim.AimSilent);
                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("Aim Content##4", ImVec2(rightPanelWidth, childHeight), true);
                    ImGui::Text("Location");
                    const char* targets[] = { "Head", "Chest", "Body" };
                    ImGui::Columns(3, NULL, false);
                    for (int i = 0; i < IM_ARRAYSIZE(targets); i++) {
                        if (ImGui::RadioButton(targets[i], static_cast<int>(Config.Aim.Target) == i))
                            Config.Aim.Target = static_cast<decltype(Config.Aim.Target)>(i);
                        ImGui::NextColumn();
                    }
                    ImGui::Columns(1);
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Text("Trigger");
                    const char* triggers[] = { "None", "Shooting", "Scoping" };
                    ImGui::Columns(3, NULL, false);
                    for (int i = 0; i < IM_ARRAYSIZE(triggers); i++) {
                        if (ImGui::RadioButton(triggers[i], static_cast<int>(Config.Aim.Trigger) == i))
                            Config.Aim.Trigger = static_cast<decltype(Config.Aim.Trigger)>(i);
                        ImGui::NextColumn();
                    }
                    ImGui::Columns(1);
                    ImGui::Spacing();
                    ImGui::Text("Target");
                    const char* tarets[] = { "Distance", "FOV" };
                    ImGui::Columns(2, NULL, false);
                    for (int i = 0; i < IM_ARRAYSIZE(tarets); i++) {
                        if (ImGui::RadioButton(tarets[i], static_cast<int>(Config.Aim.By) == i))
                            Config.Aim.By = static_cast<decltype(Config.Aim.By)>(i);
                        ImGui::NextColumn();
                    }
                    ImGui::Columns(1);
                    ImGui::EndChild();
                }

                if (tab == 5) {
                    static int current_tab = 0;
                    
                    static bool camoOff = true;
                    static bool camoDiamond = false;
                    static bool camoRedSprite = false;

                    #define ID_DIAMOND 0x1D37F758 
                    #define ID_RED_SPRITE 0x1D37F77E

                    ImGui::BeginChild("Main Skin Tab", ImVec2(725, 520), true);
                    
                    ImVec2 tab_size = ImVec2(100, 30);
                    if (ImGui::Selectable("SKINS",   current_tab == 0, 0, tab_size)) current_tab = 0;
                    ImGui::SameLine();
                    if (ImGui::Selectable("WEAPONS", current_tab == 4, 0, tab_size)) current_tab = 4;
                    ImGui::SameLine();
                    if (ImGui::Selectable("CAMOS",   current_tab == 5, 0, tab_size)) current_tab = 5;

                    ImGui::Separator();

                    if (current_tab == 0) {
                        ImGui::BeginChild("Char Skin List", ImVec2(0, 440), true); 
                        if (!charData.empty()) {
                            std::string searchLower = ToLower(searchQuery);
                            for (const auto& getchar : charData) {
                                if (!searchLower.empty() && ToLower(getchar.charName).find(searchLower) == std::string::npos) continue;

                                if (ImGui::Checkbox(getchar.charName.c_str(), &sBool[getchar.charName])) {
                                    if (sBool[getchar.charName]) {
                                        for (auto charModel : CharacterModelConfigInstance) {
                                            if (!charModel) continue;
                                            characterfields = (CharacterModelFields*)((uintptr_t)charModel + 0x10);
                                            if (characterfields->Traitor1P == 710001101) {
                                                characterfields->BRBagModel = getchar.charModel[0];
                                                characterfields->BRHeadModel = getchar.charModel[1];
                                                characterfields->BRLobby = getchar.charModel[2];
                                                characterfields->BRModel = getchar.charModel[3];
                                                characterfields->BindEffect1P = getchar.charModel[4];
                                                characterfields->ChangeClipEffect1P = getchar.charModel[5];
                                                characterfields->DefaultModelID = getchar.charModel[6];
                                                characterfields->Guarder1P = getchar.charModel[7];
                                                characterfields->Guarder3P = getchar.charModel[8];
                                                characterfields->GuarderBagModel = getchar.charModel[9];
                                                characterfields->GuarderHeadModel = getchar.charModel[10];
                                                characterfields->GuarderLobby = getchar.charModel[11];
                                            }
                                        }
                                        for (auto itemRes : itemResourceConfigInstance) {
                                            if (!itemRes) continue;
                                            itemFields = (ItemResourceFields*)((uintptr_t)itemRes + 0x10);
                                            if (itemFields->ID == 100301208) {
                                                itemFields->FxAssetID = getchar.charRes[0];
                                                itemFields->InventoryModelID = getchar.charRes[1];
                                                itemFields->ModelAssetIDRaw = getchar.charRes[2];
                                                itemFields->UIMiniSpriteName = getchar.charRes2[0];
                                                itemFields->UISmallSpriteName = getchar.charRes2[1];
                                                itemFields->UISpriteName = getchar.charRes2[2];
                                                itemFields->UISquareSpriteName = getchar.charRes2[3];
                                            }
                                        }
                                        for (auto roles : RoleConfConfigInstance) {
                                            if (!roles) continue;
                                            RoleConfFields* roleF = (RoleConfFields*)((uintptr_t)roles + 0x14);
                                            if (roleF->ID == 100301208) {
                                                roleF->roleLeftArmID = getchar.charRole[0];
                                                roleF->roleFinalSuitID = getchar.charRole[1];
                                                roleF->roleBasicHologramID = getchar.charRole[2];
                                                roleF->ColorID = getchar.charRole[3];
                                                roleF->ColorSubID = getchar.charRole[4];
                                                roleF->ShowRare = getchar.charRole[5];
                                                roleF->RoleLvGroupID = getchar.charRole[6];
                                                roleF->RolePackID = getchar.charRole[7];
                                            }
                                        }
                                        sBool[getchar.charName] = false;
                                    }
                                }
                            }
                        }
                        ImGui::EndChild();

                        ImGui::SetNextWindowSize(ImVec2(650, 440));
                        ImGui::SetNextWindowPos(ImVec2(0, 0));
                        if (ImGui::Begin("Adjust_Char", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
                            ImGui::InputText("Search", searchQuery, IM_ARRAYSIZE(searchQuery));    
                            KeyboardWindow(searchQuery, true);
                            ImGui::End();
                        }
                    }
                    else if (current_tab == 1) {
                        ImGui::BeginChild("WatchList", ImVec2(0,0), true);
                        for (const auto& forwatch : watch) {
                            if (ImGui::Checkbox(forwatch.watchname.c_str(), &sBool[forwatch.watchname])) {
                                if (sBool[forwatch.watchname]) {
                                    for (auto charModel : CharacterModelConfigInstance) {
                                        if (!charModel) continue;
                                        characterfields = (CharacterModelFields*)((uintptr_t)charModel + 0x10);
                                        if (characterfields->Traitor1P == 710001101) characterfields->BindEffect1P = forwatch.watchvalue;
                                    }
                                    sBool[forwatch.watchname] = false;
                                }
                            }
                        }
                        ImGui::EndChild();
                    }
                    else if (current_tab == 2) {
                        ImGui::BeginChild("DeadboxList", ImVec2(0,0), true);
                        for (const auto& deadx : deadboxF) {
                            if (ImGui::Checkbox(deadx.deadname.c_str(), &sBool[deadx.deadname])) {
                                if (sBool[deadx.deadname]) {
                                    for (auto deadID : BRDeadboxSkinConfigInstance) {
                                        if (!deadID) continue;
                                        deadboxFields = (BRDeadboxSkinFields*)((uintptr_t)deadID + 0x10);
                                        if (deadboxFields->ID == 180300004) {
                                            deadboxFields->ColorID = deadx.dead[0];
                                            deadboxFields->DeadBoxEffectAsset = deadx.dead[1];
                                            deadboxFields->Flag = deadx.dead[2];
                                            deadboxFields->FlagAsset = deadx.dead[3];
                                            deadboxFields->ModelAsset3P = deadx.dead[4];
                                            deadboxFields->ModelAssetUI = deadx.dead[5];
                                        }
                                    }
                                    sBool[deadx.deadname] = false;
                                }
                            }
                        }
                        ImGui::EndChild();
                    }
                    else if (current_tab == 3) {
                        ImGui::BeginChild("PlaneList", ImVec2(0,0), true);
                        if (dropplane.empty()) {
                            ImGui::TextWrapped("Goto [Safehouse] > [Arsenal] > [Operators] -> [Find Plane]");
                        } else {
                            for (const auto& planex : dropplane) {
                                if (ImGui::Checkbox(planex.planename.c_str(), &sBool[planex.planename])) {
                                    if (sBool[planex.planename]) {
                                        for (auto planeID : BRDropPlaneSkinConfigInstance) {
                                            if (!planeID) continue;
                                            dropplaneFields = (BRDropPlaneSkinFields*)((uintptr_t)planeID + 0x10);
                                            if (dropplaneFields->ID == 0) {
                                                dropplaneFields->ColorID = planex.plane[0];
                                                dropplaneFields->ModelAsset1P = planex.plane[1];
                                                dropplaneFields->ModelAsset3P = planex.plane[2];
                                                dropplaneFields->ModelAssetCutScene = planex.plane[3];
                                                dropplaneFields->ModelAssetUI = planex.plane[4];
                                                dropplaneFields->Priority = planex.plane[5];
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        ImGui::EndChild();
                    }
                    else if (current_tab == 4) {
                        ImGui::BeginChild("Gun Skin", ImVec2(0, 440), true);
                        if (!itemData.empty()) {
                            std::string searchLower = ToLower(searchQuery2);
                            for (const auto& getitem : itemData) {
                                if (!searchLower.empty() && ToLower(getitem.itemName).find(searchLower) == std::string::npos) continue;

                                if (ImGui::Checkbox(getitem.itemName.c_str(), &sBool[getitem.itemName])) {
                                    if (sBool[getitem.itemName]) {
                                        int baseWeaponID = getitem.WeaponConf[0];
                                        int itemID = getitem.Item2Inventory[0];
                                        int lootID = getitem.Item2Inventory[3];

                                        for (auto item : itemInventoryInstance) {
                                            if (!item) continue;
                                            item2Fields = (Item2InventoryFields*)((uintptr_t)item + 0x20);
                                            if (item2Fields->ItemID == itemID || item2Fields->ItemID == lootID) {
                                                item2Fields->WeaponAssetGroupID = getitem.Item2Inventory[1];
                                                item2Fields->WeaponIconID       = getitem.Item2Inventory[2];
                                            }
                                        }

                                        for (auto conf : weaponConfInstance) {
                                            if (!conf) continue;
                                            weaponconfFields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
                                            if (baseWeaponID == weaponconfFields->ID || lootID == weaponconfFields->ID) {
                                                weaponconfFields->ColorID            = getitem.WeaponConf[1];
                                                weaponconfFields->DefWeaponSkinID    = getitem.WeaponConf[2];
                                                weaponconfFields->DefaultKillBrocast = getitem.WeaponConf[3];
                                                
                                                for (auto skinConf : weaponConfInstance) {
                                                    if (!skinConf) continue;
                                                    WeaponConfFields *skinFields = (WeaponConfFields *)((uintptr_t)skinConf + 0x20);
                                                    if (skinFields->ID == getitem.WeaponConf[2]) {
                                                        weaponconfFields->LOCID_Name = skinFields->LOCID_Name;
                                                        break;
                                                    }
                                                }
                                            }
                                        }

                                        if (getitem.WeaponExtra[4] > 0) {
                                            activeKillEffects[baseWeaponID] = getitem.WeaponExtra[4];
                                            activeKillEffects[itemID] = getitem.WeaponExtra[4];
                                            activeKillEffects[lootID] = getitem.WeaponExtra[4];
                                        }

                                        for (auto extra : weaponExtraInstance) {
                                            if (!extra) continue;
                                            weaponextraFields = (WeaponConfExtraFields*)((uintptr_t)extra + 0x10);
                                            if (weaponextraFields->ID == getitem.WeaponExtra[0] || weaponextraFields->ID == getitem.WeaponConf[2]) {
                                                if (getitem.itemName.find("[M]") != std::string::npos) {
                                                    weaponextraFields->DefaultMythicArmor = getitem.WeaponExtra[1];
                                                    weaponextraFields->DefaultMythicSig   = getitem.WeaponExtra[2];
                                                }
                                                weaponextraFields->DefaultDeadReplayEffectId = getitem.WeaponExtra[3];
                                                weaponextraFields->DefaultKillEffectId        = getitem.WeaponExtra[4];
                                            }
                                        }

                                        for (auto asset : weaponAssetGroupInstance) {
                                            if (!asset) continue;
                                            weaponAssetFields = (WeaponAssetGroupFields*)((uintptr_t)asset + 0x40);
                                            if (weaponAssetFields->Id == getitem.WeaponAsset[0]) {
                                                if (getitem.WeaponAsset[1] / 10 == getitem.WeaponAsset[2] / 10 && getitem.WeaponAsset[1] > 0) {
                                                    weaponAssetFields->FireEffectGroupID = getitem.WeaponAsset[1];
                                                }
                                            }
                                        }

                                        for (auto itemResource : itemResourceConfigInstance) {
                                            if (!itemResource) continue;
                                            itemFields = (ItemResourceFields*)((uintptr_t)itemResource + 0x10);
                                            if (itemFields->ID == itemID) {
                                                itemFields->FxAssetID          = getitem.ItemResInt[0];
                                                itemFields->InventoryModelID   = getitem.ItemResInt[1];
                                                itemFields->ModelAssetIDRaw    = getitem.ItemResInt[2];
                                                itemFields->UIMiniSpriteName   = getitem.ItemRes[0];
                                                itemFields->UISmallSpriteName  = getitem.ItemRes[1];
                                                itemFields->UISpriteName       = getitem.ItemRes[2];
                                                itemFields->UISquareSpriteName = getitem.ItemRes[3];
                                            }
                                        }
                                        sBool[getitem.itemName] = false;
                                    }
                                }
                            }
                        }
                        ImGui::EndChild();

                        ImGui::SetNextWindowSize(ImVec2(650, 440));
                        ImGui::SetNextWindowPos(ImVec2(0, 0));
                        if (ImGui::Begin("Adjust_Weapon", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
                            ImGui::InputText("Search", searchQuery2, IM_ARRAYSIZE(searchQuery2));
                            KeyboardWindow(searchQuery2, true);
                            ImGui::End();
                        }
                    }
                    else if (current_tab == 5) {
                        ImGui::BeginChild("Camo Manager", ImVec2(0, 0), true);
                        ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "Inject Camo to Mythic/Legendary Skins");
                        ImGui::Dummy(ImVec2(0, 10));

                        if (ImGui::Checkbox("Default / OFF", &camoOff)) {
                            if (camoOff) {
                                camoDiamond = false;
                                camoRedSprite = false;
                                for (const auto& getitem : itemData) {
                                    if (getitem.itemName.find("[M]") != std::string::npos || getitem.itemName.find("[L]") != std::string::npos) {
                                        for (auto conf : weaponConfInstance) {
                                            if (!conf) continue;
                                            weaponconfFields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
                                            if (weaponconfFields->ID == getitem.WeaponConf[2]) weaponconfFields->DefWeaponSkinID = 0;
                                        }
                                    }
                                }
                            }
                        }

                        ImGui::Separator();

                        if (ImGui::Checkbox("Diamond Camo", &camoDiamond)) {
                            if (camoDiamond) {
                                camoOff = false;
                                camoRedSprite = false;
                                for (const auto& getitem : itemData) {
                                    if (getitem.itemName.find("[M]") != std::string::npos || getitem.itemName.find("[L]") != std::string::npos) {
                                        for (auto conf : weaponConfInstance) {
                                            if (!conf) continue;
                                            weaponconfFields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
                                            if (weaponconfFields->ID == getitem.WeaponConf[2]) {
                                                weaponconfFields->DefWeaponSkinID = ID_DIAMOND; 
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (ImGui::Checkbox("Red Sprite Camo", &camoRedSprite)) {
                            if (camoRedSprite) {
                                camoOff = false;
                                camoDiamond = false;
                                for (const auto& getitem : itemData) {
                                    if (getitem.itemName.find("[M]") != std::string::npos || getitem.itemName.find("[L]") != std::string::npos) {
                                        for (auto conf : weaponConfInstance) {
                                            if (!conf) continue;
                                            weaponconfFields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
                                            if (weaponconfFields->ID == getitem.WeaponConf[2]) {
                                                weaponconfFields->DefWeaponSkinID = ID_RED_SPRITE; 
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        ImGui::EndChild();
                    }

                    ImGui::EndChild(); 
                }

                if (tab == 6) {
                    ImGui::BeginChild("Settings Nav##7", ImVec2(leftPanelWidth, childHeight), true);
                    {
                        static float leftHue = 0.0f;
                        leftHue += ImGui::GetIO().DeltaTime * 0.1f;
                        if (leftHue > 1.0f) leftHue = 0.0f;
                        ImGui::TextColored(ImColor::HSV(leftHue, 0.8f, 0.9f), "Configuration");
                        ImGui::Separator();

                        ImGui::Spacing();
                        
                        if (ImGui::CollapsingHeader("Theme Customization")) {
                            static ImVec4 textColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
                            static ImVec4 headerColor = ImGui::GetStyle().Colors[ImGuiCol_Header];
                            static ImVec4 buttonColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
                            static ImVec4 frameBgColor = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
                            static ImVec4 windowBgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
                            static ImVec4 childBgColor = ImGui::GetStyle().Colors[ImGuiCol_ChildBg];
                            static ImVec4 borderColor = ImGui::GetStyle().Colors[ImGuiCol_Border];
                            static ImVec4 sliderGrabColor = ImGui::GetStyle().Colors[ImGuiCol_SliderGrab];
                            static ImVec4 checkMarkColor = ImGui::GetStyle().Colors[ImGuiCol_CheckMark];
                            
                            ImGui::ColorEdit4("Text", (float*)&textColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Header", (float*)&headerColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Button", (float*)&buttonColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Frame BG", (float*)&frameBgColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Window BG", (float*)&windowBgColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Child BG", (float*)&childBgColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Border", (float*)&borderColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Slider Grab", (float*)&sliderGrabColor, ImGuiColorEditFlags_NoInputs);
                            ImGui::ColorEdit4("Check Mark", (float*)&checkMarkColor, ImGuiColorEditFlags_NoInputs);
                            
                            if (ImGui::Button("Apply Theme", ImVec2(-1, 30))) {
                                ImGuiStyle& style = ImGui::GetStyle();
                                style.Colors[ImGuiCol_Text] = textColor;
                                style.Colors[ImGuiCol_Header] = headerColor;
                                style.Colors[ImGuiCol_Button] = buttonColor;
                                style.Colors[ImGuiCol_FrameBg] = frameBgColor;
                                style.Colors[ImGuiCol_WindowBg] = windowBgColor;
                                style.Colors[ImGuiCol_ChildBg] = childBgColor;
                                style.Colors[ImGuiCol_Border] = borderColor;
                                style.Colors[ImGuiCol_SliderGrab] = sliderGrabColor;
                                style.Colors[ImGuiCol_CheckMark] = checkMarkColor;
                            }
                            
                            if (ImGui::Button("Reset to Default", ImVec2(-1, 30))) {            
                            }
                            
                            ImGui::Separator();
                        }

                        if (ImGui::Button("Save Features", ImVec2(-1, 40))) {
                            saveAllConfig();
                        }
                        if (ImGui::Button("Load Features", ImVec2(-1, 40))) {
                            if (loadAllConfig()) {
                                ImGui::OpenPopup("Config Loaded");
                            } else {
                                ImGui::OpenPopup("Config Error");
                            }
                        }
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                   
                        if (Config.Aim.showFPSLevelUltraSlider) {
                            ImGui::SliderFloat("Ultra Frame Level", &Config.Aim.FpsLevelUltra1, 60, 144, "%.0f");
                        }

                        ImGui::Spacing();
 
                        if (ImGui::Button("HIDE MENU", ImVec2(-1, 0))) {
                            isMenuVisible = false;
                        }       
                        if (ImGui::Button("CLEAR CACHE", ImVec2(-1, 0))) {
                            system("rm -rf /data/data/com.garena.game.codm/cache");
                            system("rm -rf /data/data/com.garena.game.codm/files/logs");
                        };    
                        ImGui::Columns(1);
    
                        if (ImGui::Button("RESET GUEST ACCOUNT", ImVec2(-1, 0))) {
                            system("rm -f /data/data/com.garena.game.codm/shared_prefs/com.garena.msdk.persist.fallback.xml");
                            kill(getpid(), SIGKILL);
                        }                
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("Settings Content##7", ImVec2(rightPanelWidth, childHeight), true);
                    {
                        static float timer = 0.0f;
                        timer += ImGui::GetIO().DeltaTime;
                        ImVec4 color = ImVec4(
                            (sin(timer) * 0.5f + 0.5f),
                            (sin(timer + 2.0f) * 0.5f + 0.5f),
                            (sin(timer + 4.0f) * 0.5f + 0.5f),
                            1.0f
                        );

                        if (ImGui::BeginPopupModal("Config Saved", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                            ImGui::Text("All settings from all tabs have been saved!");
                            ImGui::Text("File: /storage/emulated/0/Download/Hex_config.dat");
                            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                            ImGui::EndPopup();
                        }
                        if (ImGui::BeginPopupModal("Config Loaded", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                            ImGui::Text("All settings from all tabs have been loaded!");
                            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                            ImGui::EndPopup();
                        }
                        if (ImGui::BeginPopupModal("Config Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                            ImGui::Text("Error: Could not load configuration file.");
                            ImGui::Text("Make sure you have saved settings first.");
                            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                            ImGui::EndPopup();
                        }

                        if (isLogin) {
                            ImGui::Text("License Expiry: %s", EXP.c_str());
                            ImGui::Text("License Key: %s", storedKey.c_str());
                        }

                        if (ImGui::BeginTable("Price List", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                            ImGui::TableSetupColumn("Plan");
                            ImGui::TableSetupColumn("Price (PHP)");
                            ImGui::TableHeadersRow();
                            
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("1 Days"); ImGui::TableSetColumnIndex(1); ImGui::Text("25");
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("3 Days"); ImGui::TableSetColumnIndex(1); ImGui::Text("99");
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("7 Days"); ImGui::TableSetColumnIndex(1); ImGui::Text("120");
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("14 Days"); ImGui::TableSetColumnIndex(1); ImGui::Text("170");
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("30 Days"); ImGui::TableSetColumnIndex(1); ImGui::Text("400"); 
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("60 Days"); ImGui::TableSetColumnIndex(1); ImGui::Text("520");   
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("90 Days"); ImGui::TableSetColumnIndex(1); ImGui::Text("600");                               
                            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Lifetime"); ImGui::TableSetColumnIndex(1); ImGui::Text("1.k");
                            
                            ImGui::EndTable();
                        }
                    }
                    ImGui::EndChild();
                }
            }
        }

        if (Config.ExtraMenu.Kinetic) {
            Patches.kinetic.Modify();
        } else {
            Patches.kinetic.Restore();
        }

        if (Config.ExtraMenu.Parachute) {
            Patches.chute.Modify();
        } else {
            Patches.chute.Restore();
        }
        
        if (Config.ExtraMenu.Diving) {
            Patches.wing.Modify();
        } else {
            Patches.wing.Restore();
        }
    }

    auto Input_get_touchCount = (int (*)())(Class_Input__get_touchCount);
    if (Input_get_touchCount() > 0) {
        auto Input_GetTouch = (Touch(*)(uintptr_t, int))(Class_Input__GetTouch);
        auto Input_get_mousePosition = (Vector3(*)(uintptr_t))(Class_Input__get_mousePosition);
        switch (Input_GetTouch(Config.ImGuiMenu.thiz, 0).m_Phase) {
            case TouchPhase::Began:
            case TouchPhase::Stationary:
                io->MouseDown[0] = true;
                io->MousePos = ImVec2(Input_get_mousePosition(Config.ImGuiMenu.thiz).x,
                                      get_height() -
                                      Input_get_mousePosition(Config.ImGuiMenu.thiz).y);
                break;
            case TouchPhase::Ended:
            case TouchPhase::Canceled:
                io->MouseDown[0] = false;
                Config.ImGuiMenu.clearMousePos = true;
                break;
            case TouchPhase::Moved:
                io->MousePos = ImVec2(Input_get_mousePosition(Config.ImGuiMenu.thiz).x,
                                      get_height() -
                                      Input_get_mousePosition(Config.ImGuiMenu.thiz).y);
                break;
            default:
                break;
        }
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

float ( *orig_GetAssitAimSpeed)(void * instance, Vector3 assistCentorPos, float assistDis, float dis, float angle, bool isPVE, bool gamepadInput);
float GetAssitAimSpeed(void * instance, Vector3 assistCentorPos, float assistDis, float dis, float angle, bool isPVE, bool gamepadInput) {
    if (instance != NULL) {
        if (Config.Aim.AimAssist) {
            return (float)Config.Aim.AimAssistSize;
        }
    }
    return orig_GetAssitAimSpeed(instance, assistCentorPos, assistDis, dis, angle, isPVE, gamepadInput);
}

float ( *orig_GetScaleRecoil)(void * instance);
float GetScaleRecoil(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Recoil) {
            return 0.6f;
        }
    }
    return orig_GetScaleRecoil(instance);
}

float ( *orig_GetRealSpreadModifier)(void * instance);
float GetRealSpreadModifier(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spread) {
            return 0.1f;
        }
    }
    return orig_GetRealSpreadModifier(instance);
}

bool (*orig_IsUnlocked)(void *instance);
bool IsUnlocked(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Blueprints) {
            return true;
        }
    }
    return orig_IsUnlocked(instance);
}

bool (*orig_IsInEM3Eye)(void *instance);
bool get_IsInEM3Eye(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.RedWallhack) {
            return true;
        }
    }
    return orig_IsInEM3Eye(instance);
}

Quaternion (*orig_CalcAdjustedAim)(void *instance, Quaternion initAim, float sRate, float pRate, bool includeShotSpread, bool isCameraAim, bool isWeaponRot);
Quaternion CalcAdjustedAim(void *instance, Quaternion initAim, float sRate, float pRate, bool includeShotSpread, bool isCameraAim, bool isWeaponRot) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spread) {
            return initAim;
        }
    }
    return orig_CalcAdjustedAim(instance, initAim, sRate, pRate, includeShotSpread, isCameraAim, isWeaponRot);
}

float ( *orig_get_ChangeClipTime)(void *instance);
float get_ChangeClipTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Reload) {
            return 0.1f;
        }
    }
    return orig_get_ChangeClipTime(instance);
}

float ( *orig_get_ChangeClipLoopTime)(void *instance);
float get_ChangeClipLoopTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Reload) {
            return 0.1f;
        }
    }
    return orig_get_ChangeClipLoopTime(instance);
}

void ( *orig_UpdateCameraShake)(void *instance);
void UpdateCameraShake(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Shake) {
            return (void) true;
        }
    }
    return orig_UpdateCameraShake(instance);
}

float ( *orig_get_AimingTime)(void *instance);
float get_AimingTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Scope) {
            return 0.1f;
        }
    }
    return orig_get_AimingTime(instance);
}

float ( *orig_get_EquipTime)(void *instance);
float get_EquipTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Switch) {
            return 0.1f;
        }
    }
    return orig_get_EquipTime(instance);
}

float ( *orig_get_UnequipTime)(void *instance);
float get_UnequipTime(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Switch) {
            return 0.1f;
        }
    }
    return orig_get_UnequipTime(instance);
}

float (*orig_GetCurrentDistToWaterSurface)(void* instance) = nullptr;
float hook_GetCurrentDistToWaterSurface(void* instance) {
    if (instance && Config.ExtraMenu.Water) {
        return 0.0;
    }
    return orig_GetCurrentDistToWaterSurface(instance);
}

bool (*orig_IsUnderWaterSurface)(void* instance, float distance) = nullptr;
bool hook_IsUnderWaterSurface(void* instance, float distance) {
    if (instance && Config.ExtraMenu.Water) {
        return true;
    }
    return orig_IsUnderWaterSurface(instance, distance);
}

float (*orig_get_CurrentWaterSurfaceHeight)(void* instance) = nullptr;
float hook_get_CurrentWaterSurfaceHeight(void* instance) {
    if (instance && Config.ExtraMenu.Water) {
        return 0.0;
    }
    return orig_get_CurrentWaterSurfaceHeight(instance);
}

static bool bIsEmulator = false;
bool IsEmulator(){
    return bIsEmulator;
}

static bool bIsEnableVulkan = false;
bool IsEnableVulkan(){
    return bIsEnableVulkan;
}

float ( *orig_OverHeat)(void *instance);
float OverHeat(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Over) {
            return 0.000001f;
        }
    }
    return orig_OverHeat(instance);
}

bool ( *orig_SingleLineCheckPhysics)(void *instance, int hitType, void *hitTarget, void *hitCollider, Vector3 startPos, Vector3 dir, void *impactInfo);
bool SingleLineCheckPhysics(void *instance, int hitType, void *hitTarget, void *hitCollider, Vector3 startPos, Vector3 dir, void *impactInfo) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Hit) {
            return true;
        }
    }
    return orig_SingleLineCheckPhysics(instance, hitType, hitTarget, hitCollider, startPos, dir, impactInfo);
}

void ( *orig_OpenParachute)(void *instance, bool isAuto);
void OpenParachute(void *instance, bool isAuto) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Parachute) {
            return (void) true;
        }
    }
    return orig_OpenParachute(instance, isAuto);
}

float (*orig_get_AccelerationForwardSpeedUp)(void *instance);
float get_AccelerationForwardSpeedUp(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Diving) {
            return 176.0f;
        }
    }
    return orig_get_AccelerationForwardSpeedUp(instance);
}

float (*orig_get_MaxVelocityForwardSpeedUp)(void *instance);
float get_MaxVelocityForwardSpeedUp(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Diving) {
            return 176.0f;
        }
    }
    return orig_get_MaxVelocityForwardSpeedUp(instance);
}

float (*orig_GetMaxJumpHeight)(void *);
float hook_GetMaxJumpHeight(void *instance) {
    float orig_ = orig_GetMaxJumpHeight(instance);
    return isJumpAdjustmentEnabled ? orig_ * jumpHeightMultiplier : orig_;
}

float (*o_GetSuperSlideRate)(void *ins);
float h_GetSuperSlideRate(void *ins) {
    if (SlideTackle) {
        return SlideRange + 256;
    }
    return o_GetSuperSlideRate(ins);
}

float (*o_get_SlideTackleAcclerationSpeed)(void *ins);
float h_get_SlideTackleAcclerationSpeed(void *ins) {
    if (SlideTackle) {
        return SlideRange + 1;
    }
    return o_get_SlideTackleAcclerationSpeed(ins);
}

float (*o_PawnGetMaxSpeed)(void *ins);
float h_PawnGetMaxSpeed(void *ins) {
    if (SlideTackle) {
        return SlideRange + 256;
    }
    return o_PawnGetMaxSpeed(ins);
}

void (*o_TickLocalPlayer)(void *ins, float deltaTime);
void h_TickLocalPlayer(void *ins, float deltaTime) {
    if (!SlideTackle) {
        o_TickLocalPlayer(ins, deltaTime);
    }
}

float (*o_get_SlideTackleSpeed)(void *ins);
float h_get_SlideTackleSpeed(void *ins) {
    if (SlideTackle) {
        return SlideRange;
    }
    return o_get_SlideTackleSpeed(ins);
}

float (*o_GetMaxFov_3p)(void *ins);
float h_GetMaxFov_3p(void *ins) {
    if (view3PEnabled) {
        return range3PValue;
    }
    return o_GetMaxFov_3p(ins);
}

float ( *orig_GetWeaponMoveScale)(void *instance);
float GetWeaponMoveScale(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Speed) {
            return 2.0f;
        }
    }
    return orig_GetWeaponMoveScale(instance);
}

bool (*orig_IsWidgetVisible)(void *instance, void *rec);
bool IsWidgetVisible(void *instance, void *rect) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Fps) {
            return true;
        }
    }
    return orig_IsWidgetVisible(instance, rect);
}

bool ( *orig_SameTeamFromPawn)(void *instance, void *pawn1, void *pawn2);
bool SameTeamFromPawn(void *instance, void *pawn1, void *pawn2) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Radar2) {
            return true;
        }
    }
    return orig_SameTeamFromPawn(instance, pawn1, pawn2);
}

static bool bIsTutorialEnabled = false;
bool IsTutorialEnabled() {
    return bIsTutorialEnabled;
}

float (*get_m_PhysSkisMaxSpeed)(void* instance);
float hooked_get_m_PhysSkisMaxSpeed(void* instance) {
    if (SnowB) {
        return SnowBsize;
    }
    return get_m_PhysSkisMaxSpeed(instance);
}

float (*get_m_PhysSkisSpeedDown)(void* instance);
float hooked_get_m_PhysSkisSpeedDown(void* instance) {
    if (SnowB) {
        return SnowBsize; 
    }
    return get_m_PhysSkisSpeedDown(instance);
}

float (*get_m_PhysSpeedUp)(void* instance);
float hooked_get_m_PhysSpeedUp(void* instance) {
    if (SnowB) {
        return SnowBsize;
    }
    return get_m_PhysSpeedUp(instance);
}

float (*get_m_PhysSkisSlopMaxSpeed)(void* instance);
float hooked_get_m_PhysSkisSlopMaxSpeed(void* instance) {
    if (SnowB) {
        return SnowBsize;
    }
    return get_m_PhysSkisSlopMaxSpeed(instance);
}

float (*original_CalcFinalMoveScale)(void* instance);
float hooked_CalcFinalMoveScale(void* instance) {
    if (instance == nullptr) {
        return original_CalcFinalMoveScale(instance);
    }
    if (isSpeedHackEnabled) {
        if (speedHackMultiplier > 0.0f && speedHackMultiplier <= 100.0f) {
            return speedHackMultiplier;
        }
    }
    return original_CalcFinalMoveScale(instance);
}

size_t hook_strlen(const char *thread)
{
        if (strstr(thread, "eglSwapBuffers"))
        {
            SPAMSINT0();
    }
    return strlen(thread);
}

__int64 __fastcall (*sub_old)(__int64 a1, __int64 *a2);
__int64 __fastcall hook_sub(__int64 a1, __int64 *a2) {
    return 0;
}

void (*oFunction)(const char* library, const char* function, void* newFunction, void** originalFunction);
void hookFunction(const char* library, const char* function, void* newFunction, void** originalFunction) {
    void* libraryHandle = dlopen(library, RTLD_GLOBAL | RTLD_LAZY);
    LOGI(OBFUSCATE("MPROTECT CALL: %s"), function);
    if (libraryHandle) {
        void* original = dlsym(libraryHandle, function);
        if (original) {
            *originalFunction = original;
            mprotect(original, sizeof(void*), PROT_READ | PROT_WRITE);
            *((void**)original) = newFunction;
            mprotect(original, sizeof(void*), PROT_EXEC);
        }
        dlclose(libraryHandle);
    }
}

__int64 __fastcall sub_day(int *a1, __int64 a2) {
    if (a2 && *a1 >= 2 && a1[1] < 20) {
        switch ((unsigned int)a2) {  
            case 0:
            case 1:
            case 2:
            case 3:
            case 6:
            case 8:
            case 10:
            case 11:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 20:
            case 21:
            case 25:
            case 44:
            case 56:
            case 57:
            case 58:
            case 59:
            case 69:
            case 77:
                return 0;  
            default:
                return 0;
        }
    }
    return 0;
}

bool (*orig_bypass)(void *ins);
bool hook_bypass(void *ins) {
    return false;
}
 
void Init_Thread2() {
    while (!m_Anogs) {
        m_Anogs = Tools::GetBaseAddress("libanogs.so");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LOGI("libanogs.so: %p", m_Anogs);
    MemoryPatch::createWithHex("libanogs.so", 0x216A60, "00 00 80 d2 c0 03 5f d6").Modify();
    MemoryPatch::createWithHex("libanogs.so", 0x2BA5C0, "00 00 80 d2 c0 03 5f d6").Modify();
    MemoryPatch::createWithHex("libanogs.so", 0x4731D0, "00 00 80 d2 c0 03 5f d6").Modify();
    MemoryPatch::createWithHex("libanogs.so", 0x4BC8FC, "00 00 80 d2 c0 03 5f d6").Modify();
    MemoryPatch::createWithHex("libanogs.so", 0x4BF364, "00 00 80 d2 c0 03 5f d6").Modify();
    MemoryPatch::createWithHex("libanogs.so", 0x42B224, "00 00 80 d2 c0 03 5f d6").Modify();
    MemoryPatch::createWithHex("libanogs.so", 0x4BFBB0, "00 00 80 d2 c0 03 5f d6").Modify();
    ANTIHOOK(OBFUSCATE("libanogs.so")); // Hide Anogs
}

void Init_Thread() {
    while (!m_unity) {
        m_unity = Tools::GetBaseAddress("libunity.so");
        sleep(1);
    }
    
    LOGI("libunity.so: %p", m_unity);

    Patches.kinetic = MemoryPatch::createWithHex("libunity.so", 0x7024AC4, "20 00 80 D2 C0 03 5F D6");
    Patches.chute = MemoryPatch::createWithHex("libunity.so", 0x6DEB94C, "00 00 80 D2 C0 03 5F D6");
    Patches.wing = MemoryPatch::createWithHex("libunity.so", 0x8D9D1D0, "00 00 80 D2 C0 03 5F D6");

    HOOK_LIB("libunity.so", "0xA9925F4", hooked_SetUltraFrameRateDeviceInfo, orig_SetUltraFrameRateDeviceInfo);
    HOOK_LIB("libunity.so", "0x6E0906C", get_AccelerationForwardSpeedUp, orig_get_AccelerationForwardSpeedUp);
    HOOK_LIB("libunity.so", "0x6E090D0", get_MaxVelocityForwardSpeedUp, orig_get_MaxVelocityForwardSpeedUp);
    HOOK_LIB("libunity.so", "0xAACCB88", GetScaleRecoil, orig_GetScaleRecoil);
    HOOK_LIB("libunity.so", "0xAA91084", SingleLineCheckPhysics, orig_SingleLineCheckPhysics);
    HOOK_LIB("libunity.so", "0xAAEBA24", get_EquipTime, orig_get_EquipTime);
    HOOK_LIB("libunity.so", "0xB3A0380", get_AimingTime, orig_get_AimingTime);
    HOOK_LIB("libunity.so", "0x5DAEF04", GetAssitAimSpeed, orig_GetAssitAimSpeed);
    HOOK_LIB("libunity.so", "0xB3A2470", get_ChangeClipTime, orig_get_ChangeClipTime);
    HOOK_LIB("libunity.so", "0xAACF408", CalcAdjustedAim, orig_CalcAdjustedAim);
    HOOK_LIB("libunity.so", "0x7092074", hook_GetMaxJumpHeight, orig_GetMaxJumpHeight);
    HOOK_LIB("libunity.so", "0x5DDB13C", get_IsInEM3Eye, orig_IsInEM3Eye);
    HOOK_LIB("libunity.so", "0xB8D0FD4", IsUnlocked, orig_IsUnlocked);
    HOOK_LIB("libunity.so", "0x8EA75E4", h_GetSuperSlideRate, o_GetSuperSlideRate);
    HOOK_LIB("libunity.so", "0x7A472D8", h_PawnGetMaxSpeed, o_PawnGetMaxSpeed);
    HOOK_LIB("libunity.so", "0x8DCAEE8", h_TickLocalPlayer, o_TickLocalPlayer);
    HOOK_LIB("libunity.so", "0x8EA7F08", h_get_SlideTackleSpeed, o_get_SlideTackleSpeed);
    HOOK_LIB("libunity.so", "0xAACB1F0", GetRealSpreadModifier, orig_GetRealSpreadModifier);
    HOOK_LIB("libunity.so", "0x7099274", hooked_get_m_PhysSkisSpeedDown, get_m_PhysSkisSpeedDown);
    HOOK_LIB("libunity.so", "0x70990CC", hooked_get_m_PhysSpeedUp, get_m_PhysSpeedUp);
    HOOK_LIB("libunity.so", "0x70992DC", hooked_get_m_PhysSkisMaxSpeed, get_m_PhysSkisMaxSpeed);
    HOOK_LIB("libunity.so", "0x70993AC", hooked_get_m_PhysSkisSlopMaxSpeed, get_m_PhysSkisSlopMaxSpeed);
    HOOK_LIB("libunity.so", "0x95D5034", hooked_CalcFinalMoveScale, original_CalcFinalMoveScale);
    Tools::Hook((void*)getAbsoluteAddress("libunity.so", 0x7042488), (void*)&hook_GetCurrentDistToWaterSurface, (void**)&orig_GetCurrentDistToWaterSurface);
    Tools::Hook((void*)getAbsoluteAddress("libunity.so", 0x705FAE0), (void*)&hook_IsUnderWaterSurface, (void**)&orig_IsUnderWaterSurface);
    Tools::Hook((void*)getAbsoluteAddress("libunity.so", 0x8DAA380), (void*)&hook_get_CurrentWaterSurfaceHeight, (void**)&orig_get_CurrentWaterSurfaceHeight);
    Tools::Hook((void*)getAbsoluteAddress("libunity.so", GetCurrentWeaponAddress), (void*)&_GetCurrentWeaponKillEffect, (void**)&orig_GetCurrentWeaponKillEffect);

    hookConstructor((void*)getRealOffset(Item2InventoryAddress), itemInventoryInstance);
    hookConstructor((void*)getRealOffset(WeaponConfExtraAddress), weaponExtraInstance);
    hookConstructor((void*)getRealOffset(WeaponFireEffectAddress), weaponFireEffectInstance);
    hookConstructor((void*)getRealOffset(WeaponConfAddress), weaponConfInstance);
    hookConstructor((void*)getRealOffset(WeaponAssetGroupAddress), weaponAssetGroupInstance);
    hookConstructor((void*)getRealOffset(MythicArmorConfigAddress), mythicArmorInstance);
    hookConstructor((void*)getRealOffset(MythicSightConfigAddress), mythicSightInstance);
    hookConstructor((void*)getRealOffset(WeaponSkinAddress), weaponSkinConfigInstance);
    hookConstructor((void*)getRealOffset(KillEffectItemConfConfigAddress), killEffectItemInstance);
    hookConstructor((void*)getRealOffset(ItemResourceAddress), itemResourceConfigInstance);
    hookConstructor((void*)getRealOffset(CharacterModelAddress), CharacterModelConfigInstance);
    hookConstructor((void*)getRealOffset(RoleConfAddress), RoleConfConfigInstance);
    hookConstructor((void*)getRealOffset(RoleSkinAddress), RoleSkinConfigInstance);
    hookConstructor((void*)getRealOffset(RolePackConfAddress), RolePackConfConfigInstance);
    hookConstructor((void*)getRealOffset(BRDeadboxSkinAddress), BRDeadboxSkinConfigInstance);
    hookConstructor((void*)getRealOffset(BRDropPlaneSkinAddress), BRDropPlaneSkinConfigInstance);
    
    pthread_t t2;
    pthread_create(&t2, NULL, GameUpdate, NULL);

    auto swapBuffers = ((uintptr_t) DobbySymbolResolver(OBFUSCATE("libunity.so"), OBFUSCATE("eglSwapBuffers")));
    KittyMemory::ProtectAddr((void *)swapBuffers, sizeof(swapBuffers), PROT_READ | PROT_WRITE | PROT_EXEC);
    DobbyHook((void *) swapBuffers, (void *) hook_eglSwapBuffers, (void **) &old_eglSwapBuffers);
}

__attribute__((constructor))
void native_Init(JNIEnv *env, jclass clazz, jobject mContext) {
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    VM = vm;
    std::thread(Init_Thread).detach();
    std::thread(Init_Thread2).detach();
    return JNI_VERSION_1_6;
}
