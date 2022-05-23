#pragma once

#ifndef OPTIONSFRAME_H
#define OPTIONSFRAME_H

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include <SettingsManager.h>

wxDECLARE_EVENT(wxEVT_SETTINGS_CHANGED, wxCommandEvent);

class OptionsDialog : public wxDialog
{
    public:
        enum WidgetIds
        {
            ID_Text_CHDManPath = wxID_HIGHEST + 1,
            ID_Button_Browse,
            ID_Button_OK,
            ID_Button_Cancel
        };

        OptionsDialog(SettingsManager &settingsManager, wxEvtHandler *eventHandler, wxWindow *parent, const wxPoint &pos, const wxSize &size);

    private:
        void OnBrowse(wxCommandEvent &event);
        void OnOK(wxCommandEvent &event);
        void OnCancel(wxCommandEvent &event);
        void OnCHDManPathFocus(wxFocusEvent &event);

        wxSpinCtrl *m_spnMaxConcurrentItems;
        wxTextCtrl *m_txtCHDManPath;
        SettingsManager &m_settingsManager;
        wxEvtHandler *m_eventHandler;

        wxDECLARE_EVENT_TABLE();
};

#endif