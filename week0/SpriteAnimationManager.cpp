#include "SpriteAnimationManager.h"
#include "TextureManager.h"
#include <d3dcompiler.h>

SpriteAnimationManager* SpriteAnimationManager::Instance = nullptr;

SpriteAnimationManager* SpriteAnimationManager::GetInstance() {
	if (nullptr == Instance) {
		Instance = new SpriteAnimationManager();
	}
	return Instance;
}

void SpriteAnimationManager::Initialize(ID3D11Device* device) {
	LoadShaders(device);
	CreateVertexBuffer(device);
}


void SpriteAnimationManager::RegisterAnimation(const std::wstring& name, const std::wstring& filename, int rows, int cols, float frameTime, ID3D11Device* device) {
	if (Animations.end() != Animations.find(name))
		return;

	//TextureManager���� �ؽ�ó �ε�
	ID3D11ShaderResourceView* texture = TextureManager::GetInstance()->LoadTexture(filename);

	AnimationData animation = { rows, cols, frameTime, 0, 0, nullptr, texture};

	// �ִϸ��̼� ��� �� ���  ���� ����
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * 8; // UV ������ (8�� ���)
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bufferDesc, nullptr, &animation.ConstantBuffer);

	Animations[name] = animation;
}

void SpriteAnimationManager::Update(float deltaTime) {
	std::vector<ActiveAnimation>::iterator iter;
	
	for (iter = ActiveAnimations.begin(); ActiveAnimations.end() != iter; ) {
		ActiveAnimation& activeAnimation = *iter;

		if (Animations.end() == Animations.find(activeAnimation.Name)) {
			iter = ActiveAnimations.erase(iter);
			continue;
		}

		AnimationData& animation = Animations[activeAnimation.Name];
		// ���� �ִϸ��̼� ������ ������Ʈ
		activeAnimation.CurrentTime += deltaTime;
		if (activeAnimation.CurrentTime >= animation.FrameTime) {
			activeAnimation.FrameIndex = (activeAnimation.FrameIndex + 1) % (animation.Rows * animation.Cols);
			activeAnimation.CurrentTime = 0.f;
		}

		// �ִϸ��̼� ���� ����
		activeAnimation.TimeLeft -= deltaTime;
		if (activeAnimation.TimeLeft <= 0.0f) {
			iter = ActiveAnimations.erase(iter);
		} else {
			++iter;
		}
	}
}

void SpriteAnimationManager::GetUV(const std::wstring& name, float& u, float& v, float& w, float& h) {
	if (Animations.end() == Animations.find(name))
		return;

	AnimationData& animation = Animations[name];

	int col = animation.FrameIndex % animation.Cols;
	int row = animation.FrameIndex / animation.Cols;

	u = col / (float)animation.Cols;
	v = row / (float)animation.Rows;
	w = 1.0f / (float)animation.Cols;
	h = 1.0f / (float)animation.Rows;
}

void SpriteAnimationManager::PlayAnimation(const std::wstring& name, FVector3 position, float scale){
	if (Animations.end() == Animations.find(name))
		return;

	AnimationData& animation = Animations[name];

	// �ִϸ��̼� �� ���� ���
	float totalDuration = ((float)animation.Cols * (float)animation.Rows) * animation.FrameTime;

	ActiveAnimation activeAnimation = { name, position, scale, totalDuration, 0, 0.f };
	ActiveAnimations.push_back(activeAnimation);
}

void SpriteAnimationManager::LoadShaders(ID3D11Device* device) {
	ID3DBlob* vertexShaderBlob = nullptr;
	ID3DBlob* pixelShaderBlob = nullptr;

	// Vertex Shader ������
	D3DCompileFromFile(L"SpriteShader.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexShaderBlob, nullptr);
	device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &VertexShader);

	// Pixel Shader ������
	D3DCompileFromFile(L"SpriteShader.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelShaderBlob, nullptr);
	device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &PixelShader);

	// �Է� ���̾ƿ� ����
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &InputLayout);

	vertexShaderBlob->Release();
	pixelShaderBlob->Release();
}

void SpriteAnimationManager::CreateVertexBuffer(ID3D11Device* device) {

	Vertex quadVertices[6] = {
	{-0.5f,  0.5f, 0.0f, 0.0f, 0.0f}, // �»��
	{ 0.5f,  0.5f, 0.0f, 1.0f, 0.0f}, // ����
	{-0.5f, -0.5f, 0.0f, 0.0f, 1.0f}, // ���ϴ�

	{ 0.5f,  0.5f, 0.0f, 1.0f, 0.0f}, // ����
	{ 0.5f, -0.5f, 0.0f, 1.0f, 1.0f}, // ���ϴ�
	{-0.5f, -0.5f, 0.0f, 0.0f, 1.0f}, // ���ϴ�
	};


	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) *6;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = quadVertices;

	device->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
}

void SpriteAnimationManager::Render(ID3D11DeviceContext* context) {
	std::vector<ActiveAnimation>::iterator iter;
	for (iter = ActiveAnimations.begin(); ActiveAnimations.end() != iter; ++iter) {
		RenderAnimation(context, iter->Name, iter->Position,iter->Scale, iter->FrameIndex);
	}
}

void SpriteAnimationManager::RenderAnimation(ID3D11DeviceContext* context, const std::wstring& name, FVector3 position, float scale,int frameIndex) {
	if (Animations.end() == Animations.find(name))
		return;

	AnimationData& animation = Animations[name];

	if (!animation.Texture || !animation.ConstantBuffer || !VertexBuffer)
		return;

	// ���� ������ �ε����� ����Ͽ� UV ��ǥ ������Ʈ
	float row = frameIndex / animation.Cols;
	float col = frameIndex % animation.Cols;
	float u = col / (float)animation.Cols;
	float v = row / (float)animation.Rows;
	float w = 1.0f / (float)animation.Cols;
	float h = 1.0f / (float)animation.Rows;

	// ConstantBuffer ũ�� Ȯ�� �� ������ ������Ʈ
	D3D11_BUFFER_DESC desc;
	animation.ConstantBuffer->GetDesc(&desc);
	if (desc.ByteWidth < sizeof(float) * 8) {
		return; // ũ�Ⱑ �����ϸ� ������Ʈ���� ����
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(animation.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	float* bufferData = static_cast<float*>(mappedResource.pData);
	bufferData[0] = u;
	bufferData[1] = v;
	bufferData[2] = w;
	bufferData[3] = h;
	bufferData[4] = position.x;
	bufferData[5] = position.y;
	bufferData[6] = scale; // Scale ��
	bufferData[7] = 0.0f; // �е�
	context->Unmap(animation.ConstantBuffer, 0);

	/*AddDebugLog("RenderAnimation: " + name);
	AddDebugLog("FrameIndex: " + std::to_string(frameIndex));
	AddDebugLog("Mapped UV Data: u=" + std::to_string(bufferData[0]) +
		", v=" + std::to_string(bufferData[1]) +
		", w=" + std::to_string(bufferData[2]) +
		", h=" + std::to_string(bufferData[3]));*/


	// ���ؽ� ���� ���ε�
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���̴� ����
	context->IASetInputLayout(InputLayout);
	context->VSSetShader(VertexShader, nullptr, 0);
	context->PSSetShader(PixelShader, nullptr, 0);

	// ���̴� ���ҽ��� �ؽ�ó ����
	context->PSSetShaderResources(0, 1, &animation.Texture);

	// UV ��ǥ ��� ���� ������Ʈ
	context->VSSetConstantBuffers(0, 1, &animation.ConstantBuffer);
	context->PSSetConstantBuffers(0, 1, &animation.ConstantBuffer);

	// ��������Ʈ �簢�� ������
	context->Draw(6, 0);
}


SpriteAnimationManager::SpriteAnimationManager() {
}	

SpriteAnimationManager::~SpriteAnimationManager() {
	std::unordered_map<std::wstring, AnimationData>::iterator iter;

	for (iter = Animations.begin(); Animations.end() != iter; ++iter) {
		if (nullptr != iter->second.ConstantBuffer) {
			iter->second.ConstantBuffer->Release();
			iter->second.ConstantBuffer = nullptr;
		}
	}

	if (VertexShader) {
		VertexShader->Release();
		VertexShader = nullptr;
	}
	if (PixelShader) {
		PixelShader->Release();
		PixelShader = nullptr;
	}
	if (InputLayout) {
		InputLayout->Release();
		InputLayout = nullptr;
	}
	if (VertexBuffer) {
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}
}

