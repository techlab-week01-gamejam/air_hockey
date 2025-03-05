#pragma once
#include "MainUI.h"

void MainUI::Render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    // ������ �÷��׸� �̿��� ���� ǥ����, �̵�, �������� ���� ��Ȱ��ȭ
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Main Window", nullptr, window_flags);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f); // �ձ� �𼭸� �ݰ��� 12�� ����

    // ȭ�� ũ�� ���� ȹ��
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize = io.DisplaySize;

    // ���� Ÿ��Ʋ
    const char* title = "AIR HOCKEY";
    ImGui::PushFont(FontManager::Get().GetFont("Arial72"));
    ImVec2 textSize = ImGui::CalcTextSize(title);
    ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, 200));
    ImGui::Text("%s", title);
    ImGui::PopFont();


    // �̹��� ����
    ImGui::SetCursorPos(ImVec2(windowSize.x - 480, windowSize.y - 480));
    ManagedTexture main = TextureLoader::Get().GetTexture("main");
    ImGui::Image((ImTextureID)(intptr_t)main.texture, ImVec2(480, 480));


    // ��ư ũ��� ��ư ������ �е� �� ����
    ImVec2 buttonSize = ImVec2(250, 120);
    float padding = 40.0f;

    // �� ��ư�� �е��� ������ ��ü ���� ���
    float totalButtonsHeight = buttonSize.y * 2 + padding * 4;

    // ù ��° ��ư�� Y ��ǥ�� â �ϴܿ��� ��ü ��ư ���̸� �� ������ ����
    float firstButtonY = windowSize.y - totalButtonsHeight;
    ImVec2 button1Pos = ImVec2((windowSize.x - 480) * 0.5f - buttonSize.x * 0.5f, firstButtonY);

    ImGui::PushFont(FontManager::Get().GetFont("Arial42"));
    // ù ��° ��ư ����
    ImGui::SetCursorPos(button1Pos);
    if (CreateButton("PLAY"))
    {
        // Button 1 Ŭ�� �� ó��
        manager->ReplaceUI(UIState::GAME);
    }

    // �� ��° ��ư�� ù ��° ��ư �ٷ� �Ʒ��� �е��� ���� ��ġ�� ����
    float secondButtonY = firstButtonY + buttonSize.y + padding;
    ImVec2 button2Pos = ImVec2((windowSize.x - 480) * 0.5f - buttonSize.x * 0.5f, secondButtonY);

    ImGui::SetCursorPos(button2Pos);
    if (CreateButton("QUIT",
        ImVec4(1.0f, 1.0f, 1.0f, 0.0f),
        ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        ImVec4(0.7f, 0.0f, 0.0f, 1.0f)
    ))
    {
        PostQuitMessage(0);
    }
    ImGui::PopFont();
    ImGui::PopStyleVar();

    ImGui::End();
}

void MainUI::Release()
{
	// somthing release this class
}

bool MainUI::CreateButton(const char* label)
{
    ImGui::PushID(label);

    ImVec2 buttonSize = ImVec2(250, 120);

    ImVec2 p = ImGui::GetCursorPos();
    bool isClicked = ImGui::InvisibleButton(label, buttonSize);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImU32 colorBG = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImU32 colorHover = ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImU32 colorPressed = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImU32 colorText = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    drawList->AddRectFilled(p, ImVec2(p.x + buttonSize.x, p.y + buttonSize.y), colorBG, 12.0f);

    if (ImGui::IsItemHovered())
    {
        drawList->AddRectFilled(p, ImVec2(p.x + buttonSize.x, p.y + buttonSize.y), colorHover, 12.0f);
    }

    if (ImGui::IsItemActive())
    {
        drawList->AddRectFilled(p, ImVec2(p.x + buttonSize.x, p.y + buttonSize.y), colorPressed, 12.0f);
    }

    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 textPos = ImVec2(p.x + (buttonSize.x - textSize.x) * 0.5f, p.y + (buttonSize.y - textSize.y) * 0.5f);
    drawList->AddText(textPos, colorText, label);

    ImGui::PopID();

    return isClicked;
}

bool MainUI::CreateButton(const char* label, const ImVec4& colorNormal, const ImVec4& colorHovered, const ImVec4& colorPressed)
{
    ImGui::PushID(label);

    ImVec2 buttonSize = ImVec2(250, 120);

    ImVec2 p = ImGui::GetCursorPos();
    bool isClicked = ImGui::InvisibleButton(label, buttonSize);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImU32 colorBG = ImGui::GetColorU32(colorNormal);
    ImU32 colorHover = ImGui::GetColorU32(colorHovered);
    ImU32 colorPress = ImGui::GetColorU32(colorPressed);
    ImU32 colorText = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    drawList->AddRectFilled(p, ImVec2(p.x + buttonSize.x, p.y + buttonSize.y), colorBG, 12.0f);

    if (ImGui::IsItemHovered())
    {
        drawList->AddRectFilled(p, ImVec2(p.x + buttonSize.x, p.y + buttonSize.y), colorHover, 12.0f);
    }

    if (ImGui::IsItemActive())
    {
        drawList->AddRectFilled(p, ImVec2(p.x + buttonSize.x, p.y + buttonSize.y), colorPress, 12.0f);
    }

    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 textPos = ImVec2(p.x + (buttonSize.x - textSize.x) * 0.5f, p.y + (buttonSize.y - textSize.y) * 0.5f);
    drawList->AddText(textPos, colorText, label);

    ImGui::PopID();

    return isClicked;
}
