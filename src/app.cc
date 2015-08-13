
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/menu.h>
#include <wx/process.h>
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/utils.h>
#include <wx/webview.h>

#include "config.h"

namespace config {

/// Environment variable to pass server path
constexpr char server_script_env[] = "PINEAPPLE_SERVER";
/// Default server script if none given
constexpr char server_script_default[] = "venv/bin/python scripts/eridani-main serve";
/// Default size of main window on startup
constexpr int initial_width = 900;
constexpr int initial_height = 700;
/// Local baseurl
constexpr char base_url[] = "http://localhost:8888";
/// Initial url to open
constexpr char start_url[] = "/tree/demo/TestNotebook.ipynb";
/// Title prefix
constexpr char title[] = "Pineapple";
/// Special protocol prefix
constexpr char protocol_prefix[] = "$$$$";
/// Page that shows loading animation and loads actual page
constexpr char loading_html_filename[] = "html/loading.html";
/// What to put at beginning of window title
constexpr char title_prefix[] = "Pineapple - ";

} /// namespace config

std::string load_page;
bool load_page_loaded = false;

class MainFrame: public wxFrame
{
    static constexpr int wxID_SAVE_HTML = 10000;
public:
    MainFrame(std::string url0, const wxString &title, const wxPoint &pos, const wxSize &size, bool indirect_load);
    static MainFrame *Spawn(std::string url, bool indirect_load=false);

    wxProcess *server;
    wxWebView *webview;
    std::string url;
    wxMenuBar *menubar;
    wxMenu *menu_file;
    wxMenu *menu_help;

    void OnMenuEvent(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnSubprocessTerminate(wxProcessEvent &event);
    void OnError(wxWebViewEvent &event);
    void OnTitleChanged(wxWebViewEvent &event);
    void OnNewWindow(wxWebViewEvent &event);

private:
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_WEBVIEW_ERROR(wxID_ANY, MainFrame::OnError)
    EVT_WEBVIEW_TITLE_CHANGED(wxID_ANY, MainFrame::OnTitleChanged)
    EVT_WEBVIEW_NEWWINDOW(wxID_ANY, MainFrame::OnNewWindow)
wxEND_EVENT_TABLE()

class MainApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
    void OnSubprocessTerminate(wxProcessEvent &event);

    wxProcess *server;
    MainFrame *frame;
private:
    wxDECLARE_EVENT_TABLE();

};

wxBEGIN_EVENT_TABLE(MainApp, wxApp)
    EVT_END_PROCESS(wxID_ANY, MainApp::OnSubprocessTerminate)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MainApp);

std::string replace_one(std::string &s, std::string mud, std::string gold)
{
    std::cout << s.length() << ", " << mud.length() << ", " << gold.length() << std::endl;
    return s.replace(s.find(mud), mud.length(), gold);
}

bool MainApp::OnInit()
{
    frame = MainFrame::Spawn(std::string(config::base_url) + std::string(config::start_url), true);

    wxString server_script;
    server = nullptr;
    if (!wxGetEnv(config::server_script_env, &server_script)) {
        server_script = config::server_script_default;
    }
    server = new wxProcess(frame);
    wxExecute(server_script,
        wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER,
        server);

    return true;
}

int MainApp::OnExit()
{
    std::cout << "APP OnExit()" << std::endl;
    if (server) {
        std::cout << "CLOSING SERVER" << std::endl;
        server->Kill(server->GetPid(), wxSIGTERM, wxKILL_CHILDREN);
    }
}

void MainApp::OnSubprocessTerminate(wxProcessEvent &event)
{
    std::cout << "SUBPROCESS TERMINATED" << std::endl;
}

MainFrame::MainFrame(std::string url0, const wxString &title,
    const wxPoint &pos, const wxSize &size, bool indirect_load)
    : wxFrame(nullptr, wxID_ANY, title, pos, size), url(url0)
{
    menubar = new wxMenuBar();
    menu_file = new wxMenu();
    menu_file->Append(wxID_NEW, wxT("&New"));
    menu_file->Append(wxID_SAVE_HTML, wxT("Download HTML"));
    menu_file->Append(wxID_EXIT, wxT("&Quit"));
    menubar->Append(menu_file, wxT("&File"));
    menu_help = new wxMenu();
    menu_help->Append(wxID_ABOUT, wxT("&About"));
    menubar->Append(menu_help, wxT("&Help"));
    SetMenuBar(menubar);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMenuEvent, this);

    wxBoxSizer* frame_sizer = new wxBoxSizer(wxVERTICAL);

    webview = wxWebView::New(this, wxID_ANY);
    frame_sizer->Add(webview, 1, wxEXPAND, 10);

    if (indirect_load) {
        if (!load_page_loaded) {
            // Read loading page
            std::ifstream ifs(config::loading_html_filename);
            load_page = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
            load_page_loaded = true;
        }
        // Do template replacement for url
        std::string contents{load_page};
        replace_one(contents, "{{url}}", url);
        std::cout << contents << std::endl;
        webview->SetPage(wxString(contents), "");
    } else {
        webview->LoadURL(url);
    }

    webview->Show();

    SetSizerAndFit(frame_sizer);
    SetSize(wxDefaultCoord, wxDefaultCoord, size.GetWidth(), size.GetHeight());
}

void MainFrame::OnError(wxWebViewEvent &event)
{
    std::cout << "ERROR" << std::endl;
    webview->LoadURL(url);
}

void jupyter_click_cell(wxWebView *wv, std::string id)
{
    std::string cmd = "Jupyter.menubar.element.find('#" + id + "').click();";
    wv->RunScript(cmd);
}

void MainFrame::OnMenuEvent(wxCommandEvent &event)
{
    std::cout << "MENU EVENT" << std::endl;
    switch (event.GetId()) {
        case wxID_SAVE_HTML:
        {
            jupyter_click_cell(webview, "download_markdown");
            break;
        }
        case wxID_NEW:
        {
            jupyter_click_cell(webview, "open_notebook");
            break;
        }
        case wxID_ABOUT:
        {
            std::stringstream ss;
            ss << config::version_full << "\n\n" << wxGetLibraryVersionInfo().ToString() << std::endl;            
            wxMessageBox(ss.str(), "About", wxOK | wxICON_INFORMATION);
            break;
        }
        case wxID_EXIT:
        {
            Close(true);
            break;
        }
    }
}

void MainFrame::OnClose(wxCloseEvent &event)
{
    std::cout << "CLOSE" << std::endl;
    std::cout << "CLOSING WEBVIEW" << std::endl;
    if (webview) {
        webview->Destroy();
    }
    std::cout << "DESTROY SELF" << std::endl;
    Destroy();
}

void MainFrame::OnTitleChanged(wxWebViewEvent &event)
{
    std::string title = event.GetString().ToStdString();
    std::cout << "TITLE CHANGED - " << title << std::endl;
    // Check if starts with $$$$
    std::string prefix = config::protocol_prefix;
    if (std::equal(prefix.begin(), prefix.end(), title.begin())) {
        // Prefix present
        std::string theUrl = title.substr(prefix.size());
        std::cout << "SPECIAL " << theUrl << std::endl;
        Spawn(config::base_url + theUrl);
        return;
    }
    // Otherwise actually change the title
    SetLabel(config::title_prefix + title);
}

MainFrame *MainFrame::Spawn(std::string url, bool indirect_load)
{
    MainFrame *child = new MainFrame(url, url,
        wxPoint(wxDefaultCoord, wxDefaultCoord),
        wxSize(config::initial_width, config::initial_height), indirect_load);
    child->Show();
    return child;
}

void MainFrame::OnNewWindow(wxWebViewEvent &event)
{
    wxString url(event.GetURL());
    std::cout << "NEW WINDOW " << url << std::endl;
    wxLaunchDefaultBrowser(url);
}
