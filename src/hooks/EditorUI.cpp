#include "../MoreObjectInfo.hpp"
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/utils/StringBuffer.hpp>
#include <jasmine/hook.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

class $modify(MOIEditorUI, EditorUI) {
    static void onModify(ModifyBase<ModifyDerive<MOIEditorUI, EditorUI>>& self) {
        auto dynamicObjectInfo = jasmine::setting::getValue<bool>("dynamic-object-info");
        auto moveHook = jasmine::hook::get(self.m_hooks, "EditorUI::moveObjectCall", dynamicObjectInfo);
        auto transformHook = jasmine::hook::get(self.m_hooks, "EditorUI::transformObjectCall", dynamicObjectInfo);

        SettingChangedEventV3(GEODE_MOD_ID, "dynamic-object-info").listen([moveHook, transformHook](std::shared_ptr<SettingV3> setting) {
            auto value = std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue();
            jasmine::hook::toggle(moveHook, value);
            jasmine::hook::toggle(transformHook, value);
        }).leak();

        auto extraObjectInfo = jasmine::setting::getValue<bool>("extra-object-info");
        auto initHook = jasmine::hook::get(self.m_hooks, "EditorUI::init", extraObjectInfo);
        auto updateHook = jasmine::hook::get(self.m_hooks, "EditorUI::updateObjectInfoLabel", extraObjectInfo);

        SettingChangedEventV3(GEODE_MOD_ID, "extra-object-info").listen([initHook, updateHook](std::shared_ptr<SettingV3> setting) {
            auto value = std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue();
            jasmine::hook::toggle(initHook, value);
            jasmine::hook::toggle(updateHook, value);
        }).leak();

        SettingChangedEventV3().listen([](std::string_view modID, std::string_view key, std::shared_ptr<SettingV3> setting) {
            if (modID == GEODE_MOD_ID) MoreObjectInfo::updateObjectInfoLabel();
        }).leak();
    }

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        m_objectInfoLabel->setPositionX(82.0f);

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

        if (!m_objectInfoLabel || !m_objectInfoLabel->isVisible()) return;

        auto selectedObject = m_selectedObject;
        if (!selectedObject) return;

        StringBuffer objectInfo;
        objectInfo.append(m_objectInfoLabel->getString());

        if (jasmine::setting::getValue<bool>("show-object-id")) {
            objectInfo.append("ID: {}\n", selectedObject->m_objectID);
        }

        if (jasmine::setting::getValue<bool>("show-object-position")) {
            auto position = selectedObject->getPosition() - CCPoint { 0.0f, 90.0f };
            objectInfo.append("Position: ({}, {})\n", position.x, position.y);
        }

        if (jasmine::setting::getValue<bool>("show-object-rotation")) {
            auto rotationX = selectedObject->getRotationX();
            auto rotationY = selectedObject->getRotationY();
            if (rotationX != 0.0f || rotationY != 0.0f) {
                objectInfo.append("Rotation: ");
                if (rotationX == rotationY) objectInfo.append("{}", rotationX);
                else objectInfo.append("({}, {})", rotationX, rotationY);
                objectInfo.append('\n');
            }
        }

        if (jasmine::setting::getValue<bool>("show-object-scale")) {
            auto scaleX = selectedObject->m_scaleX / selectedObject->m_pixelScaleX;
            auto scaleY = selectedObject->m_scaleY / selectedObject->m_pixelScaleY;
            if (scaleX != 1.0f || scaleY != 1.0f) {
                objectInfo.append("Scale: ");
                if (scaleX == scaleY) objectInfo.append("{}", scaleX);
                else objectInfo.append("({}, {})", scaleX, scaleY);
                objectInfo.append('\n');
            }
        }

        if (jasmine::setting::getValue<bool>("show-object-flip")) {
            auto flipX = selectedObject->isFlipX();
            auto flipY = selectedObject->isFlipY();
            if (flipX || flipY) {
                objectInfo.append("Flip: ");
                if (flipX && flipY) objectInfo.append("X, Y");
                else if (flipX) objectInfo.append('X');
                else if (flipY) objectInfo.append('Y');
                objectInfo.append('\n');
            }
        }

        if (jasmine::setting::getValue<bool>("show-object-size")) {
            auto& size = selectedObject->getObjectRect().size;
            objectInfo.append("Size: ({}, {})\n", size.width, size.height);
        }

        if (jasmine::setting::getValue<bool>("show-object-radius")) {
            auto radius = selectedObject->m_objectRadius * std::max(selectedObject->m_scaleX, selectedObject->m_scaleY);
            if (radius != 0.0f) objectInfo.append("Radius: {}\n", radius);
        }

        auto base = selectedObject->m_baseColor;
        auto detail = selectedObject->m_detailColor;

        if (jasmine::setting::getValue<bool>("show-object-base-color") && base) {
            auto& [h, s, v, sAdd, vAdd] = base->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sAdd || vAdd) {
                if (detail) objectInfo.append("Base ");
                objectInfo.append("HSV: ({}, ", h);
                if (sAdd) {
                    if (s >= 0.0f) objectInfo.append('+');
                }
                else objectInfo.append('x');
                objectInfo.append("{}%, ", s * 100.0f);
                if (vAdd) {
                    if (v >= 0.0f) objectInfo.append('+');
                }
                else objectInfo.append('x');
                objectInfo.append("{}%)\n", v * 100.0f);
            }
        }

        if (jasmine::setting::getValue<bool>("show-object-detail-color") && detail) {
            auto& [h, s, v, sAdd, vAdd] = detail->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sAdd || vAdd) {
                if (base) objectInfo.append("Detail ");
                objectInfo.append("HSV: ({}, ", h);
                if (sAdd) {
                    if (s >= 0.0f) objectInfo.append('+');
                }
                else objectInfo.append('x');
                objectInfo.append("{}%, ", s * 100.0f);
                if (vAdd) {
                    if (v >= 0.0f) objectInfo.append('+');
                }
                else objectInfo.append('x');
                objectInfo.append("{}%)\n", v * 100.0f);
            }
        }

        constexpr std::array types = {
            "Solid", "Unknown", "Hazard", "Inverse Gravity Portal", "Normal Gravity Portal", "Ship Portal",
            "Cube Portal", "Decoration", "Yellow Jump Pad", "Pink Jump Pad", "Gravity Pad", "Yellow Jump Ring",
            "Pink Jump Ring", "Gravity Ring", "Inverse Mirror Portal", "Normal Mirror Portal", "Ball Portal", "Regular Size Portal",
            "Mini Size Portal", "Ufo Portal", "Modifier", "Breakable", "Secret Coin", "Dual Portal",
            "Solo Portal", "Slope", "Wave Portal", "Robot Portal", "Teleport Portal", "Green Ring",
            "Collectible", "User Coin", "Drop Ring", "Spider Portal", "Red Jump Pad", "Red Jump Ring",
            "Custom Ring", "Dash Ring", "Gravity Dash Ring", "Collision Object", "Special", "Swing Portal",
            "Gravity Toggle Portal", "Spider Orb", "Spider Pad", "Enter Effect Object", "Teleport Orb", "Animated Hazard"
        };

        if (jasmine::setting::getValue<bool>("show-object-type")) {
            auto type = (int)selectedObject->m_objectType;
            objectInfo.append("Type: {} ({})\n", type < types.size() ? types[type] : "Unknown", type);
        }

        if (jasmine::setting::getValue<bool>("show-object-address")) {
            objectInfo.append("Address: {}\n", static_cast<void*>(selectedObject));
        }

        if (jasmine::setting::getValue<bool>("show-object-data")) {
            objectInfo.append("Data: {}\n", selectedObject->getSaveString(m_editorLayer));
        }

        if (jasmine::setting::getValue<bool>("show-object-time")) {
            objectInfo.append("Time: {}\n", m_editorLayer->timeForPos(selectedObject->m_obPosition, 0, 0, false, 0));
        }

        m_objectInfoLabel->setString(objectInfo.c_str());
    }
};
