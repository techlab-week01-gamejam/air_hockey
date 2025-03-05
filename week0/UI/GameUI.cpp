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
    ImGui::PushFont(FontManager::Get().GetFont("chewy72"));

    const char* redPlayerScore = "0";
    const char* bluePlayerScore = "0";

    // 화면 가로 크기(DisplaySize.x)와 상단 영역 높이(topAreaHeight)를 이용한 피봇 위치 계산
    float leftPivotX = DisplaySize.x * 0.25f;   // 화면 왼쪽 점수의 피봇 (25% 지점)
    float centerPivotX = DisplaySize.x * 0.5f;    // 콜론의 피봇 (50% 지점)
    float rightPivotX = DisplaySize.x * 0.75f;    // 화면 오른쪽 점수의 피봇 (75% 지점)
    // Y 위치는 상단 영역 내에서 원하는 위치를 지정합니다.
    float pivotY = topAreaHeight * 0.25f;         // 예시: 상단 영역의 25% 위치

    // 각 텍스트의 크기를 계산합니다.
    ImVec2 redScoreSize = ImGui::CalcTextSize(redPlayerScore);
    ImVec2 colonSize = ImGui::CalcTextSize(":");
    ImVec2 blueScoreSize = ImGui::CalcTextSize(bluePlayerScore);

    // 각 텍스트의 중앙(0.5,0.5)을 피봇으로 하여 위치를 결정
    // 왼쪽 점수
    float redPosX = leftPivotX - redScoreSize.x * 0.5f;
    float redPosY = pivotY - redScoreSize.y * 0.5f;
    ImGui::SetCursorPos(ImVec2(redPosX, redPosY));
    ImGui::Text("%s", redPlayerScore);

    // 콜론
    float colonPosX = centerPivotX - colonSize.x * 0.5f;
    float colonPosY = pivotY - colonSize.y * 0.5f;
    ImGui::SetCursorPos(ImVec2(colonPosX, colonPosY));
    ImGui::Text("%s", ":");

    // 오른쪽 점수
    float bluePosX = rightPivotX - blueScoreSize.x * 0.5f;
    float bluePosY = pivotY - blueScoreSize.y * 0.5f;
    ImGui::SetCursorPos(ImVec2(bluePosX, bluePosY));
    ImGui::Text("%s", bluePlayerScore);

    ImGui::PopFont();
    ImGui::End();
}

void GameUI::Release()
{

}
