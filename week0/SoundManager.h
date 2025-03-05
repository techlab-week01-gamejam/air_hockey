#pragma once

#include <string>
#include <unordered_map>
#include "../FMOD/include/fmod.hpp"

class SoundManager {
public:
    /* �̱��� �ν��Ͻ� �������� */
    static SoundManager* GetInstance();

    /* FMOD �ý��� �ʱ�ȭ */
    bool Initialize();

    /* FMOD �ý��� ���� */
    void Release();

    /* ���� �ε� */
    void LoadSound(const std::string& SoundName, const std::string& FilePath, bool Loop = false);

    /* BGM ��� */
    void PlayBGM(const std::string& BGMName, float Volume = 1.0f);

    /* BGM ���� */
    void StopBGM();

    /* ȿ���� ��� */
    void PlaySFX(const std::string& SFXName, float Volume = 1.0f);

    void Update();

private:
    SoundManager();
    ~SoundManager();

    /* sound ���� ��θ� ��ȯ */
    std::string GetSoundPath();

    /* �̱��� �ν��Ͻ� */
    static SoundManager* Instance;

    /* FMOD �ý��� */
    FMOD::System* FMODSystem;

    /* BGM ���� ���� */
    FMOD::Sound* CurrentBGM;
    FMOD::Channel* BGMChannel;

    /* ���� ����� */
    std::unordered_map<std::string, FMOD::Sound*> SoundMap;
};