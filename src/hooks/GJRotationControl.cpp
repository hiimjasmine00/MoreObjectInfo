#include "../MoreObjectInfo.hpp"
#include <Geode/modify/GJRotationControl.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

class $modify(MOIRotationControl, GJRotationControl) {
    static void onModify(ModifyBase<ModifyDerive<MOIRotationControl, GJRotationControl>>& self) {
        jasmine::hook::modify(self.m_hooks, "dynamic-object-info");
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (!GJRotationControl::ccTouchBegan(touch, event)) return false;
        MoreObjectInfo::updateObjectInfoLabel();
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        GJRotationControl::ccTouchMoved(touch, event);
        MoreObjectInfo::updateObjectInfoLabel();
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJRotationControl::ccTouchEnded(touch, event);
        MoreObjectInfo::updateObjectInfoLabel();
    }
};
