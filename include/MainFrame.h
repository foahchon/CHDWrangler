#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#endif

#ifndef MYFRAME_H
#define MYFRAME_H

#include <wx/wx.h>
#include <wx/process.h>
#include <wx/tokenzr.h>
#include <wx/renderer.h>
#include <wx/dirdlg.h>
#include <wx/dir.h>
#include <wx/dataview.h>
#include <wx/headerctrl.h>
#include <wx/stdpaths.h>

#include <CHDManProgressThread.h>
#include <CHDManProgressEvent.h>
#include <CHDManItemData.h>
#include <GaugeRenderer.h>
#include <SettingsManager.h>
#include <OptionsDialog.h>

class MainFrame : public wxFrame
{

public:
    MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

    enum WidgetIds
    {
        ID_Button_Browse = wxID_HIGHEST + 1,
        ID_Button_SelectFile,

        ID_Menu_AddCUEFile,
        ID_Menu_AddFromFolder,
        ID_Menu_Exit,
        ID_Menu_Options,
        ID_Menu_TestItems,
        ID_Menu_DeleteSelectedItems,
        ID_Menu_PrintItemData,
        ID_Menu_StartProcessForSelectedItems,
        ID_Menu_ChangeOutputFolder,
        ID_Menu_StartBatch,
        ID_Menu_PauseBatch,

        ID_Menu_About,

        ID_ListView
    };

    enum ColumnIds
    {
        ID_Column_Filename,
        ID_Column_Progress,
        ID_Column_Status,
        ID_Column_OutputPath
    };

    private:
        void BuildMenus();
        void BuildListView();
        void InsertListItemForFile(wxString filePath);
        int StartProcessForRow(int row);
        void StartNextBatchItems();
        void GatherFoldersForPath(wxString path, std::vector<wxString> &pathList, bool recurse = true);
        void ReloadSettings();
        wxString GetNameByStatus(CHDManItemData::CHDManItemStatus status);
        CHDManItemData *GetItemDataForRow(int row);
        int CountItemsByStatus(CHDManItemData::CHDManItemStatus status);
        void UpdateStatus();

        void OnAddCUEFile(wxCommandEvent &event);
        void OnAddFromFolder(wxCommandEvent &event);
        void OnExit(wxCommandEvent &event);
        void OnClose(wxCloseEvent &event);
        void OnStartBatch(wxCommandEvent &event);
        void OnPauseBatch(wxCommandEvent &event);
        void OnOptions(wxCommandEvent &event);
        void OnAbout(wxCommandEvent &event);

        void OnJobListContextMenu(wxDataViewEvent &event);
        void OnDeleteSelectedItems(wxCommandEvent &event);
        void OnStartProcessForSelectedItems(wxCommandEvent &event);
        void OnPrintItemData(wxCommandEvent &event);
        void OnChangeOutputFolder(wxCommandEvent &event);

        void OnThreadInput(wxThreadEvent &event);
        void OnProcessComplete(wxProcessEvent &processEvent);
        void OnSettingsChanged(wxEvent &event);

        wxDataViewListCtrl *m_listView;
        wxMenu *m_listViewPopupMenu;
        wxMessageQueue<CHDManProgressThread::ThreadMessage> m_messageQueue;
        SettingsManager m_settingsManager;
        std::unordered_map<int, int> m_processMap;
        std::queue<int> m_batchQueue;
        wxString m_CHDManPath;
        int m_maxProcessingItems = 2;
        int m_currentProcessingItems = 0;
        bool m_batchPaused = false;

        wxDECLARE_EVENT_TABLE();
};

#endif /* MYFRAME_H */