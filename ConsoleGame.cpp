#include <iostream>
#include <conio.h>
#include <vector>
#include <Windows.h>
#include <shellapi.h>

#include "Library/BaseLibrary.h"
#include "Library/Objects.h"
#include "Library/Levels.h"
#include "Library/Input.h"
#include "Library/AudioPlayer.h"
#include "Library/LevelsList.h"
#include "Library/UI.h"

bool isPaused = false;

const GameLibrary::Level* levels[] = {
	&(GameLibrary::Levels::TUTORIAL),
	&(GameLibrary::Levels::DUNGEON),
	&(GameLibrary::Levels::OST_FRONT),
	&(GameLibrary::Levels::CLAUSTROPHOBIC),
	&(GameLibrary::Levels::MIRROR),
	&(GameLibrary::Levels::ARENA),
	&(GameLibrary::Levels::FABULOUS_TAIGA),
	&(GameLibrary::Levels::RACING_TRACK),
	&(GameLibrary::Levels::DAMNED_MANSION),
	&(GameLibrary::Levels::THE_DEATH_CORRIDOR)
};

static void PlayerShoot(const GameLibrary::Bullet::Direction& direction,
	const GameLibrary::Dot& playerPosition, GameLibrary::Level& level,
	std::vector<GameLibrary::Bullet*>& bulletsPull)
{
	GameLibrary::Dot bulletPosition;

	switch (direction)
	{
	case GameLibrary::Bullet::Direction::UP:
	{
		bulletPosition = GameLibrary::Dot(playerPosition.GetX(), playerPosition.GetY() - 1);
		break;
	}

	case GameLibrary::Bullet::Direction::DOWN:
	{
		bulletPosition = GameLibrary::Dot(playerPosition.GetX(), playerPosition.GetY() + 1);
		break;
	}

	case GameLibrary::Bullet::Direction::LEFT:
	{
		bulletPosition = GameLibrary::Dot(playerPosition.GetX() - 1, playerPosition.GetY());
		break;
	}

	case GameLibrary::Bullet::Direction::RIGHT:
	{
		bulletPosition = GameLibrary::Dot(playerPosition.GetX() + 1, playerPosition.GetY());
		break;
	}
	}

	if (level.CanMove(bulletPosition))
	{
		GameLibrary::Sounds::AudioPlayer::Play(GameLibrary::Sounds::SoundsList::SHOOT_SOUND);
		GameLibrary::Bullet* bullet = new GameLibrary::Bullet(bulletPosition, direction, level);
		bulletsPull.push_back(bullet);
	}
}

static void Play(GameLibrary::Level level);
static void Menu();
static void LevelsMenu();

int main()
{
	Menu();
}

static void Menu()
{
	GameLibrary::Sleep(1.0f);

	bool exited = false;

	GameLibrary::UI::Menu menu = GameLibrary::UI::Menu("---Arcadium---");
	menu.AddVariant(GameLibrary::UI::MenuVariant("Play", [&exited]() { LevelsMenu(); exited = true; }, 10, 2));
	menu.AddVariant(GameLibrary::UI::MenuVariant("Exit", []() { exit(0); }, 10, 3));
	menu.AddVariant(GameLibrary::UI::MenuVariant("GitHub", []() 
		{ ShellExecute(0, 0, L"https://github.com/BlackDragon3129", 0, 0, SW_SHOW); }, 9, 4));

	menu.Draw();

	while (true)
	{
		if (_kbhit())
		{
			switch (_getch())
			{
			case 'w':
			case 'W':
			case GameLibrary::KeyCods::UP_ARROW:
				menu.MoveVariant(-1);
				break;

			case 's':
			case 'S':
			case GameLibrary::KeyCods::DOWN_ARROW:
				menu.MoveVariant(1);
				break;

			case GameLibrary::KeyCods::ENTER:
				menu.VariantAct();
				if (exited)
					return;
				break;

			case GameLibrary::KeyCods::ESCAPE:
				exit(0);
			}
		}
	}
}

static void LevelsMenu()
{
	bool exited = false;

	GameLibrary::UI::Menu menu = GameLibrary::UI::Menu("Choose a level:");

	for (unsigned char i = 0; i < GameLibrary::ArraySize(levels); i++)
	{
		const GameLibrary::Level* currentLevel = levels[i];

		menu.AddVariant(GameLibrary::UI::MenuVariant((*currentLevel).GetName(),
			[currentLevel]() { Play(*currentLevel); }, 5, i + 2));
	}
	menu.AddVariant(GameLibrary::UI::MenuVariant("back", [&exited]() { Menu(); exited = true; }, 5,
		(unsigned char)GameLibrary::ArraySize(levels) + 3));

	menu.Draw();

	while (true)
	{
		if (_kbhit())
		{
			switch (_getch())
			{
			case 'w':
			case 'W':
			case GameLibrary::KeyCods::UP_ARROW:
				menu.MoveVariant(-1);
				break;

			case 's':
			case 'S':
			case GameLibrary::KeyCods::DOWN_ARROW:
				menu.MoveVariant(1);
				break;

			case GameLibrary::KeyCods::ENTER:
				menu.VariantAct();
				if (exited)
					return;
				break;

			case GameLibrary::KeyCods::ESCAPE:
				GameLibrary::Sounds::AudioPlayer::Play(GameLibrary::Sounds::SoundsList::MENU_SWICTH_SOUND);
				Menu();
				return;
			}
		}
	}
}

static void Play(GameLibrary::Level level)
{
	std::vector<GameLibrary::BaseEnemy*> enemiesPull;
	std::vector<GameLibrary::Character*> characterPull;
	std::vector<GameLibrary::Bullet*> bulletsPull;

	GameLibrary::Player player;

	std::string* levelFilling = level.GetFilling();
	for (char y = 0; y < LEVEL_SIZE_Y; y++)
	{
		for (char x = 0; x < LEVEL_SIZE_X; x++)
		{
			if (levelFilling[y][x] == PLAYER_RENDER)
			{
				player = GameLibrary::Player(x, y);
			}
		}
	}

	characterPull.push_back(&player);

	for (char y = 0; y < LEVEL_SIZE_Y; y++)
	{
		for (char x = 0; x < LEVEL_SIZE_X; x++)
		{
			switch (levelFilling[y][x])
			{
			case NORMAL_ENEMY_RENDER:
			{
				GameLibrary::NormalEnemy* enemy =
					new GameLibrary::NormalEnemy(GameLibrary::Dot(x, y), player.GetPosition());
				enemiesPull.push_back(enemy);
				characterPull.push_back(enemy);
				break;
			}

			case LIGHT_ENEMY_RENDER:
			{
				GameLibrary::LightEnemy* enemy =
					new GameLibrary::LightEnemy(GameLibrary::Dot(x, y), player.GetPosition());
				enemiesPull.push_back(enemy);
				characterPull.push_back(enemy);
				break;
			}

			case JUGGERNAUT_RENDER:
			{
				GameLibrary::Juggernaut* enemy =
					new GameLibrary::Juggernaut(GameLibrary::Dot(x, y), player.GetPosition());
				enemiesPull.push_back(enemy);
				characterPull.push_back(enemy);
				break;
			}

			case BOSS_RENDER:
			{
				GameLibrary::Boss* enemy =
					new GameLibrary::Boss(GameLibrary::Dot(x, y), player.GetPosition());
				enemiesPull.push_back(enemy);
				characterPull.push_back(enemy);
				break;
			}
			}
		}
	}

	while (true)
	{
		if (!isPaused)
		{
			if (_kbhit())
			{
				switch (_getch())
				{
				case 'w':
				case 'W':
					player.Move(0, -1, level);
					break;

				case 's':
				case 'S':
					player.Move(0, 1, level);
					break;

				case 'a':
				case 'A':
					player.Move(-1, 0, level);
					break;

				case 'd':
				case 'D':
					player.Move(1, 0, level);
					break;

				case GameLibrary::KeyCods::UP_ARROW:
					PlayerShoot(GameLibrary::Bullet::Direction::UP, player.GetPosition(), level, bulletsPull);
					break;
				case GameLibrary::KeyCods::DOWN_ARROW:
					PlayerShoot(GameLibrary::Bullet::Direction::DOWN, player.GetPosition(), level, bulletsPull);
					break;
				case GameLibrary::KeyCods::RIGHT_ARROW:
					PlayerShoot(GameLibrary::Bullet::Direction::RIGHT, player.GetPosition(), level, bulletsPull);
					break;
				case GameLibrary::KeyCods::LEFT_ARROW:
					PlayerShoot(GameLibrary::Bullet::Direction::LEFT, player.GetPosition(), level, bulletsPull);
					break;

				case 'p':
					isPaused = !isPaused;
					break;
				case GameLibrary::KeyCods::ESCAPE:
					Menu();
					return;
				}
			}

			for (std::size_t i = 0; i < bulletsPull.size(); i++)
			{
				const bool canUpdate = (*bulletsPull[i]).Update(characterPull, level);
				if (!canUpdate)
					bulletsPull.erase(bulletsPull.begin() + i);
			}

			for (std::size_t i = 0; i < enemiesPull.size(); i++)
			{
				GameLibrary::BaseEnemy* currentEnemy = enemiesPull[i];
				if (!(*currentEnemy).IsAlive())
				{
					level.SetChar(' ', (*currentEnemy).GetPosition());
					enemiesPull.erase(enemiesPull.begin() + i);
					i--;
					continue;
				}
				for (unsigned char j = 0; j < (*currentEnemy).GetSpeed(); j++)
					(*currentEnemy).AI(player, level, bulletsPull);
			}

			level.Draw();

			if (player.GetHealth() == 0)
			{
				GameLibrary::Sounds::AudioPlayer::Play(GameLibrary::Sounds::SoundsList::DEATH_SOUND);
				GameLibrary::Sleep(1.0f);
				Menu();
				return;
			}
			if (enemiesPull.size() == 0)
			{
				GameLibrary::Sounds::AudioPlayer::Play(GameLibrary::Sounds::SoundsList::WIN_SOUND);
				GameLibrary::Sleep(1.0f);
				Menu();
				return;
			}

			std::cout << std::endl;
			std::cout << "Health: " << (unsigned short)player.GetHealth();

			GameLibrary::Sleep(0.05f);
			system("cls");
		}
		else
		{
			std::cout << "Pause...";
			if (_getch() == 'p')
				isPaused = !isPaused;
			else
				system("cls");
		}
	}
}