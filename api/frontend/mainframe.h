#pragma once

// WX Components
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);

    wxBoxSizer* CreateListPanel(
        wxWindow* parent, 
        int listId, 
        wxString titleName
    );
    wxBoxSizer* CreateListPanel2(wxWindow* parent, int listId, wxString titleName);
    wxBoxSizer* CreateTeamsListPanel(wxWindow* parent);
    wxMenuBar* CreateMenuBar();
};
