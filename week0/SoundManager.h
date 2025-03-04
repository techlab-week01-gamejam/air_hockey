#pragma once

#include <string>
#include <map>
#include "../FMOD/include/fmod.hpp"

class SoundManager {
public:
    /** 싱글톤 인스턴스 가져오기 */
    static SoundManager* GetInstance();

    /** FMOD 시스템 초기화 */
    bool Initialize();

    /** FMOD 시스템 해제 */
    void Shutdown();

    /** 사운드 로드 */
    void LoadSound(const std::string& SoundName, const std::string& FilePath, bool Loop = false);

    /** BGM 재생 */
    void PlayBGM(const std::string& BGMName, float Volume = 1.0f);

    /** BGM 정지 */
    void StopBGM();

    /** 효과음 재생 */
    void PlaySFX(const std::string& SFXName, float Volume = 1.0f);

private:
    SoundManager();
    ~SoundManager();

    /** 실행 파일이 있는 경로를 반환 */
    std::string GetExecutablePath();

    /** 싱글톤 인스턴스 */
    static SoundManager* Instance;

    /** FMOD 시스템 */
    FMOD::System* FMODSystem;

    /** BGM 관련 변수 */
    FMOD::Sound* CurrentBGM;
    FMOD::Channel* BGMChannel;

    /** 사운드 저장소 */
    std::map<std::string, FMOD::Sound*> SoundMap;
};