#pragma once
#include <iostream>
#include <vector>

#include "BaseLibrary.h"
#include "AudioPlayer.h"

namespace GameLibrary
{
	namespace UI
	{
		struct MenuVariant
		{
			const char* Name;
			unsigned char X, Y;
			Function Action;

			MenuVariant(const char* name, Function act, const unsigned char& xPos, 
				const unsigned char& yPos)
			{
				Name = name;
				Action = act;
				X = xPos;
				Y = yPos;
			}

			void CallAction() const
			{
				if (Action)
					Action();
			}
		};

		class Menu
		{
			std::vector<MenuVariant> _variants;
			const char* _name;
			char _currentVariant;

		public:
			Menu(const char* name)
			{
				_name = name;
				_currentVariant = 0;
			}
			
			void MoveVariant(const char& movement)
			{
				_currentVariant += movement;
				if (_currentVariant == -1)
					_currentVariant = (char)_variants.size() - 1;
				else if (_currentVariant == _variants.size())
					_currentVariant = 0;

				Sounds::AudioPlayer::Play(Sounds::SoundsList::MENU_SWICTH_SOUND);

				Draw();
			}

			void AddVariant(const MenuVariant& newVariant)
			{
				_variants.push_back(newVariant);
			}

			void Draw()
			{
				system("cls");

				GotoXY(5, 0);
				std::cout << _name;

				GotoXY(_variants[_currentVariant].X - 2, _variants[_currentVariant].Y);
				std::cout << '>';

				for (std::size_t i = 0; i < _variants.size(); i++)
				{
					const MenuVariant& currentVariant = _variants[i];
					GotoXY(currentVariant.X, currentVariant.Y);
					std::cout << currentVariant.Name;
				}
			}

			void VariantAct()
			{
				Sounds::AudioPlayer::Play(Sounds::SoundsList::MENU_SWICTH_SOUND);
				_variants[_currentVariant].CallAction();
			}
		};
	}
}
