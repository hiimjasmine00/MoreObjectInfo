#include <Geode/binding/EditorUI.hpp>
#include <Geode/modify/GJScaleControl.hpp>

using namespace geode::prelude;

class $modify(MOIScaleControl, GJScaleControl) {
    static void onModify(ModifyBase<ModifyDerive<MOIScaleControl, GJScaleControl>>& self) {
        auto endHookRes = self.getHook("GJScaleControl::ccTouchEnded");
        if (endHookRes.isErr()) log::error("Failed to get GJScaleControl::ccTouchEnded hook: {}", endHookRes.unwrapErr());

        auto moveHookRes = self.getHook("GJScaleControl::ccTouchMoved");
        if (moveHookRes.isErr()) log::error("Failed to get GJScaleControl::ccTouchMoved hook: {}", moveHookRes.unwrapErr());

        auto endHook = endHookRes.unwrapOr(nullptr);
        auto moveHook = moveHookRes.unwrapOr(nullptr);
        auto value = Mod::get()->getSettingValue<bool>("dynamic-object-info");
        if (endHook) endHook->setAutoEnable(value);
        if (moveHook) moveHook->setAutoEnable(value);

        listenForSettingChanges("dynamic-object-info", [endHook, moveHook](bool value) {
            if (endHook) {
                auto endChangeRes = value ? endHook->enable() : endHook->disable();
                if (endChangeRes.isErr()) log::error("Failed to {} GJScaleControl::ccTouchEnded hook: {}",
                    value ? "enable" : "disable", endChangeRes.unwrapErr());
            }
            if (moveHook) {
                auto moveChangeRes = value ? moveHook->enable() : moveHook->disable();
                if (moveChangeRes.isErr()) log::error("Failed to {} GJScaleControl::ccTouchMoved hook: {}",
                    value ? "enable" : "disable", moveChangeRes.unwrapErr());
            }
        });
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        GJScaleControl::ccTouchMoved(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJScaleControl::ccTouchEnded(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }
};
