#pragma once

// WX Components
#include <wx/wx.h> // wx Core
#include <wx/sizer.h> // wxBoxSizer
#include <wx/frame.h> // wxFrame
#include <wx/listctrl.h> // wxListCtrl
#include <wx/grid.h> // wxGrid
#include <wx/sysopt.h>

// Backend
#include "backend/team.h" // Team struct
#include "backend/match.h" // Match struct

// Frontend
#include "frontend/wxids.h"
#include "frontend/colours.h" // Common wxColours

#define APP_NAME "FRCScout"

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
public:
    friend class DataBase; // Database needs to access functions to display backend data on the UI

    explicit MainFrame(const wxString& title, bool darkModeEnabled);

    // Logging
    void LogMessage(std::string& msg, wxColour colour = *wxBLACK); // log a message to the SQL output with text colour 'colour'
    void LogSQLQuery(std::string query); // add a completed query to SQL output
    void LogErrorMessage(std::string errorMsg); // print a red error message in output with prefix "ERROR>"
    void LogBackendMessage(std::string msg); // print a blue message in SQL output with prefix "MSG>"
private:
    // Initialization
    void DisplayExistingData(); // display already existing data from the db to ui

    inline void UpdateStatusBar(); // update the text in the status bar as well as background colour.

    // Panels
    wxBoxSizer* CreateListPanel(
        wxWindow* parent,
        int listId,
        wxString titleName,
        wxString description,
        int ySize
    );

    // Columns
    void AddTeamListColumns(); // Add header columns to m_teamListView
    void AddMatchListColumns(); // Add header columns to m_matchListView

    // Editing grid UI
    wxBoxSizer* CreateEditingGrid(wxPanel* panel); // create a spreedsheets-like grid to edit items
    void ShowTeamEditGrid(); // show options for editing a team struct in the editing grid
    void ShowMatchEditGrid(); // show options for editing a match struct in the editing grid
    void PromptTeamEdit(const Team& team);
    void PromptMatchEdit(const Match& match);
    void DefaultEditGrid(); // set the editing grid to default values

    // Create data
    void CreateTeamRow(const Team& team); // create a row with info from 'team' in teamListView
    void CreateMatchRow(const Match& match); // create a row with info from 'match' in matchListView
    void RefreshTeamRow(int uid);
    void RefreshMatchRow(int matchNum);
    void FillMatchRow(int row, const Match& match);
    void FillTeamRow(int row, const Team& team);
    wxMenuBar* CreateMenuBar(); // create menu bar which contains options like File, Export..
    const Team GetTeamFromRow(int row);
    const Match GetMatchFromRow(int row);
    const int GetSelectedRowMatchNum(); // get the match num of the selected match row

    // Logging
    wxBoxSizer* CreateSQLOutputBox(wxPanel* panel); // Create a wxTextCtrl that will show all SQL output
    void ClearOutput(wxCommandEvent&);

    // Events (events.cpp)
    void OnTeamRowLeftClicked(wxCommandEvent& event);
    void OnMatchRowLeftClicked(wxCommandEvent& event);
    void OnTeamRowRightClicked(wxCommandEvent& event);
    void OnMatchRowRightClicked(wxCommandEvent& event);
    void OnListViewRightClick(wxCommandEvent& event);
    void OnToggleEditMode(wxCommandEvent& event);
    void OnCreateNewTeam(wxCommandEvent& event);
    void OnCreateNewMatch(wxCommandEvent& event);
    void OnDuplicateMatch(wxCommandEvent& event);
    void OnDuplicateTeam(wxCommandEvent& event);
    void OnDeleteTeam(wxCommandEvent& event);
    void OnDeleteMatch(wxCommandEvent& event);
    void OnExportTeamDataCSV(wxCommandEvent& event);
    void OnExportMatchDataCSV(wxCommandEvent& event);
    void OnExportTeamDataJSON(wxCommandEvent& event);
    void OnExportMatchDataJSON(wxCommandEvent& event);
    void OnGridCellChange(wxGridEvent& event);
    void OnAddButton(wxCommandEvent& event);
    void OnImportTeamDataCSV(wxCommandEvent& event);
    void OnImportMatchDataCSV(wxCommandEvent& event);
    void OnPredictMatch(wxCommandEvent& event);

    bool m_darkModeTheme; 
    bool m_isEditModeEnabled;
    int m_selectedTeamRow = -1; // team number of team that is currently selected
    int m_selectedMatchRow = -1; // match number of match that is currently selected
    int m_displayedTeamCount; // number of team rows in team list view
    int m_displayedMatchCount; // number of match rows in match list view
    wxListCtrl* m_teamListView; // container that holds rows about teams
    wxListCtrl* m_matchListView; // container that holds rows about matches

    /**
     * Ddatabase used by the frontend to communicate
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
    void* m_dataBase = nullptr;

    void* m_predictor = nullptr;
};