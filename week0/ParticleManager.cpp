#include "ParticleManager.h"
#include <cstdlib> // rand() 함수 사용

ParticleManager* ParticleManager::Instance = nullptr;

ParticleManager* ParticleManager::GetInstance() {
    if (Instance == nullptr) {
        Instance = new ParticleManager();
    }
    return Instance;
}

ParticleManager::ParticleManager() : ParticleTexture(nullptr) {}

ParticleManager::~ParticleManager() {
    Release();
}

void ParticleManager::Initialize(ID3D11Device* device) {
    //ParticleTexture = TextureManager::GetInstance()->LoadTexture("jewel_particle_blue@2x.png");
    //ParticleTexture = TextureManager::GetInstance()->LoadTexture("jewel_particle_yellow@2x.png");

    SpriteAnimationManager::GetInstance()->RegisterAnimation(
        "ParticleEffect_Blue", "jewel_particle_blue@2x.png", 1, 1, 0.1f, device
    );

    SpriteAnimationManager::GetInstance()->RegisterAnimation(
        "ParticleEffect_Yellow", "jewel_particle_yellow@2x.png", 1, 1, 0.1f, device
    );
}

void ParticleManager::SpawnParticleEffect(const std::string& texturename, FVector3 position, int count, float speed, float lifetime) {
    for (int i = 0; i < count; i++) {
        float angle = RandomFloat(0.0f, 6.283185f); // 0 ~ 2π (360도)
        float particleSpeed = RandomFloat(speed * 0.5f, speed * 1.5f);
        FVector3 velocity(cos(angle) * particleSpeed, sin(angle) * particleSpeed, 0.0f);
        Particles.emplace_back(position, velocity, lifetime, texturename);
    }
}

void ParticleManager::Update(float deltaTime) {
    for (auto it = Particles.begin(); it != Particles.end();) {
        it->Lifetime -= deltaTime;
        if (it->Lifetime <= 0.0f) {
            it = Particles.erase(it);
        } else {
            it->Position = it->Position + it->Velocity * deltaTime;
            it->Alpha -= deltaTime * 0.5f;
            ++it;
        }
    }
}

void ParticleManager::Render(ID3D11DeviceContext* context) {
    for (auto& particle : Particles) {
        SpriteAnimationManager::GetInstance()->PlayAnimation(
            particle.TextureName, particle.Position, 0.1f
        );
    }
}

void ParticleManager::Release() {
    Particles.clear();
}

float ParticleManager::RandomFloat(float Min, float Max) {
    return Min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (Max - Min));
}
