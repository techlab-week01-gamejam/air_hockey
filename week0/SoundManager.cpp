#include "SoundManager.h"
#include <iostream>
#include <Windows.h>

SoundManager* SoundManager::Instance = nullptr;

SoundManager::SoundManager()
	: FMODSystem(nullptr), CurrentBGM(nullptr), BGMChannel(nullptr) { }

SoundManager::~SoundManager() {
	Release();

	if (nullptr != Instance) {
		delete Instance;
		Instance = nullptr;
	}
}

SoundManager* SoundManager::GetInstance() {
	if (nullptr == Instance) {
		Instance = new SoundManager();
	}

	return Instance;
}

bool SoundManager::Initialize() {
	if (FMOD_OK != FMOD::System_Create(&FMODSystem)) {
		std::cerr << "[Error] FMOD 시스템 생성 실패" << std::endl;
		return false;
	}
	if (FMOD_OK != FMODSystem->init(512, FMOD_INIT_NORMAL, nullptr)) {
		std::cerr << "[Error] FMOD 초기화 실패" << std::endl;
		return false;
	}

	std::cout << "[Log] FMOD 초기화 성공" << std::endl;
	return true;
}

void SoundManager::Release() {
	std::unordered_map<std::wstring, FMOD::Sound*>::iterator iter;
	for (iter = SoundMap.begin(); SoundMap.end() != iter; ++iter) {
		if (nullptr != iter->second) {
			iter->second->release();
		}
	}
	SoundMap.clear();

	if (nullptr != CurrentBGM) {
		CurrentBGM->release();
		CurrentBGM = nullptr;
	}

	if (nullptr != FMODSystem) {
		FMODSystem->close();
		FMODSystem->release();
		FMODSystem = nullptr;
	}

	if (nullptr != Instance) {
		delete Instance;
		Instance = nullptr;
	}
}

/* sound 폴더 경로 반환 */
/* 모든 효과음, BGM과 같은 음악 파일은 recource/sound 폴더에 넣어주세요! */
std::wstring SoundManager::GetSoundPath() {
	wchar_t wpath[MAX_PATH];
	GetModuleFileNameW(nullptr, wpath, MAX_PATH);
	std::wstring exePath(wpath);

	// 실행 파일이 있는 디렉터리 가져오기
	std::wstring currentDir = exePath.substr(0, exePath.find_last_of(L"\\/"));

	// 두 단계 상위 폴더로 이동 후 "resource/sound/" 추가
	std::wstring resourcePath = currentDir + L"\\..\\..\\resource\\sound";

	return resourcePath;
}

void SoundManager::LoadSound(const std::wstring& SoundName, const std::wstring& FilePath, bool Loop) {
	if (SoundMap.find(SoundName) != SoundMap.end()) {
		return; // 이미 로드된 사운드
	}

	std::wstring wFullPath = GetSoundPath() + L"\\" + FilePath;

	FMOD::Sound* NewSound = nullptr;
	FMOD_MODE Mode = Loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;

	std::string strPath = WideToUTF8(wFullPath);

	if (FMOD_OK != FMODSystem->createSound(strPath.c_str(), Mode, nullptr, &NewSound)) {
		std::cerr << "[Error] 사운드 로드 실패: " << strPath << std::endl;
		return;
	}

	SoundMap.insert(std::make_pair(SoundName, NewSound));
	std::cout << "[Log] 사운드 로드 완료: " << SoundName.c_str() << std::endl;
}

void SoundManager::PlayBGM(const std::wstring& BGMName, float Volume) {
	std::unordered_map<std::wstring, FMOD::Sound*>::iterator iter = SoundMap.find(BGMName);
	if (SoundMap.end() == iter) {
		std::cerr << "[Error] BGM을 찾을 수 없음: " << BGMName.c_str() << std::endl;
		return;
	}

	if (nullptr != CurrentBGM) {
		CurrentBGM->release();
		CurrentBGM = nullptr;
	}

	CurrentBGM = iter->second;

	if (FMOD_OK == FMODSystem->playSound(CurrentBGM, nullptr, false, &BGMChannel)) {
		BGMChannel->setVolume(Volume);
		std::cout << "[Log] BGM 재생: " << BGMName.c_str() << std::endl;
	}
}

void SoundManager::StopBGM() {
	if (nullptr != BGMChannel) {
		BGMChannel->stop();
		std::cout << "[Log] BGM 정지" << std::endl;
	}
}

void SoundManager::PlaySFX(const std::wstring& SFXName, float Volume) {
	std::unordered_map<std::wstring, FMOD::Sound*>::iterator iter = SoundMap.find(SFXName);
	if (SoundMap.end() == iter) {
		std::cerr << "[Error] 효과음을 찾을 수 없음: " << SFXName.c_str() << std::endl;
		return;
	}

	FMOD::Channel* Channel = nullptr;
	if (FMOD_OK == FMODSystem->playSound(iter->second, nullptr, false, &Channel)) {
		Channel->setVolume(Volume);
		std::cout << "[Log] 효과음 재생: " << SFXName.c_str() << std::endl;
	}
}

void SoundManager::Update() {
	if (nullptr != FMODSystem) {
		FMODSystem->update();
	}
}


