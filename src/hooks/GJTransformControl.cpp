#include "../MoreObjectInfo.hpp"
#include <Geode/modify/GJTransformControl.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

class $modify(MOITransformControl, GJTransformControl) {
    static void onModify(ModifyBase<ModifyDerive<MOITransformControl, GJTransformControl>>& self) {
        jasmine::hook::modify(self.m_hooks, "dynamic-object-info");
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
