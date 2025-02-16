#include "MoreObjectInfo.hpp"
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

bool MoreObjectInfo::showObjectInfo(Mod* mod) {
    return mod->getSettingValue<bool>("show-object-id") || mod->getSettingValue<bool>("show-object-position") ||
        mod->getSettingValue<bool>("show-object-rotation") || mod->getSettingValue<bool>("show-object-scale") ||
        mod->getSettingValue<bool>("show-object-base-color") || mod->getSettingValue<bool>("show-object-detail-color") ||
        mod->getSettingValue<bool>("show-object-type") || mod->getSettingValue<bool>("show-object-address") ||
        mod->getSettingValue<bool>("show-object-data");
}

bool MoreObjectInfo::dynamicObjectInfo(Mod* mod) {
    return mod->getSettingValue<bool>("dynamic-object-info");
}
