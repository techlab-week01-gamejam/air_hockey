#pragma once
#include <vector>
#include <string>
#include "vector3.h"
#include "TextureManager.h"
#include "SpriteAnimationManager.h"

struct Particle {
    FVector3 Position;   // ��ġ
    FVector3 Velocity;   // �ӵ�
    float Lifetime;      // ���� ����
    float Alpha;         // ����
    std::string TextureName; // �ؽ�ó �̸�

    Particle(FVector3 pos, FVector3 vel, float lifetime, const std::string&  textureName)
        : Position(pos), Velocity(vel), Lifetime(lifetime), Alpha(1.f), TextureName(textureName){
    }
};

class ParticleManager {
public:
    static ParticleManager* GetInstance();

    void Initialize(ID3D11Device* device);
    void SpawnParticleEffect(const std::string& texturename, FVector3 position, int count, float speed, float lifetime);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* context);
    void Release();

private:
    ParticleManager();
    ~ParticleManager();

    std::vector<Particle> Particles;
    ID3D11ShaderResourceView* ParticleTexture;

    static ParticleManager* Instance;
    float RandomFloat(float Min, float Max);
};
