#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <d3d11.h>
#include "vector3.h"

struct Vertex {
	float x, y, z;  // 위치 정보
	float u, v;     // UV 좌표
};

struct ActiveAnimation {
	std::string Name;
	FVector3 Position;
	float Scale;
	float  TimeLeft; //  애니메이션이 사라질 때까지 남은 시간

	// 개별 애니메이션 상태
	int FrameIndex = 0; // 개별 애니메이션 프레임	인덱스
	float CurrentTime = 0.f; // 개별 애니메이션 경과 시간
};

struct AnimationData {
	int Rows, Cols;
	float FrameTime;
	int FrameIndex;
	float CurrentTime;
	ID3D11Buffer* ConstantBuffer; // UV 좌표를 저장할 상수 버퍼
	ID3D11ShaderResourceView* Texture;
};

class SpriteAnimationManager {
public:
	static SpriteAnimationManager* GetInstance();
	void Initialize(ID3D11Device* device);
	void RegisterAnimation(const std::string& name, const std::string& filename, int rows, int cols, float frameTime, ID3D11Device* device);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);
	void GetUV(const std::string& name, float& u, float& v, float& w, float& h);
	void PlayAnimation(const std::string& name, FVector3 position, float scale);

private:
	SpriteAnimationManager();
	~SpriteAnimationManager();
	static SpriteAnimationManager* Instance;

	std::unordered_map<std::string, AnimationData> Animations;
	std::vector<ActiveAnimation> ActiveAnimations;

	ID3D11VertexShader* VertexShader = nullptr;  // 정점 셰이더
	ID3D11PixelShader* PixelShader = nullptr;    // 픽셀 셰이더
	ID3D11InputLayout* InputLayout = nullptr;    // 입력 레이아웃

	ID3D11Buffer* VertexBuffer = nullptr;

	void RenderAnimation(ID3D11DeviceContext* context, const std::string& name, FVector3 position, float scale, int frameIndex);
	void LoadShaders(ID3D11Device* device);
	void CreateVertexBuffer(ID3D11Device* device);
};