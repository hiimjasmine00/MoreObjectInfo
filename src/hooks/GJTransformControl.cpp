#include "../MoreObjectInfo.hpp"
#include <Geode/modify/GJTransformControl.hpp>

using namespace geode::prelude;

class $modify(MOITransformControl, GJTransformControl) {
    static void onModify(ModifyBase<ModifyDerive<MOITransformControl, GJTransformControl>>& self) {
        MoreObjectInfo::modify(self.m_hooks);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        auto ret = GJTransformControl::ccTouchBegan(touch, event);
        MoreObjectInfo::updateObjectInfoLabel();
        return ret;
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
