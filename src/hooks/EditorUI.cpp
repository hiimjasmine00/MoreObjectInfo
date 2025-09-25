#include "../MoreObjectInfo.hpp"
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(MOIEditorUI, EditorUI) {
    static void onModify(ModifyBase<ModifyDerive<MOIEditorUI, EditorUI>>& self) {
        auto& settings = MoreObjectInfo::getSettings();
        auto dynamicObjectInfo = MoreObjectInfo::get(settings, "dynamic-object-info");
        auto extraObjectInfo = MoreObjectInfo::get(settings, "extra-object-info");

        Hook* initHook = nullptr;
        if (auto it = self.m_hooks.find("EditorUI::init"); it == self.m_hooks.end()) {
            initHook = it->second.get();
            initHook->setAutoEnable(extraObjectInfo);
        }

        Hook* moveHook = nullptr;
        if (auto it = self.m_hooks.find("EditorUI::moveObjectCall"); it == self.m_hooks.end()) {
            moveHook = it->second.get();
            moveHook->setAutoEnable(dynamicObjectInfo);
        }

        Hook* transformHook = nullptr;
        if (auto it = self.m_hooks.find("EditorUI::transformObjectCall"); it == self.m_hooks.end()) {
            transformHook = it->second.get();
            transformHook->setAutoEnable(dynamicObjectInfo);
        }

        Hook* updateHook = nullptr;
        if (auto it = self.m_hooks.find("EditorUI::updateObjectInfoLabel"); it == self.m_hooks.end()) {
            updateHook = it->second.get();
            updateHook->setAutoEnable(extraObjectInfo);
        }

        new EventListener([initHook, moveHook, transformHook, updateHook](std::shared_ptr<SettingV3> setting) {
            auto settingName = setting->getKey();
            auto value = static_cast<BoolSettingV3*>(setting.get())->getValue();
            if (settingName == "extra-object-info") {
                MoreObjectInfo::toggle(initHook, value);
                MoreObjectInfo::toggle(updateHook, value);
            }
            else if (settingName == "dynamic-object-info") {
                MoreObjectInfo::toggle(moveHook, value);
                MoreObjectInfo::toggle(transformHook, value);
            }
            MoreObjectInfo::updateObjectInfoLabel();
        }, SettingChangedFilterV3(GEODE_MOD_ID, std::nullopt));
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

        std::string objectInfo = m_objectInfoLabel->getString();
        auto& settings = MoreObjectInfo::getSettings();

        if (MoreObjectInfo::get(settings, "show-object-id")) {
            objectInfo += fmt::format("ID: {}\n", selectedObject->m_objectID);
        }

        if (MoreObjectInfo::get(settings, "show-object-position")) {
            auto& pos = selectedObject->m_obPosition;
            objectInfo += fmt::format("Position: ({}, {})\n", pos.x, pos.y - 90.0f);
        }

        if (MoreObjectInfo::get(settings, "show-object-rotation")) {
            auto rX = selectedObject->m_fRotationX;
            auto rY = selectedObject->m_fRotationY;
            if (rX != 0.0f || rY != 0.0f) {
                objectInfo += rX != rY ? fmt::format("Rotation: ({}, {})\n", rX, rY) : fmt::format("Rotation: {}\n", rX);
            }
        }

        if (MoreObjectInfo::get(settings, "show-object-scale")) {
            auto sX = selectedObject->m_scaleX;
            auto sY = selectedObject->m_scaleY;
            if (sX != 1.0f || sY != 1.0f) {
                objectInfo += sX != sY ? fmt::format("Scale: ({}, {})\n", sX, sY) : fmt::format("Scale: {}\n", sX);
            }
        }

        if (MoreObjectInfo::get(settings, "show-object-size")) {
            auto& size = selectedObject->getObjectRect().size;
            objectInfo += fmt::format("Size: ({}, {})\n", size.width, size.height);
        }

        if (MoreObjectInfo::get(settings, "show-object-radius")) {
            auto radius = selectedObject->m_objectRadius * std::max(selectedObject->m_scaleX, selectedObject->m_scaleY);
            if (radius != 0.0f) {
                objectInfo += fmt::format("Radius: {}\n", radius);
            }
        }

        auto base = selectedObject->m_baseColor;
        auto detail = selectedObject->m_detailColor;

        if (MoreObjectInfo::get(settings, "show-object-base-color") && base) {
            auto& [h, s, v, sa, va] = base->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sa || va) {
                objectInfo += fmt::format(
                    "{}HSV: ({}, {}{}%, {}{}%)\n",
                    detail ? "Base " : "",
                    h,
                    sa ? s >= 0.0f ? "+" : "" : "x",
                    s * 100.0f,
                    va ? v >= 0.0f ? "+" : "" : "x",
                    v * 100.0f
                );
            }
        }

        if (MoreObjectInfo::get(settings, "show-object-detail-color") && detail) {
            auto& [h, s, v, sa, va] = detail->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sa || va) {
                objectInfo += fmt::format(
                    "{}HSV: ({}, {}{}%, {}{}%)\n",
                    base ? "Detail " : "",
                    h,
                    sa ? s >= 0.0f ? "+" : "" : "x",
                    s * 100.0f,
                    va ? v >= 0.0f ? "+" : "" : "x",
                    v * 100.0f
                );
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

        if (MoreObjectInfo::get(settings, "show-object-type")) {
            auto type = (int)selectedObject->m_objectType;
            objectInfo += fmt::format("Type: {} ({})\n", type < types.size() ? types[type] : "Unknown", type);
        }

        if (MoreObjectInfo::get(settings, "show-object-address")) {
            objectInfo += fmt::format("Address: 0x{:x}\n", reinterpret_cast<uintptr_t>(selectedObject));
        }

        if (MoreObjectInfo::get(settings, "show-object-data")) {
            objectInfo += fmt::format("Data: {}\n", GEODE_ANDROID(std::string)(selectedObject->getSaveString(m_editorLayer)));
        }

        if (MoreObjectInfo::get(settings, "show-object-time")) {
            auto& pos = selectedObject->m_obPosition;
            objectInfo += fmt::format("Time: {}\n", m_editorLayer->timeForPos(pos, 0, 0, false, 0));
        }

        m_objectInfoLabel->setString(objectInfo.c_str());
    }
};
