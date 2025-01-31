#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GJRotationControl.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/GJTransformControl.hpp>

using namespace geode::prelude;

class $modify(MOIEditorUI, EditorUI) {
    struct Fields {
        bool m_dynamicObjectInfo;
        bool m_showObjectID;
        bool m_showObjectPosition;
        bool m_showObjectRotation;
        bool m_showObjectScale;
        bool m_showObjectBaseColor;
        bool m_showObjectDetailColor;
        bool m_showObjectType;
        bool m_showObjectAddress;
    };

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

        if (f->m_showObjectID) info += fmt::format("ID: {}\n", selectedObject->m_objectID);

        if (f->m_showObjectPosition) info += fmt::format("Position: {}, {}\n", selectedObject->getPositionX(), selectedObject->getPositionY() - 90.0f);

        auto rotationX = selectedObject->getRotationX();
        auto rotationY = selectedObject->getRotationY();
        if (f->m_showObjectRotation && (rotationX != 0.0f || rotationY != 0.0f))
            info += fmt::format("Rotation: {}{}\n", rotationX, rotationX != rotationY ? fmt::format(", {}", rotationY) : "");

        auto scaleX = selectedObject->getScaleX();
        auto scaleY = selectedObject->getScaleY();
        if (f->m_showObjectScale && (scaleX != 1.0f || scaleY != 1.0f))
            info += fmt::format("Scale: {}{}\n", scaleX, scaleX != scaleY ? fmt::format(", {}", scaleY) : "");

        auto baseColor = selectedObject->m_baseColor;
        auto detailColor = selectedObject->m_detailColor;
        if (f->m_showObjectBaseColor && baseColor) {
            auto hsv = baseColor->m_hsv;
            if (hsv.h != 0.0f || hsv.s != 1.0f || hsv.v != 1.0f || hsv.absoluteSaturation || hsv.absoluteBrightness)
                info += fmt::format("{}HSV: {}, {}{}, {}{}\n", detailColor ? "Base " : "", hsv.h,
                    hsv.absoluteSaturation && hsv.s >= 0 ? "+" : "x", hsv.s,
                    hsv.absoluteBrightness && hsv.v >= 0 ? "+" : "x", hsv.v);
        }
        if (f->m_showObjectDetailColor && detailColor) {
            auto hsv = detailColor->m_hsv;
            if (hsv.h != 0.0f || hsv.s != 1.0f || hsv.v != 1.0f || hsv.absoluteSaturation || hsv.absoluteBrightness)
                info += fmt::format("{}HSV: {}, {}{}, {}{}\n", baseColor ? "Detail " : "", hsv.h,
                    hsv.absoluteSaturation && hsv.s >= 0 ? "+" : "x", hsv.s,
                    hsv.absoluteBrightness && hsv.v >= 0 ? "+" : "x", hsv.v);
        }

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
            info += fmt::format("Type: {} ({})\n", typeStr, (int)objectType);
        }

        if (f->m_showObjectAddress) info += fmt::format("Address: 0x{:x}\n", reinterpret_cast<uintptr_t>(selectedObject));

        m_objectInfoLabel->setString(info.c_str());
    }
};

class $modify(MOIRotationControl, GJRotationControl) {
    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJRotationControl::ccTouchEnded(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }
};

class $modify(MOIScaleControl, GJScaleControl) {
    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJScaleControl::ccTouchEnded(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }
};

class $modify(MOITransformControl, GJTransformControl) {
    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJTransformControl::ccTouchEnded(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }
};

#define SETTING_LISTENER(settingName, memberName) listenForSettingChanges(settingName, [](decltype(MOIEditorUI::Fields::memberName) value) { \
    if (auto editorUI = static_cast<MOIEditorUI*>(EditorUI::get())) { \
        editorUI->m_fields->memberName = value; \
        editorUI->EditorUI::updateObjectInfoLabel(); \
    } \
});

$on_mod(Loaded) {
    SETTING_LISTENER("dynamic-object-info", m_dynamicObjectInfo);
    SETTING_LISTENER("show-object-id", m_showObjectID);
    SETTING_LISTENER("show-object-position", m_showObjectPosition);
    SETTING_LISTENER("show-object-rotation", m_showObjectRotation);
    SETTING_LISTENER("show-object-scale", m_showObjectScale);
    SETTING_LISTENER("show-object-base-color", m_showObjectBaseColor);
    SETTING_LISTENER("show-object-detail-color", m_showObjectDetailColor);
    SETTING_LISTENER("show-object-type", m_showObjectType);
    SETTING_LISTENER("show-object-address", m_showObjectAddress);
}
