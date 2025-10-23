#include "../MoreObjectInfo.hpp"
#include <Geode/modify/GJRotationControl.hpp>

using namespace geode::prelude;

class $modify(MOIRotationControl, GJRotationControl) {
    static void onModify(ModifyBase<ModifyDerive<MOIRotationControl, GJRotationControl>>& self) {
        MoreObjectInfo::modify(self.m_hooks);
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
