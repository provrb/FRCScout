// Frontend
#include "frontend/mainframe.h"

// Backend
#include "backend/data.h" // DataBase class
#include "backend/team.h"
#include "backend/match.h"

// STD
#include <fstream>

/*
    This file includes any and all events regarding 
    the MainFrame class such as button clicks, list
    item clicks, and right-click context menus.

    When making a new event handler, the function should
    be defined in the MainFrame class in mainframe.h, and
    the function body should be here in events.cpp
*/

/**
 * @brief Handles left-click events on a team row.
 *
 * Retrieves the selected row and displays relevant team information.
 * Prevents unnecessary recalculations if the same row is clicked again.
 *
 * @param event The wxCommandEvent triggered by the user clicking a team row.
 */
void MainFrame::OnTeamRowLeftClicked(wxCommandEvent& event) {
    // Check the row # that is selected
    int row = m_teamListView->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( m_currentSelectedTeamRow == row ) // information would already be displayed, don't bother recalculating
        return;

    m_currentSelectedTeamRow = row;
    const Team team = GetTeamFromRow(row);

    PromptTeamEdit(team);
}

/**
 * @brief Handles left-click events on a match row.
 *
 * Retrieves the selected row and displays relevant match information.
 * Prevents unnecessary recalculations if the same row is clicked again.
 *
 * @param event The wxCommandEvent triggered by the user clicking a match row.
 */
void MainFrame::OnMatchRowLeftClicked(wxCommandEvent& event) {
    int row = m_matchListView->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( m_displayedMatchCount > 2 && m_currentSelectedMatchRow == row )
        return;

    m_currentSelectedMatchRow = row;
    const Match match = GetMatchFromRow(row);

    PromptMatchEdit(match);
}

/**
 * @brief Handles right-click events on a team row.
 *
 * Displays a context menu with an option to delete the selected team.
 *
 * @param event The wxCommandEvent triggered by the user right-clicking a team row.
 */
void MainFrame::OnTeamRowRightClicked(wxCommandEvent& event) {
    wxMenu rightClickMenu;
    rightClickMenu.Append(wxID_DELETE, "Delete Team");
    rightClickMenu.Append(wxID_DUPLICATE, "Duplicate Team");

    rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnDeleteTeam, this, wxID_DELETE);
    rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnDuplicateTeam, this, wxID_DUPLICATE);

    PopupMenu(&rightClickMenu);
}

/**
 * @brief Handles right-click events on a match row.
 *
 * Displays a context menu with an option to delete the selected match.
 *
 * @param event The wxCommandEvent triggered by the user right-clicking a match row.
 */
void MainFrame::OnMatchRowRightClicked(wxCommandEvent& event) {
    wxMenu rightClickMenu;
    rightClickMenu.Append(wxID_DELETE, "Delete Match");
    rightClickMenu.Append(wxID_DUPLICATE, "Duplicate Match");

    rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnDeleteMatch, this, wxID_DELETE);
    rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnDuplicateMatch, this, wxID_DUPLICATE);

    PopupMenu(&rightClickMenu);
}

/**
 * @brief Handles right-click events on a list view.
 *
 * Determines whether the user clicked on the team or match list view,
 * and presents a corresponding context menu with creation options.
 *
 * @param event The wxCommandEvent triggered by the user right-clicking a list view.
 */
void MainFrame::OnListViewRightClick(wxCommandEvent& event) {
    const int eventId = event.GetId();
    wxMenu rightClickMenu;

    if ( eventId == kTeamListView ) {
        rightClickMenu.Append(wxID_NEW, "Create New Team");
        rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnCreateNewTeam, this, wxID_NEW);
    }
    else if ( eventId == kMatchListView ) {
        rightClickMenu.Append(wxID_NEW, "Create New Match");
        rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnCreateNewMatch, this, wxID_NEW);
    }

    PopupMenu(&rightClickMenu);
}

/**
 * @brief Toggles the edit mode for modifying team or match details.
 *
 * Enables or disables editing mode, updates UI elements accordingly,
 * and modifies the button label based on the current mode.
 *
 * @param event The wxCommandEvent triggered by toggling edit mode.
 */
void MainFrame::OnToggleEditMode(wxCommandEvent& event) {
    m_isEditModeEnabled = !m_isEditModeEnabled; // invert edit mode on toggle

    // get grid object
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    if ( !grid )
        return;

    grid->EnableEditing(m_isEditModeEnabled);

    // Get title and description
    wxStaticText* gridTitle = ( wxStaticText* ) FindWindow(kEditingDataTitle);
    wxStaticText* gridDesc = ( wxStaticText* ) FindWindow(kEditingDataDesc);

    // Default text
    gridTitle->SetLabelText("Edit and View");
    gridDesc->SetLabelText("Edit and view fields of objects");

    // get button
    wxButton* editModeButton = ( wxButton* ) FindWindow(kEditModeButton);

    // check if editing a team or match
    wxString colText = grid->GetRowLabelValue(0); // 0 will contain "Match #" or "Team #"
    if ( colText.Contains("Team") ) {
        // editing team
        wxString teamNumber = grid->GetCellValue(0, 0);

        if ( m_isEditModeEnabled ) {
            gridTitle->SetLabelText("Editing Team # " + teamNumber);
            gridDesc->SetLabelText("Editing all fields for team # " + teamNumber);
        }
        else {
            gridTitle->SetLabelText("Viewing Team # " + teamNumber);
            gridDesc->SetLabelText("Viewing all fields for team # " + teamNumber);
        }
    }
    else if ( colText.Contains("Match") ) {
        // editing match
        wxString matchNumber = grid->GetCellValue(0, 0);

        if ( m_isEditModeEnabled ) {
            gridTitle->SetLabelText("Editing Match # " + matchNumber);
            gridDesc->SetLabelText("Editing all fields for match # " + matchNumber);
        }
        else {
            gridTitle->SetLabelText("Viewing Match # " + matchNumber);
            gridDesc->SetLabelText("Viewing all fields for match # " + matchNumber);
        }
    }

    ( m_isEditModeEnabled ) ? editModeButton->SetLabelText("View") : editModeButton->SetLabelText("Edit");
}

/**
 * @brief Creates and inserts a new team entry.
 *
 * Initializes a new team, inserts it into the list view, and attempts
 * to save it in the database if available. Also prompts for team edits.
 *
 * @param event The wxCommandEvent triggered when creating a new team.
 */
void MainFrame::OnCreateNewTeam(wxCommandEvent& event) {
    // check if database is active and add it to database, otherwise return
    if ( !g_DataBase ) {
        LogMessage("Database not available, cannot save team. Closing this app will delete all progress.", *wxRED);
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );

    Team team = {};
    team.uid = db->GetNextTeamUID();
    team.teamNum = m_displayedTeamCount + 1;

    CreateTeamRow(team);
    db->AddTeam(team);
    PromptTeamEdit(team);
}

/**
 * @brief Creates and inserts a new match entry.
 *
 * Initializes a new match, inserts it into the list view, and attempts
 * to save it in the database if available. Also prompts for match edits.
 *
 * @param event The wxCommandEvent triggered when creating a new match.
 */
void MainFrame::OnCreateNewMatch(wxCommandEvent& event) {
    Match match = {};
    match.matchNum = m_displayedMatchCount + 1;

    CreateMatchRow(match);

    // data base check
    if ( !g_DataBase ) {
        LogMessage("Database not available, cannot save team. Closing this app will delete all progress.", *wxRED);
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->AddMatch(match);

    PromptMatchEdit(match);
}

/**
 * @brief Duplicates the currently selected team.
 *
 * Retrieves the team data from the selected row, assigns a new unique identifier,
 * and adds it to the database. The duplicated team is then displayed in the UI
 * and opened for editing.
 *
 * @param event The `wxCommandEvent` triggered by the duplication action.
 */
void MainFrame::OnDuplicateTeam(wxCommandEvent& event) {
    // data base check
    if ( !g_DataBase ) {
        LogMessage("Database not available, cannot save team.", *wxRED);
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    const Team team = GetTeamFromRow(m_currentSelectedTeamRow);

    Team newTeam = team;
    newTeam.uid = db->GetNextTeamUID();

    db->AddTeam(newTeam);

    CreateTeamRow(newTeam);
    PromptTeamEdit(newTeam);
}

/**
 * @brief Duplicates the currently selected match.
 *
 * Retrieves the match data from the selected row, assigns a new match number,
 * and adds it to the database. The duplicated match is then displayed in the UI
 * and opened for editing.
 *
 * @param event The `wxCommandEvent` triggered by the duplication action.
 */
void MainFrame::OnDuplicateMatch(wxCommandEvent& event) {
    const Match match = GetMatchFromRow(m_currentSelectedMatchRow);

    Match newMatch = match;
    newMatch.matchNum = m_displayedMatchCount + 1;
    CreateMatchRow(newMatch);

    // data base check
    if ( !g_DataBase ) {
        LogMessage("Database not available, cannot save team. Closing this app will delete all progress.", *wxRED);
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->AddMatch(newMatch);
    PromptMatchEdit(newMatch);
}

/**
 * @brief Deletes the currently selected team.
 *
 * Prompts the user for confirmation, removes the team from the database
 * if available, and updates the list view accordingly.
 *
 * @param event The wxCommandEvent triggered when deleting a team.
 */
void MainFrame::OnDeleteTeam(wxCommandEvent& event) {
    int opt = MessageBoxA(NULL, "Delete Team", "Are you sure?", MB_YESNOCANCEL | MB_ICONWARNING);
    if ( opt != IDYES )
        return;

    // get team number
    const Team team = GetTeamFromRow(m_currentSelectedTeamRow);
    if ( team.teamNum == 0 )
        return;

    // remove team from database
    if ( !g_DataBase ) {
        LogBackendMessage("Database not available, cannot delete team.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->RemoveTeam(team.uid);

    // remove team from list view
    m_teamListView->DeleteItem(m_currentSelectedTeamRow);
    m_displayedTeamCount--;

    LogMessage("Successfully deleted team " + std::to_string(team.teamNum) + "\n\n", *wxRED);
}

/**
 * @brief Deletes the currently selected match.
 *
 * Prompts the user for confirmation, removes the match from the database
 * if available, and updates the list view accordingly.
 *
 * @param event The wxCommandEvent triggered when deleting a match.
 */
void MainFrame::OnDeleteMatch(wxCommandEvent& event) {
    int opt = MessageBoxA(NULL, "Delete Match", "Are you sure?", MB_YESNOCANCEL | MB_ICONWARNING);
    if ( opt != IDYES )
        return;

    // get match number
    const Match match = GetMatchFromRow(m_currentSelectedMatchRow);
    if ( match.matchNum == 0 )
        return;

    // remove match from database
    if ( !g_DataBase ) {
        LogBackendMessage("Database not available, cannot delete match.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->RemoveMatch(match.matchNum);

    // remove match from list view
    m_matchListView->DeleteItem(m_currentSelectedMatchRow);
    m_displayedMatchCount--;

    LogMessage("Successfully deleted match " + std::to_string(match.matchNum) + "\n\n", *wxRED);
}

/**
 * @brief Exports team data to a CSV file and generates a QR code for the data.
 *
 * This function opens a file dialog for the user to select the path and file name to save the team data as a CSV file.
 * It checks if the database is available, and if not, logs an error message. If the database is available, it exports
 * the team data from the database to the CSV file. After saving the file, it reads the CSV file into a string and generates
 * a QR code based on the CSV data, saving the QR code as an image file ("TeamData.png").
 *
 * @param event The wxCommandEvent triggered by the user action (e.g., button click).
 */
void MainFrame::OnExportTeamDataCSV(wxCommandEvent& event) {
    wxFileDialog fileDialog(this, "Save Team Data as CSV", "", "", "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    int res = fileDialog.ShowModal();
    if ( res != wxID_OK )
        return;

    wxString path = fileDialog.GetPath();

    if ( !g_DataBase ) {
        LogSQLError("Database not available, cannot export team data.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->ExportTableToCSV(TEAM_TABLE, path.ToStdString());

    // Read csv file into a string
    std::ifstream file(path.ToStdString());
    std::string csvData(( std::istreambuf_iterator< char >(file) ), std::istreambuf_iterator< char >());
    file.close();

    db->ExportTOQRCode(csvData, "TeamData.png");
}

/**
 * @brief Exports match data to a CSV file and generates a QR code for the data.
 *
 * This function opens a file dialog for the user to select the path and file name to save the match data as a CSV file.
 * It checks if the database is available, and if not, logs an error message. If the database is available, it exports
 * the match data from the database to the CSV file. After saving the file, it reads the CSV file into a string and generates
 * a QR code based on the CSV data, saving the QR code as an image file ("MatchData.png").
 *
 * @param event The wxCommandEvent triggered by the user action (e.g., button click).
 */
void MainFrame::OnExportMatchDataCSV(wxCommandEvent& event) {
    wxFileDialog fileDialog(this, "Save Match Data as CSV", "", "", "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    int res = fileDialog.ShowModal();
    if ( res != wxID_OK )
        return;

    wxString path = fileDialog.GetPath();

    if ( !g_DataBase ) {
        LogSQLError("Database not available, cannot export team data.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->ExportTableToCSV(MATCH_TABLE, path.ToStdString());

    // Read csv file into a string
    std::ifstream file(path.ToStdString());
    std::string csvData(( std::istreambuf_iterator< char >(file) ), std::istreambuf_iterator< char >());
    file.close();

    db->ExportTOQRCode(csvData, "MatchData.png");
}

/**
 * @brief Exports team data from the database to a JSON file.
 *
 * Opens a file dialog for the user to specify the save location. If the database
 * is available, it exports the `TEAM_TABLE` data to the specified JSON file.
 *
 * @param event The `wxCommandEvent` triggered by the export action.
 */
void MainFrame::OnExportTeamDataJSON(wxCommandEvent& event) {
    wxFileDialog fileDialog(this, "Save Team Data as JSON", "", "", "JSON files (*.json)|*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    int res = fileDialog.ShowModal();
    if ( res != wxID_OK )
        return;

    wxString path = fileDialog.GetPath();
    if ( !g_DataBase ) {
        LogSQLError("Database not available, cannot export team data.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->ExportTableToJSON(TEAM_TABLE, path.ToStdString());
}

/**
 * @brief Exports match data from the database to a JSON file.
 *
 * Opens a file dialog for the user to specify the save location. If the database
 * is available, it exports the `MATCH_TABLE` data to the specified JSON file.
 *
 * @param event The `wxCommandEvent` triggered by the export action.
 */
void MainFrame::OnExportMatchDataJSON(wxCommandEvent& event) {
    wxFileDialog fileDialog(this, "Save Match Data as JSON", "", "", "JSON files (*.json)|*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    int res = fileDialog.ShowModal();
    if ( res != wxID_OK )
        return;

    wxString path = fileDialog.GetPath();

    if ( !g_DataBase ) {
        LogSQLError("Database not available, cannot export team data.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->ExportTableToJSON(MATCH_TABLE, path.ToStdString());
}

/**
 * @brief Handles changes to cells in the wxGrid.
 *
 * This function is triggered when a cell in the wxGrid is modified. It determines
 * whether the user is editing team data or match data based on the row label,
 * updates the corresponding `Team` or `Match` object, and updates the database
 * accordingly.
 *
 * @param event The wxGridEvent containing information about the changed cell.
 *
 * @note The function assumes that cell values can be safely converted to integers
 *       where applicable. It does not handle conversion errors.
 */
void MainFrame::OnGridCellChange(wxGridEvent& event) {
    int row = event.GetRow();
    int col = event.GetCol();
    wxGrid* grid = reinterpret_cast< wxGrid* >( event.GetEventObject() );

    if ( !grid )
        return;

    if ( !g_DataBase )
        return;

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );

    wxString val = grid->GetCellValue(row, col);
    bool editingTeam = grid->GetRowLabelValue(kRowTeamNum).Contains("Team");

    if ( editingTeam ) {
        Team team = GetTeamFromRow(m_currentSelectedTeamRow);
        switch ( row ) {
        case kRowTeamNum:
            // team number
            team.teamNum = std::stoi(val.ToStdString());
            break;
        case kRowOverall:
            // overall score
            team.overall = std::stoi(val.ToStdString());
            break;
        case kRowEliminated:
            // eliminated status
            team.eliminated = ( val == "Y" );
            break;
        case kRowHangAttempt:
            // hang attempt
            team.hangAttempt = ( val == "Y" );
            break;
        case kRowHangSuccess:
            // hang success
            team.hangSuccess = ( val == "Y" );
            break;
        case kRowRobotCycleSpeed:
            // robot cycle speed
            team.robotCycleSpeed = std::stoi(val.ToStdString());
            break;
        case kRowCoralPoints:
            // coral points
            team.coralPoints = std::stoi(val.ToStdString());
            break;
        case kRowDefense:
            // defense
            team.defense = std::stoi(val.ToStdString());
            break;
        case kRowAutonomousPoints:
            // autonomous points
            team.autonomousPoints = std::stoi(val.ToStdString());
            break;
        case kRowDriverSkill:
            // driver skill
            team.driverSkill = std::stoi(val.ToStdString());
            break;
        case kRowFouls:
            // fouls
            team.fouls = std::stoi(val.ToStdString());
            break;
        case kRowRankingPoints:
            // ranking points
            team.rankingPoints = std::stoi(val.ToStdString());
            break;
        case kRowPPM:
            // points per match
            team.ppm = std::stoi(val.ToStdString());
            break;
        }

        db->UpdateTeam(team);
        RefreshTeamRow(team.uid);
        return;
    }

    Match match = GetMatchFromRow(m_currentSelectedMatchRow);
    switch ( row ) {
    case kRowMatchNum:
        // match number
        match.matchNum = std::stoi(val.ToStdString());
        break;
    case kRowPlayed:
        // played status
        match.played = ( val == "Y" );
        break;
    case kRowRedWin:
        // red win status
        match.redWin = ( val == "Y" );
        break;
    case kRowBlueWin:
        // blue win status
        match.blueWin = ( val == "Y" );
        break;
    case kRowTeam1:
        match.teams[0].teamNum = std::stoi(val.ToStdString());
        break;
    case kRowTeam2:
        match.teams[1].teamNum = std::stoi(val.ToStdString());
        break;
    case kRowTeam3:
        match.teams[2].teamNum = std::stoi(val.ToStdString());
        break;
    case kRowTeam4:
        match.teams[3].teamNum = std::stoi(val.ToStdString());
        break;
    case kRowTeam5:
        match.teams[4].teamNum = std::stoi(val.ToStdString());
        break;
    case kRowTeam6:
        match.teams[5].teamNum = std::stoi(val.ToStdString());
        break;
    }

    db->UpdateMatch(match);
    RefreshMatchRow(match.matchNum);
}

/**
 * @brief Handles the event when an "Add" button is clicked.
 *
 * This function retrieves the button that triggered the event and checks its
 * associated client data to determine which action to perform. If the button's
 * client data corresponds to `kTeamListView`, a new team is created. If it
 * corresponds to `kMatchListView`, a new match is created.
 *
 * @param event The `wxCommandEvent` triggered by the button click.
 */
void MainFrame::OnAddButton(wxCommandEvent& event) {
    wxButton* addButton = static_cast< wxButton* >( event.GetEventObject() );

    if ( !addButton ) 
        return;

    int buttonData = reinterpret_cast< std::intptr_t >( addButton->GetClientData() );

    if ( buttonData == kTeamListView )
        OnCreateNewTeam(event);
    else if ( buttonData == kMatchListView )
        OnCreateNewMatch(event);
}
