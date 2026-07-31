#pragma once
#pragma once

#include <array>
#include <iostream>
#include <list>
#include <optional>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class WorldGraph
{
public:
	void addLocation(const std::string& location)
	{
		adjacencyList.try_emplace(location);
	}

	void connectLocations(
		const std::string& first,
		const std::string& second)
	{
		addLocation(first);
		addLocation(second);

		adjacencyList[first].push_back(second);
		adjacencyList[second].push_back(first);
	}

	const std::vector<std::string>&
		getConnectedLocations(const std::string& location) const
	{
		auto iterator = adjacencyList.find(location);

		if (iterator == adjacencyList.end())
		{
			throw std::out_of_range("Location does not exist.");
		}

		return iterator->second;
	}

	bool areConnected(
		const std::string& first,
		const std::string& second) const
	{
		auto iterator = adjacencyList.find(first);

		if (iterator == adjacencyList.end())
		{
			return false;
		}

		for (const std::string& location : iterator->second)
		{
			if (location == second)
			{
				return true;
			}
		}

		return false;
	}

private:
	std::unordered_map<
		std::string,
		std::vector<std::string>
	> adjacencyList;
};