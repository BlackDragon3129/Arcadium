#pragma once
#include <math.h>
#include <chrono>
#include <thread>
#include <functional>

#define LEVEL_SIZE_X 90
#define LEVEL_SIZE_Y 15

namespace GameLibrary
{
	struct Math
	{
		static float Clamp(const float& value, const float& min, const float& max)
		{
			if (value > max - 1)
				return max - 1;
			else if (value < min)
				return min;
			return value;
		}
		static int Clamp(const int& value, const int& min, const int& max)
		{
			if (value > max - 1)
				return max - 1;
			else if (value < min)
				return min;
			return value;
		}
	};

	class Dot
	{
		unsigned char _x, _y;
	public:
		Dot()
		{
			_x = 0;
			_y = 0;
		}
		Dot(const unsigned char& x, const unsigned char& y)
		{
			_x = Math::Clamp(x, -1, LEVEL_SIZE_X);
			_y = Math::Clamp(y, -1, LEVEL_SIZE_Y);
		}

		Dot operator +(const Dot& dot)
		{
			return Dot(this->_x + dot.GetX(), this->_y + dot.GetY());
		}
		Dot operator *(const float& multiplier)
		{
			return Dot((unsigned char)(this->_x * multiplier), (unsigned char)(this->_y * multiplier));
		}
		bool operator ==(const Dot& dot)
		{
			return _x == dot.GetX() && _y == dot.GetY();
		}

		unsigned char GetX() const
		{
			return _x;
		}
		unsigned char GetY() const
		{
			return _y;
		}

		void SetX(const unsigned char& newValue)
		{
			_x = Math::Clamp(newValue, -1, LEVEL_SIZE_X);
		}
		void SetY(const unsigned char& newValue)
		{
			_y = Math::Clamp(newValue, -1, LEVEL_SIZE_Y);
		}

		static float Distance(const Dot& pos1, const Dot& pos2)
		{
			return (float)sqrt( pow( (pos2.GetX() - pos1.GetX()), 2 ) + pow( (pos2.GetY() - pos1.GetY()), 2 ) );
		}
	};

	void Sleep(float seconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(seconds * 1000)));
	}

	static void GotoXY(int x, int y)
	{
		COORD coord = COORD();
		coord.X = x;
		coord.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	template <typename T, std::size_t size>
	std::size_t ArraySize(T(&)[size]) {
		return size;
	}

	typedef std::function<void()> Function;
}
