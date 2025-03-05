
#pragma once


#include <d3d11.h>
#include <string>
#include <unordered_map>
#include "UI.h"
#include "../ImGui/imgui.h"
#include "../stb_image.h"

enum class EUIState
{
	NONE,
	MAIN,
	GAME,
	PAUSE,
    RESULT
};

struct ScreenSize
{
	float width;
	float height;
};

struct ManagedTexture
{
    int width;
    int height;
    ID3D11ShaderResourceView* texture;
};

class FontManager
{
public:
	static FontManager& Get()
	{
		static FontManager fm;
		return fm;
	}

	// 폰트를 로드하여 내부 맵에 저장하는 함수
	ImFont* LoadFont(const char* filename, float sizePixel, const std::string& fontName, const ImWchar* fontGlyph = ImGui::GetIO().Fonts->GetGlyphRangesDefault())
	{
		ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
			filename, sizePixel, nullptr, fontGlyph);
		if (font)
		{
			fonts_[fontName] = font;
		}
		return font;
	}

	// 저장된 폰트를 이름으로 조회하는 함수
	ImFont* GetFont(const std::string& fontName)
	{
		auto it = fonts_.find(fontName);
		return it != fonts_.end() ? it->second : nullptr;
	}
private:
	FontManager() {};
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	~FontManager() {}

	std::unordered_map<std::string, ImFont*> fonts_;
};

class TextureLoader
{
public:
    ID3D11Device* g_pd3dDevice = nullptr;
    static TextureLoader& Get()
    {
        static TextureLoader tl;
        return tl;
    }
    
    // Simple helper function to load an image into a DX11 texture with common settings
    bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, const std::string& textureName)
    {
        // Load from disk into a raw RGBA buffer
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
        if (image_data == NULL)
            return false;

        // Create texture
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = image_width;
        desc.Height = image_height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = image_data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
        
        ManagedTexture mt;
        mt.width = image_width;
        mt.height = image_height;
        mt.texture = *out_srv;

        textures_[textureName] = mt;

        pTexture->Release();
        stbi_image_free(image_data);

        return true;
    }

    // Open and read a file, then forward to LoadTextureFromMemory()
    bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, const std::string& textureName)
    {
        FILE* f = nullptr;
        if (fopen_s(&f, file_name, "rb") != 0 || f == nullptr)
            return false;

        fseek(f, 0, SEEK_END);
        long pos = ftell(f);
        if (pos < 0)
        {
            fclose(f);
            return false;
        }
        size_t file_size = static_cast<size_t>(pos);
        fseek(f, 0, SEEK_SET);

        void* file_data = IM_ALLOC(file_size);
        if (fread(file_data, 1, file_size, f) != file_size)
        {
            fclose(f);
            IM_FREE(file_data);
            return false;
        }
        fclose(f);

        bool ret = LoadTextureFromMemory(file_data, file_size, out_srv, textureName);
        IM_FREE(file_data);
        return ret;
    }

    ManagedTexture GetTexture(const std::string& textureName)
    {
        auto it = textures_.find(textureName);
        return it != textures_.end() ? it->second : ManagedTexture();
    }

private:
    TextureLoader() {};
    TextureLoader(const TextureLoader&) = delete;
    TextureLoader& operator=(const TextureLoader&) = delete;
    ~TextureLoader() {}

    std::unordered_map<std::string, ManagedTexture> textures_;
};


class UIManager
{
public:
	UIManager(){ screen.width = 1024.0f, screen.height = 1024.0f; }
	
	void Initialize(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, HWND hWnd);

	void Update();
	void Release();
	void ReplaceUI(EUIState newState);

    void TogglePause();

    EUIState GetCurrentState() { return currentState; }
private:
	ScreenSize screen;
    EUIState currentState = EUIState::NONE;

	UI* currentHUD = nullptr;

	void PrepareFonts();
    void PrepareTexture();
};

