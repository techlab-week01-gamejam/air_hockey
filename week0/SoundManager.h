#pragma once

#include <string>
#include <Windows.h>
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
    void LoadSound(const std::wstring& SoundName, const std::wstring& FilePath, bool Loop = false);

    /* BGM ��� */
    void PlayBGM(const std::wstring& BGMName, float Volume = 1.0f);

    /* BGM ���� */
    void StopBGM();

    /* ȿ���� ��� */
    void PlaySFX(const std::wstring& SFXName, float Volume = 1.0f);

    void Update();

private:
    SoundManager();
    ~SoundManager();

    /* sound ���� ��θ� ��ȯ */
    std::wstring GetSoundPath();

    // wide ���ڿ�(UTF-16) �� utf8 ���ڿ�(UTF-8)
    std::string WideToUTF8(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();

        // ��ȯ �� �ʿ��� ���� ũ�⸦ ���� ���
        int sizeNeeded = WideCharToMultiByte(
            CP_UTF8,        // ��ȯ ��� �ڵ�������(UTF-8)
            0,              // ��ȯ �ɼ� (�⺻ 0)
            wstr.c_str(),
            (int)wstr.size(),
            nullptr,
            0,
            nullptr,
            nullptr
        );

        // ��ȯ ����� ���� std::string ����
        std::string utf8str(sizeNeeded, 0);

        // ���� ��ȯ
        WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.c_str(),
            (int)wstr.size(),
            &utf8str[0],
            sizeNeeded,
            nullptr,
            nullptr
        );

        return utf8str;
    }

    /* �̱��� �ν��Ͻ� */
    static SoundManager* Instance;

    /* FMOD �ý��� */
    FMOD::System* FMODSystem;

    /* BGM ���� ���� */
    FMOD::Sound* CurrentBGM;
    FMOD::Channel* BGMChannel;

    /* ���� ����� */
    std::unordered_map<std::wstring, FMOD::Sound*> SoundMap;
};