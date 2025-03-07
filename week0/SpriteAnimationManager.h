#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <d3d11.h>
#include "vector3.h"

struct Vertex {
	float x, y, z;  // ��ġ ����
	float u, v;     // UV ��ǥ
};

struct ActiveAnimation {
	std::wstring Name;
	FVector3 Position;
	float Scale;
	float  TimeLeft; //  �ִϸ��̼��� ����� ������ ���� �ð�

	// ���� �ִϸ��̼� ����
	int FrameIndex = 0; // ���� �ִϸ��̼� ������	�ε���
	float CurrentTime = 0.f; // ���� �ִϸ��̼� ��� �ð�
};

struct AnimationData {
	int Rows, Cols;
	float FrameTime;
	int FrameIndex;
	float CurrentTime;
	ID3D11Buffer* ConstantBuffer; // UV ��ǥ�� ������ ��� ����
	ID3D11ShaderResourceView* Texture;
};

class SpriteAnimationManager {
public:
	static SpriteAnimationManager* GetInstance();
	void Initialize(ID3D11Device* device);
	void RegisterAnimation(const std::wstring& name, const std::wstring& filename, int rows, int cols, float frameTime, ID3D11Device* device);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);
	void GetUV(const std::wstring& name, float& u, float& v, float& w, float& h);
	void PlayAnimation(const std::wstring& name, FVector3 position, float scale);

private:
	SpriteAnimationManager();
	~SpriteAnimationManager();
	static SpriteAnimationManager* Instance;

	std::unordered_map<std::wstring, AnimationData> Animations;
	std::vector<ActiveAnimation> ActiveAnimations;

	ID3D11VertexShader* VertexShader = nullptr;  // ���� ���̴�
	ID3D11PixelShader* PixelShader = nullptr;    // �ȼ� ���̴�
	ID3D11InputLayout* InputLayout = nullptr;    // �Է� ���̾ƿ�

	ID3D11Buffer* VertexBuffer = nullptr;

	void RenderAnimation(ID3D11DeviceContext* context, const std::wstring& name, FVector3 position, float scale, int frameIndex);
	void LoadShaders(ID3D11Device* device);
	void CreateVertexBuffer(ID3D11Device* device);
};