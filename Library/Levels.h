#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "BaseLibrary.h"

namespace GameLibrary
{
	class Level
	{
		const char* _name;
		std::string _filling[LEVEL_SIZE_Y];

	public:
		Level(const char* name, const std::string& filePath)
		{
			_name = name;

			std::ifstream level(filePath);
			if (level.is_open())
			{
				std::string line;
				int yPos = 0;

				while (std::getline(level, line))
				{
					_filling[yPos] = line;
					std::cout << line.length();
					yPos++;
				}
			}
		}

		void Draw() const
		{
			for (char i = 0; i < LEVEL_SIZE_Y; i++)
				std::cout << _filling[i] << std::endl;
		}

		bool CanMove(const Dot& newDot) const
		{
			try
			{
				return _filling[newDot.GetY()][newDot.GetX()] == ' ';
			}
			catch (std::out_of_range)
			{
				return false;
			}
		}
		bool CanMove(const char& x, const char& y) const
		{
			try
			{
				return _filling[y][x] == ' ';
			}
			catch (std::out_of_range)
			{
				return false;
			}
		}

		void Swap(const Dot& oldDot, const Dot& newDot)
		{
			const unsigned short oldX = oldDot.GetX(), oldY = oldDot.GetY(),
								 newX = newDot.GetX(), newY = newDot.GetY();

			_filling[newY][newX] = _filling[oldY][oldX];
			_filling[oldY][oldX] = ' ';
		}
		void Swap(const unsigned short& oldX, const unsigned short& oldY,
				  const unsigned short& newX, const unsigned short& newY)
		{
			_filling[newY][newX] = _filling[oldY].at(oldX);
			_filling[oldY][oldX] = ' ';
		}

		std::string* GetFilling()
		{
			return _filling;
		}

		const char* GetName() const
		{
			return _name;
		}

		unsigned char WaysCount(const Dot& position) const
		{
			const unsigned char x = position.GetX(), y = position.GetY();

			unsigned char ways = 0;

			ways += CanMove(x + 1, y) ? 1 : 0;
			ways += CanMove(x - 1, y) ? 1 : 0;
			ways += CanMove(x, y + 1) ? 1 : 0;
			ways += CanMove(x, y - 1) ? 1 : 0;

			return ways;
		}
		unsigned char WaysCount(const unsigned char& x, const unsigned char& y) const
		{
			unsigned char ways = 0;

			ways += CanMove(x + 1, y) ? 1 : 0;
			ways += CanMove(x - 1, y) ? 1 : 0;
			ways += CanMove(x, y + 1) ? 1 : 0;
			ways += CanMove(x, y - 1) ? 1 : 0;

			return ways;
		}

		void SetChar(const char& newChar, const Dot& position)
		{
			_filling[position.GetY()][position.GetX()] = newChar;
		}
		void SetChar(const char& newChar, const unsigned char& x, const unsigned char& y)
		{
			_filling[y][x] = newChar;
		}
	};
}
