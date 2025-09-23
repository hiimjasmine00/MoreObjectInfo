#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "format.hpp"

using namespace geode::prelude;
using geode::event::v2::EventHandler;

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

template <TemplateString S>
bool settingEnabled() {
	static bool enabled = Mod::get()->getSettingValue<bool>(S);
	static EventHandler<SettingChangedEventV3> handle([](auto* ev) { return ev->getSetting()->getKey() == std::string(S); });

	handle.listen([&](auto) {
		enabled = Mod::get()->getSettingValue<bool>(S);
		return ListenerResult::Propagate;
	});

	return enabled;
}

class $modify(EditorUI) {
	bool init(LevelEditorLayer* lel) {
		if (!EditorUI::init(lel)) return false;

		schedule(schedule_selector(EditorUI::updateObjectInfoLabel));
		return true;
	}

	void updateObjectInfoLabel() {
		if (!m_objectInfoLabel->isVisible())
			return;
		EditorUI::updateObjectInfoLabel();

		if (auto obj = m_selectedObject) {
			std::string newInfo = std::string(m_objectInfoLabel->getString()) + "\n";

			if (settingEnabled<"show-object-id">())
				newInfo += fmt::format("ID: {}\n", obj->m_objectID);

			if (settingEnabled<"show-object-position">())
				newInfo += fmt::format("Position: ({}, {})\n", obj->getPositionX(), obj->getPositionY());

			if (settingEnabled<"show-object-rotation">() && obj->getRotation())
				newInfo += fmt::format("Rotation: {}\n", obj->getRotation());

			if (settingEnabled<"show-object-content-size">())
				newInfo += fmt::format("Content Size: ({}, {})\n", obj->getObjectRect().size.width, obj->getObjectRect().size.width);

			if (settingEnabled<"show-object-scale">() && (obj->m_scaleX != 1.0f || obj->m_scaleY != 1.0f)) {
				if (obj->m_scaleX == obj->m_scaleY)
					newInfo += fmt::format("Scale: {}\n", obj->m_scaleX);
				else
					newInfo += fmt::format("Scale: ({}, {})\n", obj->m_scaleX, obj->m_scaleY);
			}

			if (settingEnabled<"show-object-base-color">() && obj->m_baseColor && obj->m_baseUsesHSV)
				newInfo += fmt::format("{}HSV: {}\n", obj->m_detailColor ? "Base " : "", obj->m_baseColor->m_hsv);

			if (settingEnabled<"show-object-base-color">() && obj->m_detailColor && obj->m_detailUsesHSV)
				newInfo += fmt::format("{}HSV: {}\n", obj->m_baseColor ? "Detail " : "", obj->m_detailColor->m_hsv);

			if (settingEnabled<"show-object-type">())
				newInfo += fmt::format("Type: {} ({})\n", obj->getType(), (int)obj->getType());

			if (settingEnabled<"show-object-address">())
				newInfo += fmt::format("Address: {}", static_cast<void*>(obj));

			if (settingEnabled<"show-object-data">())
				newInfo += fmt::format("Data: {}", obj->getSaveString(m_editorLayer).c_str());

			if (settingEnabled<"show-object-time">())
				newInfo += fmt::format("Time: {}", m_editorLayer->timeForPos(obj->getPosition(), 0, 0, false, 0));


			m_objectInfoLabel->setString(newInfo.c_str());
		}
	}
};


