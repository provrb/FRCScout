#pragma once

// WX Components
#include <wx/wx.h> // wx Core
#include <wx/sizer.h> // wxBoxSizer
#include <wx/frame.h> // wxFrame
#include <wx/listctrl.h> // wxListCtrl
#include <wx/grid.h> // wxGrid

// Backend
#include "backend/team.h" // Team struct
#include "backend/match.h" // Match struct

/**
 * Global database used by the frontend to communicate
 * with the backend
 * 
 * Type: DataBase*
 * 
 * This is the method used to include DataBase
 * without circular dependencies and is how the frontend
 * will communicate with the backend.
 * 
 * The backend can directly communicate with the frontend
 * because it has a MainFrame* member variable.
 * This is initialized in the constructor of MainFrame
*/
static void* g_DataBase = nullptr;

/**
 * @class MainFrame
 * @brief The main user interface window for the application.
 *
 * This class represents the main frame of the application, which contains panels for displaying and editing
 * team and match data. It interacts with the backend to retrieve and manipulate data and provides a UI to
 * view and modify that data in a user-friendly way. The frame includes a list view for teams and matches,
 * a spreadsheet-like grid for editing, and a text box to display SQL query results.
 *
 * **Key Responsibilities:**
 * - Display a list of teams and matches with options to edit their details.
 * - Provide a menu bar for additional options like File and Export.
 * - Display SQL query results and errors.
 * - Handle the creation and management of UI components like lists and grids.
 *
 * The MainFrame class is tightly integrated with the backend components (such as `Team` and `Match`),
 * which provide the data for the UI elements. The database operations are executed, and relevant information
 * is displayed in the interface.
 */
class MainFrame : public wxFrame {
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
    void DisplayExistingData(); // display existing data from the db to ui

    wxGrid* CreateEditingGrid(wxPanel* panel); // create a spreedsheets-like grid to edit items
    void ShowTeamEditGrid(); // show options for editing a team struct in the editing grid
    void ShowMatchEditGrid(); // show options for editing a match struct in the editing grid
    void PromptTeamEdit(const Team& team);
    void PromptMatchEdit(const Match& match);
    void CreateTeamRow(const Team& team); // create a row with info from 'team' in teamListView
    void CreateMatchRow(const Match& match); // create a row with info from 'match' in matchListView
    wxMenuBar* CreateMenuBar(); // create menu bar which contains options like File, Export..
    const Team GetTeamFromRow(int row);
    const Match GetMatchFromRow(int row);

    wxTextCtrl* CreateSQLOutputBox(wxPanel* panel); // Create a wxTextCtrl that will show all SQL output
    void LogSQLQuery(std::string queryHistory); // add a completed query to SQL output
    void LogSQLError(std::string errorMsg); // print a red error message in SQL output with prefix "ERROR>"
    void LogBackendMessage(std::string msg); // print a blue message in SQL output with prefix "MSG>"

    void OnTeamRowClicked(wxCommandEvent& event);
    void OnMatchRowClicked(wxCommandEvent& event);
    void OnListViewRightClick(wxCommandEvent& event);
    void OnToggleEditMode(wxCommandEvent& event);
    void OnCreateNewTeam(wxCommandEvent& event);
    void OnCreateNewMatch(wxCommandEvent& event);

    bool m_isEditModeEnabled;
    int m_currentSelectedTeamRow = -1; // team number of team that is currently selected
    int m_currentSelectedMatchRow = -1; // match number of match that is currently selected
    int m_displayedTeamCount; // number of team rows in team list view
    int m_displayedMatchCount; // number of match rows in match list view
    wxListCtrl* m_teamListView; // container that holds rows about teams
    wxListCtrl* m_matchListView; // container that holds rows about matches
};