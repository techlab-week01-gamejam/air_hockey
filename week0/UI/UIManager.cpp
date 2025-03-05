#include "UIManager.h"

#include <windows.h>

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../imGui/imgui_impl_win32.h"

#include "MainUI.h"
#include "GameUI.h"
#include "PauseUI.h"

void UIManager::Initialize(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, HWND hWnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(Device, DeviceContext);

    TextureLoader::Get().g_pd3dDevice = Device;
    
    PrepareFonts();
    PrepareTexture();
}

void UIManager::Update()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (currentHUD)
    {
        currentHUD->Render();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Release()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::ReplaceUI(UIState newState)
{
    if (currentHUD)
    {
        delete currentHUD;
        currentHUD = nullptr;
    }

    switch (newState)
    {
    case UIState::NONE:
        break;
    case UIState::MAIN:
        currentState = UIState::MAIN;
        currentHUD = new MainUI(this);
        break;
    case UIState::GAME:
        currentState = UIState::GAME;
        currentHUD = new GameUI(this);
        break;
    case UIState::PAUSE:
        currentState = UIState::PAUSE;
        currentHUD = new PauseUI(this);
        break;
    default:
        break;
    }
}

void UIManager::TogglePause()
{
    if (currentState == UIState::GAME || currentState == UIState::PAUSE)
    {
        ReplaceUI(currentState == UIState::GAME ? UIState::PAUSE : UIState::GAME);
    }
}

void UIManager::PrepareFonts()
{
    // Font Initialize
    ImGui::GetIO().Fonts->AddFontDefault();
    FontManager::Get().LoadFont("C:\\Windows\\Fonts\\Arial.ttf", 42.0f, "Arial42");
    FontManager::Get().LoadFont("C:\\Windows\\Fonts\\Arial.ttf", 54.0f, "Arial54");
    FontManager::Get().LoadFont("C:\\Windows\\Fonts\\Arial.ttf", 72.0f, "Arial72");
}

void UIManager::PrepareTexture()
{
    ID3D11ShaderResourceView* mainBackgroundTexture;
    bool ret = TextureLoader::Get().LoadTextureFromFile("./textures/hockey.png", &mainBackgroundTexture, "main");
    IM_ASSERT(ret);
}
