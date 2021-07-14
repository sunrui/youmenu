#include "wx/wx.h"
#include "wxMainFrame.h"

class MyApp: public wxApp
{
    virtual bool OnInit();
	
    wxMainFrame *frame;
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    frame = new wxMainFrame(NULL, wxT("平台功能测试"), wxDefaultPosition, wxSize(900,675));
    frame->Show();

    return true;
}