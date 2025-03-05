#include "PauseUI.h"

void PauseUI::Render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowBgAlpha(0.8f); // 완전 투명

    // 윈도우 플래그를 이용해 제목 표시줄, 이동, 리사이즈 등을 비활성화
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Pause Window", nullptr, window_flags);

    // 화면 크기 정보 획득
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize = io.DisplaySize;

    // 메인 타이틀
    const char* title = "PAUSED";
    ImGui::PushFont(FontManager::Get().GetFont("Arial72"));
    ImVec2 textSize = ImGui::CalcTextSize(title);
    ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, 400));
    ImGui::Text("%s", title);
    ImGui::PopFont();

    ImGui::SetCursorPos(ImVec2(25.0f, ImGui::GetIO().DisplaySize.y * 0.8f));

    if (CreateButton("CONTINUE"))
    {
        manager->ReplaceUI(UIState::GAME);
    }

    ImGui::SetCursorPos(ImVec2(25.0f, ImGui::GetIO().DisplaySize.y * 0.8f + 60.0f));
    if (CreateButton("RESTART"))
    {
        manager->ReplaceUI(UIState::MAIN);
    }

    ImGui::SetCursorPos(ImVec2(25.0f, ImGui::GetIO().DisplaySize.y * 0.8f + 120.0f));
    if (CreateButton("TO MAIN MENU"))
    {
        manager->ReplaceUI(UIState::MAIN);
    }

    ImGui::End();
}

void PauseUI::Release()
{

}

bool PauseUI::CreateButton(const char* label)
{
    ImGui::PushID(label);

    // 두 폰트로 텍스트 크기를 계산
    ImGui::PushFont(FontManager::Get().GetFont("Arial42"));
    ImVec2 normalSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    ImGui::PushFont(FontManager::Get().GetFont("Arial54"));
    ImVec2 largeSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    // 둘 중 더 큰 크기를 버튼 영역으로 사용
    ImVec2 buttonSize = ImVec2(
        (normalSize.x > largeSize.x) ? normalSize.x : largeSize.x,
        (normalSize.y > largeSize.y) ? normalSize.y : largeSize.y
    );

    // InvisibleButton 생성
    bool isClicked = ImGui::InvisibleButton(label, buttonSize);

    // 버튼 영역의 시작 위치로 커서를 이동
    ImVec2 position = ImGui::GetItemRectMin();
    ImGui::SetCursorPos(position);

    // hover 상태에 따라 다른 스타일로 텍스트 출력
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushFont(FontManager::Get().GetFont("Arial54"));
        ImGui::Text("%s", label);
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushFont(FontManager::Get().GetFont("Arial42"));
        ImGui::Text("%s", label);
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }

    ImGui::PopID();

    return isClicked;
}
