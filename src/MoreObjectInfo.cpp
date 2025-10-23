#include "MoreObjectInfo.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Hook.hpp>
#include <Geode/loader/ModSettingsManager.hpp>
#include <ranges>

using namespace geode::prelude;

static std::unordered_map<std::string_view, BoolSettingV3*> settings = [] {
    std::unordered_map<std::string_view, BoolSettingV3*> settings;
    auto msm = ModSettingsManager::from(getMod());
    constexpr std::array keys = {
        "extra-object-info",
        "dynamic-object-info",
        "show-object-id",
        "show-object-position",
        "show-object-rotation",
        "show-object-scale",
        "show-object-size",
        "show-object-radius",
        "show-object-base-color",
        "show-object-detail-color",
        "show-object-type",
        "show-object-address",
        "show-object-data",
        "show-object-time"
    };
    for (auto key : keys) {
        if (auto setting = std::static_pointer_cast<BoolSettingV3>(msm->get(key))) {
            settings.emplace(key, setting.get());
        }
    }
    return settings;
}();

bool MoreObjectInfo::get(std::string_view key) {
    if (auto it = settings.find(key); it != settings.end()) return it->second->getValue();
    return false;
}

void MoreObjectInfo::updateObjectInfoLabel() {
    static GameManager* gm = GameManager::sharedState();
    if (auto lel = gm->m_levelEditorLayer) {
        if (auto ui = lel->m_editorUI) ui->updateObjectInfoLabel();
    }
}

void MoreObjectInfo::toggle(Hook* hook, bool enable) {
    if (hook) {
        if (auto err = hook->toggle(enable).err()) {
            log::error("Failed to toggle {} hook: {}", hook->getDisplayName(), *err);
        }
    }
}

void MoreObjectInfo::modify(std::map<std::string, std::shared_ptr<Hook>>& hooks) {
    if (hooks.empty()) return;

    auto dynamicObjectInfo = MoreObjectInfo::get("dynamic-object-info");
    for (auto& hook : std::views::values(hooks)) {
        hook->setAutoEnable(dynamicObjectInfo);
    }

    new EventListener([hooks = hooks](std::shared_ptr<SettingV3> setting) {
        auto value = std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue();
        for (auto& hook : std::views::values(hooks)) {
            toggle(hook.get(), value);
        }
    }, SettingChangedFilterV3(GEODE_MOD_ID, "dynamic-object-info"));
}
