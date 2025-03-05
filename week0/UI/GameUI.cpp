#include "GameUI.h"
#include "../GameManager.h"

void GameUI::Render()
{
    const ImVec2 DisplaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(DisplaySize);
    ImGui::SetNextWindowBgAlpha(0.0f); // ���� ����

    const float topAreaHeight = DisplaySize.y * 0.525f; // ���� ���� ����

    // ������ �÷��׸� �̿��� ���� ǥ����, �̵�, �������� ���� ��Ȱ��ȭ
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse;

	ImGui::Begin("Game Window", nullptr, window_flags);
    ImGui::PushFont(FontManager::Get().GetFont("chewy72"));

    std::string score1 = std::to_string(GameManager::Get().GetPlayer1Score());
    std::string score2 = std::to_string(GameManager::Get().GetPlayer2Score());

    const char* Player1Score = score1.c_str();
    const char* Player2Score = score2.c_str();

    // ȭ�� ���� ũ��(DisplaySize.x)�� ��� ���� ����(topAreaHeight)�� �̿��� �Ǻ� ��ġ ���
    float leftPivotX = DisplaySize.x * 0.25f;   // ȭ�� ���� ������ �Ǻ� (25% ����)
    float centerPivotX = DisplaySize.x * 0.5f;    // �ݷ��� �Ǻ� (50% ����)
    float rightPivotX = DisplaySize.x * 0.75f;    // ȭ�� ������ ������ �Ǻ� (75% ����)
    // Y ��ġ�� ��� ���� ������ ���ϴ� ��ġ�� �����մϴ�.
    float pivotY = topAreaHeight * 0.25f;         // ����: ��� ������ 25% ��ġ

    // �� �ؽ�Ʈ�� ũ�⸦ ����մϴ�.
    ImVec2 redScoreSize = ImGui::CalcTextSize(Player1Score);
    ImVec2 colonSize = ImGui::CalcTextSize(":");
    ImVec2 blueScoreSize = ImGui::CalcTextSize(Player2Score);

    // �� �ؽ�Ʈ�� �߾�(0.5,0.5)�� �Ǻ����� �Ͽ� ��ġ�� ����
    // ���� ����
    float redPosX = leftPivotX - redScoreSize.x * 0.5f;
    float redPosY = pivotY - redScoreSize.y * 0.5f;
    ImGui::SetCursorPos(ImVec2(redPosX, redPosY));
    ImGui::Text("%s", Player1Score);

    // �ݷ�
    float colonPosX = centerPivotX - colonSize.x * 0.5f;
    float colonPosY = pivotY - colonSize.y * 0.5f;
    ImGui::SetCursorPos(ImVec2(colonPosX, colonPosY));
    ImGui::Text("%s", ":");

    // ������ ����
    float bluePosX = rightPivotX - blueScoreSize.x * 0.5f;
    float bluePosY = pivotY - blueScoreSize.y * 0.5f;
    ImGui::SetCursorPos(ImVec2(bluePosX, bluePosY));
    ImGui::Text("%s", Player2Score);

    ImGui::PopFont();
    ImGui::End();
}

void GameUI::Release()
{

}
