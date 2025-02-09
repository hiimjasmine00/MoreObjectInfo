namespace geode {
    class Mod;
    Mod* getMod();
}

class MoreObjectInfo {
public:
    static bool showObjectInfo(geode::Mod* mod = geode::getMod());
    static bool dynamicObjectInfo(geode::Mod* mod = geode::getMod());
};
