#include "PauseUI.h"

void PauseUI::Render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowBgAlpha(0.8f); // ���� ����

    // ������ �÷��׸� �̿��� ���� ǥ����, �̵�, �������� ���� ��Ȱ��ȭ
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Pause Window", nullptr, window_flags);

    // ȭ�� ũ�� ���� ȹ��
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize = io.DisplaySize;

    // ���� Ÿ��Ʋ
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

    // �� ��Ʈ�� �ؽ�Ʈ ũ�⸦ ���
    ImGui::PushFont(FontManager::Get().GetFont("Arial42"));
    ImVec2 normalSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    ImGui::PushFont(FontManager::Get().GetFont("Arial54"));
    ImVec2 largeSize = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    // �� �� �� ū ũ�⸦ ��ư �������� ���
    ImVec2 buttonSize = ImVec2(
        (normalSize.x > largeSize.x) ? normalSize.x : largeSize.x,
        (normalSize.y > largeSize.y) ? normalSize.y : largeSize.y
    );

    // InvisibleButton ����
    bool isClicked = ImGui::InvisibleButton(label, buttonSize);

    // ��ư ������ ���� ��ġ�� Ŀ���� �̵�
    ImVec2 position = ImGui::GetItemRectMin();
    ImGui::SetCursorPos(position);

    // hover ���¿� ���� �ٸ� ��Ÿ�Ϸ� �ؽ�Ʈ ���
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
