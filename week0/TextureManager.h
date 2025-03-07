#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <wincodec.h>
#include <unordered_map>
#include <string>

using namespace Microsoft::WRL;

class TextureManager {
public:
	static TextureManager* GetInstance();

	void Initiallize(ID3D11Device* device, ID3D11DeviceContext* context);
	ID3D11ShaderResourceView* LoadTexture(const std::wstring& filename);
	ID3D11ShaderResourceView* GetTexture(const std::wstring& filename);
	void Release();

private:
	TextureManager();
	~TextureManager();

	HRESULT LoadTextureFromFile(const std::wstring& filename, ID3D11ShaderResourceView** textureView);
	std::wstring StringToWString(const std::string& str);

	static TextureManager* Instance;
	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> TextureCache;
	std::wstring GetTexturePath();
};