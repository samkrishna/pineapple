#include <string>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/app.h>
#include <wx/process.h>

#include "MainFrame.hh"

class MainApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
    void OnSubprocessTerminate(wxProcessEvent &event);

    wxProcess *server;
    MainFrame *frame;
    std::string blank_notebook;
    std::string load_page;

private:
    wxDECLARE_EVENT_TABLE();

};

wxDECLARE_APP(MainApp);
