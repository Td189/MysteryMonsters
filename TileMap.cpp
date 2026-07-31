#include "TileMap.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

sf::Color TileMap::getFallbackColor(int tileId) const
{
	switch (tileId)
	{
	case 0:
		return sf::Color(70, 180, 70);   // Grass

	case 1:
		return sf::Color(150, 100, 55);  // Dirt

	case 2:
		return sf::Color(50, 130, 220);  // Water

	case 3:
		return sf::Color(30, 100, 40);   // Tree

	case 4:
		return sf::Color(255, 100, 220); // Healing tile

	default:
		std::cerr
			<< "Unknown tile ID: "
			<< tileId
			<< '\n';

		return sf::Color::Magenta;
	}
}

bool TileMap::load(
	const std::string& mapFilename,
	sf::Vector2u newTileSize)
{
	tileSize = newTileSize;

	const std::filesystem::path fullMapPath =
		std::filesystem::absolute(mapFilename);

	std::cout
		<< "Loading map from:\n"
		<< fullMapPath
		<< '\n';

	std::ifstream mapFile(fullMapPath);

	if (!mapFile.is_open())
	{
		std::cerr
			<< "ERROR: Could not open map file.\n";

		return false;
	}

	std::string firstLine;

	if (!std::getline(mapFile, firstLine))
	{
		std::cerr
			<< "ERROR: Could not read map dimensions.\n";

		return false;
	}

	// Remove a UTF-8 byte-order mark.
	if (
		firstLine.size() >= 3 &&
		static_cast<unsigned char>(firstLine[0]) == 0xEF &&
		static_cast<unsigned char>(firstLine[1]) == 0xBB &&
		static_cast<unsigned char>(firstLine[2]) == 0xBF)
	{
		firstLine.erase(0, 3);
	}

	// Remove the carriage return sometimes used by Windows.
	if (
		!firstLine.empty() &&
		firstLine.back() == '\r')
	{
		firstLine.pop_back();
	}

	std::istringstream dimensionStream(firstLine);

	if (!(dimensionStream >> mapWidth >> mapHeight))
	{
		std::cerr
			<< "ERROR: Could not parse map dimensions.\n"
			<< "First line was: ["
			<< firstLine
			<< "]\n";

		return false;
	}

	if (mapWidth <= 0 || mapHeight <= 0)
	{
		std::cerr
			<< "ERROR: Invalid map dimensions: "
			<< mapWidth
			<< " x "
			<< mapHeight
			<< '\n';

		return false;
	}

	tileIds.assign(
		static_cast<std::size_t>(mapHeight),
		std::vector<int>(
			static_cast<std::size_t>(mapWidth),
			0
		)
	);

	for (int row = 0; row < mapHeight; ++row)
	{
		for (int column = 0;
			column < mapWidth;
			++column)
		{
			if (!(mapFile >> tileIds[row][column]))
			{
				std::cerr
					<< "ERROR: Missing or invalid tile at row "
					<< row
					<< ", column "
					<< column
					<< ".\n";

				return false;
			}

			const int tileId =
				tileIds[row][column];

			// Valid tile IDs are now 0 through 4.
			if (tileId < 0 || tileId > 4)
			{
				std::cerr
					<< "ERROR: Unknown tile ID "
					<< tileId
					<< " at row "
					<< row
					<< ", column "
					<< column
					<< ".\n";

				return false;
			}
		}
	}

	std::cout
		<< "Map loaded successfully: "
		<< mapWidth
		<< " x "
		<< mapHeight
		<< '\n';

	return true;
}

void TileMap::loadTextures(
	const std::string& grassPath,
	const std::string& dirtPath,
	const std::string& waterPath,
	const std::string& treePath)
{
	const std::array<std::string, 4> paths{
		grassPath,
		dirtPath,
		waterPath,
		treePath
	};

	for (std::size_t i = 0; i < paths.size(); ++i)
	{
		textureAvailable[i] =
			tileTextures[i].loadFromFile(paths[i]);

		if (textureAvailable[i])
		{
			tileTextures[i].setSmooth(false);

			std::cout
				<< "Loaded tile texture: "
				<< paths[i]
				<< '\n';
		}
		else
		{
			std::cerr
				<< "WARNING: Could not load texture: "
				<< paths[i]
				<< "\nUsing fallback color instead.\n";
		}
	}
}

bool TileMap::isBlocked(
	int column,
	int row) const
{
	const int tileId =
		getTile(column, row);

	if (tileId == -1)
	{
		return true;
	}

	return tileId == 2 ||
		tileId == 3;
}

int TileMap::getTile(int x, int y) const
{
	if (
		x < 0 ||
		y < 0 ||
		x >= mapWidth ||
		y >= mapHeight)
	{
		return -1;
	}

	return tileIds[y][x];
}

int TileMap::getWidth() const
{
	return mapWidth;
}

int TileMap::getHeight() const
{
	return mapHeight;
}

void TileMap::draw(
	sf::RenderTarget& target,
	sf::RenderStates states) const
{
	for (int row = 0; row < mapHeight; ++row)
	{
		for (int column = 0; column < mapWidth; ++column)
		{
			const int tileId =
				tileIds[row][column];

			const sf::Vector2f position{
				static_cast<float>(
					column * tileSize.x),
				static_cast<float>(
					row * tileSize.y)
			};

			const bool hasTexture =
				tileId >= 0 &&
				tileId < static_cast<int>(
					textureAvailable.size()) &&
				textureAvailable[tileId];

			if (hasTexture)
			{
				sf::Sprite tileSprite(
					tileTextures[tileId]
				);

				tileSprite.setPosition(position);

				const sf::Vector2u textureSize =
					tileTextures[tileId].getSize();

				if (
					textureSize.x > 0 &&
					textureSize.y > 0)
				{
					tileSprite.setScale({
						static_cast<float>(tileSize.x) /
							static_cast<float>(textureSize.x),

						static_cast<float>(tileSize.y) /
							static_cast<float>(textureSize.y)
						});
				}

				target.draw(tileSprite, states);
			}
			else
			{
				sf::RectangleShape tileShape({
					static_cast<float>(tileSize.x),
					static_cast<float>(tileSize.y)
					});

				tileShape.setPosition(position);

				tileShape.setFillColor(
					getFallbackColor(tileId)
				);

				target.draw(tileShape, states);
			}
		}
	}
}