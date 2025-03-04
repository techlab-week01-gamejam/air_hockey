#pragma once

#include <string>
#include <map>
#include "../FMOD/include/fmod.hpp"

class SoundManager {
public:
    /** �̱��� �ν��Ͻ� �������� */
    static SoundManager* GetInstance();

    /** FMOD �ý��� �ʱ�ȭ */
    bool Initialize();

    /** FMOD �ý��� ���� */
    void Shutdown();

    /** ���� �ε� */
    void LoadSound(const std::string& SoundName, const std::string& FilePath, bool Loop = false);

    /** BGM ��� */
    void PlayBGM(const std::string& BGMName, float Volume = 1.0f);

    /** BGM ���� */
    void StopBGM();

    /** ȿ���� ��� */
    void PlaySFX(const std::string& SFXName, float Volume = 1.0f);

private:
    SoundManager();
    ~SoundManager();

    /** ���� ������ �ִ� ��θ� ��ȯ */
    std::string GetExecutablePath();

    /** �̱��� �ν��Ͻ� */
    static SoundManager* Instance;

    /** FMOD �ý��� */
    FMOD::System* FMODSystem;

    /** BGM ���� ���� */
    FMOD::Sound* CurrentBGM;
    FMOD::Channel* BGMChannel;

    /** ���� ����� */
    std::map<std::string, FMOD::Sound*> SoundMap;
};