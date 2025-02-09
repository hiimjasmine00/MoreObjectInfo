#include "../MoreObjectInfo.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/modify/GJTransformControl.hpp>

using namespace geode::prelude;

class $modify(MOITransformControl, GJTransformControl) {
    static void onModify(ModifyBase<ModifyDerive<MOITransformControl, GJTransformControl>>& self) {
        auto endHookRes = self.getHook("GJTransformControl::ccTouchEnded");
        if (endHookRes.isErr()) log::error("Failed to get GJTransformControl::ccTouchEnded hook: {}", endHookRes.unwrapErr());

        auto moveHookRes = self.getHook("GJTransformControl::ccTouchMoved");
        if (moveHookRes.isErr()) log::error("Failed to get GJTransformControl::ccTouchMoved hook: {}", moveHookRes.unwrapErr());

        auto endHook = endHookRes.unwrapOr(nullptr);
        if (endHook) endHook->setAutoEnable(MoreObjectInfo::dynamicObjectInfo());
        auto moveHook = moveHookRes.unwrapOr(nullptr);
        if (moveHook) moveHook->setAutoEnable(MoreObjectInfo::dynamicObjectInfo());

        listenForSettingChanges<bool>("dynamic-object-info", [endHook, moveHook](bool value) {
            if (endHook) {
                auto changeRes = value ? endHook->enable() : endHook->disable();
                if (changeRes.isErr()) log::error("Failed to {} GJTransformControl::ccTouchEnded hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
            }
            if (moveHook) {
                auto changeRes = value ? moveHook->enable() : moveHook->disable();
                if (changeRes.isErr()) log::error("Failed to {} GJTransformControl::ccTouchMoved hook: {}", value ? "enable" : "disable", changeRes.unwrapErr());
            }
        });
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        GJTransformControl::ccTouchMoved(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJTransformControl::ccTouchEnded(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }
};
