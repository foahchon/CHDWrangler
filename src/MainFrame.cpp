#include <MainFrame.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    
    EVT_MENU(WidgetIds::ID_Menu_Exit, MainFrame::OnExit)
    EVT_MENU(WidgetIds::ID_Menu_About, MainFrame::OnAbout)
    EVT_MENU(WidgetIds::ID_Menu_AddCUEFile, MainFrame::OnAddCUEFile)
    EVT_MENU(MainFrame::ID_Menu_AddFromFolder, MainFrame::OnAddFromFolder)

    EVT_MENU(WidgetIds::ID_Menu_StartBatch, MainFrame::OnStartBatch)
    EVT_MENU(WidgetIds::ID_Menu_PauseBatch, MainFrame::OnPauseBatch)
    EVT_MENU(WidgetIds::ID_Menu_Options, MainFrame::OnOptions)

    EVT_MENU(WidgetIds::ID_Menu_PrintItemData, MainFrame::OnPrintItemData)
    EVT_MENU(WidgetIds::ID_Menu_StartProcessForSelectedItems, MainFrame::OnStartProcessForSelectedItems)
    EVT_MENU(WidgetIds::ID_Menu_ChangeOutputFolder, MainFrame::OnChangeOutputFolder)
    EVT_MENU(WidgetIds::ID_Menu_DeleteSelectedItems, MainFrame::OnDeleteSelectedItems)
    
    EVT_DATAVIEW_ITEM_CONTEXT_MENU(MainFrame::ID_ListView, MainFrame::OnJobListContextMenu)

    EVT_CLOSE(MainFrame::OnClose)

wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size),
    m_settingsManager(wxString::Format("%s%s", wxPathOnly(wxStandardPaths::Get().GetExecutablePath()), "\\settings.ini"))
{
#ifdef WXDEBUG
        wxLogWindow *logWindow = new wxLogWindow(this, "Log Window");
        logWindow->GetFrame()->Move(500, 100);
        wxLog::SetActiveTarget(logWindow);
    #endif
    
    BuildMenus();
    BuildListView();
    CreateStatusBar();
    SetStatusText("Ready.");

    wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
    boxSizer->Add(m_listView, 1, wxEXPAND);
    SetSizer(boxSizer);

    Bind(wxEVT_THREAD_STDIN, &MainFrame::OnThreadInput, this);
    Bind(wxEVT_THREAD_STDERR, &MainFrame::OnThreadInput, this);
    Bind(wxEVT_END_PROCESS, &MainFrame::OnProcessComplete, this);
    Bind(wxEVT_SETTINGS_CHANGED, &MainFrame::OnSettingsChanged, this);

    ReloadSettings();
}

void MainFrame::BuildMenus()
{
    wxMenu *mnuFile = new wxMenu;
    mnuFile->Append(WidgetIds::ID_Menu_AddCUEFile, "Add CUE F&ile...\tCtrl-I");
    mnuFile->Append(WidgetIds::ID_Menu_AddFromFolder, "Add From &Folder...\tCtrl-F");
    mnuFile->AppendSeparator();
    mnuFile->Append(WidgetIds::ID_Menu_Exit, "E&xit\tAlt-F4");

    wxMenu *mnuActions = new wxMenu;
    mnuActions->Append(WidgetIds::ID_Menu_StartBatch, "&Start Batch\tF5");
    mnuActions->Append(WidgetIds::ID_Menu_PauseBatch, "&Pause Batch\tF7");
    mnuActions->AppendSeparator();
    mnuActions->Append(WidgetIds::ID_Menu_StartProcessForSelectedItems, "&Process Selected\tF2");
    mnuActions->Append(WidgetIds::ID_Menu_ChangeOutputFolder, "Change Output &Folder For Selected...\tCtrl-F");
    mnuActions->Append(WidgetIds::ID_Menu_DeleteSelectedItems, "&Delete Selected Items\tDel");
    mnuActions->AppendSeparator();
    mnuActions->Append(WidgetIds::ID_Menu_Options, "O&ptions...\tCtrl-O");

    wxMenu *mnuHelp = new wxMenu;
    mnuHelp->Append(WidgetIds::ID_Menu_About, "About...\tF1");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(mnuFile, "&File");
    menuBar->Append(mnuActions, "&Actions");
    menuBar->Append(mnuHelp, "&Help");
    
    SetMenuBar(menuBar);

    m_listViewPopupMenu = new wxMenu;
    m_listViewPopupMenu->Append(WidgetIds::ID_Menu_AddCUEFile, "Add CUE F&ile...\tCtrl-I");
    m_listViewPopupMenu->Append(WidgetIds::ID_Menu_AddFromFolder, "Add From &Folder...\tCtrl-F");
    m_listViewPopupMenu->AppendSeparator();
    #ifdef WXDEBUG
        m_listViewPopupMenu->Append(WidgetIds::ID_Menu_PrintItemData, "Print &Item Data");
        m_listViewPopupMenu->AppendSeparator();
    #endif
    m_listViewPopupMenu->Append(WidgetIds::ID_Menu_StartProcessForSelectedItems, "&Process Selected\tF2");
    m_listViewPopupMenu->Append(WidgetIds::ID_Menu_ChangeOutputFolder, "Change Output &Folder For Selected...\tCtrl-F");
    m_listViewPopupMenu->Append(WidgetIds::ID_Menu_DeleteSelectedItems, "&Delete Selected Items\tDel");
}

void MainFrame::BuildListView()
{
    wxDataViewTextRenderer *textRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT, wxALIGN_LEFT);
    wxDataViewTextRenderer *centerTextRenderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER);
    GaugeRenderer *gaugeRenderer = new GaugeRenderer(m_listView, ColumnIds::ID_Column_Progress);
    m_listView = new wxDataViewListCtrl(this, MainFrame::ID_ListView, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE);

    wxDataViewColumn *filenameColumn = new wxDataViewColumn("Filename", textRenderer, ColumnIds::ID_Column_Filename, 120, wxALIGN_LEFT);
    wxDataViewColumn *progressGaugeColumn = new wxDataViewColumn("Progress", gaugeRenderer, ColumnIds::ID_Column_Progress, 120);
    wxDataViewColumn *statusColumn = new wxDataViewColumn("Status", centerTextRenderer, ColumnIds::ID_Column_Status, 80);
    wxDataViewColumn *outputPathColumn = new wxDataViewColumn("Output Path", textRenderer, ColumnIds::ID_Column_OutputPath);

    progressGaugeColumn->SetMinWidth(120);

    m_listView->AppendColumn(filenameColumn);
    m_listView->AppendColumn(progressGaugeColumn);
    m_listView->AppendColumn(statusColumn);
    m_listView->AppendColumn(outputPathColumn);
}

void MainFrame::InsertListItemForFile(wxString filePath)
{
    wxFileName fileName(filePath);
    wxVector<wxVariant> columnValues;

    CHDManItemData *newItemData = new CHDManItemData();
    newItemData->inputPath = fileName;
    newItemData->listIndex = m_listView->GetItemCount();
    newItemData->outputPath = wxFileName(newItemData->inputPath);
    newItemData->outputPath.SetExt("chd");

    columnValues.push_back(wxVariant(newItemData->inputPath.GetFullName()));
    columnValues.push_back(newItemData->progressPercentage);
    columnValues.push_back(wxVariant(GetNameByStatus(newItemData->status)));
    columnValues.push_back(wxVariant(newItemData->outputPath.GetFullPath()));

    m_listView->AppendItem(columnValues, (wxUIntPtr)newItemData);
}

int MainFrame::StartProcessForRow(int row)
{
    if (row <= 0 || row >= m_listView->GetItemCount())
    {
        return 0;
    }

    CHDManItemData *itemData = GetItemDataForRow(row);

    if (itemData->status == CHDManItemData::CHDManItemStatus::Pending)
    {
        itemData->status = CHDManItemData::CHDManItemStatus::Processing;
        m_listView->SetValue(wxVariant(GetNameByStatus(itemData->status)), row, ColumnIds::ID_Column_Status);
        wxLogDebug(wxString::Format("Starting process for row '%d'", row));
        wxProcess *process = new wxProcess(this);
        process->Redirect();
        CHDManProgressThread *progressThread = new CHDManProgressThread(process, this, &m_messageQueue);
        itemData->progressThread = progressThread;

        progressThread->Run();
        if (!progressThread->IsRunning())
        {
            wxMessageBox("Unable to create thread.");
            return 0;
        }

        int processId = wxExecute(wxString::Format("\"%s\" createcd -i \"%s\" -o \"%s\" -f",
                                                    m_CHDManPath,
                                                    itemData->inputPath.GetFullPath(),
                                                    itemData->outputPath.GetFullPath()),
                                   wxEXEC_ASYNC, process);

        if (processId != 0)
        {
            itemData->processId = processId;
            m_processMap[processId] = row;
            m_currentProcessingItems++;
            UpdateStatus();
            return processId;
        }
        else
        {
            m_messageQueue.Post(CHDManProgressThread::ThreadMessage::ExitThread);
            itemData->status = CHDManItemData::CHDManItemStatus::ErroredOut;
            m_listView->SetValue(wxVariant(GetNameByStatus(itemData->status)), row, ColumnIds::ID_Column_Status);

            return 0;
        }
    }
    else
    {
        wxLogDebug(wxString::Format("Skipping row '%d' (status is not Pending), queue has '%d' items", row, (int)m_batchQueue.size()));
        return 0;
    }
}

void MainFrame::StartNextBatchItems()
{
    if (!m_batchPaused)
    {
        wxLogDebug("Starting next batch items...");
        while ((m_currentProcessingItems < m_maxProcessingItems) && !m_batchQueue.empty())
        {
            StartProcessForRow(m_batchQueue.front());
            wxLogDebug(wxString::Format("Popping; %d items in queue...", (int)m_batchQueue.size()));
            m_batchQueue.pop();
        }
    }
}

void MainFrame::GatherFoldersForPath(wxString path, std::vector<wxString> &pathList, bool recurse)
{
    wxString foundPath;
    wxDir dir(path);

    if (!dir.IsOpened() || !dir.HasSubDirs())
    {
        return;
    }

    int dirFound = dir.GetFirst(&foundPath, wxEmptyString, wxDIR_DIRS);

    while (dirFound)
    {
        wxString fullPath = wxString::Format("%s%s", dir.GetNameWithSep(), foundPath);
        pathList.push_back(fullPath);

        if (recurse)
        {
            GatherFoldersForPath(fullPath, pathList, recurse);
        }

        dirFound = dir.GetNext(&foundPath);
    }
}

void MainFrame::ReloadSettings()
{
    m_CHDManPath = m_settingsManager.GetCHDManPath();
    m_maxProcessingItems = m_settingsManager.GetMaxConcurrentItems();
}

CHDManItemData *MainFrame::GetItemDataForRow(int row)
{
    wxDataViewItem item = m_listView->RowToItem(row);
    if (item.IsOk())
    {
        return (CHDManItemData *)m_listView->GetItemData(item);
    }
    else
    {
        return NULL;
    }
}

int MainFrame::CountItemsByStatus(CHDManItemData::CHDManItemStatus status)
{
    wxDataViewItem currentItem;
    CHDManItemData *currentItemData;
    int count = 0;

    for (int i = 0; i < m_listView->GetItemCount(); i++)
    {
        currentItem = m_listView->RowToItem(i);
        currentItemData = (CHDManItemData *)m_listView->GetItemData(currentItem);

        if ((currentItemData != NULL) && (currentItemData->status == status))
        {
            count++;
        }
    }

    return count;
}

void MainFrame::UpdateStatus()
{
    int total = m_listView->GetItemCount();
    int processing = CountItemsByStatus(CHDManItemData::CHDManItemStatus::Processing);
    int completed = CountItemsByStatus(CHDManItemData::CHDManItemStatus::Complete);

    if (processing == 0)
    {
        SetStatusText("Done.");
    }
    else
    {
        SetStatusText(wxString::Format("Processing %d item(s) (%d/%d complete)...", m_currentProcessingItems, completed, total));
    }
}

void MainFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(wxString::Format("CHD Wrangler v1.0\nby Vincent Wixsom %s2022", wxString::FromUTF8("\xc2\xa9")),
                 "About", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnAddCUEFile(wxCommandEvent &event)
{
    wxFileDialog fileDialog(this, "Select CUE file", wxEmptyString, wxEmptyString, "CUE files (*.cue)|*.cue", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (fileDialog.ShowModal() == wxID_OK)
    {
        InsertListItemForFile(fileDialog.GetPath());
    }
}

void MainFrame::OnAddFromFolder(wxCommandEvent &event)
{
    wxDirDialog dirDialog(this, "Choose folder to add CUE files from", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    wxString searchResult;
    std::vector<wxString> gatheredPaths;

    if (dirDialog.ShowModal() == wxID_OK)
    {
        wxString selectedPath = dirDialog.GetPath();

        GatherFoldersForPath(selectedPath, gatheredPaths);
        gatheredPaths.push_back(selectedPath);

        for (wxString path : gatheredPaths)
        {
            wxDir dir(path);
            wxString foundFileName;
            wxString fullFilePath;

            if (dir.IsOpened())
            {
                bool fileFound = dir.GetFirst(&foundFileName, "*.cue");

                while (fileFound)
                {
                    fullFilePath = wxString::Format("%s%s", dir.GetNameWithSep(), foundFileName);
                    InsertListItemForFile(fullFilePath);
                    fileFound = dir.GetNext(&foundFileName);
                }
            }
        }
    }
}

void MainFrame::OnClose(wxCloseEvent &event)
{
    if (CountItemsByStatus(CHDManItemData::CHDManItemStatus::Processing) > 0)
    {
        if (wxMessageBox("There are still processes running. Are you sure you want to exit?\nAny current processes will continue running until they're finished.",
                         "Confirmation", wxYES_NO | wxICON_WARNING) != wxYES)
        {
            event.Veto();
            return;
        }
    }

    for (int i = 0; i < m_listView->GetItemCount(); i++)
    {
        CHDManItemData *itemData = GetItemDataForRow(i);

        if (itemData != NULL)
        {
            delete itemData;
        }
    }

    if (m_listViewPopupMenu != NULL)
    {
        delete m_listViewPopupMenu;
    }

    event.Skip();
}

void MainFrame::OnStartBatch(wxCommandEvent &event)
{
    if (!wxFileExists(m_settingsManager.GetCHDManPath()))
    {
        wxMessageBox("CHDMan path is invalid!\nPlease set a valid CHDMan path in the Options dialog.", "CHDMan Path invvalid!", wxICON_WARNING | wxYES_NO);
        return;
    }

    m_batchPaused = false;

    for (int i = 0; i < m_listView->GetItemCount(); i++)
    {
        m_batchQueue.push(i);
        wxLogDebug(wxString::Format("Enqueued row '%d'; queue has '%d' items", i, (int)m_batchQueue.size()));
    }

    StartNextBatchItems();
}

void MainFrame::OnPauseBatch(wxCommandEvent &event)
{
    m_batchPaused = true;
}

void MainFrame::OnOptions(wxCommandEvent &event)
{
    OptionsDialog optionsDialog(m_settingsManager, this, this, wxDefaultPosition, wxSize(400, 190));
    optionsDialog.ShowModal();
}

void MainFrame::OnJobListContextMenu(wxDataViewEvent &event)
{
    PopupMenu(m_listViewPopupMenu);
}

void MainFrame::OnDeleteSelectedItems(wxCommandEvent &event)
{
    for (int i = m_listView->GetItemCount() - 1; i >= 0; i--)
    {
        if (m_listView->IsRowSelected(i))
        {
            CHDManItemData *itemData = GetItemDataForRow(i);

            if (itemData != NULL 
                && itemData->status != CHDManItemData::CHDManItemStatus::Processing)
            {
                delete itemData;
                m_listView->DeleteItem(i);
            }
        }
    }
}

void MainFrame::OnStartProcessForSelectedItems(wxCommandEvent &event)
{
    if (!wxFileExists(m_settingsManager.GetCHDManPath()))
    {
        wxMessageBox("CHDMan path is invalid!\nPlease set a valid CHDMan path in the Options dialog.", "CHDMan Path invvalid!", wxICON_WARNING | wxYES_NO);
        return;
    }

    for (int i = 0; i < m_listView->GetItemCount(); i++)
    {
        if (m_listView->IsRowSelected(i))
        {
            m_batchQueue.push(i);
            wxLogDebug(wxString::Format("Enqueued row '%d'; queue has '%d' items", i, (int)m_batchQueue.size()));
        }
    }

    StartNextBatchItems();
}

void MainFrame::OnPrintItemData(wxCommandEvent &event)
{
    int selectedRow = m_listView->GetSelectedRow();

    if (selectedRow != wxNOT_FOUND)
    {
        CHDManItemData *itemData = GetItemDataForRow(selectedRow);

        if (itemData != NULL)
        {

            if (itemData != NULL)
            {
                wxLogDebug("====================");
                wxLogDebug(wxString::Format("Full path: %s", itemData->inputPath.GetFullPath()));
                wxLogDebug(wxString::Format("Index: %d", itemData->listIndex));
                wxLogDebug("====================");
            }
        }
    }
}

void MainFrame::OnChangeOutputFolder(wxCommandEvent &event)
{
    if (m_listView->GetSelectedItemsCount() == 0)
    {
        return;
    }

    wxDirDialog dirDialog(this, "Choose output path", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dirDialog.ShowModal() == wxID_OK)
    {
        wxString selectedPath = dirDialog.GetPath();

        for (int i = 0; i < m_listView->GetItemCount(); i++)
        {
            if (m_listView->IsRowSelected(i))
            {
                CHDManItemData *itemData = GetItemDataForRow(i);

                if (itemData != NULL)
                {
                    itemData->outputPath.SetPath(selectedPath);
                    m_listView->SetValue(wxVariant(itemData->outputPath.GetFullPath()), i, ColumnIds::ID_Column_OutputPath);
                }
            }
        }
    }
}

void MainFrame::OnThreadInput(wxThreadEvent &event)
{
    if (event.GetEventType() == wxEVT_THREAD_STDERR)
    {
        CHDManProgressEvent *progressEvent = event.GetPayload<CHDManProgressEvent *>();

        if (progressEvent != NULL)
        {
            wxString firstWord = event.GetString().SubString(0, event.GetString().find(" ", 0));
            wxString secondWord = event.GetString().SubString(firstWord.Length(), event.GetString().find(" ", firstWord.Length()));
            int itemRow = m_processMap[progressEvent->processId];
            CHDManItemData *itemData = GetItemDataForRow(itemRow);

            long progress;
            if (firstWord == "Compressing, ")
            {
                secondWord.ToLong(&progress);
                m_listView->SetValue(wxVariant(progress), itemRow, ColumnIds::ID_Column_Progress);

                if (itemData != NULL)
                {
                    itemData->progressPercentage = progress;
                }
            }
            else if ((firstWord == "Compression ") && secondWord.StartsWith("complete"))
            {
                if (itemData != NULL)
                {
                    itemData->progressPercentage = 100;
                }
                progress = 100;

            }

            delete progressEvent;
        }
    }
}

void MainFrame::OnProcessComplete(wxProcessEvent &processEvent)
{
    m_messageQueue.Post(CHDManProgressThread::ThreadMessage::ProcessComplete);

    if (m_processMap.find(processEvent.GetPid()) != m_processMap.end())
    {
        int row = m_processMap[processEvent.GetPid()];
        CHDManItemData *itemData = GetItemDataForRow(row);

        if (itemData != NULL)
        {
            if (itemData->progressPercentage == 100)
            {
                itemData->status = CHDManItemData::CHDManItemStatus::Complete;
                m_listView->SetValue(wxVariant(GetNameByStatus(itemData->status)), row, ColumnIds::ID_Column_Status);
            }
            else
            {
                itemData->status = CHDManItemData::CHDManItemStatus::ErroredOut;
                m_listView->SetValue(wxVariant(GetNameByStatus(itemData->status)), row, ColumnIds::ID_Column_Status);
            }

            itemData->progressThread->Wait();
            delete itemData->progressThread;
        }

        wxLogDebug(wxString::Format("Proccess for row '%d' completed.", m_processMap[processEvent.GetPid()]));
        m_listView->SetValue(wxVariant(100), m_processMap[processEvent.GetPid()], ColumnIds::ID_Column_Progress);
        m_processMap.erase(processEvent.GetPid());

        m_currentProcessingItems--;
        UpdateStatus();
        StartNextBatchItems();
    }
}

void MainFrame::OnSettingsChanged(wxEvent &event)
{
    ReloadSettings();
}

wxString MainFrame::GetNameByStatus(CHDManItemData::CHDManItemStatus status)
{
    switch (status)
    {
        case CHDManItemData::CHDManItemStatus::Pending:
            return "Pending";
        case CHDManItemData::CHDManItemStatus::Processing:
            return "Processing";
        case CHDManItemData::CHDManItemStatus::Complete:
            return "Complete";
        default:
            return "Errored Out";
    }
}