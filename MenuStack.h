#pragma once

#include "GameTypes.h"

#include <optional>
#include <stack>

class MenuStack
{
public:
	void open(MenuType menu);

	void close();

	void clear();

	std::optional<MenuType> current() const;

	bool empty() const;

private:
	std::stack<MenuType> menus;
};