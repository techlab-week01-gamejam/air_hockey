#pragma once
#include "MainUI.h"

void MainUI::Render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    // 윈도우 플래그를 이용해 제목 표시줄, 이동, 리사이즈 등을 비활성화
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Main Window", nullptr, window_flags);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f); // 둥근 모서리 반경을 12로 설정

    // 화면 크기 정보 획득
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize = io.DisplaySize;

    // 이미지 설정 : 원장님 - 기본
    ImGui::SetCursorPos(ImVec2(0, 0));
    ManagedTexture king = TextureLoader::Get().GetTexture("king");
    ManagedTexture kingBlack = TextureLoader::Get().GetTexture("king-black");
    ImVec2 imageSize(king.width, king.height);

    // 보이지 않는 버튼 생성
    ImGui::InvisibleButton("ImageArea", imageSize);
    // hover 상태에 따라 렌더링할 텍스처를 선택합니다.
    ManagedTexture displayTexture = ImGui::IsItemHovered() ? kingBlack : king;
    ImGui::SetCursorPos(ImGui::GetItemRectMin()); // 버튼 영역과 동일한 위치에서 이미지 출력
    ImGui::Image((ImTextureID)(intptr_t)displayTexture.texture, imageSize);

    if (ImGui::IsItemHovered())
    {
        ImGui::PushFont(FontManager::Get().GetFont("noto50"));
        ImGui::SetCursorPos(ImVec2(imageSize.x, 140));
        ImGui::Text("%s", u8"  ㄴ여러분이 남습니다.ㅎㅎ");
        ImGui::PopFont();
    }

    // 이미지 설정 : JUNGLE
    ImGui::SetCursorPos(ImVec2(0, windowSize.y - 430));
    ManagedTexture jungle = TextureLoader::Get().GetTexture("jungle");
    ImGui::Image((ImTextureID)(intptr_t)jungle.texture, ImVec2(windowSize.x, jungle.height * 0.5f));

    // 이미지 설정 : HOCKEY PLAYER
    ImGui::SetCursorPos(ImVec2(windowSize.x - 480, windowSize.y - 480));
    ManagedTexture main = TextureLoader::Get().GetTexture("main");
    ImGui::Image((ImTextureID)(intptr_t)main.texture, ImVec2(480, 480));


    // 대사 1
    ImGui::PushFont(FontManager::Get().GetFont("noto50"));
    ImGui::SetCursorPos(ImVec2(imageSize.x, 100));
    ImGui::Text("%s", u8"이렇게 교육하면 뭐가 남나요?");
    ImGui::PopFont();

    // 메인 타이틀
    const char* title = "AIR HOCKEY";
    ImGui::PushFont(FontManager::Get().GetFont("chewy120"));
    ImVec2 textSize = ImGui::CalcTextSize(title);
    ImGui::SetCursorPos(ImVec2((windowSize.x - textSize.x) * 0.5f, 200));
    ImGui::Text("%s", title);
    ImGui::PopFont();

    // 버튼 크기와 버튼 사이의 패딩 값 설정
    ImVec2 buttonSize = ImVec2(250, 120);
    float padding = 40.0f;

    // 두 버튼과 패딩을 포함한 전체 높이 계산
    float totalButtonsHeight = buttonSize.y * 2 + padding * 4;

    // 첫 번째 버튼의 Y 좌표는 창 하단에서 전체 버튼 높이를 뺀 값으로 설정
    float firstButtonY = windowSize.y - totalButtonsHeight;
    ImVec2 button1Pos = ImVec2((windowSize.x - 480) * 0.5f - buttonSize.x * 0.5f, firstButtonY);

    ImGui::PushFont(FontManager::Get().GetFont("chewy72"));
    // 첫 번째 버튼 생성
    ImGui::SetCursorPos(button1Pos);
    if (CreateButton("PLAY"))
    {
        // Button 1 클릭 시 처리
        manager->ReplaceUI(UIState::GAME);
    }

    // 두 번째 버튼은 첫 번째 버튼 바로 아래에 패딩을 더한 위치에 생성
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
