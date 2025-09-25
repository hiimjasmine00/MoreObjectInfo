#include "MoreObjectInfo.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Hook.hpp>
#include <Geode/loader/ModSettingsManager.hpp>
#include <ranges>

using namespace geode::prelude;

void addSetting(std::unordered_map<std::string_view, BoolSettingV3*>& settings, ModSettingsManager* msm, std::string_view key) {
    settings.emplace(key, static_cast<BoolSettingV3*>(msm->get(key).get()));
}

const std::unordered_map<std::string_view, BoolSettingV3*>& MoreObjectInfo::getSettings() {
    static std::unordered_map<std::string_view, BoolSettingV3*> settings = []() {
        std::unordered_map<std::string_view, BoolSettingV3*> settings;
        auto msm = ModSettingsManager::from(getMod());
        addSetting(settings, msm, "extra-object-info");
        addSetting(settings, msm, "dynamic-object-info");
        addSetting(settings, msm, "show-object-id");
        addSetting(settings, msm, "show-object-position");
        addSetting(settings, msm, "show-object-rotation");
        addSetting(settings, msm, "show-object-scale");
        addSetting(settings, msm, "show-object-size");
        addSetting(settings, msm, "show-object-radius");
        addSetting(settings, msm, "show-object-base-color");
        addSetting(settings, msm, "show-object-detail-color");
        addSetting(settings, msm, "show-object-type");
        addSetting(settings, msm, "show-object-address");
        addSetting(settings, msm, "show-object-data");
        addSetting(settings, msm, "show-object-time");
        return settings;
    }();
    return settings;
}

bool MoreObjectInfo::get(const std::unordered_map<std::string_view, BoolSettingV3*>& settings, std::string_view key) {
    if (auto it = settings.find(key); it != settings.end()) {
        if (it->second) return it->second->getValue();
    }
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
        if (auto res = hook->toggle(enable); res.isErr()) {
            log::error("Failed to toggle hook: {}", res.unwrapErr());
        }
    }
}

void MoreObjectInfo::modify(std::map<std::string, std::shared_ptr<Hook>>& hooks) {
    auto dynamicObjectInfo = MoreObjectInfo::get(MoreObjectInfo::getSettings(), "dynamic-object-info");
    for (auto& hook : std::views::values(hooks)) {
        hook->setAutoEnable(dynamicObjectInfo);
    }

    if (!hooks.empty()) {
        new EventListener([hooks = hooks](std::shared_ptr<SettingV3> setting) {
            auto value = static_cast<BoolSettingV3*>(setting.get())->getValue();
            for (auto& hook : std::views::values(hooks)) {
                toggle(hook.get(), value);
            }
        }, SettingChangedFilterV3(GEODE_MOD_ID, "dynamic-object-info"));
    }
}
