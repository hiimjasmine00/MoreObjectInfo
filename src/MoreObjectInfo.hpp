#include <Geode/modify/Modify.hpp>

class MoreObjectInfo {
public:
    template <class Derived, class Base, typename... Args, typename = std::enable_if_t<(std::is_same_v<Args, std::string_view> && ...)>>
    static void addHookListener(geode::modifier::ModifyBase<geode::modifier::ModifyDerive<Derived, Base>>& self, std::string_view hookName, Args... args) {
        static_assert(sizeof...(args) > 0, "At least one setting name must be provided");

        geode::Result<geode::Hook*> hookRes = self.getHook(hookName);
        if (hookRes.isErr()) {
            geode::log::error("Failed to get {} hook: {}", hookName, hookRes.unwrapErr());
            return;
        }

        auto hook = hookRes.unwrap();
        auto mod = geode::Mod::get();
        auto enableHook = false;
        for (auto settingName : {args...}) {
            if (mod->getSettingValue<bool>(settingName)) {
                enableHook = true;
                break;
            }
        }
        hook->setAutoEnable(enableHook);

        if (sizeof...(args) > 1) geode::listenForAllSettingChanges([hook, mod, args...](auto) {
            auto enableHook = false;
            for (auto settingName : {args...}) {
                if (mod->getSettingValue<bool>(settingName)) {
                    enableHook = true;
                    break;
                }
            }

            auto changeRes = enableHook ? hook->enable() : hook->disable();
            if (changeRes.isErr()) geode::log::error("Failed to {} {} hook: {}", enableHook ? "enable" : "disable", hook->getDisplayName(), changeRes.unwrapErr());
        });
        else geode::listenForSettingChanges<bool>(std::get<0>(std::make_tuple(args...)), [hook](bool value) {
            auto changeRes = value ? hook->enable() : hook->disable();
            if (changeRes.isErr()) geode::log::error("Failed to {} {} hook: {}", value ? "enable" : "disable", hook->getDisplayName(), changeRes.unwrapErr());
        });
    }
};
