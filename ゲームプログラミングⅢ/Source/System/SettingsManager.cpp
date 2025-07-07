#include "SettingsManager.h"
#include <fstream>
#include <filesystem>
#include <Windows.h>

void SettingsManager::Save()
{
    std::ofstream out("./Data/Settings/settings.bin", std::ios::binary);
    if (out)
    {
        out.write(reinterpret_cast<const char*>(&settings), sizeof(GameSettings));
    }
    else
    {
        OutputDebugStringA("[ERROR] couldn't save settings.\n");
    }
}

void SettingsManager::Load()
{
    std::ifstream input("./Data/Settings/settings.bin", std::ios::binary);
    if (input)
    {
        input.read(reinterpret_cast<char*>(&settings), sizeof(GameSettings));
    }
    else
    {
        OutputDebugStringA("[ERROR] couldn't load settings.\n");
    }
}
