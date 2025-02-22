#include <Geode/loader/Mod.hpp>

#define MOI_AUTO_ENABLE(className, hookName, funcName) \
    auto hookName##Res = self.getHook(#className"::"#hookName); \
    if (hookName##Res.isErr()) log::logImpl(Severity::Error, mod, "Failed to get "#className"::"#hookName" hook: {}", hookName##Res.unwrapErr()); \
    auto hookName##Hook = hookName##Res.unwrapOr(nullptr); \
    if (hookName##Hook) hookName##Hook->setAutoEnable(MoreObjectInfo::funcName(mod));

#define MOI_HOOK_TOGGLE(className, hookName) \
    if (hookName##Hook) { \
        auto changeRes = value ? hookName##Hook->enable() : hookName##Hook->disable(); \
        if (changeRes.isErr()) log::logImpl(Severity::Error, hookName##Hook->getOwner(), \
            "Failed to {} "#className"::"#hookName" hook: {}", value ? "enable" : "disable", changeRes.unwrapErr()); \
    }

#define MOI_DYNAMIC_CONTROL(className) \
    class $modify(MOI##className, className) { \
        static void onModify(ModifyBase<ModifyDerive<MOI##className, className>>& self) { \
            auto mod = Mod::get(); \
            MOI_AUTO_ENABLE(className, ccTouchBegan, dynamicObjectInfo) \
            MOI_AUTO_ENABLE(className, ccTouchMoved, dynamicObjectInfo) \
            MOI_AUTO_ENABLE(className, ccTouchEnded, dynamicObjectInfo) \
            MoreObjectInfo::settingListener<"dynamic-object-info", bool>([ccTouchBeganHook, ccTouchMovedHook, ccTouchEndedHook](bool value) { \
                MOI_HOOK_TOGGLE(className, ccTouchBegan) \
                MOI_HOOK_TOGGLE(className, ccTouchMoved) \
                MOI_HOOK_TOGGLE(className, ccTouchEnded) \
            }, mod); \
        } \
        bool ccTouchBegan(CCTouch* touch, CCEvent* event) { \
            auto ret = className::ccTouchBegan(touch, event); \
            if (auto editorUI = MoreObjectInfo::editorUI()) editorUI->updateObjectInfoLabel(); \
            return ret; \
        } \
        void ccTouchMoved(CCTouch* touch, CCEvent* event) { \
            className::ccTouchMoved(touch, event); \
            if (auto editorUI = MoreObjectInfo::editorUI()) editorUI->updateObjectInfoLabel(); \
        } \
        void ccTouchEnded(CCTouch* touch, CCEvent* event) { \
            className::ccTouchEnded(touch, event); \
            if (auto editorUI = MoreObjectInfo::editorUI()) editorUI->updateObjectInfoLabel(); \
        } \
    };

// Thanks Prevter https://github.com/EclipseMenu/EclipseMenu/blob/v1.1.0/src/modules/config/config.hpp#L135
template <size_t N>
struct TemplateString {
    static constexpr size_t size = N;
    char value[N]{};
    constexpr TemplateString() = default;
    constexpr TemplateString(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    constexpr operator std::string_view() const {
        return { value, N - 1 };
    }
};

class MoreObjectInfo {
private:
    template <class T>
    static T getInternal(geode::Mod* mod, std::string_view key) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        if (std::shared_ptr<SettingType> setting = std::static_pointer_cast<SettingType>(mod->getSetting(key))) {
            return setting->getValue();
        }
        return T();
    }
public:
    inline static GameManager* GAME_MANAGER = nullptr;

    template <TemplateString key, class T>
    static geode::EventListener<geode::SettingChangedFilterV3>* settingListener(auto&& callback, geode::Mod* mod = geode::Mod::get()) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        return new geode::EventListener(
            [callback = std::move(callback)](std::shared_ptr<geode::SettingV3> setting) {
                callback(std::static_pointer_cast<SettingType>(setting)->getValue());
            },
            geode::SettingChangedFilterV3(mod, std::string(key))
        );
    }

    template <TemplateString key, class T>
    static T get(geode::Mod* mod = geode::Mod::get()) {
        static T value = (settingListener<key, T>([](T newValue) {
            value = newValue;
        }, mod), getInternal<T>(mod, key));
        return value;
    }

    static bool showObjectID(geode::Mod* mod = geode::Mod::get());
    static bool showObjectPosition(geode::Mod* mod = geode::Mod::get());
    static bool showObjectRotation(geode::Mod* mod = geode::Mod::get());
    static bool showObjectScale(geode::Mod* mod = geode::Mod::get());
    static bool showObjectBaseColor(geode::Mod* mod = geode::Mod::get());
    static bool showObjectDetailColor(geode::Mod* mod = geode::Mod::get());
    static bool showObjectType(geode::Mod* mod = geode::Mod::get());
    static bool showObjectAddress(geode::Mod* mod = geode::Mod::get());
    static bool showObjectData(geode::Mod* mod = geode::Mod::get());
    static bool showObjectInfo(geode::Mod* mod = geode::Mod::get());
    static bool dynamicObjectInfo(geode::Mod* mod = geode::Mod::get());
    static EditorUI* editorUI();
};
