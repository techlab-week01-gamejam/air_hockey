#pragma once
#include <unordered_map>
#include <string>

struct AnimationData {
	int Rows, Cols;
	float FrameTime;
	int FrameIndex;
	float CurrentTime;
};

class SpriteAnimationManager {
public:
	static SpriteAnimationManager* GetInstance();

	void RegisterAnimation(const std::string& name, int rows, int cols, float frameTime);
	void Update(float deltaTime);
	void GetUV(const std::string& name, float& u, float& v, float& w, float& h);

private:
	SpriteAnimationManager();
	static SpriteAnimationManager* Instance;

	std::unordered_map<std::string, AnimationData> Animations;
};