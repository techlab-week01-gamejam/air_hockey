#pragma once
#include "UI.h"
#include "UIManager.h"

class MainUI : public UI
{
public:
	MainUI(UIManager* InManager) { manager = InManager; };
	~MainUI() {};

	virtual void Render() override;
	virtual void Release() override;

private:
	bool CreateButton(const char* label);
	bool CreateButton(const char* label, const ImVec4& colorNormal, const ImVec4& colorHovered = ImVec4(1.0f, 1.0f, 1.0f, 0.5f), const ImVec4& colorPressed = ImVec4(1.0f, 1.0f, 1.0f, 0.3f));

	UIManager* manager = nullptr;
};

