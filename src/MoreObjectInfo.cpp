#include "MoreObjectInfo.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>

void MoreObjectInfo::updateObjectInfoLabel() {
    static GameManager* gameManager = GameManager::sharedState();
    if (auto lel = gameManager->m_levelEditorLayer) {
        if (auto ui = lel->m_editorUI) ui->updateObjectInfoLabel();
    }
}
