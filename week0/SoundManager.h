#pragma once

#include <string>
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
    void LoadSound(const std::string& SoundName, const std::string& FilePath, bool Loop = false);

    /* BGM 재생 */
    void PlayBGM(const std::string& BGMName, float Volume = 1.0f);

    /* BGM 정지 */
    void StopBGM();

    /* 효과음 재생 */
    void PlaySFX(const std::string& SFXName, float Volume = 1.0f);

    void Update();

private:
    SoundManager();
    ~SoundManager();

    /* sound 폴더 경로를 반환 */
    std::string GetSoundPath();

    /* 싱글톤 인스턴스 */
    static SoundManager* Instance;

    /* FMOD 시스템 */
    FMOD::System* FMODSystem;

    /* BGM 관련 변수 */
    FMOD::Sound* CurrentBGM;
    FMOD::Channel* BGMChannel;

    /* 사운드 저장소 */
    std::unordered_map<std::string, FMOD::Sound*> SoundMap;
};