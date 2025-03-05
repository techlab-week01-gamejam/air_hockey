#pragma once
#include "UI.h"
#include "UIManager.h"

class GameUI : public UI
{
public:
	GameUI(UIManager* InManager) { manager = InManager; }
	~GameUI() {};

	virtual void Render() override;
	virtual void Release() override;

private:
	UIManager* manager = nullptr;
};

