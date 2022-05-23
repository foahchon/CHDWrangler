#include <SettingsManager.h>

SettingsManager::SettingsManager(wxString settingsFilePath)
{
    m_settingsFilePath = settingsFilePath;

    wxFileConfig configFile(wxEmptyString, wxEmptyString, m_settingsFilePath);

    configFile.SetPath("/settings");
    m_CHDManPath = configFile.Read("CHDManPath", "");
    m_maxConcurrentItems = configFile.Read("MaxConcurrentItems", 1);
}

SettingsManager::~SettingsManager()
{
    Flush();
}

void SettingsManager::SetCHDManPath(wxString newPath)
{
    m_CHDManPath = newPath;
}

void SettingsManager::SetMaxConcurrentItems(int maxConcurrentItems)
{
    m_maxConcurrentItems = maxConcurrentItems;
}

wxString SettingsManager::GetCHDManPath()
{
    return m_CHDManPath;
}

int SettingsManager::GetMaxConcurrentItems()
{
    return m_maxConcurrentItems;
}

void SettingsManager::Flush()
{
    wxFileConfig configFile(wxEmptyString, wxEmptyString, m_settingsFilePath);

    configFile.SetPath("/settings");
    configFile.Write("CHDManPath", m_CHDManPath);
    configFile.Write("MaxConcurrentItems", m_maxConcurrentItems);

    configFile.Flush();
}