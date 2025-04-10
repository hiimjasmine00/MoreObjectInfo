#include "../MoreObjectInfo.hpp"
#include <Geode/binding/DrawGridLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJSpriteColor.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(MOIEditorUI, EditorUI) {
    inline static constexpr std::array types = {
        "Solid", "Unknown", "Hazard", "Inverse Gravity Portal", "Normal Gravity Portal", "Ship Portal",
        "Cube Portal", "Decoration", "Yellow Jump Pad", "Pink Jump Pad", "Gravity Pad", "Yellow Jump Ring",
        "Pink Jump Ring", "Gravity Ring", "Inverse Mirror Portal", "Normal Mirror Portal", "Ball Portal", "Regular Size Portal",
        "Mini Size Portal", "Ufo Portal", "Modifier", "Breakable", "Secret Coin", "Dual Portal",
        "Solo Portal", "Slope", "Wave Portal", "Robot Portal", "Teleport Portal", "Green Ring",
        "Collectible", "User Coin", "Drop Ring", "Spider Portal", "Red Jump Pad", "Red Jump Ring",
        "Custom Ring", "Dash Ring", "Gravity Dash Ring", "Collision Object", "Special", "Swing Portal",
        "Gravity Toggle Portal", "Spider Orb", "Spider Pad", "Enter Effect Object", "Teleport Orb", "Animated Hazard"
    };

    static void onModify(ModifyBase<ModifyDerive<MOIEditorUI, EditorUI>>& self) {
        auto mod = Mod::get();
        MOI_AUTO_ENABLE(EditorUI, moveObjectCall, dynamicObjectInfo)
        MOI_AUTO_ENABLE(EditorUI, transformObjectCall, dynamicObjectInfo)
        MOI_AUTO_ENABLE(EditorUI, updateObjectInfoLabel, showObjectInfo)

        listenForAllSettingChangesV3([moveObjectCallHook, transformObjectCallHook, updateObjectInfoLabelHook](std::shared_ptr<SettingV3> setting) {
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

        if (!MoreObjectInfo::variable<"0041">()) return;

        auto selectedObject = m_selectedObject;
        if (!selectedObject) return;

        std::string objectInfo = m_objectInfoLabel->getString();
        auto mod = Mod::get();

        if (MoreObjectInfo::showObjectID(mod)) objectInfo += fmt::format("ID: {}\n", selectedObject->m_objectID);

        if (MoreObjectInfo::showObjectPosition(mod))
            objectInfo += fmt::format("Position: ({}, {})\n", selectedObject->m_obPosition.x, selectedObject->m_obPosition.y - 90.0f);

        auto rX = selectedObject->m_fRotationX;
        auto rY = selectedObject->m_fRotationY;
        if (MoreObjectInfo::showObjectRotation(mod) && (rX != 0.0f || rY != 0.0f))
            objectInfo += rX != rY ? fmt::format("Rotation: ({}, {})\n", rX, rY) : fmt::format("Rotation: {}\n", rX);

        auto sX = selectedObject->m_scaleX;
        auto sY = selectedObject->m_scaleY;
        if (MoreObjectInfo::showObjectScale(mod) && (sX != 1.0f || sY != 1.0f))
            objectInfo += sX != sY ? fmt::format("Scale: ({}, {})\n", sX, sY) : fmt::format("Scale: {}\n", sX);

        auto base = selectedObject->m_baseColor;
        auto detail = selectedObject->m_detailColor;

        if (MoreObjectInfo::showObjectBaseColor(mod) && base) {
            auto& [h, s, v, sa, va] = base->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sa || va) objectInfo += fmt::format("{}HSV: ({}, {}{:.2f}, {}{:.2f})\n",
                detail ? "Base " : "", h, sa ? s >= 0 ? "+" : "" : "x", s, va ? v >= 0 ? "+" : "" : "x", v);
        }

        if (MoreObjectInfo::showObjectDetailColor(mod) && detail) {
            auto& [h, s, v, sa, va] = detail->m_hsv;
            if (h != 0.0f || s != 1.0f || v != 1.0f || sa || va) objectInfo += fmt::format("{}HSV: ({}, {}{:.2f}, {}{:.2f})\n",
                base ? "Detail " : "", h, sa ? s >= 0 ? "+" : "" : "x", s, va ? v >= 0 ? "+" : "" : "x", v);
        }

        auto type = (int)selectedObject->m_objectType;
        objectInfo += MoreObjectInfo::showObjectType(mod) ? fmt::format("Type: {} ({})\n", type < types.size() ? types[type] : "Unknown", type) : "";

        objectInfo += MoreObjectInfo::showObjectAddress(mod) ? fmt::format("Address: 0x{:x}\n", reinterpret_cast<uintptr_t>(selectedObject)) : "";

        objectInfo += MoreObjectInfo::showObjectData(mod) ? fmt::format("Data: {}\n",
            GEODE_ANDROID(std::string)(selectedObject->getSaveString(m_editorLayer))) : "";

        objectInfo += MoreObjectInfo::showObjectTime(mod) ? fmt::format("Time: {}\n",
            m_editorLayer->m_drawGridLayer->timeForPos(selectedObject->m_obPosition, 0, 0, false, true, false, 0)) : "";

        m_objectInfoLabel->setString(objectInfo.c_str());
    }
};
