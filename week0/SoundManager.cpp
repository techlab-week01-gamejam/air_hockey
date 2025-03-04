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
		std::cerr << "[Error] FMOD �ý��� ���� ����" << std::endl;
		return false;
	}
	if (FMOD_OK != FMODSystem->init(512, FMOD_INIT_NORMAL, nullptr)) {
		std::cerr << "[Error] FMOD �ʱ�ȭ ����" << std::endl;
		return false;
	}

	std::cout << "[Log] FMOD �ʱ�ȭ ����" << std::endl;
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

/* ���� ���� ��� ��ȯ */
std::string SoundManager::GetExecutablePath() {
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string exePath(path);
	return exePath.substr(0, exePath.find_last_of("\\/"));
}

void SoundManager::LoadSound(const std::string& SoundName, const std::string& FilePath, bool Loop = false) {
	if (SoundMap.find(SoundName) != SoundMap.end()) {
		return; // �̹� �ε�� ����
	}

	std::string FullPath = GetExecutablePath() + "\\" + FilePath;

	FMOD::Sound* NewSound = nullptr;
	FMOD_MODE Mode = Loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;

	if (FMOD_OK != FMODSystem->createSound(FullPath.c_str(), Mode, nullptr, &NewSound)) {
		std::cerr << "[Error] ���� �ε� ����: " << FullPath << std::endl;
		return;
	}

	SoundMap.insert(std::make_pair(SoundName, NewSound));
	std::cout << "[Log] ���� �ε� �Ϸ�: " << SoundName << std::endl;
}

void SoundManager::PlayBGM(const std::string& BGMName, float Volume = 1.0f) {
	std::unordered_map<std::string, FMOD::Sound*>::iterator iter = SoundMap.find(BGMName);
	if (SoundMap.end() == iter) {
		std::cerr << "[Error] BGM�� ã�� �� ����: " << BGMName << std::endl;
		return;
	}

	if (nullptr != CurrentBGM) {
		CurrentBGM->release();
		CurrentBGM = nullptr;
	}

	CurrentBGM = iter->second;

	if (FMOD_OK == FMODSystem->playSound(CurrentBGM, nullptr, false, &BGMChannel)) {
		BGMChannel->setVolume(Volume);
		std::cout << "[Log] BGM �缺: " << BGMName << std::endl;
	}
}

void SoundManager::PlaySFX(const std::string& SFXName, float Volume = 1.0f) {
	std::unordered_map<std::string, FMOD::Sound*>::iterator iter = SoundMap.find(SFXName);
	if (SoundMap.end() == iter) {
		std::cerr << "[Error] ȿ������ ã�� �� ����: " << SFXName << std::endl;
		return;
	}

	FMOD::Channel* Channel = nullptr;
	if (FMOD_OK == FMODSystem->playSound(iter->second, nullptr, false, &Channel)) {
		Channel->setVolume(Volume);
		std::cout << "[Log] ȿ���� ���: " << SFXName << std::endl;
	}
}


