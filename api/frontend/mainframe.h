#pragma once

// WX Components
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/grid.h>

// Backend
#include "backend/team.h"
#include "backend/match.h"

class MainFrame : public wxFrame
{
private:
    friend class DataBase; // Database needs to access functions to display backend data on the UI
    friend class App; // App should be the only thing calling MainFrame constructor

    MainFrame(const wxString& title);
private:
    wxBoxSizer* CreateListPanel(
        wxWindow* parent,
        int listId,
        wxString titleName,
        wxString description
    );

    void AddTeamListColumns(); // Add header columns to m_teamListView
    void AddMatchListColumns(); // Add header columns to m_matchListView

    wxGrid* CreateEditingGrid(wxPanel* panel); // create a spreedsheets-like grid to edit items
    void ShowTeamEditGrid(); // show options for editing a team struct in the editing grid
    void ShowMatchEditGrid(); // show options for editing a match struct in the editing grid
    void CreateTeamRow(const Team& team); // create a row with info from 'team' in teamListView
    void CreateMatchRow(const Match& match); // create a row with info from 'match' in matchListView
    wxMenuBar* CreateMenuBar(); // create menu bar which contains options like File, Export..

    wxTextCtrl* CreateSQLHistoryBox(wxPanel* panel); // Create a wxTextCtrl that will show all SQL output
    bool UpdateQueryHistory(std::string queryHistory); // add a completed query to SQL output
    void LogSQLError(std::string errorMsg); // print a red error message in SQL output with prefix "ERROR>"
    void LogBackendMessage(std::string msg); // print a blue message in SQL output with prefix "MSG>"

    int m_displayedTeamCount; // number of team rows in team list view
    int m_displayedMatchCount; // number of match rows in match list view
    wxListCtrl* m_teamListView; // container that holds rows about teams
    wxListCtrl* m_matchListView; // container that holds rows about matches
};