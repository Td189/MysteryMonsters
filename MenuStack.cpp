#include "MenuStack.h"

void MenuStack::open(MenuType menu)
{
	menus.push(menu);
}

void MenuStack::close()
{
	if (!menus.empty())
	{
		menus.pop();
	}
}

void MenuStack::clear()
{
	while (!menus.empty())
	{
		menus.pop();
	}
}

std::optional<MenuType> MenuStack::current() const
{
	if (menus.empty())
	{
		return std::nullopt;
	}

	return menus.top();
}

bool MenuStack::empty() const
{
	return menus.empty();
}