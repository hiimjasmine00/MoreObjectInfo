#include <Geode/Geode.hpp>

template <>
struct fmt::formatter<cocos2d::ccHSVValue> : fmt::formatter<std::string> {
	template <typename T>
	auto format(const cocos2d::ccHSVValue& hsv, T& ctx) const {
		auto sat = hsv.s * 100;
		auto val = hsv.v * 100;
		return fmt::formatter<std::string>::format(
			fmt::format("({}, {}{}%, {}{}%)",
				hsv.h,
				sat, hsv.absoluteSaturation ? "+" : "",
				val, hsv.absoluteBrightness ? "+" : ""
			),
			ctx
		);
	}
};

constexpr std::array objTypes = {
    "Solid", "Unknown", "Hazard", "Inverse Gravity Portal", "Normal Gravity Portal", "Ship Portal",
    "Cube Portal", "Decoration", "Yellow Jump Pad", "Pink Jump Pad", "Gravity Pad", "Yellow Jump Ring",
    "Pink Jump Ring", "Gravity Ring", "Inverse Mirror Portal", "Normal Mirror Portal", "Ball Portal", "Regular Size Portal",
    "Mini Size Portal", "Ufo Portal", "Modifier", "Breakable", "Secret Coin", "Dual Portal",
    "Solo Portal", "Slope", "Wave Portal", "Robot Portal", "Teleport Portal", "Green Ring",
    "Collectible", "User Coin", "Drop Ring", "Spider Portal", "Red Jump Pad", "Red Jump Ring",
    "Custom Ring", "Dash Ring", "Gravity Dash Ring", "Collision Object", "Special", "Swing Portal",
    "Gravity Toggle Portal", "Spider Orb", "Spider Pad", "Enter Effect Object", "Teleport Orb", "Animated Hazard"
};
template <>
struct fmt::formatter<GameObjectType> : fmt::formatter<std::string> {
	template <typename T>
	auto format(const GameObjectType& type, T& ctx) const {
		auto index = static_cast<int>(type);
		if (index < 0 || index >= static_cast<int>(objTypes.size())) {
			return fmt::formatter<std::string>::format("Unknown", ctx);
		}
		return fmt::formatter<std::string>::format(objTypes[index], ctx);
	}
};
