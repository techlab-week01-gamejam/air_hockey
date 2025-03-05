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

TextureManager::TextureManager()
	: Device(nullptr), DeviceContext(nullptr) {
}

TextureManager::~TextureManager() {
	Release();

	if (nullptr != Instance) {
		delete Instance;
		Instance = nullptr;
	}
}

ID3D11ShaderResourceView* TextureManager::LoadTexture(const std::string& filename) {
	std::string fullPath = GetTexturePath() + "\\" + filename;

	if (TextureCache.end() != TextureCache.find(fullPath)) {
		return TextureCache[fullPath];
	}

	ID3D11ShaderResourceView* textureView = nullptr;
	if (SUCCEEDED(LoadTextureFromFile(fullPath, &textureView))) {
		TextureCache[fullPath] = textureView;
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

/* WIC를 사용해서 이미지 로드 */
HRESULT TextureManager::LoadTextureFromFile(const std::string& filename, ID3D11ShaderResourceView** textureView) {
    std::wstring wFilename = StringToWString(filename); // string → wstring 변환

    ComPtr<IWICImagingFactory> wicFactory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> converter;
    ComPtr<ID3D11Texture2D> texture;

    CoInitialize(nullptr);
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));

    wicFactory->CreateDecoderFromFilename(wFilename.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    decoder->GetFrame(0, &frame);

    wicFactory->CreateFormatConverter(&converter);
    converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);

    UINT width, height;
    frame->GetSize(&width, &height);

    std::vector<BYTE> pixels(width * height * 4);
    converter->CopyPixels(nullptr, width * 4, pixels.size(), pixels.data());

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = { pixels.data(), width * 4 };
    Device->CreateTexture2D(&texDesc, &initData, &texture);
    Device->CreateShaderResourceView(texture.Get(), nullptr, textureView);

    return S_OK;
}

std::wstring TextureManager::StringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

std::string TextureManager::GetTexturePath() {
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string exePath(path);
	std::string currentDir = exePath.substr(0, exePath.find_last_of("\\/"));
	std::string resourcePath = currentDir + "\\..\\..\\resource\\texture";
	return resourcePath;
}