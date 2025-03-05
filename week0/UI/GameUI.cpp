#include "GameUI.h"

void GameUI::Render()
{
    const ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(DisplaySize);
    ImGui::SetNextWindowBgAlpha(0.0f); // 완전 투명

    const float topAreaHeight = DisplaySize.y * 0.525f; // 위쪽 남은 영역

    // 윈도우 플래그를 이용해 제목 표시줄, 이동, 리사이즈 등을 비활성화
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("Game Window", nullptr, window_flags);

    ImGui::PushFont(FontManager::Get().GetFont("Arial54"));

    ImGui::SetCursorPos(ImVec2(DisplaySize.x * 0.25f, (topAreaHeight - ImGui::CalcTextSize("0").y) * 0.25f));
    ImGui::Text("%s", "0");

    ImGui::SetCursorPos(ImVec2(DisplaySize.x * 0.5f, (topAreaHeight - ImGui::CalcTextSize(":").y) * 0.25f));
    ImGui::Text("%s", ":");

    ImGui::SetCursorPos(ImVec2(DisplaySize.x * 0.75f, (topAreaHeight - ImGui::CalcTextSize("0").y) * 0.25f));
    ImGui::Text("%s", "0");

    ImGui::PopFont();

    ImGui::End();
}

void GameUI::Release()
{

}
