#include "MoreObjectInfo.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/modify/GJScaleControl.hpp>

using namespace geode::prelude;
using namespace std::string_view_literals;

class $modify(MOIScaleControl, GJScaleControl) {
    static void onModify(ModifyBase<ModifyDerive<MOIScaleControl, GJScaleControl>>& self) {
        MoreObjectInfo::addHookListener(self, "GJScaleControl::ccTouchEnded", "dynamic-object-info"sv);
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJScaleControl::ccTouchEnded(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }
};
