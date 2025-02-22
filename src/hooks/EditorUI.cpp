#include "../MoreObjectInfo.hpp"
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(MOIEditorUI, EditorUI) {
    static void onModify(ModifyBase<ModifyDerive<MOIEditorUI, EditorUI>>& self) {
        auto mod = Mod::get();
        MOI_AUTO_ENABLE(EditorUI, moveObjectCall, dynamicObjectInfo)
        MOI_AUTO_ENABLE(EditorUI, transformObjectCall, dynamicObjectInfo)
        MOI_AUTO_ENABLE(EditorUI, updateObjectInfoLabel, showObjectInfo)

        listenForAllSettingChanges([moveObjectCallHook, transformObjectCallHook, updateObjectInfoLabelHook](std::shared_ptr<SettingV3> setting) {
            auto settingName = setting->getKey();
            if (settingName.starts_with("show-object-")) {
                if (auto editorUI = MoreObjectInfo::editorUI()) editorUI->updateObjectInfoLabel();

                auto value = MoreObjectInfo::showObjectInfo(setting->getMod());
                MOI_HOOK_TOGGLE(EditorUI, updateObjectInfoLabel)
            }
            else if (settingName == "dynamic-object-info") {
                auto value = std::static_pointer_cast<BoolSettingV3>(setting)->getValue();
                MOI_HOOK_TOGGLE(EditorUI, moveObjectCall)
                MOI_HOOK_TOGGLE(EditorUI, transformObjectCall)
            }
        }, mod);
    }

    void moveObjectCall(EditCommand command) {
        EditorUI::moveObjectCall(command);
        EditorUI::updateObjectInfoLabel();
    }

    void transformObjectCall(EditCommand command) {
        EditorUI::transformObjectCall(command);
        EditorUI::updateObjectInfoLabel();
    }

    void updateObjectInfoLabel() {
        EditorUI::updateObjectInfoLabel();

        if (!MoreObjectInfo::GAME_MANAGER || !MoreObjectInfo::GAME_MANAGER->m_valueKeeper->valueForKey("gv_0041")->boolValue()) return;

        auto selectedObject = m_selectedObject;
        if (!selectedObject) return;

        auto mod = Mod::get();

        auto objID = MoreObjectInfo::showObjectID(mod) ? fmt::format("ID: {}\n", selectedObject->m_objectID) : "";

        auto objPosition = MoreObjectInfo::showObjectPosition(mod) ?
            fmt::format("Position: {}, {}\n", selectedObject->getPositionX(), selectedObject->getPositionY() - 90.0f) : "";

        auto rotationX = selectedObject->getRotationX();
        auto rotationY = selectedObject->getRotationY();
        auto objRotation = MoreObjectInfo::showObjectRotation(mod) && (rotationX != 0.0f || rotationY != 0.0f) ?
            rotationX != rotationY ? fmt::format("Rotation: {}, {}\n", rotationX, rotationY) : fmt::format("Rotation: {}\n", rotationX) : "";

        auto scaleX = selectedObject->getScaleX();
        auto scaleY = selectedObject->getScaleY();
        auto objScale = MoreObjectInfo::showObjectScale(mod) && (scaleX != 1.0f || scaleY != 1.0f) ?
            scaleX != scaleY ? fmt::format("Scale: {}, {}\n", scaleX, scaleY) : fmt::format("Scale: {}\n", scaleX) : "";

        auto baseColor = selectedObject->m_baseColor;
        auto detailColor = selectedObject->m_detailColor;

        std::string objBaseColor;
        if (MoreObjectInfo::showObjectBaseColor(mod) && baseColor) {
            auto& hsv = baseColor->m_hsv;
            if (hsv.h != 0.0f || hsv.s != 1.0f || hsv.v != 1.0f || hsv.absoluteSaturation || hsv.absoluteBrightness)
                objBaseColor = fmt::format("{}HSV: {}, {}{:.2f}, {}{:.2f}\n", detailColor ? "Base " : "", hsv.h,
                    hsv.absoluteSaturation ? hsv.s >= 0 ? "+" : "" : "x", hsv.s,
                    hsv.absoluteBrightness ? hsv.v >= 0 ? "+" : "" : "x", hsv.v);
        }

        std::string objDetailColor;
        if (MoreObjectInfo::showObjectDetailColor(mod) && detailColor) {
            auto& hsv = detailColor->m_hsv;
            if (hsv.h != 0.0f || hsv.s != 1.0f || hsv.v != 1.0f || hsv.absoluteSaturation || hsv.absoluteBrightness)
                objDetailColor = fmt::format("{}HSV: {}, {}{:.2f}, {}{:.2f}\n", baseColor ? "Detail " : "", hsv.h,
                    hsv.absoluteSaturation ? hsv.s >= 0 ? "+" : "" : "x", hsv.s,
                    hsv.absoluteBrightness ? hsv.v >= 0 ? "+" : "" : "x", hsv.v);
        }

        std::string objType;
        if (MoreObjectInfo::showObjectType(mod)) {
            auto objectType = selectedObject->m_objectType;
            auto typeStr = "";
            switch (objectType) {
                case GameObjectType::Solid: typeStr = "Solid"; break;
                case GameObjectType::Hazard: typeStr = "Hazard"; break;
                case GameObjectType::InverseGravityPortal: typeStr = "Inverse Gravity Portal"; break;
                case GameObjectType::NormalGravityPortal: typeStr = "Normal Gravity Portal"; break;
                case GameObjectType::ShipPortal: typeStr = "Ship Portal"; break;
                case GameObjectType::CubePortal: typeStr = "Cube Portal"; break;
                case GameObjectType::Decoration: typeStr = "Decoration"; break;
                case GameObjectType::YellowJumpPad: typeStr = "Yellow Jump Pad"; break;
                case GameObjectType::PinkJumpPad: typeStr = "Pink Jump Pad"; break;
                case GameObjectType::GravityPad: typeStr = "Gravity Pad"; break;
                case GameObjectType::YellowJumpRing: typeStr = "Yellow Jump Ring"; break;
                case GameObjectType::PinkJumpRing: typeStr = "Pink Jump Ring"; break;
                case GameObjectType::GravityRing: typeStr = "Gravity Ring"; break;
                case GameObjectType::InverseMirrorPortal: typeStr = "Inverse Mirror Portal"; break;
                case GameObjectType::NormalMirrorPortal: typeStr = "Normal Mirror Portal"; break;
                case GameObjectType::BallPortal: typeStr = "Ball Portal"; break;
                case GameObjectType::RegularSizePortal: typeStr = "Regular Size Portal"; break;
                case GameObjectType::MiniSizePortal: typeStr = "Mini Size Portal"; break;
                case GameObjectType::UfoPortal: typeStr = "Ufo Portal"; break;
                case GameObjectType::Modifier: typeStr = "Modifier"; break;
                case GameObjectType::Breakable: typeStr = "Breakable"; break;
                case GameObjectType::SecretCoin: typeStr = "Secret Coin"; break;
                case GameObjectType::DualPortal: typeStr = "Dual Portal"; break;
                case GameObjectType::SoloPortal: typeStr = "Solo Portal"; break;
                case GameObjectType::Slope: typeStr = "Slope"; break;
                case GameObjectType::WavePortal: typeStr = "Wave Portal"; break;
                case GameObjectType::RobotPortal: typeStr = "Robot Portal"; break;
                case GameObjectType::TeleportPortal: typeStr = "Teleport Portal"; break;
                case GameObjectType::GreenRing: typeStr = "Green Ring"; break;
                case GameObjectType::Collectible: typeStr = "Collectible"; break;
                case GameObjectType::UserCoin: typeStr = "User Coin"; break;
                case GameObjectType::DropRing: typeStr = "Drop Ring"; break;
                case GameObjectType::SpiderPortal: typeStr = "Spider Portal"; break;
                case GameObjectType::RedJumpPad: typeStr = "Red Jump Pad"; break;
                case GameObjectType::RedJumpRing: typeStr = "Red Jump Ring"; break;
                case GameObjectType::CustomRing: typeStr = "Custom Ring"; break;
                case GameObjectType::DashRing: typeStr = "Dash Ring"; break;
                case GameObjectType::GravityDashRing: typeStr = "Gravity Dash Ring"; break;
                case GameObjectType::CollisionObject: typeStr = "Collision Object"; break;
                case GameObjectType::Special: typeStr = "Special"; break;
                case GameObjectType::SwingPortal: typeStr = "Swing Portal"; break;
                case GameObjectType::GravityTogglePortal: typeStr = "Gravity Toggle Portal"; break;
                case GameObjectType::SpiderOrb: typeStr = "Spider Orb"; break;
                case GameObjectType::SpiderPad: typeStr = "Spider Pad"; break;
                case GameObjectType::EnterEffectObject: typeStr = "Enter Effect Object"; break;
                case GameObjectType::TeleportOrb: typeStr = "Teleport Orb"; break;
                case GameObjectType::AnimatedHazard: typeStr = "Animated Hazard"; break;
                default: typeStr = "Unknown"; break;
            }
            objType = fmt::format("Type: {} ({})\n", typeStr, (int)objectType);
        }

        auto objAddress = MoreObjectInfo::showObjectAddress(mod) ? fmt::format("Address: 0x{:x}\n", reinterpret_cast<uintptr_t>(selectedObject)) : "";

        auto objData = MoreObjectInfo::showObjectData(mod) ? fmt::format("Data: {}\n", GEODE_ANDROID(std::string)(selectedObject->getSaveString(m_editorLayer))) : "";

        m_objectInfoLabel->setString(fmt::format("{}{}{}{}{}{}{}{}{}{}", m_objectInfoLabel->getString(),
            objID, objPosition, objRotation, objScale, objBaseColor, objDetailColor, objType, objAddress, objData).c_str());
    }
};
