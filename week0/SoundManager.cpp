#include "SoundManager.h"
#include <iostream>
#include <Windows.h>

SoundManager* SoundManager::Instance = nullptr;

SoundManager::SoundManager()
	: FMODSystem(nullptr), CurrentBGM(nullptr), BGMChannel(nullptr) { }

SoundManager::~SoundManager() {
	Release();
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
	std::unordered_map<std::string, FMOD::Sound*>::iterator iter;
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

/* 실행 파일 경로 반환 */
std::string SoundManager::GetExecutablePath() {
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string exePath(path);
	return exePath.substr(0, exePath.find_last_of("\\/"));
}

void SoundManager::LoadSound(const std::string& SoundName, const std::string& FilePath, bool Loop = false) {
	if (SoundMap.find(SoundName) != SoundMap.end()) {
		return; // 이미 로드된 사운드
	}

	std::string FullPath = GetExecutablePath() + "\\" + FilePath;

	FMOD::Sound* NewSound = nullptr;
	FMOD_MODE Mode = Loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;

	if (FMOD_OK != FMODSystem->createSound(FullPath.c_str(), Mode, nullptr, &NewSound)) {
		std::cerr << "[Error] 사운드 로드 실패: " << FullPath << std::endl;
		return;
	}

	SoundMap.insert(std::make_pair(SoundName, NewSound));
	std::cout << "[Log] 사운드 로드 완료: " << SoundName << std::endl;
}

void SoundManager::PlayBGM(const std::string& BGMName, float Volume = 1.0f) {
	std::unordered_map<std::string, FMOD::Sound*>::iterator iter = SoundMap.find(BGMName);
	if (SoundMap.end() == iter) {
		std::cerr << "[Error] BGM을 찾을 수 없음: " << BGMName << std::endl;
		return;
	}

	if (nullptr != CurrentBGM) {
		CurrentBGM->release();
		CurrentBGM = nullptr;
	}

	CurrentBGM = iter->second;

	if (FMOD_OK == FMODSystem->playSound(CurrentBGM, nullptr, false, &BGMChannel)) {
		BGMChannel->setVolume(Volume);
		std::cout << "[Log] BGM 재성: " << BGMName << std::endl;
	}
}

void SoundManager::PlaySFX(const std::string& SFXName, float Volume = 1.0f) {
	std::unordered_map<std::string, FMOD::Sound*>::iterator iter = SoundMap.find(SFXName);
	if (SoundMap.end() == iter) {
		std::cerr << "[Error] 효과음을 찾을 수 없음: " << SFXName << std::endl;
		return;
	}

	FMOD::Channel* Channel = nullptr;
	if (FMOD_OK == FMODSystem->playSound(iter->second, nullptr, false, &Channel)) {
		Channel->setVolume(Volume);
		std::cout << "[Log] 효과음 재생: " << SFXName << std::endl;
	}
}


