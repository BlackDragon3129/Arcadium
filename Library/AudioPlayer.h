#pragma once
#include <Windows.h>

#pragma comment(lib, "winmm.lib")

namespace GameLibrary
{
	namespace Sounds
	{
		struct SoundsList
		{
			static const wchar_t* SHOOT_SOUND;
			static const wchar_t* WIN_SOUND;
			static const wchar_t* DEATH_SOUND;
			static const wchar_t* MENU_SWICTH_SOUND;
		};
		const wchar_t* SoundsList::SHOOT_SOUND = L"Sounds\\Shoot.wav";
		const wchar_t* SoundsList::WIN_SOUND = L"Sounds\\Win.wav";
		const wchar_t* SoundsList::DEATH_SOUND = L"Sounds\\Death.wav";
		const wchar_t* SoundsList::MENU_SWICTH_SOUND = L"Sounds\\MenuSwitch.wav";

		struct AudioPlayer
		{
			static void Play(const wchar_t* path)
			{
				PlaySound(path, NULL, SND_FILENAME | SND_ASYNC);
			}
		};
	}
}