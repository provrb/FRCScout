#ifdef _USING_UI

// Frontend
#include "frontend/mainframe.h" // MainFrame class
#include "frontend/wxids.h" // WinId enum
#include "frontend/colours.h" // Common wxColours

// Backend
#include "backend/data.h" // DataBase class

/**
 * @brief Constructor for the MainFrame class, initializing the main window with a specified title.
 *
 * This constructor sets up the main frame of the application, creating and arranging the UI components.
 * It creates a panel with a horizontal sizer that contains:
 * - Two list panels (for teams and matches) arranged vertically on the left side.
 * - A grid and a text box on the right side for editing data.
 *
 * The constructor also sets up the menu bar and initializes a `DataBase` object for interacting with the SQLite database.
 *
 * @param title The title of the window.
 *
 * @note The initial window size is set to 800x600 pixels. The list panels display views for teams and matches,
 *       and the grid allows for editing of individual fields. The text box below the grid displays SQL query results.
 */
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) // Initial window size
{
    // Create global database
    DataBase* db = new DataBase("data.db", this);
    g_DataBase = reinterpret_cast< void* >( db );

    // Panels
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);

    // Create two list panels
    leftSizer->Add(CreateListPanel(panel, kTeamListView, "Teams", "View and edit specific fields of any team."), 1, wxEXPAND | wxALL, 10);
    leftSizer->Add(CreateListPanel(panel, kMatchListView, "Matches", "View and modify individual fields of a match."), 1, wxEXPAND | wxALL, 10);

    // Get list views
    m_teamListView = ( wxListCtrl* ) FindWindow(kTeamListView);
    m_matchListView = ( wxListCtrl* ) FindWindow(kMatchListView);

    // Add columns to list views
    AddTeamListColumns();
    AddMatchListColumns();

    // Create the grid panel on the right side
    wxPanel* rightPanel = new wxPanel(panel, wxID_ANY);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    wxGrid* grid = CreateEditingGrid(rightPanel);
    wxTextCtrl* sqlOutput = CreateSQLOutputBox(rightPanel);

    // Add grid to the rightSizer
    rightSizer->Add(grid, 1, wxEXPAND | wxALL, 10);
    rightSizer->Add(sqlOutput, 1, wxEXPAND | wxALL, 10);  // Text box will share the same space and dimensions

    // Add the leftSizer (team list & match list) and rightSizer (grid) to the mainSizer
    mainSizer->Add(leftSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(rightPanel, 0, wxEXPAND | wxALL, 10);

    // Set sizer for the panel
    panel->SetSizer(mainSizer);
    this->Layout();

    // Add menu bar
    CreateMenuBar();
    
    DisplayExistingData();
}

/**
 * @brief Creates a panel with a list view, title, and description.
 *
 * This function constructs a panel that contains a title, description, and a list view. It uses sizers to arrange
 * the elements in a vertical layout, where the title and description are stacked on top of the list view. The list
 * view is created with a specified width and a fixed height of 200px. The function also sets the font for the title
 * and description texts, with the title being bold and the description using a normal font weight.
 *
 * The layout is as follows:
 * - A vertical stack that contains:
 *   - A horizontal stack for the title and description
 *   - A list view (`wxListCtrl`) that fills the remaining space of the panel.
 *
 * The function returns a `wxBoxSizer` that contains the entire layout.
 *
 * @param parent The parent window for the list panel.
 * @param listId The ID to be used for the list control.
 * @param titleName The title text to be displayed above the list view.
 * @param description A brief description of the list view displayed under the title.
 *
 * @return A pointer to a `wxBoxSizer` that contains the list panel layout.
 *
 * @note The width of the list view is calculated as 1.3 times the width of the parent window.
 *       The list view is set to have a height of 200px.
 */
wxBoxSizer* MainFrame::CreateListPanel(wxWindow* parent, int listId, wxString titleName, wxString description) {
    int listWidth = this->GetSize().GetWidth() * 1.27;

    // Sizers
    wxBoxSizer* listSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);  // New vertical sizer for title and description

    // Title and description
    wxStaticText* titleText = new wxStaticText(parent, wxID_ANY, titleName, wxDefaultPosition, wxDefaultSize, 0);
    titleText->SetFont(wxFontInfo(18).Bold());

    wxStaticText* descText = new wxStaticText(parent, wxID_ANY, description, wxDefaultPosition, wxDefaultSize, 0);
    descText->SetFont(wxFontInfo(10));

    // Add title and description to the vertical sizer
    textSizer->Add(titleText, 0, wxALIGN_LEFT);
    textSizer->Add(descText, 0, wxALIGN_LEFT | wxTOP, 2);  // Small space between title and description

    // Add elements to the horizontal top sizer
    topSizer->Add(textSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);  // Add title + desc stack
    topSizer->AddSpacer(10);  // Some space before buttons

    // List view
    wxListCtrl* listCtrl = new wxListCtrl(parent, listId, wxDefaultPosition, wxSize(listWidth, 200), wxLC_REPORT);

    // Add topSizer and list view to listSizer
    listSizer->Add(topSizer, 0, wxEXPAND | wxBOTTOM, 5);
    listSizer->Add(listCtrl, 1, wxEXPAND);

    listCtrl->Bind(wxEVT_CONTEXT_MENU, &MainFrame::OnListViewRightClick, this);

    return listSizer;
}

/**
 * @brief Adds columns to the team list view with specific headers and widths.
 *
 * This function adds columns to the team list view (`m_teamListView`) by first checking if the view exists.
 * It sets the font for the column headers to be bold and then appends columns to display team-related information.
 * Each column is given a header label, aligned appropriately (right or center), and a width proportional to the
 * overall width of the team list view.
 *
 * The widths of the columns are dynamically set based on the total width of the team list view.
 *
 * @note This function does not return any value.
 */
void MainFrame::AddTeamListColumns() {
    if ( !m_teamListView )
        return;

    const int teamListWidth = m_teamListView->GetSize().GetWidth();

    // Set bold column headers
    m_teamListView->SetFont(wxFontInfo(9).Bold());

    // Add team list columns
    m_teamListView->AppendColumn("Team #", wxLIST_FORMAT_RIGHT, teamListWidth * 0.055);
    m_teamListView->AppendColumn("OVR", wxLIST_FORMAT_CENTER, teamListWidth * 0.05);
    m_teamListView->AppendColumn("Out", wxLIST_FORMAT_CENTER, teamListWidth * 0.05);
    m_teamListView->AppendColumn("Hang Attempt", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    m_teamListView->AppendColumn("Hang Success", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    m_teamListView->AppendColumn("Robot Cycle Speed", wxLIST_FORMAT_CENTER, teamListWidth * 0.12);
    m_teamListView->AppendColumn("Coral Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.085);
    m_teamListView->AppendColumn("Defense", wxLIST_FORMAT_CENTER, teamListWidth * 0.06);
    m_teamListView->AppendColumn("Auto. Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.085);
    m_teamListView->AppendColumn("Driver Skill", wxLIST_FORMAT_CENTER, teamListWidth * 0.08);
    m_teamListView->AppendColumn("Fouls", wxLIST_FORMAT_CENTER, teamListWidth * 0.06);
    m_teamListView->AppendColumn("Ranking Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    m_teamListView->AppendColumn("PPM", wxLIST_FORMAT_CENTER, teamListWidth * 0.06); // points per match
}

/**
 * @brief Adds columns to the match list view with specific headers and widths.
 *
 * This function adds columns to the match list view (`m_matchListView`) by first checking if the view exists.
 * It sets the font for the column headers to be bold and then appends columns to display match information.
 * Each column is given a header label, aligned at the center, and a width proportional to the overall width of
 * the list view.
 *
 * The columns added are:
 * - Match #
 * - Finished
 * - Red Win
 * - Blue Win
 * - Team 1 # to Team 6 #
 *
 * The widths of the columns are dynamically set based on the total width of the match list view.
 *
 * @note This function does not return any value.
 */
void MainFrame::AddMatchListColumns() {
    if ( !m_matchListView )
        return;

    const int matchListWidth = m_matchListView->GetSize().GetWidth();

    // Set bold column headers
    m_matchListView->SetFont(wxFontInfo(9).Bold());

    // Add match list columns
    m_matchListView->AppendColumn("Match #", wxLIST_FORMAT_CENTER, matchListWidth * 0.08);
    m_matchListView->AppendColumn("Finished", wxLIST_FORMAT_CENTER, matchListWidth * 0.08);
    m_matchListView->AppendColumn("Red Win", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_matchListView->AppendColumn("Blue Win", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_matchListView->AppendColumn("Team 1 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_matchListView->AppendColumn("Team 2 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_matchListView->AppendColumn("Team 3 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_matchListView->AppendColumn("Team 4 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_matchListView->AppendColumn("Team 5 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_matchListView->AppendColumn("Team 6 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
}

/**
 * @brief Populates the UI with existing team and match data from the database.
 *
 * This function retrieves all teams and matches from the database (`g_DataBase`)
 * and updates the UI list views (`m_teamListView` and `m_matchListView`).
 * If either of these list views is uninitialized or if the global database
 * pointer (`g_DataBase`) is null, the function returns early.
 *
 * @note This function assumes `g_DataBase` is a valid pointer to a `DataBase` instance.
 *       If `g_DataBase` is incorrectly set or uninitialized, this may lead to undefined behavior.
 *
 * @warning `g_DataBase` is a `void*` and requires a `reinterpret_cast` to `DataBase*`.
 *          Ensure `g_DataBase` actually points to a valid `DataBase` object before calling this function.
 *
 */
void MainFrame::DisplayExistingData() {
    if ( !m_teamListView || !g_DataBase )
        return;

    m_teamListView->DeleteAllItems();

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase ); // cast database

    // show teams
    std::vector<Team> teams = db->GetTeams();
    for ( const auto& team : teams )
        CreateTeamRow(team);

    if ( !m_matchListView )
        return;

    m_matchListView->DeleteAllItems();

    // show matches
    std::vector<Match> matches = db->GetMatches();
    for ( const auto& match : matches )
        CreateMatchRow(match);
}

/**
 * @brief Creates a grid for editing values with labels and predefined settings.
 *
 * This function creates a grid within the specified `panel`, which is used for editing values. It configures
 * the grid with 35 rows and 1 column, and sets various options such as column and row sizes, font styles,
 * label background color, and label alignment. The function also disables resizing for both rows and columns.
 * Additionally, a title and description are added above the grid.
 *
 * @param panel The wxPanel in which the editing grid will be created.
 *
 * @return wxGrid* A pointer to the created grid for editing values.
 */
wxGrid* MainFrame::CreateEditingGrid(wxPanel* panel) {
    // Create the grid
    wxGrid* grid = new wxGrid(panel, kEditItemGrid, wxPoint(0, 65), wxSize(400, 900));
    grid->CreateGrid(35, 1);

    // Options
    grid->SetColLabelValue(0, "Value");
    grid->DisableColResize(0);
    grid->SetLabelBackgroundColour(wxColour(255, 255, 255));
    grid->SetRowLabelAlignment(wxALIGN_LEFT, wxALIGN_TOP);
    grid->SetLabelFont(wxFontInfo(9).Bold());

    // Sizing
    grid->SetRowLabelSize(150);
    grid->SetColSize(0, 230);
    grid->SetRowSize(35, 31);

    // Set row size, disable row resize, and set default row value for all rows
    for ( int i = 0; i < 36; i++ ) {
        if ( i % 2 == 0 ) {
            grid->SetLabelBackgroundColour(LIGHT_GRAY_ACCENT_1);
            grid->SetCellBackgroundColour(i, 0, LIGHT_GRAY_ACCENT_1);
        }
        else {
            grid->SetLabelBackgroundColour(LIGHT_GRAY_ACCENT_2);
            grid->SetCellBackgroundColour(i, 0, LIGHT_GRAY_ACCENT_2);
        }
        grid->DisableRowResize(i);
        grid->SetRowSize(i, 25);
        grid->SetRowLabelValue(i, "...");
    }

    grid->Bind(wxEVT_GRID_CELL_CHANGED, &MainFrame::OnGridCellChange, this);

    // Title above the grid
    wxStaticText* gridTitle = new wxStaticText(panel, kEditingDataTitle, "Edit and View", wxPoint(0, 10), wxDefaultSize, 0);
    gridTitle->SetFont(wxFontInfo(18).Bold());

    // Description above the grid
    wxStaticText* gridDesc = new wxStaticText(panel, kEditingDataDesc, "Edit and view fields of objects", wxPoint(0, 41), wxDefaultSize, 0);
    gridDesc->SetFont(wxFontInfo(10));

    // Add edit and view mode button
    wxButton* editModeButton = new wxButton(panel, kEditModeButton, "Edit Mode", wxPoint(305, 15), wxDefaultSize);
    editModeButton->SetBackgroundColour(LIGHT_GRAY_ACCENT_3);
    editModeButton->SetFont(wxFontInfo(8));
    editModeButton->Bind(wxEVT_BUTTON, &MainFrame::OnToggleEditMode, this);

    return grid;
}

/**
 * @brief Creates a read-only SQL output text box in the specified panel.
 *
 * This function creates a multi-line, read-only text box within the provided `rightPanel`.
 *
 * @param rightPanel The wxPanel in which the SQL output text box will be placed.
 * @return wxTextCtrl* A pointer to the created SQL output text box.
 */
wxTextCtrl* MainFrame::CreateSQLOutputBox(wxPanel* rightPanel) {
    // Create a text box that is read only
    wxTextCtrl* sqlOutput = new wxTextCtrl(
        rightPanel,
        kSQLHistoryTextBox,
        wxEmptyString,
        wxPoint(425, 65),
        wxSize(400, 900),
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2
    );

    // Create font to use in the SQL output text box
    wxFont font(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
    font.Scale(.9);

    // Set the font
    sqlOutput->SetFont(font);

    return sqlOutput;
}

/**
 * @brief Sets up and displays the grid for editing team data.
 *
 * This function configures the grid for editing team data by setting row labels with appropriate field names.
 * The labels include fields such as team number, overall score, elimination status, robot cycle speed,
 * defense, fouls, and ranking points. The function first checks if the grid is valid (i.e., not null) before
 * proceeding to set the labels.
 */
void MainFrame::ShowTeamEditGrid() {
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    if ( !grid )
        return;

    grid->SetReadOnly(kRowTeamNum, 0, false);
    grid->SetRowLabelValue(kRowTeamNum, "Team #");
    grid->SetRowLabelValue(kRowOverall, "Overall (0-100)");
    grid->SetRowLabelValue(kRowEliminated, "Eliminated (Y/N)");
    grid->SetRowLabelValue(kRowHangAttempt, "Hang Attempt (Y/N)");
    grid->SetRowLabelValue(kRowHangSuccess, "Hang Success (Y/N)");
    grid->SetRowLabelValue(kRowRobotCycleSpeed, "Robot Cycle Speed");
    grid->SetRowLabelValue(kRowCoralPoints, "Coral Points");
    grid->SetRowLabelValue(kRowDefense, "Defense (0-100)");
    grid->SetRowLabelValue(kRowAutonomousPoints, "Auto. Points");
    grid->SetRowLabelValue(kRowDriverSkill, "Driver Skill (0-100)");
    grid->SetRowLabelValue(kRowFouls, "Fouls");
    grid->SetRowLabelValue(kRowRankingPoints, "Ranking Points");
    grid->SetRowLabelValue(kRowPPM, "Points Per Match");
}

/**
 * @brief Sets up and displays the grid for editing match data.
 *
 * This function configures the grid for editing match data by setting row labels with appropriate field names.
 * The labels include fields such as match number, win status, team numbers for all participants, and other
 * match-related data. The function first checks if the grid is valid (i.e., not null) before proceeding to set the labels.
 */
void MainFrame::ShowMatchEditGrid() {
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    if ( !grid )
        return;

    grid->SetReadOnly(kRowMatchNum, 0, true);
    grid->SetRowLabelValue(kRowMatchNum, "Match #");
    grid->SetRowLabelValue(kRowPlayed, "Finished (Y/N)");
    grid->SetRowLabelValue(kRowRedWin, "Red Win (Y/N)");
    grid->SetRowLabelValue(kRowBlueWin, "Blue Win (Y/N)");
    grid->SetRowLabelValue(kRowTeam1, "Team 1 #");
    grid->SetRowLabelValue(kRowTeam2, "Team 2 #");
    grid->SetRowLabelValue(kRowTeam3, "Team 3 #");
    grid->SetRowLabelValue(kRowTeam4, "Team 4 #");
    grid->SetRowLabelValue(kRowTeam5, "Team 5 #");
    grid->SetRowLabelValue(kRowTeam6, "Team 6 #");
}

void MainFrame::PromptTeamEdit(const Team& team) {
    // show details of team in editing grid
    ShowTeamEditGrid();

    // get grid object
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    if ( !grid )
        return;

    grid->EnableEditing(m_isEditModeEnabled);

    std::string teamNum = std::to_string(team.teamNum);

    // Set cell values
    grid->SetCellValue(kRowTeamNum, 0, teamNum);
    grid->SetCellValue(kRowOverall, 0, std::to_string(team.overall));
    grid->SetCellValue(kRowEliminated, 0, ( team.eliminated ) ? "Y" : "N");
    grid->SetCellValue(kRowHangAttempt, 0, ( team.hangAttempt ) ? "Y" : "N");
    grid->SetCellValue(kRowHangSuccess, 0, ( team.hangSuccess ) ? "Y" : "N");
    grid->SetCellValue(kRowRobotCycleSpeed, 0, std::to_string(team.robotCycleSpeed));
    grid->SetCellValue(kRowCoralPoints, 0, std::to_string(team.coralPoints));
    grid->SetCellValue(kRowDefense, 0, std::to_string(team.defense));
    grid->SetCellValue(kRowAutonomousPoints, 0, std::to_string(team.autonomousPoints));
    grid->SetCellValue(kRowDriverSkill, 0, std::to_string(team.driverSkill));
    grid->SetCellValue(kRowFouls, 0, std::to_string(team.fouls));
    grid->SetCellValue(kRowRankingPoints, 0, std::to_string(team.rankingPoints));
    grid->SetCellValue(kRowPPM, 0, std::to_string(team.ppm));

    grid->SetCellEditor(kRowTeamNum, 0, new wxGridCellNumberEditor(0, 10000));
    grid->SetCellEditor(kRowOverall, 0, new wxGridCellNumberEditor(0, 100));
    grid->SetCellEditor(kRowEliminated, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowHangAttempt, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowHangSuccess, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowRobotCycleSpeed, 0, new wxGridCellNumberEditor(0));
    grid->SetCellEditor(kRowCoralPoints, 0, new wxGridCellNumberEditor(0));
    grid->SetCellEditor(kRowDefense, 0, new wxGridCellNumberEditor(0, 100));
    grid->SetCellEditor(kRowAutonomousPoints, 0, new wxGridCellNumberEditor(0));
    grid->SetCellEditor(kRowDriverSkill, 0, new wxGridCellNumberEditor(0, 100));
    grid->SetCellEditor(kRowFouls, 0, new wxGridCellNumberEditor(0));
    grid->SetCellEditor(kRowRankingPoints, 0, new wxGridCellNumberEditor(0));
    grid->SetCellEditor(kRowPPM, 0, new wxGridCellNumberEditor(0));

    // Set grid title and description
    wxStaticText* gridTitle = ( wxStaticText* ) FindWindow(kEditingDataTitle);
    wxStaticText* gridDesc = ( wxStaticText* ) FindWindow(kEditingDataDesc);

    if ( m_isEditModeEnabled ) {
        gridTitle->SetLabelText("Editing Team # " + teamNum);
        gridDesc->SetLabelText("Editing all fields for team # " + teamNum);
    }
    else {
        gridTitle->SetLabelText("Viewing Team # " + teamNum);
        gridDesc->SetLabelText("Viewing all fields for team # " + teamNum);
    }
}

void MainFrame::PromptMatchEdit(const Match& match) {
    std::string matchNum = std::to_string(match.matchNum);

    // show details of match in editing grid
    ShowMatchEditGrid();

    // get grid object
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    if ( !grid )
        return;

    grid->EnableEditing(m_isEditModeEnabled);

    // Set cell values
    grid->SetCellValue(kRowMatchNum, 0, matchNum);
    grid->SetCellValue(kRowPlayed, 0, ( match.played ) ? "Y" : "N");
    grid->SetCellValue(kRowRedWin, 0, ( match.redWin ) ? "Y" : "N");
    grid->SetCellValue(kRowBlueWin, 0, ( match.blueWin ) ? "Y" : "N");
    grid->SetCellValue(kRowTeam1, 0, std::to_string(match.Team1().teamNum));
    grid->SetCellValue(kRowTeam2, 0, std::to_string(match.Team2().teamNum));
    grid->SetCellValue(kRowTeam3, 0, std::to_string(match.Team3().teamNum));
    grid->SetCellValue(kRowTeam4, 0, std::to_string(match.Team4().teamNum));
    grid->SetCellValue(kRowTeam5, 0, std::to_string(match.Team5().teamNum));
    grid->SetCellValue(kRowTeam6, 0, std::to_string(match.Team6().teamNum));

    // Set grid editor
    grid->SetCellEditor(kRowMatchNum, 0, new wxGridCellNumberEditor(0, 10000));
    grid->SetCellEditor(kRowPlayed, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowRedWin, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowBlueWin, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowTeam1, 0, new wxGridCellNumberEditor(0, 10000));
    grid->SetCellEditor(kRowTeam2, 0, new wxGridCellNumberEditor(0, 10000));
    grid->SetCellEditor(kRowTeam3, 0, new wxGridCellNumberEditor(0, 10000));
    grid->SetCellEditor(kRowTeam4, 0, new wxGridCellNumberEditor(0, 10000));
    grid->SetCellEditor(kRowTeam5, 0, new wxGridCellNumberEditor(0, 10000));
    grid->SetCellEditor(kRowTeam6, 0, new wxGridCellNumberEditor(0, 10000));

    // Set grid title and description
    wxStaticText* gridTitle = ( wxStaticText* ) FindWindow(kEditingDataTitle);
    wxStaticText* gridDesc = ( wxStaticText* ) FindWindow(kEditingDataDesc);

    if ( m_isEditModeEnabled ) {
        gridTitle->SetLabelText("Editing Match # " + matchNum);
        gridDesc->SetLabelText("Editing all fields for match # " + matchNum);
    }
    else {
        gridTitle->SetLabelText("Viewing Match # " + matchNum);
        gridDesc->SetLabelText("Viewing all fields for Match # " + matchNum);
    }
}

/**
 * @brief Creates and returns the main menu bar for the application.
 *
 * This function sets up the menu bar, including the "File", "Export", and "Import" menus. It adds various
 * submenu options under the "Export" menu, such as exporting data to CSV or JSON formats. The CSV and JSON
 * submenus allow for the export of team data and match data in respective formats. Additionally, the function
 * sets the menu bar for the main window of the application.
 *
 * @return wxMenuBar* The constructed menu bar with all the necessary menus and options.
 */
wxMenuBar* MainFrame::CreateMenuBar() {
    // Add menus such as file, export, import
    wxMenu* menuFile = new wxMenu;
    wxMenu* menuExport = new wxMenu;
    wxMenu* menuImport = new wxMenu;

    ///// Exporting options

    /// CSV
    wxMenu* exportCSV = new wxMenu;
    wxMenuItem* exportTeamDataCSV = new wxMenuItem(NULL, kExportTeamDataCSV, "Team Data CSVV File");
    wxMenuItem* exportMatchDataCSV = new wxMenuItem(NULL, kExportMatchDataCSV, "Match Data As .CSV File");

    // Add options to main exportCSV menu
    exportCSV->Append(exportTeamDataCSV);
    exportCSV->Append(exportMatchDataCSV);

    // Add a submenu 'Export CSV Options' to Export menu
    menuExport->AppendSubMenu(exportCSV, "&Export CSV Options");

    /// JSON
    wxMenu* exportJSON = new wxMenu;
    wxMenuItem* exportTeamDataJSON = new wxMenuItem(NULL, kExportTeamDataJSON, "Team Data As JSON File");
    wxMenuItem* exportMatchDataJSON = new wxMenuItem(NULL, kExportMatchDataJSON, "Match Data As JSON File");

    // Add options to main exportCSV menu
    exportJSON->Append(exportTeamDataJSON);
    exportJSON->Append(exportMatchDataJSON);

    // Add a submenu 'Export CSV Options' to Export menu
    menuExport->AppendSubMenu(exportJSON, "&Export JSON Options");

    // Import options


    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuExport, "&Export");
    menuBar->Append(menuImport, "&Import");
    SetMenuBar(menuBar);

    return menuBar;
}

const Team MainFrame::GetTeamFromRow(int row) {
    if ( row > this->m_displayedTeamCount || !g_DataBase ) {
        LogBackendMessage("Row (" + std::to_string(row) + 
            ") is greater than displayed team count (" 
            + std::to_string(this->m_displayedTeamCount) + ")");
        return {};
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    int uid = m_teamListView->GetItemData(row);

    LogBackendMessage("Getting team with UID: " + std::to_string(uid));

    return db->GetTeam(uid);
}

const Match MainFrame::GetMatchFromRow(int row) {
    if ( row > this->m_displayedMatchCount || !g_DataBase )
        return {};

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );

    wxString colText = m_matchListView->GetItemText(row);
    int matchNumber = std::stoi(colText.ToStdString());

    return db->GetMatch(matchNumber);
}

/**
 * @brief Creates and inserts a new row in the team list view.
 *
 * This function creates a new row in the list view for displaying a team's data. It initializes the row
 * with the team number and various statistics such as overall score, elimination status, robot cycle speed,
 * and other relevant team data. The function also alternates the background color for each row for better readability.
 *
 * @param team The team object containing the data to be displayed in the row.
 *
 * @note The item id for each row is automatically incremented based on how many rows are currently displayed.
 *       The font for each item is set to 9pt and non-bold.
 *       Each column in the list item is populated with the team’s data, converting numerical values to strings.
 *       The background color alternates between two shades of light gray based on the row index (odd/even).
 */
void MainFrame::CreateTeamRow(const Team& team) {
    const int itemId = m_displayedTeamCount++; // item id is always how many rows displayed + 1

    // Insert the actual row item
    wxListItem item;
    item.SetId(itemId);
    m_teamListView->InsertItem(item);

    // Set the font to not be bold
    m_teamListView->SetItemFont(itemId, wxFontInfo(9));

    // Set each column value in the item
    FillTeamRow(itemId, team);

    // change the background colour of the 
    // row depending on itemId for readability
    if ( itemId % 2 == 0 )
        m_teamListView->SetItemBackgroundColour(itemId, LIGHT_GRAY_ACCENT_1);
    else
        m_teamListView->SetItemBackgroundColour(itemId, LIGHT_GRAY_ACCENT_2);

    m_teamListView->Bind(wxEVT_LIST_ITEM_SELECTED, &MainFrame::OnTeamRowLeftClicked, this);
    m_teamListView->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &MainFrame::OnTeamRowRightClicked, this);

    m_currentSelectedTeamRow = m_displayedTeamCount;
    m_teamListView->SetItemData(itemId, team.uid);
    LogBackendMessage("Set item metadata for team uid " + std::to_string(team.uid));
}

/**
 * @brief Creates and adds a match entry to the match list view.
 *
 * This function inserts a new row into the match list, populating it with match details
 * such as match number, whether the match was played, which alliance won, and the team numbers.
 * The background color of the row alternates between two shades for readability.
 *
 * @param match The match data containing match number, status, and team information.
 */
void MainFrame::CreateMatchRow(const Match& match) {
    const int itemId = m_displayedMatchCount++; // item id is always how many rows displayed + 1

    // Insert the actual row item
    wxListItem item;
    item.SetId(itemId);
    m_matchListView->InsertItem(item);

    // Set the font to not be bold
    m_matchListView->SetItemFont(itemId, wxFontInfo(9));

    // Set each column value in the item
    FillMatchRow(itemId, match);

    // change the background colour of the 
    // row depending on itemId for readability
    if ( itemId % 2 == 0 )
        m_matchListView->SetItemBackgroundColour(itemId, wxColour(245, 245, 245));
    else
        m_matchListView->SetItemBackgroundColour(itemId, wxColour(250, 250, 250));

    m_matchListView->Bind(wxEVT_LIST_ITEM_SELECTED, &MainFrame::OnMatchRowLeftClicked, this);
    m_matchListView->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &MainFrame::OnMatchRowRightClicked, this);

    m_currentSelectedMatchRow = m_displayedMatchCount;
}

void MainFrame::RefreshTeamRow(int teamNum) {
    if ( !m_teamListView || !g_DataBase )
        return;

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    const Team team = db->GetTeam(teamNum);

    // Find the row with the team number
    int row = -1;
    for ( int i = 0; i < m_displayedTeamCount; i++ ) {
        if ( m_teamListView->GetItemText(i) == std::to_string(teamNum) ) {
            row = i;
            break;
        }
    }

    if ( row == -1 )
        return;

    // Update the row with the new team data
    FillTeamRow(row, team);
}

void MainFrame::RefreshMatchRow(int matchNum) {
    if ( !m_matchListView || !g_DataBase )
        return;

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    const Match match = db->GetMatch(matchNum);

    // Find the row with the match number
    int row = -1;
    for ( int i = 0; i < m_displayedMatchCount; i++ ) {
        if ( m_matchListView->GetItemText(i) == std::to_string(matchNum) ) {
            row = i;
            break;
        }
    }

    if ( row == -1 )
        return;

    // Update the row with the new team data
    FillMatchRow(row, match);
}

void MainFrame::FillTeamRow(int row, const Team& team) {
    if ( !m_teamListView )
        return;

    m_teamListView->SetItem(row, 0, std::to_string(team.teamNum));
    m_teamListView->SetItem(row, 1, std::to_string(team.overall));
    m_teamListView->SetItem(row, 2, ( team.eliminated ) ? "Y" : "N");
    m_teamListView->SetItem(row, 3, ( team.hangAttempt ) ? "Y" : "N");
    m_teamListView->SetItem(row, 4, ( team.hangSuccess ) ? "Y" : "N");
    m_teamListView->SetItem(row, 5, std::to_string(team.robotCycleSpeed));
    m_teamListView->SetItem(row, 6, std::to_string(team.coralPoints));
    m_teamListView->SetItem(row, 7, std::to_string(team.defense));
    m_teamListView->SetItem(row, 8, std::to_string(team.autonomousPoints));
    m_teamListView->SetItem(row, 9, std::to_string(team.driverSkill));
    m_teamListView->SetItem(row, 10, std::to_string(team.fouls));
    m_teamListView->SetItem(row, 11, std::to_string(team.rankingPoints));
    m_teamListView->SetItem(row, 12, std::to_string(team.ppm));
}

void MainFrame::FillMatchRow(int row, const Match& match) {
    if ( !m_matchListView )
        return;

    m_matchListView->SetItem(row, 0, std::to_string(match.matchNum));
    m_matchListView->SetItem(row, 1, ( match.played ) ? "Y" : "N");
    m_matchListView->SetItem(row, 2, ( match.redWin ) ? "Y" : "N");
    m_matchListView->SetItem(row, 3, ( match.blueWin ) ? "Y" : "N");
    m_matchListView->SetItem(row, 4, std::to_string(match.Team1().teamNum));
    m_matchListView->SetItem(row, 5, std::to_string(match.Team2().teamNum));
    m_matchListView->SetItem(row, 6, std::to_string(match.Team3().teamNum));
    m_matchListView->SetItem(row, 7, std::to_string(match.Team4().teamNum));
    m_matchListView->SetItem(row, 8, std::to_string(match.Team5().teamNum));
    m_matchListView->SetItem(row, 9, std::to_string(match.Team6().teamNum));
}

/**
 * @brief Updates the SQL query history in the text box.
 *
 * This function appends an SQL query that was ran by the backend, prefixed by
 * "SQL> ". Used to display all executed SQL queries for debugging purposes.
 *
 * @param queryHistory The SQL query string to append.
 */
void MainFrame::LogSQLQuery(std::string queryHistory) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return;

    if ( queryHistory.empty() )
        return;

    queryHistory = "SQL> " + queryHistory + "\n\n";
    SQLHistoryTextBox->AppendText(queryHistory);

    return;
}

/**
 * @brief Logs an SQL error message in red text.
 *
 * This function appends an error message to the SQL history text box,
 * with a prefix "ERROR> ", and changes the text color to red to indicate an issue.
 *
 * @param errorMsg The error message to log.
 */
void MainFrame::LogSQLError(std::string errorMsg) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return;

    if ( errorMsg.empty() )
        return;

    errorMsg = "ERROR> " + errorMsg + "\n\n";

    const wxTextAttr defaultAttr = SQLHistoryTextBox->GetDefaultStyle();
    SQLHistoryTextBox->SetDefaultStyle(wxTextAttr(*wxRED)); // change text colour to red
    SQLHistoryTextBox->AppendText(errorMsg);
    SQLHistoryTextBox->SetDefaultStyle(defaultAttr); // reset text colour
}

/**
 * @brief Logs a backend message in blue text.
 *
 * This function appends a message to the SQL history text box,
 * with a prefix "MSG> ", and changes the text color to blue to distinguish it.
 *
 * @param msg The backend message to log.
 */
void MainFrame::LogBackendMessage(std::string msg) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return;

    if ( msg.empty() )
        return;

    msg = "MSG> " + msg + "\n\n";

    const wxTextAttr defaultAttr = SQLHistoryTextBox->GetDefaultStyle();
    SQLHistoryTextBox->SetDefaultStyle(wxTextAttr(*wxBLUE)); // set text colour to blue
    SQLHistoryTextBox->AppendText(msg);
    SQLHistoryTextBox->SetDefaultStyle(defaultAttr); // reset text colour
}

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

    LogBackendMessage("Team row selected: " + std::to_string(row));
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
    rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnDeleteTeam, this, wxID_DELETE);
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
    rightClickMenu.Bind(wxEVT_MENU, &MainFrame::OnDeleteMatch, this, wxID_DELETE);
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

    ( m_isEditModeEnabled ) ? editModeButton->SetLabelText("View Mode") : editModeButton->SetLabelText("Edit Mode");
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
        LogBackendMessage("Database not available, cannot save team. Closing this app will delete all progress.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    
    Team team = {};
    team.uid = db->GenerateUID();
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
        LogBackendMessage("Database not available, cannot save team. Closing this app will delete all progress.");
        return;
    }

    DataBase* db = reinterpret_cast< DataBase* >( g_DataBase );
    db->AddMatch(match);

    PromptMatchEdit(match);
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
}

// TODO: Update the row the team is in
void MainFrame::OnGridCellChange(wxGridEvent& event) {
    //int row = event.GetRow();
    //int col = event.GetCol();
    //wxGrid* grid = reinterpret_cast< wxGrid* >( event.GetEventObject() );
    //
    //if ( !grid )
    //    return;

    //wxString val = grid->GetCellValue(row, col);
    //const Team team = GetTeamFromRow(m_currentSelectedTeamRow);

    //switch ( row ) {
    //case kColTeamNum: {
    //    // team number
    //    
    //    break;
    //}
}

#endif // _USING_UI