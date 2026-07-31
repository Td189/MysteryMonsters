#include <SFML/Graphics.hpp>
#include "PlatformerBossFight.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <random>
#include <vector>
#include "Animation.h"
#include "GameData.h"
#include "TileMap.h"
#include "DataStructures.h"
// ---------------------------------------------------------
// Game constants
// ---------------------------------------------------------

constexpr unsigned int TILE_SIZE = 32;

constexpr unsigned int VISIBLE_TILES_X = 12;
constexpr unsigned int VISIBLE_TILES_Y = 12;

constexpr unsigned int WINDOW_WIDTH =
VISIBLE_TILES_X * TILE_SIZE;

constexpr unsigned int WINDOW_HEIGHT =
VISIBLE_TILES_Y * TILE_SIZE;

constexpr float MOVE_DURATION = 0.16f;
constexpr int BOSS_TILE_ID = 5;

DataStructures DS;

bool bossDefeated = false;
bool bossTileTriggered = false;
bool bossResultHandled = true;

// Player movement helpers


sf::Vector2i getHeldDirection()
{
	if (
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
	{
		return { 0, -1 };
	}

	if (
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
	{
		return { 0, 1 };
	}

	if (
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		return { -1, 0 };
	}

	if (
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		return { 1, 0 };
	}

	return { 0, 0 };
}

sf::Vector2f tileToWorld(sf::Vector2i tilePosition)
{
	return {
		static_cast<float>(
			tilePosition.x * TILE_SIZE + 4
		),
		static_cast<float>(
			tilePosition.y * TILE_SIZE + 4
		)
	};
}

// ---------------------------------------------------------
// Camera helper
// ---------------------------------------------------------

sf::Vector2f clampCameraCenter(
	sf::Vector2f desiredCenter,
	sf::Vector2f cameraSize,
	float mapWidth,
	float mapHeight)
{
	const float halfCameraWidth =
		cameraSize.x / 2.f;

	const float halfCameraHeight =
		cameraSize.y / 2.f;

	if (mapWidth <= cameraSize.x)
	{
		desiredCenter.x = mapWidth / 2.f;
	}
	else
	{
		desiredCenter.x = std::clamp(
			desiredCenter.x,
			halfCameraWidth,
			mapWidth - halfCameraWidth
		);
	}

	if (mapHeight <= cameraSize.y)
	{
		desiredCenter.y = mapHeight / 2.f;
	}
	else
	{
		desiredCenter.y = std::clamp(
			desiredCenter.y,
			halfCameraHeight,
			mapHeight - halfCameraHeight
		);
	}

	return desiredCenter;
}


// Battle-screen drawing helpers


void drawHealthBar(
	sf::RenderWindow& window,
	sf::Vector2f position,
	sf::Vector2f size,
	int currentHealth,
	int maximumHealth)
{
	sf::RectangleShape border(size);

	border.setPosition(position);
	border.setFillColor(
		sf::Color(55, 55, 55)
	);

	window.draw(border);

	float healthPercentage = 0.f;

	if (maximumHealth > 0)
	{
		healthPercentage =
			static_cast<float>(currentHealth) /
			static_cast<float>(maximumHealth);
	}

	healthPercentage = std::clamp(
		healthPercentage,
		0.f,
		1.f
	);

	const float innerWidth =
		std::max(
			0.f,
			(size.x - 4.f) * healthPercentage
		);

	sf::RectangleShape health({
		innerWidth,
		size.y - 4.f
		});

	health.setPosition({
		position.x + 2.f,
		position.y + 2.f
		});

	if (healthPercentage > 0.5f)
	{
		health.setFillColor(
			sf::Color(60, 200, 75)
		);
	}
	else if (healthPercentage > 0.25f)
	{
		health.setFillColor(
			sf::Color(235, 200, 45)
		);
	}
	else
	{
		health.setFillColor(
			sf::Color(220, 60, 60)
		);
	}

	window.draw(health);
}

//----------------------

void drawBattleScreen(
	sf::RenderWindow& window,
	BattleSystem& battleSystem,
	const Animation& playerAnimation,
	const Animation& enemyAnimation)
{
	// Sky/background.
	sf::RectangleShape background({
		static_cast<float>(WINDOW_WIDTH),
		static_cast<float>(WINDOW_HEIGHT)
		});

	background.setPosition({ 0.f, 0.f });
	background.setFillColor(
		sf::Color(150, 210, 235)
	);

	window.draw(background);

	// Grass on the bottom half.
	sf::RectangleShape ground({
		static_cast<float>(WINDOW_WIDTH),
		155.f
		});

	ground.setPosition({
		0.f,
		static_cast<float>(WINDOW_HEIGHT) - 155.f
		});

	ground.setFillColor(
		sf::Color(105, 185, 105)
	);

	window.draw(ground);

	// Enemy platform.
	sf::CircleShape enemyPlatform(60.f);

	enemyPlatform.setScale({ 1.45f, 0.40f });
	enemyPlatform.setPosition({ 238.f, 123.f });
	enemyPlatform.setFillColor(
		sf::Color(90, 145, 90)
	);

	window.draw(enemyPlatform);

	// Player platform.
	sf::CircleShape playerPlatform(68.f);

	playerPlatform.setScale({ 1.45f, 0.40f });
	playerPlatform.setPosition({ 25.f, 238.f });
	playerPlatform.setFillColor(
		sf::Color(80, 135, 80)
	);

	window.draw(playerPlatform);

	// Draw the enemy and player monster animations.
	enemyAnimation.draw(window);
	playerAnimation.draw(window);

	Monster* playerMonster =
		battleSystem.getPlayerMonster();

	const Monster& enemyMonster =
		battleSystem.getEnemyMonster();

	// Enemy information panel.
	sf::RectangleShape enemyPanel({
		175.f,
		52.f
		});

	enemyPanel.setPosition({ 15.f, 20.f });

	enemyPanel.setFillColor(
		sf::Color(245, 245, 225)
	);

	enemyPanel.setOutlineColor(
		sf::Color::Black
	);

	enemyPanel.setOutlineThickness(2.f);

	window.draw(enemyPanel);

	drawHealthBar(
		window,
		{ 30.f, 47.f },
		{ 145.f, 13.f },
		enemyMonster.currentHealth,
		enemyMonster.maximumHealth
	);

	// Player information panel.
	sf::RectangleShape playerPanel({
		175.f,
		52.f
		});

	playerPanel.setPosition({
		194.f,
		202.f
		});

	playerPanel.setFillColor(
		sf::Color(245, 245, 225)
	);

	playerPanel.setOutlineColor(
		sf::Color::Black
	);

	playerPanel.setOutlineThickness(2.f);

	window.draw(playerPanel);

	if (playerMonster != nullptr)
	{
		drawHealthBar(
			window,
			{ 209.f, 229.f },
			{ 145.f, 13.f },
			playerMonster->currentHealth,
			playerMonster->maximumHealth
		);
	}

	// Tackle button.
	sf::RectangleShape tackleButton({
		170.f,
		44.f
		});

	tackleButton.setPosition({
		12.f,
		328.f
		});

	tackleButton.setFillColor(
		sf::Color(220, 220, 210)
	);

	tackleButton.setOutlineColor(
		sf::Color::Black
	);

	tackleButton.setOutlineThickness(2.f);

	window.draw(tackleButton);

	// Flame Paw button.
	sf::RectangleShape flamePawButton({
		170.f,
		44.f
		});

	flamePawButton.setPosition({
		202.f,
		328.f
		});

	flamePawButton.setFillColor(
		sf::Color(235, 125, 55)
	);

	flamePawButton.setOutlineColor(
		sf::Color::Black
	);

	flamePawButton.setOutlineThickness(2.f);

	window.draw(flamePawButton);

	/*
		The buttons currently do not contain text because no font
		is being loaded yet.

		Left button:
			Number 1 = Tackle

		Right button:
			Number 2 = Flame Paw
	*/
}

//----------------------

struct MonsterAnimationInfo
{
	std::string folder;
	std::size_t frameCount = 0;
};

std::string toLowerCopy(std::string text)
{
	std::transform(
		text.begin(),
		text.end(),
		text.begin(),
		[](unsigned char character)
		{
			return static_cast<char>(
				std::tolower(character)
				);
		}
	);

	return text;
}

std::optional<MonsterAnimationInfo> getMonsterAnimationInfo(
	const Monster& monster)
{
	const std::string monsterName =
		toLowerCopy(monster.name);

	if (monsterName == "stuffight")
	{
		return MonsterAnimationInfo{
			"assets/sprites/stuffight",
			4
		};
	}

	if (monsterName == "cellicious")
	{
		return MonsterAnimationInfo{
			"assets/sprites/cellicious",
			4
		};
	}

	if (monsterName == "chititan")
	{
		return MonsterAnimationInfo{
			"assets/sprites/chititan",
			4
		};
	}

	if (monsterName == "doodlebob")
	{
		return MonsterAnimationInfo{
			"assets/sprites/doodleBob",
			2
		};
	}
	return std::nullopt;
}

//----------------------

bool loadMonsterAnimation(
	Animation& animation,
	const Monster& monster,
	sf::Vector2f position,
	sf::Vector2f displaySize)
{
	const std::optional<MonsterAnimationInfo> animationInfo =
		getMonsterAnimationInfo(monster);

	if (!animationInfo.has_value())
	{
		std::cerr
			<< "No animation-folder mapping exists for monster: "
			<< monster.name
			<< ".\n";
		return false;
	}

	const bool loaded = animation.loadNumberedFrames(
		animationInfo->folder,
		"front",
		animationInfo->frameCount,
		0.18f
	);

	if (!loaded)
	{
		std::cerr
			<< "Failed to load animation for "
			<< monster.name
			<< " from folder "
			<< animationInfo->folder
			<< ".\n";
		return false;
	}

	animation.setDisplaySize(displaySize);
	animation.setPosition(position);
	animation.reset();
	return true;
}

//----------------------

Monster createMonster(
	const std::string& name,
	int level,
	int currentHealth,
	int maximumHealth,
	int attack,
	int defense,
	int speed)
{
	return Monster{
		name,
		level,
		currentHealth,
		maximumHealth,
		attack,
		defense,
		speed
	};
}

//----------------------

Monster createRandomWildMonster()
{
	static std::random_device randomDevice;
	static std::mt19937 randomEngine(randomDevice());

	std::uniform_int_distribution<int> levelDistribution(2, 6);
	const int level = levelDistribution(randomEngine);

	const std::vector<Monster> possibleMonsters =
	{
		createMonster(
			"Stuffight",
			level,
			18,
			18,
			6,
			3,
			4
		),

		createMonster(
			"Cellicious",
			level,
			22,
			22,
			8,
			4,
			7
		),

		createMonster(
			"Chititan",
			level,
			21,
			21,
			7,
			6,
			5
		),

		createMonster(
			"DoodleBob",
			level,
			26,
			26,
			9,
			5,
			8
		)
	};

	std::uniform_int_distribution<std::size_t> monsterDistribution(
		0,
		possibleMonsters.size() - 1
	);

	// Keep this while testing DoodleBob.
	//return possibleMonsters[3];

	// Restore random encounters later:
	return possibleMonsters[monsterDistribution(randomEngine)];
}


// ---------------------------------------------------------
// Main
// ---------------------------------------------------------

int main()
{
	GameData gameData;
	TileMap map;
	PlatformerBossFight platformerBossFight;
	Animation playerBattleAnimation;
	Animation enemyBattleAnimation;
	// -----------------------------------------------------
	// Load the map
	// -----------------------------------------------------
	Monster doodleBob{
	"DoodleBob",
	5,
	26,
	26,
	9,
	5,
	8
	};

	gameData.getParty().addMonster(doodleBob);
	if (!map.load(
		R"(C:\Users\gavin\source\repos\MysteryMons\map1.txt)",
		{ TILE_SIZE, TILE_SIZE }))
	{
		std::cerr
			<< "Failed to load map.\n";

		return 1;
	}

	// Missing texture files are replaced by fallback colors.
	map.loadTextures(
		R"(C:\Users\gavin\source\repos\MysteryMons\assets\tiles\grass.png)",
		R"(C:\Users\gavin\source\repos\MysteryMons\assets\tiles\dirt.png)",
		R"(C:\Users\gavin\source\repos\MysteryMons\assets\tiles\water.png)",
		R"(C:\Users\gavin\source\repos\MysteryMons\assets\tiles\tree.png)"
	);

	gameData.getInventory().addItem(
		"capture_orb",
		"Capture Orb",
		"Used to catch wild creatures.",
		5
	);
	std::cout << "    =====KEYS===== \n";
	std::cout << "A,S,W,D & Arrow Keys : Movement\n";
	std::cout << "B : Battle a wild Creature\n";
	std::cout << "1 : Attack 1 during battle\n";
	std::cout << "2 : Attack 2 during battle\n";
	std::cout << "3 : Switch Monster\n";
	std::cout << "C : Throw Orb\n";
	std::cout << "F : Boss Fight\n";
	std::cout << "A,D, Space Bar : Movement during Boss Fight";
	std::cout << "K : Attack during Boss Fight\n";
	std::cout << "Escape : Leave Boss Fight\n";
	std::cout << "O : Use Data Structures" << std::endl;
	// -----------------------------------------------------
	// Create the game window
	// -----------------------------------------------------

	sf::RenderWindow window(
		sf::VideoMode({
			WINDOW_WIDTH,
			WINDOW_HEIGHT
			}),
		"MysteryMons"
	);

	window.setFramerateLimit(60);

	// -----------------------------------------------------
	// Create the map camera
	// -----------------------------------------------------

	sf::View camera(
		sf::FloatRect(
			{ 0.f, 0.f },
			{
				static_cast<float>(WINDOW_WIDTH),
				static_cast<float>(WINDOW_HEIGHT)
			}
		)
	);


	// Create the player placeholder


	sf::RectangleShape player({
		static_cast<float>(TILE_SIZE - 8),
		static_cast<float>(TILE_SIZE - 8)
		});

	player.setFillColor(
		sf::Color::Blue
	);

	// Player starts at column 1, row 1.
	sf::Vector2i playerTile{ 1, 1 };

	sf::Vector2f startPosition =
		tileToWorld(playerTile);

	sf::Vector2f targetPosition =
		startPosition;

	player.setPosition(startPosition);

	bool moving = false;
	float moveProgress = 0.f;

	sf::Clock clock;


	// Main game loop


	while (window.isOpen())
	{


		// Process window and keyboard events


		while (const std::optional event =
			window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
				continue;
			}
			if (platformerBossFight.isActive())
			{
				platformerBossFight.handleEvent(*event);

				// Only stop normal game input while the boss fight
				// actually remains active.
				if (platformerBossFight.isActive())
				{
					continue;
				}
			}
			const auto* keyPressed =
				event->getIf<
				sf::Event::KeyPressed
				>();

			if (keyPressed == nullptr)
			{
				continue;
			}

			BattleSystem& battleSystem =
				gameData.getBattleSystem();


			// Start the platformer boss fight with F


			if (
				keyPressed->code ==
				sf::Keyboard::Key::F &&
				!battleSystem.isActive() &&
				!platformerBossFight.isActive())
			{
				moving = false;
				moveProgress = 0.0f;

				platformerBossFight.start(
					window.getSize()
				);

				std::cout
					<< "Platformer boss fight started.\n"
					<< "A/D = Move, Space = Jump, "
					<< "K = Attack, Escape = Exit.\n";

				// Do not let this same F event reach any
				// overworld or turn-based battle controls.
				continue;
			}

			// Start a test turn-based battle with B

			if (
				keyPressed->code ==
				sf::Keyboard::Key::B &&
				!battleSystem.isActive() &&
				!moving)
			{
				Monster* playerMonster =
					gameData
					.getParty()
					.getMonster(0);

				if (playerMonster == nullptr)
				{
					std::cout
						<< "There is no monster "
						<< "in the player's party.\n";
				}
				else if (
					playerMonster->isFainted())
				{
					std::cout
						<< playerMonster->name
						<< " has fainted and cannot "
						<< "start another battle.\n";
				}
				else
				{
					Monster wildMonster =
						createRandomWildMonster();

					const bool playerAnimationLoaded =
						loadMonsterAnimation(
							playerBattleAnimation,
							*playerMonster,
							{ 45.f, 170.f },
							{ 110.f, 110.f }
						);

					const bool enemyAnimationLoaded =
						loadMonsterAnimation(
							enemyBattleAnimation,
							wildMonster,
							{ 270.f, 65.f },
							{ 95.f, 95.f }
						);

					if (!playerAnimationLoaded)
					{
						std::cerr
							<< "Failed to load the player monster animation.\n";
					}

					if (!enemyAnimationLoaded)
					{
						std::cerr
							<< "Failed to load the wild monster animation.\n";
					}

					battleSystem.startBattle(
						playerMonster,
						wildMonster
					);

					std::cout
						<< "A wild "
						<< wildMonster.name
						<< " appeared at level "
						<< wildMonster.level
						<< "!\n";

					std::cout
						<< "Battle controls:\n"
						<< "1 = Tackle\n"
						<< "2 = Flame Paw\n"
						<< "3 = Switch Monster\n"
						<< "C = Throw Capture Orb\n" << std::endl;
				}

				continue;
			}


			// Battle controls



// Battle controls


			if (battleSystem.isActive())
			{

				// Party-switch selection


				if (
					battleSystem.getState() ==
					BattleState::ChoosingSwitch)
				{
					std::optional<std::size_t> selectedIndex;

					if (
						keyPressed->code ==
						sf::Keyboard::Key::Num1)
					{
						selectedIndex = 0;
					}
					else if (
						keyPressed->code ==
						sf::Keyboard::Key::Num2)
					{
						selectedIndex = 1;
					}
					else if (
						keyPressed->code ==
						sf::Keyboard::Key::Num3)
					{
						selectedIndex = 2;
					}
					else if (
						keyPressed->code ==
						sf::Keyboard::Key::Num4)
					{
						selectedIndex = 3;
					}
					else if (
						keyPressed->code ==
						sf::Keyboard::Key::Num5)
					{
						selectedIndex = 4;
					}
					else if (
						keyPressed->code ==
						sf::Keyboard::Key::Num6)
					{
						selectedIndex = 5;
					}
					else if (
						keyPressed->code ==
						sf::Keyboard::Key::Escape)
					{
						battleSystem.cancelSwitch();
					}

					if (selectedIndex.has_value())
					{
						const SwitchResult switchResult =
							battleSystem.switchMonster(
								gameData.getParty(),
								*selectedIndex
							);

						if (switchResult == SwitchResult::Switched)
						{
							Monster* activeMonster =
								battleSystem.getPlayerMonster();

							if (activeMonster != nullptr)
							{
								loadMonsterAnimation(
									playerBattleAnimation,
									*activeMonster,
									{ 45.f, 170.f },
									{ 110.f, 110.f }
								);
							}
						}
					}

					continue;
				}


				// Normal battle actions


				if (
					keyPressed->code ==
					sf::Keyboard::Key::Num1)
				{
					battleSystem.chooseAttack(
						"Tackle"
					);
				}
				else if (
					keyPressed->code ==
					sf::Keyboard::Key::Num2)
				{
					battleSystem.chooseAttack(
						"Flame Paw"
					);
				}
				else if (
					keyPressed->code ==
					sf::Keyboard::Key::Num3)
				{
					battleSystem.beginSwitch(
						gameData.getParty()
					);
				}
				else if (
					keyPressed->code ==
					sf::Keyboard::Key::C)
				{
					const CaptureResult result =
						battleSystem.attemptCapture(
							gameData.getParty(),
							gameData.getInventory()
						);

					switch (result)
					{
					case CaptureResult::Caught:
						std::cout
							<< "The monster joined your party!\n";
						break;

					case CaptureResult::Failed:
						std::cout
							<< "The monster broke free!\n";
						break;

					case CaptureResult::PartyFull:
						std::cout
							<< "Your party is full!\n";
						break;

					case CaptureResult::NoCaptureItems:
						std::cout
							<< "You don't have any Capture Orbs!\n";
						break;

					case CaptureResult::NotInBattle:
						break;
					}
				}


				continue;
			}


			// Map-menu controls


			if (
				keyPressed->code ==
				sf::Keyboard::Key::I)
			{
				gameData
					.getMenus()
					.open(MenuType::Inventory);

				std::cout
					<< "Inventory opened.\n";
			}
			else if (
				keyPressed->code ==
				sf::Keyboard::Key::Escape)
			{
				gameData
					.getMenus()
					.close();

				std::cout
					<< "Current menu closed.\n";
			}
			else if (
				keyPressed->code ==
				sf::Keyboard::Key::P)
			{
				std::cout
					<< "Potions: "
					<< gameData
					.getInventory()
					.getQuantity("potion")
					<< '\n';
			}
			else if (
				keyPressed->code == sf::Keyboard::Key::O
				) {
				DS.structures();
			}
		}


		// Calculate frame time


		const float deltaTime =
			clock.restart().asSeconds();

		playerBattleAnimation.update(deltaTime);
		enemyBattleAnimation.update(deltaTime);

		BattleSystem& battleSystem =
			gameData.getBattleSystem();


		// Update the active game mode


		if (platformerBossFight.isActive())
		{

			platformerBossFight.update(deltaTime);
		}
		else
		{

			// Begin player movement


			if (
				!moving &&
				!battleSystem.isActive())
			{
				const sf::Vector2i direction =
					getHeldDirection();

				if (
					direction !=
					sf::Vector2i{ 0, 0 })
				{
					const sf::Vector2i nextTile =
						playerTile + direction;

					if (
						!map.isBlocked(
							nextTile.x,
							nextTile.y))
					{
						startPosition =
							player.getPosition();

						playerTile = nextTile;

						targetPosition =
							tileToWorld(playerTile);

						moveProgress = 0.f;
						moving = true;
					}
				}
			}


			// Animate player movement


			if (moving)
			{
				moveProgress +=
					deltaTime / MOVE_DURATION;

				if (moveProgress >= 1.f)
				{
					moveProgress = 1.f;
					moving = false;

					const int tile =
						map.getTile(
							playerTile.x,
							playerTile.y);

					// Healing tile.
					if (tile == 4)
					{
						gameData
							.getParty()
							.healAll();

						std::cout
							<< "Your party was fully healed!\n";
					}

					// Platformer boss tile.
					if (
						tile == BOSS_TILE_ID &&
						!bossDefeated &&
						!bossTileTriggered)
					{
						bossTileTriggered = true;
						bossResultHandled = false;

						platformerBossFight.start(
							window.getSize()
						);

						std::cout
							<< "\nThe platformer boss fight begins!\n";
					}
					else if (tile != BOSS_TILE_ID)
					{

						bossTileTriggered = false;
					}
				}

				const sf::Vector2f newPosition =
					startPosition +
					(
						targetPosition -
						startPosition
						) * moveProgress;

				player.setPosition(newPosition);
			}


			// Update map camera


			const float mapPixelWidth =
				static_cast<float>(
					map.getWidth() * TILE_SIZE
					);

			const float mapPixelHeight =
				static_cast<float>(
					map.getHeight() * TILE_SIZE
					);

			const sf::Vector2f playerCenter =
				player.getPosition() +
				player.getSize() / 2.f;

			const sf::Vector2f cameraCenter =
				clampCameraCenter(
					playerCenter,
					camera.getSize(),
					mapPixelWidth,
					mapPixelHeight
				);

			camera.setCenter(cameraCenter);
		}


		// Handle a completed platformer boss fight once


		if (
			!platformerBossFight.isActive() &&
			!bossResultHandled)
		{
			bossResultHandled = true;

			switch (platformerBossFight.getResult())
			{
			case PlatformerBossResult::PlayerWon:
				bossDefeated = true;
				gameData.getInventory().addItem(
					"capture_orb",
					"Capture Orb",
					"Used to catch wild creatures.",
					5
				);
				std::cout
					<< "You defeated the platformer boss!\n"
					<< "You've been awarded 5 Capture Orbs!" << std::endl;
				break;

			case PlatformerBossResult::PlayerLost:
				std::cout
					<< "The boss defeated you.\n"
					<< "Step away and return to try again.\n";
				break;

			case PlatformerBossResult::Cancelled:
				std::cout
					<< "Boss fight cancelled.\n";
				break;

			case PlatformerBossResult::Running:
				break;
			}
		}


		// Draw the current game screen


		window.clear(sf::Color::Black);

		if (platformerBossFight.isActive())
		{
			// The platformer fight uses fixed window coordinates.
			window.setView(window.getDefaultView());
			platformerBossFight.draw(window);
		}
		else if (battleSystem.isActive())
		{
			// The turn-based battle also uses window coordinates.
			window.setView(window.getDefaultView());
			drawBattleScreen(
				window,
				battleSystem,
				playerBattleAnimation,
				enemyBattleAnimation
			);
		}
		else
		{
			// Restore the scrolling map camera in the overworld.
			window.setView(camera);
			window.draw(map);
			window.draw(player);
		}

		window.display();
	}

	return 0;
}