#pragma once

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class Animation
{
public:
	bool loadNumberedFrames(
		const std::string& folder,
		const std::string& prefix,
		std::size_t frameCount,
		float secondsPerFrame = 0.15f
	);

	void update(float deltaTime);
	void draw(sf::RenderWindow& window) const;

	void setPosition(sf::Vector2f position);
	void setDisplaySize(sf::Vector2f size);

	void reset();
	bool isLoaded() const;

private:
	void applyCurrentFrame();

	std::vector<sf::Texture> frames;
	std::unique_ptr<sf::Sprite> sprite;

	std::size_t currentFrame = 0;
	float frameTimer = 0.f;
	float frameDuration = 0.15f;

	sf::Vector2f displaySize{ 64.f, 64.f };
};