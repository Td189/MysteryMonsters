#include "Animation.h"

#include <filesystem>
#include <iostream>
#include <utility>

// ---------------------------------------------------------
// Load sequentially numbered animation frames.
//
// Example:
// folder = "photos/EmberFox"
// prefix = "back"
// frameCount = 4
//
// Loads:
// photos/EmberFox/back0.png
// photos/EmberFox/back1.png
// photos/EmberFox/back2.png
// photos/EmberFox/back3.png
// ---------------------------------------------------------
bool Animation::loadNumberedFrames(
	const std::string& folder,
	const std::string& prefix,
	std::size_t frameCount,
	float secondsPerFrame)
{
	// Remove any previously loaded animation.
	frames.clear();
	sprite.reset();

	currentFrame = 0;
	frameTimer = 0.f;

	if (frameCount == 0)
	{
		std::cerr
			<< "Animation error: frame count cannot be zero.\n";

		return false;
	}

	if (secondsPerFrame <= 0.f)
	{
		std::cerr
			<< "Animation error: frame duration must be positive.\n";

		return false;
	}

	frameDuration = secondsPerFrame;

	// Prevent the vector from relocating textures while loading.
	frames.reserve(frameCount);

	for (std::size_t index = 0; index < frameCount; ++index)
	{
		const std::filesystem::path filePath =
			std::filesystem::path(folder) /
			(
				prefix +
				std::to_string(index) +
				".png"
				);

		sf::Texture texture;

		if (!texture.loadFromFile(filePath.string()))
		{
			std::cerr
				<< "Failed to load animation frame: "
				<< filePath.string()
				<< '\n';

			frames.clear();
			sprite.reset();

			return false;
		}

		// Prevent blurry filtering for pixel-art sprites.
		texture.setSmooth(false);

		frames.push_back(std::move(texture));
	}

	// SFML 3 sprites require a texture when created.
	sprite = std::make_unique<sf::Sprite>(frames[0]);

	applyCurrentFrame();

	std::cout
		<< "Loaded "
		<< frames.size()
		<< " animation frames from "
		<< folder
		<< '\n';

	return true;
}

// ---------------------------------------------------------
// Advance the animation according to elapsed frame time.
// ---------------------------------------------------------
void Animation::update(float deltaTime)
{
	if (
		frames.empty() ||
		sprite == nullptr ||
		frames.size() <= 1 ||
		deltaTime <= 0.f)
	{
		return;
	}

	frameTimer += deltaTime;

	// Using while instead of if prevents skipped timing when a frame
	// takes longer than the animation's frame duration.
	while (frameTimer >= frameDuration)
	{
		frameTimer -= frameDuration;

		currentFrame =
			(currentFrame + 1) % frames.size();

		applyCurrentFrame();
	}
}

// ---------------------------------------------------------
// Draw the current animation frame.
// ---------------------------------------------------------
void Animation::draw(sf::RenderWindow& window) const
{
	if (sprite == nullptr || frames.empty())
	{
		return;
	}

	window.draw(*sprite);
}

// ---------------------------------------------------------
// Change the sprite's screen position.
// ---------------------------------------------------------
void Animation::setPosition(sf::Vector2f position)
{
	if (sprite == nullptr)
	{
		return;
	}

	sprite->setPosition(position);
}

// ---------------------------------------------------------
// Scale every frame to the requested display dimensions.
// ---------------------------------------------------------
void Animation::setDisplaySize(sf::Vector2f size)
{
	displaySize = size;

	if (sprite == nullptr || frames.empty())
	{
		return;
	}

	const sf::Vector2u textureSize =
		frames[currentFrame].getSize();

	if (textureSize.x == 0 || textureSize.y == 0)
	{
		return;
	}

	const float scaleX =
		displaySize.x /
		static_cast<float>(textureSize.x);

	const float scaleY =
		displaySize.y /
		static_cast<float>(textureSize.y);

	sprite->setScale({ scaleX, scaleY });
}

// ---------------------------------------------------------
// Return the animation to its first frame.
// ---------------------------------------------------------
void Animation::reset()
{
	currentFrame = 0;
	frameTimer = 0.f;

	if (sprite != nullptr && !frames.empty())
	{
		applyCurrentFrame();
	}
}

// ---------------------------------------------------------
// Check whether frames were successfully loaded.
// ---------------------------------------------------------
bool Animation::isLoaded() const
{
	return !frames.empty() && sprite != nullptr;
}

// ---------------------------------------------------------
// Apply the current texture while preserving position and
// recalculating the requested display size.
// ---------------------------------------------------------
void Animation::applyCurrentFrame()
{
	if (
		sprite == nullptr ||
		frames.empty() ||
		currentFrame >= frames.size())
	{
		return;
	}

	sprite->setTexture(
		frames[currentFrame],
		true
	);

	const sf::Vector2u textureSize =
		frames[currentFrame].getSize();

	if (textureSize.x == 0 || textureSize.y == 0)
	{
		return;
	}

	const float scaleX =
		displaySize.x /
		static_cast<float>(textureSize.x);

	const float scaleY =
		displaySize.y /
		static_cast<float>(textureSize.y);

	sprite->setScale({ scaleX, scaleY });
}