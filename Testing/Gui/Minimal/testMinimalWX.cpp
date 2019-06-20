/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testMinimalWX
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaIncludeWX.h"

// ============================================================================
class myApp : public wxApp
// ============================================================================
{
public:
    virtual bool OnInit();
};
// ============================================================================
class myFrame : public wxFrame
// ============================================================================
{
public:
    myFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
private:
    DECLARE_EVENT_TABLE()
};
// ----------------------------------------------------------------------------
// IDs for the controls and the menu commands
// ----------------------------------------------------------------------------
enum
{
    Minimal_Quit = 1,
    Minimal_About = wxID_ABOUT
};
// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(myFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  myFrame::OnQuit)
    EVT_MENU(Minimal_About, myFrame::OnAbout)
END_EVENT_TABLE()

//--------------------------------------------------------------------------------
// Create the Application
IMPLEMENT_APP(myApp)
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
bool myApp::OnInit()
//--------------------------------------------------------------------------------
{
    myFrame *frame = new myFrame("albaGUITest",wxPoint(50, 50), wxSize(450, 340));
    frame->Show(TRUE);
    return TRUE;
}
//--------------------------------------------------------------------------------
myFrame::myFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(NULL, -1, title, pos, size, style)
//--------------------------------------------------------------------------------
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About...\tF1", "Show about dialog");

    menuFile->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
}
//--------------------------------------------------------------------------------
void myFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
//--------------------------------------------------------------------------------
{
    Close(TRUE);
}
//--------------------------------------------------------------------------------
void myFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
//--------------------------------------------------------------------------------
{
    wxString msg;
    msg.Printf( "This is the About dialog of the minimal sample.\n",
                "Welcome to %s",
                wxVERSION_STRING);

    wxMessageBox(msg, "About Minimal", wxOK | wxICON_INFORMATION, this);
}
