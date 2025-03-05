#include "SpriteAnimationManager.h"

SpriteAnimationManager* SpriteAnimationManager::Instance = nullptr;

SpriteAnimationManager* SpriteAnimationManager::GetInstance() {
	if (nullptr == Instance) {
		Instance = new SpriteAnimationManager();
	}
	return Instance;
}

void SpriteAnimationManager::RegisterAnimation(const std::string& name, int rows, int cols, float frameTime) {
	Animations[name] = { rows, cols, frameTime, 0, 0 };
}

void SpriteAnimationManager::Update(float deltaTime) {
	std::unordered_map<std::string, AnimationData>::iterator iter;
	
	for (iter = Animations.begin(); Animations.end() != iter; ++iter) {
		AnimationData& animation = iter->second;
		animation.CurrentTime += deltaTime;

		if (animation.CurrentTime >= animation.FrameTime) {
			animation.FrameIndex = (animation.FrameIndex + 1) % (animation.Rows * animation.Cols);
			animation.CurrentTime = 0.f;
		}
	}
}

void SpriteAnimationManager::GetUV(const std::string& name, float& u, float& v, float& w, float& h) {
	if (Animations.end() == Animations.find(name))
		return;

	AnimationData& animation = Animations[name];

	int col = animation.FrameIndex % animation.Cols;
	int row = animation.FrameIndex / animation.Cols;

	u = col / (float)animation.Cols;
	v = row / (float)animation.Rows;
	w = 1.0f / animation.Cols;
	h = 1.0f / animation.Rows;
}
