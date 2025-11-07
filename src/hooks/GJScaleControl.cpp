#include "../MoreObjectInfo.hpp"
#include <Geode/modify/GJScaleControl.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

class $modify(MOIScaleControl, GJScaleControl) {
    static void onModify(ModifyBase<ModifyDerive<MOIScaleControl, GJScaleControl>>& self) {
        jasmine::hook::modify(self.m_hooks, "dynamic-object-info");
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
