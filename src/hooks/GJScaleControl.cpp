#include "../MoreObjectInfo.hpp"
#include <Geode/modify/GJScaleControl.hpp>

using namespace geode::prelude;

class $modify(MOIScaleControl, GJScaleControl) {
    static void onModify(ModifyBase<ModifyDerive<MOIScaleControl, GJScaleControl>>& self) {
        MoreObjectInfo::modify(self.m_hooks);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (!GJScaleControl::ccTouchBegan(touch, event)) return false;
        MoreObjectInfo::updateObjectInfoLabel();
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        GJScaleControl::ccTouchMoved(touch, event);
        MoreObjectInfo::updateObjectInfoLabel();
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        GJScaleControl::ccTouchEnded(touch, event);
        MoreObjectInfo::updateObjectInfoLabel();
    }
};
