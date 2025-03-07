#pragma once

#include <string>
#include <Windows.h>
#include <unordered_map>
#include "../FMOD/include/fmod.hpp"

class SoundManager {
public:
    /* 싱글톤 인스턴스 가져오기 */
    static SoundManager* GetInstance();

    /* FMOD 시스템 초기화 */
    bool Initialize();

    /* FMOD 시스템 해제 */
    void Release();

    /* 사운드 로드 */
    void LoadSound(const std::wstring& SoundName, const std::wstring& FilePath, bool Loop = false);

    /* BGM 재생 */
    void PlayBGM(const std::wstring& BGMName, float Volume = 1.0f);

    /* BGM 정지 */
    void StopBGM();

    /* 효과음 재생 */
    void PlaySFX(const std::wstring& SFXName, float Volume = 1.0f);

    void Update();

private:
    SoundManager();
    ~SoundManager();

    /* sound 폴더 경로를 반환 */
    std::wstring GetSoundPath();

    // wide 문자열(UTF-16) → utf8 문자열(UTF-8)
    std::string WideToUTF8(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();

        // 변환 후 필요한 버퍼 크기를 먼저 계산
        int sizeNeeded = WideCharToMultiByte(
            CP_UTF8,        // 변환 대상 코드페이지(UTF-8)
            0,              // 변환 옵션 (기본 0)
            wstr.c_str(),
            (int)wstr.size(),
            nullptr,
            0,
            nullptr,
            nullptr
        );

        // 변환 결과를 받을 std::string 생성
        std::string utf8str(sizeNeeded, 0);

        // 실제 변환
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

    /* 싱글톤 인스턴스 */
    static SoundManager* Instance;

    /* FMOD 시스템 */
    FMOD::System* FMODSystem;

    /* BGM 관련 변수 */
    FMOD::Sound* CurrentBGM;
    FMOD::Channel* BGMChannel;

    /* 사운드 저장소 */
    std::unordered_map<std::wstring, FMOD::Sound*> SoundMap;
};