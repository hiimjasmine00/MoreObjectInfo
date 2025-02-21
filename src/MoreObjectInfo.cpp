#include "MoreObjectInfo.hpp"
#include <Geode/binding/GameManager.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {
    MoreObjectInfo::GAME_MANAGER = GameManager::get();
}

bool MoreObjectInfo::showObjectID(Mod* mod) {
    return get<"show-object-id", bool>(mod);
}

bool MoreObjectInfo::showObjectPosition(Mod* mod) {
    return get<"show-object-position", bool>(mod);
}

bool MoreObjectInfo::showObjectRotation(Mod* mod) {
    return get<"show-object-rotation", bool>(mod);
}

bool MoreObjectInfo::showObjectScale(Mod* mod) {
    return get<"show-object-scale", bool>(mod);
}

bool MoreObjectInfo::showObjectBaseColor(Mod* mod) {
    return get<"show-object-base-color", bool>(mod);
}

bool MoreObjectInfo::showObjectDetailColor(Mod* mod) {
    return get<"show-object-detail-color", bool>(mod);
}

bool MoreObjectInfo::showObjectType(Mod* mod) {
    return get<"show-object-type", bool>(mod);
}

bool MoreObjectInfo::showObjectAddress(Mod* mod) {
    return get<"show-object-address", bool>(mod);
}

bool MoreObjectInfo::showObjectData(Mod* mod) {
    return get<"show-object-data", bool>(mod);
}

bool MoreObjectInfo::showObjectInfo(Mod* mod) {
    return showObjectID(mod) || showObjectPosition(mod) || showObjectRotation(mod) || showObjectScale(mod) ||
        showObjectBaseColor(mod) || showObjectDetailColor(mod) || showObjectType(mod) || showObjectAddress(mod) ||
        showObjectData(mod);
}

bool MoreObjectInfo::dynamicObjectInfo(Mod* mod) {
    return get<"dynamic-object-info", bool>(mod);
}
