#pragma once

class UI
{
public:
	UI() {};
	~UI() {};

	virtual void Render() = 0;
	virtual void Release() = 0;
};