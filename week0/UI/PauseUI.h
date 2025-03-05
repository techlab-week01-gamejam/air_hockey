#pragma once

#include "UI.h"
#include "UIManager.h"

class PauseUI : public UI
{

public:
	PauseUI(UIManager* InManager) { manager = InManager; };
	~PauseUI() {};

	virtual void Render() override;
	virtual void Release() override;

private:
	UIManager* manager = nullptr;

	bool CreateButton(const char* label);
};

