#include <OptionsDialog.h>

wxBEGIN_EVENT_TABLE(OptionsDialog, wxDialog)
    EVT_BUTTON(WidgetIds::ID_Button_Browse, OptionsDialog::OnBrowse)
    EVT_BUTTON(WidgetIds::ID_Button_OK, OptionsDialog::OnOK)
    EVT_BUTTON(WidgetIds::ID_Button_Cancel, OptionsDialog::OnCancel)
wxEND_EVENT_TABLE()

OptionsDialog::OptionsDialog(SettingsManager &settingsManager, wxEvtHandler *eventHandler, wxWindow *parent, const wxPoint &pos, const wxSize &size)
    : wxDialog(parent, wxID_ANY, "Options", pos, size), m_settingsManager(settingsManager), m_eventHandler(eventHandler)
{
    wxPanel *panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(-1, 15);

    wxBoxSizer *CHDManPathCaptionRow = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *lblCHDManPathCaption = new wxStaticText(panel, wxID_ANY, "Path to CHDMan executable:");
    CHDManPathCaptionRow->Add(lblCHDManPathCaption);
    mainSizer->Add(CHDManPathCaptionRow, 0, wxLEFT | wxRIGHT, 10);

    wxBoxSizer *CHDManPathRow = new wxBoxSizer(wxHORIZONTAL);
    m_txtCHDManPath = new wxTextCtrl(panel, WidgetIds::ID_Text_CHDManPath);
    m_txtCHDManPath->Bind(wxEVT_SET_FOCUS, &OptionsDialog::OnCHDManPathFocus, this);
    wxButton *btnCHDManPathBrowse = new wxButton(panel, WidgetIds::ID_Button_Browse, "Browse...");
    CHDManPathRow->Add(m_txtCHDManPath, 8, wxLEFT | wxALIGN_CENTER, 0);
    CHDManPathRow->AddSpacer(5);
    CHDManPathRow->Add(btnCHDManPathBrowse, 2, wxRIGHT | wxALIGN_CENTER);
    mainSizer->Add(CHDManPathRow, 0, wxLEFT | wxRIGHT, 10);

    mainSizer->Add(-1, 25);

    wxBoxSizer *concurrentItemsRow = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *lblConcurrentItemsCaption = new wxStaticText(panel, wxID_ANY, "Number of items to process concurrently:");
    m_spnMaxConcurrentItems = new wxSpinCtrl(panel, wxID_ANY);
    m_spnMaxConcurrentItems->SetRange(1, 5);
    concurrentItemsRow->Add(lblConcurrentItemsCaption, 9, wxLEFT | wxALIGN_CENTER, 10);
    concurrentItemsRow->Add(m_spnMaxConcurrentItems, 1, wxRIGHT, 10);
    mainSizer->Add(concurrentItemsRow);

    mainSizer->Add(-1, 25);

    wxBoxSizer *okayCancelButtonsRow = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btnOK = new wxButton(panel, WidgetIds::ID_Button_OK, "OK");
    wxButton *btnCancel = new wxButton(panel, WidgetIds::ID_Button_Cancel, "Cancel");
    okayCancelButtonsRow->Add(btnOK);
    okayCancelButtonsRow->Add(btnCancel);
    mainSizer->Add(okayCancelButtonsRow, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);

    m_txtCHDManPath->SetValue(m_settingsManager.GetCHDManPath());
    m_spnMaxConcurrentItems->SetValue(m_settingsManager.GetMaxConcurrentItems());

    panel->SetSizer(mainSizer);
}

void OptionsDialog::OnBrowse(wxCommandEvent &event)
{
    wxFileDialog fileDialog(this, "Select CHDMan executable", wxEmptyString, wxEmptyString, "CHDMan executable (chdman.exe)|chdman.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fileDialog.ShowModal() == wxID_OK)
    {
        m_txtCHDManPath->SetValue(fileDialog.GetPath());
    }
}

void OptionsDialog::OnOK(wxCommandEvent &event)
{
    if (wxFileExists(m_txtCHDManPath->GetValue()))
    {
        m_settingsManager.SetCHDManPath(m_txtCHDManPath->GetValue());
        m_settingsManager.SetMaxConcurrentItems(m_spnMaxConcurrentItems->GetValue());

        m_settingsManager.Flush();
        
        wxCommandEvent *settingsChangedEvent = new wxCommandEvent(wxEVT_SETTINGS_CHANGED, wxID_ANY);
        m_eventHandler->QueueEvent(settingsChangedEvent);
        
        Close();
    }
    else
    {
        wxMessageBox("Path to CHDMan executable must exist!", "CHDMan Executable not found", wxICON_ERROR);
    }
}

void OptionsDialog::OnCancel(wxCommandEvent &event)
{
    Close();
}

void OptionsDialog::OnCHDManPathFocus(wxFocusEvent &event)
{
    m_txtCHDManPath->SetSelection(0, 0);
    event.Skip();
}