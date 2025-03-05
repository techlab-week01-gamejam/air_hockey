#pragma once

#include "UI.h"
#include "UIManager.h"

class ResultUI : public UI
{
public:
	ResultUI(UIManager* InManager) { manager = InManager; }
	~ResultUI() {};

	virtual void Render() override;
	virtual void Release() override;

private:
	UIManager* manager = nullptr;

	bool CreateButton(const char* label);
};

