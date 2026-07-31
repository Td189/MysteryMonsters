#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <string>
#include <vector>

class TileMap : public sf::Drawable
{
public:
	bool load(
		const std::string& mapFilename,
		sf::Vector2u tileSize
	);

	void loadTextures(
		const std::string& grassPath,
		const std::string& dirtPath,
		const std::string& waterPath,
		const std::string& treePath
	);

	bool isBlocked(int column, int row) const;

	int getWidth() const;
	int getHeight() const;
	int getTile(int x, int y) const;
private:
	sf::Color getFallbackColor(int tileId) const;

	void draw(
		sf::RenderTarget& target,
		sf::RenderStates states
	) const override;

	std::vector<std::vector<int>> tileIds;

	std::array<sf::Texture, 4> tileTextures;

	// Tracks which images loaded successfully.
	std::array<bool, 4> textureAvailable{
		false,
		false,
		false,
		false
	};

	sf::Vector2u tileSize{ 32, 32 };

	int mapWidth = 0;
	int mapHeight = 0;
};