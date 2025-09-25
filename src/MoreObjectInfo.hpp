#include <Geode/loader/SettingV3.hpp>

class MoreObjectInfo {
public:
    static const std::unordered_map<std::string_view, geode::BoolSettingV3*>& getSettings();
    static bool get(const std::unordered_map<std::string_view, geode::BoolSettingV3*>& settings, std::string_view key);
    static GameManager* gameManager();
    static void updateObjectInfoLabel();
    static void toggle(geode::Hook* hook, bool enable);
    static void modify(std::map<std::string, std::shared_ptr<geode::Hook>>& hooks);
};
