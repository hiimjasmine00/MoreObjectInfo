#include "../MoreObjectInfo.hpp"
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <jasmine/hook.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

class $modify(MOIEditorUI, EditorUI) {
    static void onModify(ModifyBase<ModifyDerive<MOIEditorUI, EditorUI>>& self) {
        auto dynamicObjectInfo = jasmine::setting::getValue<bool>("dynamic-object-info");
        auto extraObjectInfo = jasmine::setting::getValue<bool>("extra-object-info");

        auto initHook = jasmine::hook::get(self.m_hooks, "EditorUI::init", extraObjectInfo);
        auto moveHook = jasmine::hook::get(self.m_hooks, "EditorUI::moveObjectCall", dynamicObjectInfo);
        auto transformHook = jasmine::hook::get(self.m_hooks, "EditorUI::transformObjectCall", dynamicObjectInfo);
        auto updateHook = jasmine::hook::get(self.m_hooks, "EditorUI::updateObjectInfoLabel", extraObjectInfo);

        new EventListener([initHook, moveHook, transformHook, updateHook](std::shared_ptr<SettingV3> setting) {
            auto settingName = setting->getKey();
            auto value = std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue();
            if (settingName == "extra-object-info") {
                jasmine::hook::toggle(initHook, value);
                jasmine::hook::toggle(updateHook, value);
            }
            else if (settingName == "dynamic-object-info") {
                jasmine::hook::toggle(moveHook, value);
                jasmine::hook::toggle(transformHook, value);
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

        fmt::memory_buffer objectInfo;
        fmt::format_to(std::back_inserter(objectInfo), "{}", m_objectInfoLabel->getString());

        if (jasmine::setting::getValue<bool>("show-object-id")) {
            fmt::format_to(std::back_inserter(objectInfo), "ID: {}\n", selectedObject->m_objectID);
        }

        if (jasmine::setting::getValue<bool>("show-object-position")) {
            auto& pos = selectedObject->m_obPosition;
            fmt::format_to(std::back_inserter(objectInfo), "Position: ({}, {})\n", pos.x, pos.y - 90.0f);
        }

        if (jasmine::setting::getValue<bool>("show-object-rotation")) {
            auto rX = selectedObject->m_fRotationX;
            auto rY = selectedObject->m_fRotationY;
            if (rX != 0.0f || rY != 0.0f) {
                if (rX == rY) fmt::format_to(std::back_inserter(objectInfo), "Rotation: {}\n", rX);
                else fmt::format_to(std::back_inserter(objectInfo), "Rotation: ({}, {})\n", rX, rY);
            }
        }

        if (jasmine::setting::getValue<bool>("show-object-scale")) {
            auto sX = selectedObject->m_scaleX;
            auto sY = selectedObject->m_scaleY;
            if (sX != 1.0f || sY != 1.0f) {
                if (sX == sY) fmt::format_to(std::back_inserter(objectInfo), "Scale: {}\n", sX);
                else fmt::format_to(std::back_inserter(objectInfo), "Scale: ({}, {})\n", sX, sY);
            }
        }

        if (jasmine::setting::getValue<bool>("show-object-size")) {
            auto& size = selectedObject->getObjectRect().size;
            fmt::format_to(std::back_inserter(objectInfo), "Size: ({}, {})\n", size.width, size.height);
        }

        if (jasmine::setting::getValue<bool>("show-object-radius")) {
            auto radius = selectedObject->m_objectRadius * std::max(selectedObject->m_scaleX, selectedObject->m_scaleY);
            if (radius != 0.0f) fmt::format_to(std::back_inserter(objectInfo), "Radius: {}\n", radius);
        }

        auto base = selectedObject->m_baseColor;
        auto detail = selectedObject->m_detailColor;

        if (jasmine::setting::getValue<bool>("show-object-base-color") && base) {
            auto& [h, s, v, sa, va] = base->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sa || va) {
                fmt::format_to(
                    std::back_inserter(objectInfo),
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

        if (jasmine::setting::getValue<bool>("show-object-detail-color") && detail) {
            auto& [h, s, v, sa, va] = detail->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sa || va) {
                fmt::format_to(
                    std::back_inserter(objectInfo),
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

        if (jasmine::setting::getValue<bool>("show-object-type")) {
            auto type = (int)selectedObject->m_objectType;
            fmt::format_to(std::back_inserter(objectInfo), "Type: {} ({})\n", type < types.size() ? types[type] : "Unknown", type);
        }

        if (jasmine::setting::getValue<bool>("show-object-address")) {
            fmt::format_to(std::back_inserter(objectInfo), "Address: 0x{:x}\n", reinterpret_cast<uintptr_t>(selectedObject));
        }

        if (jasmine::setting::getValue<bool>("show-object-data")) {
            fmt::format_to(std::back_inserter(objectInfo), "Data: {}\n", std::string(selectedObject->getSaveString(m_editorLayer)));
        }

        if (jasmine::setting::getValue<bool>("show-object-time")) {
            fmt::format_to(std::back_inserter(objectInfo), "Time: {}\n", m_editorLayer->timeForPos(selectedObject->m_obPosition, 0, 0, false, 0));
        }

        m_objectInfoLabel->setString(fmt::to_string(objectInfo).c_str());
    }
};
