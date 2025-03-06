#include "ResultUI.h"
#include "../GameManager.h"

void ResultUI::Render()
{
    const ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(DisplaySize);

    const float topAreaHeight = DisplaySize.y * 0.525f; // ���� ���� ����

    // ������ �÷��׸� �̿��� ���� ǥ����, �̵�, �������� ���� ��Ȱ��ȭ
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
    std::string afterStr = u8" ��(��) �¸��Ͽ����ϴ�.";
    std::string finalStr = winnerStr + afterStr;

    // ��ü �ؽ�Ʈ ũ�⸦ ����մϴ�.
    ImVec2 textSize = ImGui::CalcTextSize(finalStr.c_str());

    // ȭ�� �߾ӿ� ��ġ�ϵ��� Ŀ�� ��ġ ����
    ImGui::SetCursorPos(ImVec2((DisplaySize.x - textSize.x) * 0.5f, DisplaySize.y * 0.5f - 50.f));

    // �¸��� �÷��̾� �̸��� ������ ���
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Text("%s", winnerStr.c_str());
    ImGui::PopStyleColor();

    // ���� ���ο� ������ �ؽ�Ʈ ��� (�⺻ ����)
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

    // �� ��Ʈ�� �ؽ�Ʈ ũ�⸦ ���
    ImGui::PushFont(FontManager::Get().GetFont("chewy72"));
    ImVec2 normalSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    ImGui::PushFont(FontManager::Get().GetFont("chewy84"));
    ImVec2 largeSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    // �� �� �� ū ũ�⸦ ��ư �������� ���
    ImVec2 buttonSize = ImVec2(
        (normalSize.x > largeSize.x) ? normalSize.x : largeSize.x,
        (normalSize.y > largeSize.y) ? normalSize.y : largeSize.y
    );

    ImVec2 buttonPos = ImVec2((ImGui::GetIO().DisplaySize.x - buttonSize.x) * 0.5f, (ImGui::GetIO().DisplaySize.y - buttonSize.y) * 0.8f);
    ImGui::SetCursorPos(buttonPos);

    // InvisibleButton ����
    bool isClicked = ImGui::InvisibleButton(label, buttonSize);

    // ��ư ������ ���� ��ġ�� Ŀ���� �̵�
    ImVec2 position = ImGui::GetItemRectMin();
    ImGui::SetCursorPos(position);

    // hover ���¿� ���� �ٸ� ��Ÿ�Ϸ� �ؽ�Ʈ ���
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
