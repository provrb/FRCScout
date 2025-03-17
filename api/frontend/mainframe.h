#pragma once

// WX Components
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/grid.h>

#include "frontend/mainframe.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);

    friend class DataBase;
private:
    wxBoxSizer* CreateListPanel(
        wxWindow* parent,
        int listId,
        wxString titleName,
        wxString description
    );

    wxGrid* CreateEditingGrid(wxPanel* panel);
    wxMenuBar* CreateMenuBar();
    bool UpdateQueryHistory(std::string queryHistory);
    void LogSQLError(std::string errorMsg);

    wxListCtrl* m_TeamListView;
    wxListCtrl* m_MatchListView;
};