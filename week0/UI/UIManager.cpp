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

void UIManager::ReplaceUI(EUIState newState)
{
    if (currentHUD)
    {
        delete currentHUD;
        currentHUD = nullptr;
    }

    switch (newState)
    {
    case EUIState::NONE:
        break;
    case EUIState::MAIN:
        currentState = EUIState::MAIN;
        currentHUD = new MainUI(this);
        break;
    case EUIState::GAME:
        currentState = EUIState::GAME;
        currentHUD = new GameUI(this);
        break;
    case EUIState::PAUSE:
        currentState = EUIState::PAUSE;
        currentHUD = new PauseUI(this);
        break;
    default:
        break;
    }
}

void UIManager::TogglePause()
{
    if (currentState == EUIState::GAME || currentState == EUIState::PAUSE)
    {
        ReplaceUI(currentState == EUIState::GAME ? EUIState::PAUSE : EUIState::GAME);
    }
}

void UIManager::PrepareFonts()
{
    // Font Initialize
    ImGui::GetIO().Fonts->AddFontDefault();
    FontManager::Get().LoadFont("C:\\Windows\\Fonts\\Arial.ttf", 42.0f, "Arial42");
    FontManager::Get().LoadFont("C:\\Windows\\Fonts\\Arial.ttf", 54.0f, "Arial54");
    FontManager::Get().LoadFont("C:\\Windows\\Fonts\\Arial.ttf", 72.0f, "Arial72");
    FontManager::Get().LoadFont("fonts/Chewy-Regular.ttf", 72.0f, "chewy72");
    FontManager::Get().LoadFont("fonts/Chewy-Regular.ttf", 84.0f, "chewy84");
    FontManager::Get().LoadFont("fonts/Chewy-Regular.ttf", 120.0f, "chewy120");
    FontManager::Get().LoadFont("fonts/Noto.ttf", 50.0f, "noto50", ImGui::GetIO().Fonts->GetGlyphRangesKorean());
}

void UIManager::PrepareTexture()
{
    ID3D11ShaderResourceView* mainBackgroundTexture;
    TextureLoader::Get().LoadTextureFromFile("./textures/hockey.png", &mainBackgroundTexture, "main");
    
    ID3D11ShaderResourceView* jungleTexture;
    TextureLoader::Get().LoadTextureFromFile("./textures/jungle.png", &jungleTexture, "jungle");

    ID3D11ShaderResourceView* king1Texture;
    TextureLoader::Get().LoadTextureFromFile("./textures/king.png", &king1Texture, "king");

    ID3D11ShaderResourceView* king2Texture;
    TextureLoader::Get().LoadTextureFromFile("./textures/king-black.png", &king2Texture, "king-black");
}
