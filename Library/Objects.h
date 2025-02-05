#pragma once
#include <vector>

#include "BaseLibrary.h"
#include "Levels.h"
#include "AudioPlayer.h"

#define WALL_RENDER '#'
#define PLAYER_RENDER '@'
#define NORMAL_ENEMY_RENDER '0'
#define LIGHT_ENEMY_RENDER 'o'
#define JUGGERNAUT_RENDER 'D'
#define BOSS_RENDER 'B'
#define EMPTY_RENDER ' '
#define BULLET_FORWARD_RENDER '|'
#define BULLET_SIDE_RENDER '-'

namespace GameLibrary
{
	class GameObject
	{
	protected:
		char _render;
		Dot _position;
	public:
		Dot GetPosition() const
		{
			return _position;
		}
		char GetRender() const
		{
			return _render;
		}
	};

	class Damagable
	{
	protected:
		unsigned char _health;
	public:
		void TakeDamage()
		{
			_health--;
		}
		unsigned char GetHealth() const
		{
			return _health;
		}
	};

	class Character : public GameObject, public Damagable
	{
	public:
		void Move(const Dot& velocity, Level& lvl)
		{
			const unsigned char newX = Math::Clamp(_position.GetX() + velocity.GetX(), 0, LEVEL_SIZE_X);
			const unsigned char newY = Math::Clamp(_position.GetY() + velocity.GetY(), 0, LEVEL_SIZE_Y);
			const Dot newPosition = Dot(newX, newY);
			if (lvl.CanMove(newPosition))
			{
				lvl.Swap(_position, newPosition);
				_position = newPosition;
			}
		}
		void Move(const char& x, const char& y, Level& lvl)
		{
			const unsigned char newX = Math::Clamp(_position.GetX() + x, 0, LEVEL_SIZE_X);
			const unsigned char newY = Math::Clamp(_position.GetY() + y, 0, LEVEL_SIZE_Y);
			const Dot newPosition = Dot(newX, newY);
			if (lvl.CanMove(newPosition))
			{
				lvl.Swap(_position, newPosition);
				_position = newPosition;
			}
		}
	};

	class Player : public Character
	{
	public:
		Player()
		{
			_render = PLAYER_RENDER;
			this->_position = Dot(0, 0);
			_health = 3;
		}
		Player(const Dot& dot)
		{
			_render = PLAYER_RENDER;
			_position = dot;
			_health = 3;
		}
		Player(const unsigned char& x, const unsigned char& y)
		{
			_render = PLAYER_RENDER;
			_position = Dot(x, y);
			_health = 3;
		}
	};

	class Bullet : public GameObject
	{
		bool Move(const Dot& newPosition, Level& lvl)
		{
			if (lvl.CanMove(newPosition))
			{
				lvl.Swap(_position, newPosition);
				_position = newPosition;
				return true;
			}
			else
			{
				lvl.SetChar(' ', _position);
				return false;
			}
		}

		void TakeDamageToCharacter(std::vector<Character*>& characterPull)
		{
			for (std::size_t i = 0; i < characterPull.size(); i++)
			{
				if (GetNewPosition() == (*characterPull[i]).GetPosition())
				{
					(*characterPull[i]).TakeDamage();
					return;
				}
			}
		}

	public:
		enum Direction : unsigned char { UP, DOWN, LEFT, RIGHT };
	private:
		Direction _direction;

	public:
		Bullet(const Dot& position, const Direction& dir, Level& lvl)
		{
			_position = position;
			_direction = dir;
			switch (_direction)
			{
			case UP:
			case DOWN:
				_render = BULLET_FORWARD_RENDER;
				break;

			case LEFT:
			case RIGHT:
				_render = BULLET_SIDE_RENDER;
				break;
			}
			lvl.SetChar(_render, position);
		}
		Bullet(const char& x, const char& y, const Direction& dir, Level& lvl)
		{
			_position = Dot(x, y);
			_direction = dir;
			switch (_direction)
			{
			case UP:
			case DOWN:
				_render = BULLET_FORWARD_RENDER;
				break;

			case LEFT:
			case RIGHT:
				_render = BULLET_SIDE_RENDER;
				break;
			}
			lvl.SetChar(_render, x, y);
		}

		Dot GetNewPosition()
		{
			switch (_direction)
			{
			case UP:
				return Dot(_position.GetX(), _position.GetY() - 1);
			case DOWN:
				return Dot(_position.GetX(), _position.GetY() + 1);
			case LEFT:
				return Dot(_position.GetX() - 1, _position.GetY());
			case RIGHT:
				return Dot(_position.GetX() + 1, _position.GetY());
			default:
				return Dot(0, 0);
			}
		}

		bool Update(std::vector<Character*>& characterPull, Level& lvl)
		{
			bool canMove1 = Move(GetNewPosition(), lvl);
			if (!canMove1)
			{
				TakeDamageToCharacter(characterPull);
				return false;
			}

			bool canMove2 = Move(GetNewPosition(), lvl);
			if (!canMove2)
			{
				TakeDamageToCharacter(characterPull);
				return false;
			}

			return true;
		}
	};

	class BaseEnemy : public Character
	{
		bool _canMove = true;
		bool _started = false;
		bool _canShoot = false;

		void AIDirectionChoice(const Dot& playerPosition, const Level& lvl)
		{
			const unsigned char xPos = _position.GetX(), yPos = _position.GetY(),
				playerX = playerPosition.GetX(), playerY = playerPosition.GetY();

			if (playerY > yPos)
			{
				if (lvl.CanMove(xPos, yPos + 1) && _movementDirection != DOWN)
					_movementDirection = UP;
				else if (lvl.CanMove(xPos + 1, yPos) && _movementDirection != LEFT)
					_movementDirection = RIGHT;
				else if (lvl.CanMove(xPos - 1, yPos) && _movementDirection != RIGHT)
					_movementDirection = LEFT;
				else if (lvl.CanMove(xPos, yPos - 1) && _movementDirection != UP)
					_movementDirection = DOWN;

				if (lvl.WaysCount(_position) == 1 && _movementDirection == DOWN)
					_movementDirection = UP;
			}
			else if (playerY < yPos)
			{
				if (lvl.CanMove(xPos, yPos - 1) && _movementDirection != UP)
					_movementDirection = DOWN;
				else if (lvl.CanMove(xPos + 1, yPos) && _movementDirection != LEFT)
					_movementDirection = RIGHT;
				else if (lvl.CanMove(xPos - 1, yPos) && _movementDirection != RIGHT)
					_movementDirection = LEFT;
				else if (lvl.CanMove(xPos, yPos + 1) && _movementDirection != DOWN)
					_movementDirection = UP;

				if (lvl.WaysCount(_position) == 1 && _movementDirection == UP)
					_movementDirection = DOWN;
			}
			else
			{
				if (playerX < xPos)
				{
					if (lvl.CanMove(xPos - 1, yPos) && _movementDirection != RIGHT)
						_movementDirection = LEFT;
					else if (lvl.CanMove(xPos, yPos - 1) && _movementDirection != UP)
						_movementDirection = DOWN;
					else if (lvl.CanMove(xPos, yPos + 1) && _movementDirection != DOWN)
						_movementDirection = UP;
					else if (lvl.CanMove(xPos + 1, yPos) && _movementDirection != LEFT)
						_movementDirection = RIGHT;

					if (lvl.WaysCount(_position) == 1 && _movementDirection == RIGHT)
						_movementDirection = LEFT;
				}
				else if (playerX > xPos)
				{
					if (lvl.CanMove(xPos + 1, yPos) && _movementDirection != LEFT)
						_movementDirection = RIGHT;
					else if (lvl.CanMove(xPos, yPos - 1) && _movementDirection != UP)
						_movementDirection = DOWN;
					else if (lvl.CanMove(xPos, yPos + 1) && _movementDirection != DOWN)
						_movementDirection = UP;
					else if (lvl.CanMove(xPos - 1, yPos) && _movementDirection != RIGHT)
						_movementDirection = LEFT;

					if (lvl.WaysCount(_position) == 1 && _movementDirection == LEFT)
						_movementDirection = RIGHT;
				}
			}
		}

		void Shoot(Player& player, const Bullet::Direction& bulletDir, std::vector<Bullet*>& bulletsPull,
			Level& lvl)
		{
			_canShoot = !_canShoot;
			if (!_canShoot)
				return;

			if (Dot::Distance(_position, player.GetPosition()) == 1.0f)
			{
				player.TakeDamage();
				return;
			}

			Dot bulletPosition;
			switch (bulletDir)
			{
			case GameLibrary::Bullet::Direction::UP:
			{
				bulletPosition = GameLibrary::Dot(_position.GetX(), _position.GetY() - 1);
				break;
			}

			case GameLibrary::Bullet::Direction::DOWN:
			{
				bulletPosition = GameLibrary::Dot(_position.GetX(), _position.GetY() + 1);
				break;
			}

			case GameLibrary::Bullet::Direction::LEFT:
			{
				bulletPosition = GameLibrary::Dot(_position.GetX() - 1, _position.GetY());
				break;
			}

			case GameLibrary::Bullet::Direction::RIGHT:
			{
				bulletPosition = GameLibrary::Dot(_position.GetX() + 1, _position.GetY());
				break;
			}
			}

			Sounds::AudioPlayer::Play(Sounds::SoundsList::SHOOT_SOUND);
			bulletsPull.push_back(new Bullet(bulletPosition, bulletDir, lvl));
		}

	protected:
		enum Direction { UP, DOWN, LEFT, RIGHT, EMPTY };
		Direction _movementDirection = EMPTY;
		unsigned char _speed;

	public:
		void AI(Player& player, Level& lvl, std::vector<Bullet*>& bulletsPull)
		{
			const Dot& playerPosition = player.GetPosition();

			if (!_started)
			{
				AIDirectionChoice(playerPosition, lvl);
				_started = true;
			}

			_canMove = !_canMove;
			if (!_canMove)
				return;

			const float distanceToPlayer = Dot::Distance(_position, playerPosition);
			if (playerPosition.GetX() == _position.GetX())
				{
					// Player below
					if (playerPosition.GetY() > _position.GetY())
					{
						unsigned char checkingY = _position.GetY() + 1;
						for (char i = 0; i < distanceToPlayer - 1; i++)
						{
							if (!lvl.CanMove(_position.GetX(), checkingY))
								goto Movements;
							checkingY++;
						}
						Shoot(player, Bullet::DOWN, bulletsPull, lvl);
					}
					// Player above
					else
					{
						unsigned char checkingY = _position.GetY() - 1;
						for (char i = 0; i < distanceToPlayer - 1; i++)
						{
							if (!lvl.CanMove(_position.GetX(), checkingY))
								goto Movements;
							checkingY--;
						}
						Shoot(player, Bullet::UP, bulletsPull, lvl);
					}
				}
			else if (playerPosition.GetY() == _position.GetY())
				{
					// Player located to the right
					if (playerPosition.GetX() > _position.GetX())
					{
						unsigned char checkingX = _position.GetX() + 1;
						for (char i = 0; i < distanceToPlayer - 1; i++)
						{
							if (!lvl.CanMove(checkingX, _position.GetY()))
								goto Movements;
							checkingX++;
						}
						Shoot(player, Bullet::RIGHT, bulletsPull, lvl);
					}
					// Player located to the left
					else
					{
						unsigned char checkingX = _position.GetX() - 1;
						for (char i = 0; i < distanceToPlayer - 1; i++)
						{
							if (!lvl.CanMove(checkingX, _position.GetY()))
								goto Movements;
							checkingX--;
						}
						Shoot(player, Bullet::LEFT, bulletsPull, lvl);
					}
				}

		Movements:
			const unsigned char xPos = _position.GetX(), yPos = _position.GetY(),
				playerX = playerPosition.GetX(), playerY = _position.GetY();

			switch (_movementDirection)
			{
			case UP:
				if (lvl.CanMove(xPos, yPos + 1))
					Move(0, 1, lvl);
				else
					AIDirectionChoice(playerPosition, lvl);
				break;

			case DOWN:
				if (lvl.CanMove(xPos, yPos - 1))
					Move(0, -1, lvl);
				else
					AIDirectionChoice(playerPosition, lvl);
				break;

			case RIGHT:
				if (lvl.CanMove(xPos + 1, yPos))
					Move(1, 0, lvl);
				else
					AIDirectionChoice(playerPosition, lvl);
				break;

			case LEFT:
				if (lvl.CanMove(xPos - 1, yPos))
					Move(-1, 0, lvl);
				else
					AIDirectionChoice(playerPosition, lvl);
				break;
			}

			if (lvl.WaysCount(_position) >= 2)
				AIDirectionChoice(playerPosition, lvl);
		}

		bool IsAlive() const
		{
			return _health > 0;
		}

		unsigned char GetSpeed() const
		{
			return _speed;
		}
	};

	class NormalEnemy : public BaseEnemy
	{
	public:
		NormalEnemy(const Dot& position, const Dot& playerPosition)
		{
			_position = position;
			_render = NORMAL_ENEMY_RENDER;
			_health = 2;
			_speed = 1;
		}
		NormalEnemy(const unsigned char& x, const unsigned char& y,
			const unsigned char& playerX, const unsigned char& playerY)
		{
			_position = Dot(x, y);
			_render = NORMAL_ENEMY_RENDER;
			_health = 2;
			_speed = 1;
		}
	};

	class LightEnemy : public BaseEnemy
	{
	public:
		LightEnemy(const Dot& position, const Dot& playerPosition)
		{
			_position = position;
			_render = LIGHT_ENEMY_RENDER;
			_health = 1;
			_speed = 2;
		}
		LightEnemy(const unsigned char& x, const unsigned char& y,
			const unsigned char& playerX, const unsigned char& playerY)
		{
			_position = Dot(x, y);
			_render = LIGHT_ENEMY_RENDER;
			_health = 1;
			_speed = 2;
		}
	};

	class Juggernaut : public BaseEnemy
	{
	public:
		Juggernaut(const Dot& position, const Dot& playerPosition)
		{
			_position = position;
			_render = JUGGERNAUT_RENDER;
			_health = 5;
			_speed = 1;
		}
		Juggernaut(const unsigned char& x, const unsigned char& y,
			const unsigned char& playerX, const unsigned char& playerY)
		{
			_position = Dot(x, y);
			_render = JUGGERNAUT_RENDER;
			_health = 5;
			_speed = 1;
		}
	};

	class Boss : public BaseEnemy
	{
	public:
		Boss(const Dot& position, const Dot& playerPosition)
		{
			_position = position;
			_render = BOSS_RENDER;
			_health = 10;
			_speed = 2;
		}
		Boss(const unsigned char& x, const unsigned char& y,
			const unsigned char& playerX, const unsigned char& playerY)
		{
			_position = Dot(x, y);
			_render = BOSS_RENDER;
			_health = 10;
			_speed = 2;
		}
	};
}
