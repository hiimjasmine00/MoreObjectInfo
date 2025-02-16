#include "../MoreObjectInfo.hpp"
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(MOIEditorUI, EditorUI) {
    struct Fields {
        bool m_showObjectID;
        bool m_showObjectPosition;
        bool m_showObjectRotation;
        bool m_showObjectScale;
        bool m_showObjectBaseColor;
        bool m_showObjectDetailColor;
        bool m_showObjectType;
        bool m_showObjectAddress;
        bool m_showObjectData;
    };

    static void onModify(ModifyBase<ModifyDerive<MOIEditorUI, EditorUI>>& self) {
        auto moveHookRes = self.getHook("EditorUI::moveObjectCall");
        if (moveHookRes.isErr()) log::error("Failed to get EditorUI::moveObjectCall hook: {}", moveHookRes.unwrapErr());

        auto transformHookRes = self.getHook("EditorUI::transformObjectCall");
        if (transformHookRes.isErr()) log::error("Failed to get EditorUI::transformObjectCall hook: {}", transformHookRes.unwrapErr());

        auto updateHookRes = self.getHook("EditorUI::updateObjectInfoLabel");
        if (updateHookRes.isErr()) log::error("Failed to get EditorUI::updateObjectInfoLabel hook: {}", updateHookRes.unwrapErr());

        auto moveHook = moveHookRes.unwrapOr(nullptr);
        if (moveHook) moveHook->setAutoEnable(MoreObjectInfo::dynamicObjectInfo());
        auto transformHook = transformHookRes.unwrapOr(nullptr);
        if (transformHook) transformHook->setAutoEnable(MoreObjectInfo::dynamicObjectInfo());
        auto updateHook = updateHookRes.unwrapOr(nullptr);
        if (updateHook) updateHook->setAutoEnable(MoreObjectInfo::showObjectInfo());

        listenForAllSettingChanges([moveHook, transformHook, updateHook](std::shared_ptr<SettingV3> setting) {
            auto settingName = setting->getKey();
            if (settingName.starts_with("show-object-")) {
                if (auto editorUI = static_cast<MOIEditorUI*>(EditorUI::get())) {
                    auto propertyName = settingName.substr(12);
                    auto settingValue = std::static_pointer_cast<BoolSettingV3>(setting)->getValue();
                    auto f = editorUI->m_fields.self();
                    if (propertyName == "id") f->m_showObjectID = settingValue;
                    else if (propertyName == "position") f->m_showObjectPosition = settingValue;
                    else if (propertyName == "rotation") f->m_showObjectRotation = settingValue;
                    else if (propertyName == "scale") f->m_showObjectScale = settingValue;
                    else if (propertyName == "base-color") f->m_showObjectBaseColor = settingValue;
                    else if (propertyName == "detail-color") f->m_showObjectDetailColor = settingValue;
                    else if (propertyName == "type") f->m_showObjectType = settingValue;
                    else if (propertyName == "address") f->m_showObjectAddress = settingValue;
                    else if (propertyName == "data") f->m_showObjectData = settingValue;

                    editorUI->EditorUI::updateObjectInfoLabel();
                }

                auto value = MoreObjectInfo::showObjectInfo(setting->getMod());
                if (updateHook) {
                    auto changeRes = value ? updateHook->enable() : updateHook->disable();
                    if (changeRes.isErr()) log::error("Failed to {} EditorUI::updateObjectInfoLabel hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
                }
            }
            else if (settingName == "dynamic-object-info") {
                auto value = std::static_pointer_cast<BoolSettingV3>(setting)->getValue();
                if (moveHook) {
                    auto changeRes = value ? moveHook->enable() : moveHook->disable();
                    if (changeRes.isErr()) log::error("Failed to {} EditorUI::moveObjectCall hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
                }
                if (transformHook) {
                    auto changeRes = value ? transformHook->enable() : transformHook->disable();
                    if (changeRes.isErr()) log::error("Failed to {} EditorUI::transformObjectCall hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
                }
            }
        });
    }

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto mod = Mod::get();
        auto f = m_fields.self();
        f->m_showObjectID = mod->getSettingValue<bool>("show-object-id");
        f->m_showObjectPosition = mod->getSettingValue<bool>("show-object-position");
        f->m_showObjectRotation = mod->getSettingValue<bool>("show-object-rotation");
        f->m_showObjectScale = mod->getSettingValue<bool>("show-object-scale");
        f->m_showObjectBaseColor = mod->getSettingValue<bool>("show-object-base-color");
        f->m_showObjectDetailColor = mod->getSettingValue<bool>("show-object-detail-color");
        f->m_showObjectType = mod->getSettingValue<bool>("show-object-type");
        f->m_showObjectAddress = mod->getSettingValue<bool>("show-object-address");
        f->m_showObjectData = mod->getSettingValue<bool>("show-object-data");

        return true;
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

        if (!GameManager::get()->getGameVariable("0041") || !m_selectedObject) return;

        std::string info = m_objectInfoLabel->getString();
        auto f = m_fields.self();
        auto selectedObject = m_selectedObject;

        auto objID = f->m_showObjectID ? fmt::format("ID: {}\n", selectedObject->m_objectID) : "";

        auto objPosition = f->m_showObjectPosition ? fmt::format("Position: {}, {}\n", selectedObject->getPositionX(), selectedObject->getPositionY() - 90.0f) : "";

        auto rotationX = selectedObject->getRotationX();
        auto rotationY = selectedObject->getRotationY();
        auto objRotation = f->m_showObjectRotation && (rotationX != 0.0f || rotationY != 0.0f) ?
            rotationX != rotationY ? fmt::format("Rotation: {}, {}\n", rotationX, rotationY) : fmt::format("Rotation: {}\n", rotationX) : "";

        auto scaleX = selectedObject->getScaleX();
        auto scaleY = selectedObject->getScaleY();
        auto objScale = f->m_showObjectScale && (scaleX != 1.0f || scaleY != 1.0f) ?
            scaleX != scaleY ? fmt::format("Scale: {}, {}\n", scaleX, scaleY) : fmt::format("Scale: {}\n", scaleX) : "";

        std::string objBaseColor;
        std::string objDetailColor;
        auto baseColor = selectedObject->m_baseColor;
        auto detailColor = selectedObject->m_detailColor;
        if (f->m_showObjectBaseColor && baseColor) {
            auto& hsv = baseColor->m_hsv;
            if (hsv.h != 0.0f || hsv.s != 1.0f || hsv.v != 1.0f || hsv.absoluteSaturation || hsv.absoluteBrightness)
                objBaseColor = fmt::format("{}HSV: {}, {}{}, {}{}\n", detailColor ? "Base " : "", hsv.h,
                    hsv.absoluteSaturation && hsv.s >= 0 ? "+" : "x", hsv.s,
                    hsv.absoluteBrightness && hsv.v >= 0 ? "+" : "x", hsv.v);
        }
        if (f->m_showObjectDetailColor && detailColor) {
            auto& hsv = detailColor->m_hsv;
            if (hsv.h != 0.0f || hsv.s != 1.0f || hsv.v != 1.0f || hsv.absoluteSaturation || hsv.absoluteBrightness)
                objDetailColor = fmt::format("{}HSV: {}, {}{}, {}{}\n", baseColor ? "Detail " : "", hsv.h,
                    hsv.absoluteSaturation && hsv.s >= 0 ? "+" : "x", hsv.s,
                    hsv.absoluteBrightness && hsv.v >= 0 ? "+" : "x", hsv.v);
        }

        std::string objType;
        if (f->m_showObjectType) {
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

        auto objAddress = f->m_showObjectAddress ? fmt::format("Address: 0x{:x}\n", reinterpret_cast<uintptr_t>(selectedObject)) : "";

        auto objData = f->m_showObjectData ? fmt::format("Data: {}\n", std::string(selectedObject->getSaveString(m_editorLayer))) : "";

        m_objectInfoLabel->setString(fmt::format("{}{}{}{}{}{}{}{}{}{}", m_objectInfoLabel->getString(),
            objID, objPosition, objRotation, objScale, objBaseColor, objDetailColor, objType, objAddress, objData).c_str());
    }
};
