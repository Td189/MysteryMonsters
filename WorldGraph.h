#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class WorldGraph
{
public:
	void addLocation(const std::string& location);

	void connectLocations(
		const std::string& first,
		const std::string& second
	);

	bool areConnected(
		const std::string& first,
		const std::string& second
	) const;

	const std::vector<std::string>& getConnectedLocations(
		const std::string& location
	) const;

private:
	std::unordered_map<
		std::string,
		std::vector<std::string>
	> adjacencyList;
};