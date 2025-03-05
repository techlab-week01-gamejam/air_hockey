#include "TextureManager.h"
#include <vector>
#include <Windows.h>

TextureManager* TextureManager::Instance = nullptr;

TextureManager* TextureManager::GetInstance() {
	if (nullptr == Instance) {
		Instance = new TextureManager();
	}
	return Instance;
}

void TextureManager::Initiallize(ID3D11Device* device, ID3D11DeviceContext* context) {
	Device = device;
	DeviceContext = context;
}

TextureManager::~TextureManager() {
	Release();
}

ID3D11ShaderResourceView* TextureManager::LoadTexture(const std::string& filename) {
	if (TextureCache.end() != TextureCache.find(filename)) {
		return TextureCache[filename];
	}

	ID3D11ShaderResourceView* textureView = nullptr;
	if (SUCCEEDED(LoadTextureFromFile(filename, &textureView))) {
		TextureCache[filename] = textureView;
	}

	return textureView;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(const std::string& filename) {
	if (TextureCache.end() != TextureCache.find(filename)) {
		return TextureCache[filename];
	}

	return nullptr;
}

void TextureManager::Release() {
	std::unordered_map<std::string, ID3D11ShaderResourceView*>::iterator iter;
	for (iter = TextureCache.begin(); TextureCache.end() != iter; ++iter) {
		if (nullptr != iter->second) {
			iter->second->Release();
		}
	}
	TextureCache.clear();
}