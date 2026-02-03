//
// Created by jag on 2026-02-03.
//

#include "Editor.h"

using namespace sk::Editor;

bool cEditor::IsGameRunning() const
{
    return m_is_game_running_.load();
}
