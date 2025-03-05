#include "ResultUI.h"
#include "../GameManager.h"

void ResultUI::Render()
{
    const ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(DisplaySize);

    const float topAreaHeight = DisplaySize.y * 0.525f; // 위쪽 남은 영역

    // 윈도우 플래그를 이용해 제목 표시줄, 이동, 리사이즈 등을 비활성화
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Result Window", nullptr, window_flags);

    ManagedTexture perk = TextureLoader::Get().GetTexture("perk");
    ImGui::SetCursorPos(ImVec2((DisplaySize.x - perk.width) * 0.5f, (DisplaySize.y - perk.height) * 0.05f));
    ImGui::Image((ImTextureID)(intptr_t)perk.texture, ImVec2(perk.width, perk.height));



    ImGui::PushFont(FontManager::Get().GetFont("noto50"));
    EPlayer winner = GameManager::Get().GetWinner();

    std::string winnerStr = (winner == EPlayer::Player1) ? "Player 1" : "Player 2";
    std::string afterStr = u8" 이(가) 승리하였습니다.";
    std::string finalStr = winnerStr + afterStr;

    // 전체 텍스트 크기를 계산합니다.
    ImVec2 textSize = ImGui::CalcTextSize(finalStr.c_str());

    // 화면 중앙에 위치하도록 커서 위치 설정
    ImGui::SetCursorPos(ImVec2((DisplaySize.x - textSize.x) * 0.5f, DisplaySize.y * 0.5f - 50.f));

    // 승리한 플레이어 이름만 빨갛게 출력
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Text("%s", winnerStr.c_str());
    ImGui::PopStyleColor();

    // 같은 라인에 나머지 텍스트 출력 (기본 색상)
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Text("%s", afterStr.c_str());

    ImGui::PopFont();

    if (CreateButton("TO MAIN MENU"))
    {
        // GameManager::Get().StartNewGame();
        manager->ReplaceUI(EUIState::MAIN);
    }

    ImGui::End();
}

void ResultUI::Release()
{

}

bool ResultUI::CreateButton(const char* label)
{
   
    ImGui::PushID(label);

    // 두 폰트로 텍스트 크기를 계산
    ImGui::PushFont(FontManager::Get().GetFont("chewy72"));
    ImVec2 normalSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    ImGui::PushFont(FontManager::Get().GetFont("chewy84"));
    ImVec2 largeSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    // 둘 중 더 큰 크기를 버튼 영역으로 사용
    ImVec2 buttonSize = ImVec2(
        (normalSize.x > largeSize.x) ? normalSize.x : largeSize.x,
        (normalSize.y > largeSize.y) ? normalSize.y : largeSize.y
    );

    ImVec2 buttonPos = ImVec2((ImGui::GetIO().DisplaySize.x - buttonSize.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - buttonSize.y) * 0.8f);
    ImGui::SetCursorPos(buttonPos);

    // InvisibleButton 생성
    bool isClicked = ImGui::InvisibleButton(label, buttonSize);

    // 버튼 영역의 시작 위치로 커서를 이동
    ImVec2 position = ImGui::GetItemRectMin();
    ImGui::SetCursorPos(position);

    // hover 상태에 따라 다른 스타일로 텍스트 출력
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushFont(FontManager::Get().GetFont("chewy72"));
        ImGui::Text("%s", label);
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushFont(FontManager::Get().GetFont("chewy72"));
        ImGui::Text("%s", label);
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }

    ImGui::PopID();

    return isClicked;
}
