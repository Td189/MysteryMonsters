#include "Inventory.h"

void Inventory::addItem(
	const std::string& itemId,
	const std::string& name,
	const std::string& description,
	int amount)
{
	if (amount <= 0)
	{
		return;
	}

	auto iterator = items.find(itemId);

	if (iterator == items.end())
	{
		items.emplace(
			itemId,
			InventoryItem{
				name,
				description,
				amount
			}
		);
	}
	else
	{
		iterator->second.quantity += amount;
	}
}

bool Inventory::removeItem(
	const std::string& itemId,
	int amount)
{
	auto iterator = items.find(itemId);

	if (
		iterator == items.end() ||
		amount <= 0 ||
		iterator->second.quantity < amount
		)
	{
		return false;
	}

	iterator->second.quantity -= amount;

	if (iterator->second.quantity == 0)
	{
		items.erase(iterator);
	}

	return true;
}

bool Inventory::useItem(
	const std::string& itemId)
{
	return removeItem(itemId, 1);
}

int Inventory::getQuantity(
	const std::string& itemId) const
{
	const auto iterator =
		items.find(itemId);

	if (iterator == items.end())
	{
		return 0;
	}

	return iterator->second.quantity;
}

const InventoryItem* Inventory::getItem(
	const std::string& itemId) const
{
	const auto iterator =
		items.find(itemId);

	if (iterator == items.end())
	{
		return nullptr;
	}

	return &iterator->second;
}

const std::unordered_map<
	std::string,
	InventoryItem
>& Inventory::getAllItems() const
{
	return items;
}