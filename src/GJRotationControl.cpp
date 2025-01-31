#include "MoreObjectInfo.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/modify/GJRotationControl.hpp>

using namespace geode::prelude;
using namespace std::string_view_literals;

class $modify(MOIRotationControl, GJRotationControl) {
    static void onModify(ModifyBase<ModifyDerive<MOIRotationControl, GJRotationControl>>& self) {
        MoreObjectInfo::addHookListener(self, "GJRotationControl::ccTouchEnded", "dynamic-object-info"sv);
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJRotationControl::ccTouchEnded(touch, event);
        if (auto editorUI = EditorUI::get()) editorUI->updateObjectInfoLabel();
    }
};
