#pragma once

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <wx/wx.h>

#include <wx/fileconf.h>

class SettingsManager
{
    public:
        SettingsManager(wxString settingsFilePath);
        ~SettingsManager();

        void SetCHDManPath(wxString newPath);
        void SetMaxConcurrentItems(int maxConcurrentItems);

        wxString GetCHDManPath();
        int GetMaxConcurrentItems();

        void Flush();

    private:
        wxString m_filePath;
        wxString m_CHDManPath;

        wxString m_settingsFilePath;
        int m_maxConcurrentItems;
};

#endif