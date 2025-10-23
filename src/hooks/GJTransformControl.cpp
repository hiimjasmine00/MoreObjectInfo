#include "../MoreObjectInfo.hpp"
#include <Geode/modify/GJTransformControl.hpp>

using namespace geode::prelude;

class $modify(MOITransformControl, GJTransformControl) {
    static void onModify(ModifyBase<ModifyDerive<MOITransformControl, GJTransformControl>>& self) {
        MoreObjectInfo::modify(self.m_hooks);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (!GJTransformControl::ccTouchBegan(touch, event)) return false;
        MoreObjectInfo::updateObjectInfoLabel();
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        GJTransformControl::ccTouchMoved(touch, event);
        MoreObjectInfo::updateObjectInfoLabel();
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJTransformControl::ccTouchEnded(touch, event);
        MoreObjectInfo::updateObjectInfoLabel();
    }
};
