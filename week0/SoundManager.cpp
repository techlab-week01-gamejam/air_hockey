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