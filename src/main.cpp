// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <MainFrame.h>

wxDEFINE_EVENT(wxEVT_THREAD_STDIN, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_THREAD_STDERR, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_SETTINGS_CHANGED, wxCommandEvent);

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};


bool MyApp::OnInit()
{
    MainFrame *mainFrame = new MainFrame("CHD Wrangler", wxPoint(50, 50), wxSize(450, 340));
    mainFrame->Show();

    return true;
}

wxIMPLEMENT_APP(MyApp);