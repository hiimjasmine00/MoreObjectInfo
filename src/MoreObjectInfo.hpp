#include <Geode/loader/Types.hpp>

class MoreObjectInfo {
public:
    static bool get(std::string_view key);
    static void updateObjectInfoLabel();
    static void toggle(geode::Hook* hook, bool enable);
    static void modify(std::map<std::string, std::shared_ptr<geode::Hook>>& hooks);
};
