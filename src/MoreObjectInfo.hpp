#include <Geode/binding/GameManager.hpp>
#include <Geode/loader/Mod.hpp>

#define MOI_AUTO_ENABLE(className, hookName, funcName) \
    auto hookName##Hook = self.getHook(#className"::"#hookName).inspect([mod](geode::Hook* hook) { \
        hook->setAutoEnable(MoreObjectInfo::funcName(mod)); \
    }).inspectErr([mod](const std::string& err) { \
        geode::log::logImpl(geode::Severity::Error, mod, "Failed to get "#className"::"#hookName" hook: {}", err); \
    }).unwrapOr(nullptr); \

#define MOI_HOOK_TOGGLE(className, hookName) \
    if (hookName##Hook) (void)(value ? hookName##Hook->enable().inspectErr([hookName##Hook](const std::string& err) { \
        geode::log::error("Failed to enable "#className"::"#hookName" hook: {}", err); \
    }) : hookName##Hook->disable().inspectErr([hookName##Hook](const std::string& err) { \
        geode::log::error("Failed to disable "#className"::"#hookName" hook: {}", err); \
    })); \

#define MOI_DYNAMIC_CONTROL(className) \
    class $modify(MOI##className, className) { \
        static void onModify(auto& self) { \
            auto mod = geode::Mod::get(); \
            auto dynamicObjectInfo = MoreObjectInfo::dynamicObjectInfo(mod); \
            auto& hooks = self.m_hooks; \
            for (auto& [name, hook] : hooks) { \
                hook->setAutoEnable(dynamicObjectInfo); \
            } \
            if (!hooks.empty()) MoreObjectInfo::settingListener<"dynamic-object-info", bool>([hooks](bool value) { \
                for (auto& [name, hook] : hooks) { \
                    (void)(value ? hook->enable().inspectErr([&name](const std::string& err) { \
                        geode::log::error("Failed to enable {} hook: {}", name, err); \
                    }) : hook->disable().inspectErr([&name](const std::string& err) { \
                        geode::log::error("Failed to disable {} hook: {}", name, err); \
                    })); \
                } \
            }, mod); \
        } \
        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) { \
            auto ret = className::ccTouchBegan(touch, event); \
            if (auto editorUI = MoreObjectInfo::editorUI()) editorUI->updateObjectInfoLabel(); \
            return ret; \
        } \
        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) { \
            className::ccTouchMoved(touch, event); \
            if (auto editorUI = MoreObjectInfo::editorUI()) editorUI->updateObjectInfoLabel(); \
        } \
        void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) { \
            className::ccTouchEnded(touch, event); \
            if (auto editorUI = MoreObjectInfo::editorUI()) editorUI->updateObjectInfoLabel(); \
        } \
    };

// Thanks Prevter https://github.com/EclipseMenu/EclipseMenu/blob/v1.1.0/src/modules/config/config.hpp#L135
template <size_t N>
struct TemplateString {
    char value[N]{};
    constexpr TemplateString() = default;
    constexpr TemplateString(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    constexpr operator std::string_view() const {
        return { value, N - 1 };
    }
};

template <size_t N>
struct VariableString : public TemplateString<N + 3> {
    using Base = TemplateString<N + 3>;
    constexpr VariableString(const char (&str)[N]) {
        std::copy_n("gv_", 3, Base::value);
        std::copy_n(str, N, Base::value + 3);
    }
};

class MoreObjectInfo {
private:
    template <class T>
    static T getInternal(geode::Mod* mod, std::string_view key) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        if (std::shared_ptr<SettingType> setting = std::static_pointer_cast<SettingType>(mod->getSetting(key))) return setting->getValue();
        else return T();
    }
public:
    inline static GameManager* gameManager = nullptr;

    template <TemplateString key, class T>
    static geode::EventListener<geode::SettingChangedFilterV3>* settingListener(auto&& callback, geode::Mod* mod = geode::Mod::get()) {
        using SettingType = typename geode::SettingTypeForValueType<T>::SettingType;
        return new geode::EventListener([callback = std::move(callback)](std::shared_ptr<geode::SettingV3> setting) {
            callback(std::static_pointer_cast<SettingType>(setting)->getValue());
        }, geode::SettingChangedFilterV3(mod, std::string(key)));
    }

    template <TemplateString key, class T>
    static T get(geode::Mod* mod = geode::Mod::get()) {
        static T value = (settingListener<key, T>([](T newValue) {
            value = newValue;
        }, mod), getInternal<T>(mod, key));
        return value;
    }

    template <VariableString key>
    static bool variable() {
        return gameManager && gameManager->m_valueKeeper->valueForKey(std::string(key))->boolValue();
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
    static bool showObjectTime(geode::Mod* mod = geode::Mod::get());
    static bool showObjectInfo(geode::Mod* mod = geode::Mod::get());
    static bool dynamicObjectInfo(geode::Mod* mod = geode::Mod::get());
    static EditorUI* editorUI();
};
