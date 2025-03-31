// Frontend
#include "frontend/mainframe.h" // MainFrame class

// Backend
#include "backend/data.h" // DataBase class
#include "backend/team.h"
#include "backend/match.h"
#include "backend/rfpredict.h"

// STD
#include <filesystem> // exists(), absolute()
#include <string>

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
 * @param darkModeEnabled Whether or not App has set dark mode to true
 */
MainFrame::MainFrame(const wxString& title, bool darkModeEnabled)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)), m_darkModeTheme(darkModeEnabled)
{
    // Add menu bar. Menu bar also correctly adjusts
    // if dark mode is enabled or not since it will be white
    // if dark mode is truly disable, or black if dark mode is truly enabled.
    // 
    // If you set your windows theme to light mode, MSWEnableDarkMode will return true
    // even though it didn't actually enable since MSWEnableDarkMode only enables if 
    // the system theme is dark. Because the return value is stored in m_darkModeTheme
    // m_darkModeTheme will always be true even if the system theme isnt dark. This tricks
    // the UI like list views, text ctrls, etc into thinking its dark mode, and thus switches
    // colours to DARK_GRAY instead of the correct LIGHT_GRAY_X_ACCENT's
    CreateMenuBar();

    // Create global database
    DataBase* db = new DataBase(DB_PATH, this);
    m_dataBase = reinterpret_cast< void* >( db );

    // set the window title to app name - database path
    // e.g: "FRCScout - C:\Users\user\Desktop\data.db"
    this->SetTitle(std::string(APP_NAME) + " - " + std::filesystem::absolute(DB_PATH).string());

    // Panels
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);

    // Create two list panels
    leftSizer->Add(CreateListPanel(panel, kTeamListView, "Teams", "View and edit specific fields of any team.", 0), 1, wxEXPAND | wxALL, 10);
    leftSizer->Add(CreateListPanel(panel, kMatchListView, "Matches", "View and modify individual fields of a match.", 0), 1, wxEXPAND | wxALL, 10);

    // Get list views
    m_teamListView = ( wxListCtrl* ) FindWindow(kTeamListView);
    m_matchListView = ( wxListCtrl* ) FindWindow(kMatchListView);

    // Add columns to list views
    AddTeamListColumns();
    AddMatchListColumns();

    // Create the grid panel on the right side
    wxPanel* rightPanel = new wxPanel(panel, wxID_ANY);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxHORIZONTAL);  // Use HORIZONTAL sizer

    // Create the grid and SQL output
    wxBoxSizer* grid = CreateEditingGrid(rightPanel);
    wxBoxSizer* sqlOutput = CreateSQLOutputBox(rightPanel);

    // Add grid and sqlOutput to the horizontal sizer
    rightSizer->Add(grid, 1, wxEXPAND | wxALL, 10);  // Grid takes up available space
    rightSizer->Add(sqlOutput, 1, wxEXPAND | wxALL, 10);  // Add some space between them

    rightPanel->SetSizerAndFit(rightSizer);

    // Add the leftSizer (team list & match list) and rightPanel (grid + sqlOutput) to the mainSizer
    mainSizer->Add(leftSizer, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(rightPanel, 0, wxEXPAND | wxLEFT, 10);

    // Set sizer for the panel
    panel->SetSizer(mainSizer);
    this->Layout();

    CreateStatusBar();
    UpdateStatusBar();
    DisplayExistingData();

    // Create global predictor
    RFPredictor* predictor = new RFPredictor(this, db);
    m_predictor = reinterpret_cast< void* >( predictor );

    if ( m_darkModeTheme )
        this->SetBackgroundColour(DARK_GRAY_1);
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
wxBoxSizer* MainFrame::CreateListPanel(wxWindow* parent, int listId, wxString titleName, wxString description, int ySize) {
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

    wxButton* addButton = new wxButton(parent, wxID_ANY, "Add", wxDefaultPosition, wxSize(50, 30), wxBORDER_NONE);
    addButton->SetClientData(reinterpret_cast< void* >( listId )); // save list id in metadata, e.g kTeamListView
    addButton->Bind(wxEVT_BUTTON, &MainFrame::OnAddButton, this);
    
    if ( m_darkModeTheme ) {
        addButton->SetBackgroundColour(DARK_GRAY_4);
        addButton->SetFont(wxFontInfo(9).Bold());
    }

    // Add elements to the horizontal top sizer
    topSizer->Add(textSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);  // Add title + desc stack
    topSizer->AddSpacer(10);  // Some space before buttons
    topSizer->Add(addButton, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);

    // List view
    wxListCtrl* listCtrl = new wxListCtrl(parent, listId, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxBORDER_NONE);

    // Add topSizer and list view to listSizer
    listSizer->Add(topSizer, 0, wxEXPAND | wxBOTTOM, 5);
    listSizer->Add(listCtrl, 1, wxEXPAND);

    listCtrl->Bind(wxEVT_CONTEXT_MENU, &MainFrame::OnListViewRightClick, this);

    if ( m_darkModeTheme )
        listCtrl->SetBackgroundColour(DARK_GRAY_5);

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
    m_teamListView->AppendColumn("Team #", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Match #", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("OVR", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Hang Attempt", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Hang Success", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Cycle Speed", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Coral Points", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Defense", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Auto. Points", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Driver Skill", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Penaltys", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->AppendColumn("Rank Points", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_teamListView->SetColumnWidth(0, teamListWidth * 0.2);

    m_teamListView->AppendColumn("", wxLIST_FORMAT_CENTER, 180);
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

    // Get the total width of the list control
    const int matchListWidth = m_matchListView->GetSize().GetWidth();

    // Set bold column headers
    m_matchListView->SetFont(wxFontInfo(9).Bold());

    // Add match list columns with proper alignment
    m_matchListView->AppendColumn("Match #", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Red Win", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Blue Win", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Red 1", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Red 2", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Red 3", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Blue 4", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Blue 5", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    m_matchListView->AppendColumn("Blue 6", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);

    m_matchListView->SetColumnWidth(0, matchListWidth * 0.2);

    // Add the last blank column with the remaining width
    m_matchListView->AppendColumn("", wxLIST_FORMAT_CENTER, 580);
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
    if ( !m_teamListView || !m_dataBase )
        return;

    m_teamListView->DeleteAllItems();

    DataBase* db = reinterpret_cast< DataBase* >( m_dataBase ); // cast database

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

inline void MainFrame::UpdateStatusBar() {
    SetStatusText(wxString::Format("FRCScout - %d Teams, %d Matches", m_displayedTeamCount, m_displayedMatchCount));
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
wxBoxSizer* MainFrame::CreateEditingGrid(wxPanel* panel) {
    // Create a sizer for the top section (static text and button)
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* gridSizer = new wxBoxSizer(wxVERTICAL);

    // Title above the grid
    wxStaticText* gridTitle = new wxStaticText(panel, kEditingDataTitle, "Edit and View", wxDefaultPosition, wxDefaultSize, 0);
    gridTitle->SetFont(wxFontInfo(18).Bold());

    // Description above the grid
    wxStaticText* gridDesc = new wxStaticText(panel, kEditingDataDesc, "Edit and view fields of objects", wxDefaultPosition, wxDefaultSize, 0);
    gridDesc->SetFont(wxFontInfo(10));

    // Add title and description to the topSizer
    textSizer->Add(gridTitle, 0, wxALIGN_LEFT);
    textSizer->Add(gridDesc, 0, wxALIGN_LEFT | wxTOP, 2);

    // Add edit and view mode button
    wxButton* editModeButton = new wxButton(panel, kEditModeButton, "Edit", wxDefaultPosition, wxSize(50, 30), wxBORDER_NONE);
    editModeButton->Bind(wxEVT_BUTTON, &MainFrame::OnToggleEditMode, this);

    if ( m_darkModeTheme ) {
        editModeButton->SetBackgroundColour(DARK_GRAY_4);
        editModeButton->SetFont(wxFontInfo(9).Bold());
    }

    // Add the button to the topSizer
    topSizer->Add(textSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    topSizer->AddSpacer(10);
    topSizer->Add(editModeButton, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);

    // Create the grid
    wxGrid* grid = new wxGrid(panel, kEditItemGrid, wxPoint(1000, 1000), wxSize(400, 200), wxLC_REPORT | wxBORDER_NONE);
    grid->CreateGrid(34, 1);

    // Grid Options
    grid->SetColLabelValue(0, "Value");
    grid->DisableColResize(0);
    grid->SetRowLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
    grid->SetLabelFont(wxFontInfo(9).Bold());
    grid->DisableOverlaySelection();
    
    // Sizing for grid rows and columns
    grid->SetRowLabelSize(150);
    grid->SetColSize(0, 250);
    grid->SetUseNativeColLabels(false);

    DefaultEditGrid();

    // Set row size, disable row resize, and set default row value for all rows
    for ( int i = 0; i < grid->GetNumberRows(); i++ ) {
        grid->DisableRowResize(i);
        grid->SetRowSize(i, 25);
        
        // alternate between colours
        if ( i % 2 == 0 ) {
            wxColour labelCol = ( m_darkModeTheme ) ? DARK_GRAY_2 : LIGHT_GRAY_ACCENT_1;
            grid->SetCellBackgroundColour(i, 0, labelCol);
        }
        else {
            wxColour labelCol = ( m_darkModeTheme ) ? DARK_GRAY_3 : LIGHT_GRAY_ACCENT_2;
            grid->SetCellBackgroundColour(i, 0, labelCol);
        }
    }

    if ( m_darkModeTheme )
        grid->SetLabelBackgroundColour(wxColour(DARK_GRAY_2));
    
    grid->Bind(wxEVT_GRID_CELL_CHANGED, &MainFrame::OnGridCellChange, this);

    gridSizer->Add(topSizer, 0, wxEXPAND | wxBOTTOM, 5);
    gridSizer->Add(grid, 1, wxEXPAND);

    return gridSizer;
}

/**
 * @brief Creates a read-only SQL output text box in the specified panel.
 *
 * This function creates a multi-line, read-only text box within the provided `rightPanel`.
 *
 * @param rightPanel The wxPanel in which the SQL output text box will be placed.
 * @return wxTextCtrl* A pointer to the created SQL output text box.
 */
wxBoxSizer* MainFrame::CreateSQLOutputBox(wxPanel* rightPanel) {
    // Sizers
    wxBoxSizer* textCtrlSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);  // New vertical sizer for title and description
    
    // Title above the box
    wxStaticText* title = new wxStaticText(rightPanel, kEditingDataTitle, "Log Output", wxDefaultPosition, wxDefaultSize, 0);
    title->SetFont(wxFontInfo(18).Bold());

    // Description above the box
    wxStaticText* desc = new wxStaticText(rightPanel, kEditingDataDesc, "Real-time logs", wxDefaultPosition, wxDefaultSize, 0);
    desc->SetFont(wxFontInfo(10));

    // Add clear button
    wxButton* clearButton = new wxButton(rightPanel, kClearOutputButton, "Clear", wxDefaultPosition, wxSize(50, 30), wxBORDER_NONE);
    clearButton->Bind(wxEVT_BUTTON, &MainFrame::ClearOutput, this);
    
    if ( m_darkModeTheme ) {
        clearButton->SetBackgroundColour(DARK_GRAY_4);
        clearButton->SetFont(wxFontInfo(9).Bold());
    }

    textSizer->Add(title, 0, wxALIGN_LEFT);
    textSizer->Add(desc, 0, wxALIGN_LEFT | wxTOP, 2);  // Small space between title and description

    topSizer->Add(textSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);  // Add title + desc stack
    topSizer->AddSpacer(10);  // Some space before buttons
    topSizer->Add(clearButton, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);

    // Create a text box that is read only
    wxTextCtrl* sqlOutput = new wxTextCtrl(
        rightPanel,
        kSQLHistoryTextBox,
        wxEmptyString,
        wxDefaultPosition,
        wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxBORDER_NONE
    );

    // Create font to use in the SQL output text box
    wxFont font(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
    font.Scale(0.9);

    // Set the font
    sqlOutput->SetFont(font);

    textCtrlSizer->Add(topSizer, 0, wxEXPAND | wxBOTTOM, 5);
    textCtrlSizer->Add(sqlOutput, 1, wxEXPAND);

    if ( m_darkModeTheme )
        sqlOutput->SetBackgroundColour(DARK_GRAY_5);

    return textCtrlSizer;
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

    DefaultEditGrid();

    grid->SetReadOnly(kRowTeamNum, 0, false);
    grid->SetRowLabelValue(kRowTeamNum, "Team #");
    grid->SetRowLabelValue(kRowInMatchNum, "Match #");
    grid->SetRowLabelValue(kRowOverall, "Overall (0-100)");
    grid->SetRowLabelValue(kRowHangAttempt, "Hang Attempt (Y/N)");
    grid->SetRowLabelValue(kRowHangSuccess, "Hang Success (Y/N)");
    grid->SetRowLabelValue(kRowRobotCycleSpeed, "Robot Cycle Speed");
    grid->SetRowLabelValue(kRowCoralPoints, "Coral Points");
    grid->SetRowLabelValue(kRowDefense, "Defense (0-100)");
    grid->SetRowLabelValue(kRowAutonomousPoints, "Auto. Points");
    grid->SetRowLabelValue(kRowDriverSkill, "Driver Skill (0-100)");
    grid->SetRowLabelValue(kRowPenaltys, "Penaltys");
    grid->SetRowLabelValue(kRowRankingPoints, "Ranking Points");
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

    DefaultEditGrid();

    grid->SetReadOnly(kRowMatchNum, 0, true);
    grid->SetRowLabelValue(kRowMatchNum, "Match #");
    grid->SetRowLabelValue(kRowRedWin, "Red Win (Y/N)");
    grid->SetRowLabelValue(kRowBlueWin, "Blue Win (Y/N)");
    grid->SetRowLabelValue(kRowRed1, "Red 1");
    grid->SetRowLabelValue(kRowRed2, "Red 2");
    grid->SetRowLabelValue(kRowRed3, "Red 3");
    grid->SetRowLabelValue(kRowBlue4, "Blue 4");
    grid->SetRowLabelValue(kRowBlue5, "Blue 5");
    grid->SetRowLabelValue(kRowBlue6, "Blue 6");
}

/**
 * @brief Displays the team details in an editable grid for modification.
 *
 * This function sets up the editing grid for a given team, where the user can modify
 * various attributes of the team, such as overall performance, robot cycle speed,
 * defense, and other statistics. The grid is populated with the team's data and
 * appropriate cell editors are set to allow for different types of input. It also
 * updates the grid's title and description based on whether editing mode is enabled.
 *
 * @param team The team object whose details are displayed and can be edited.
 *
 * @note The grid allows for editing numerical values (e.g., team number, overall points, etc.)
 * and selection-based values (e.g., eliminated status, hang attempt, etc.).
 *
 * @note If editing mode is disabled, the grid will display the team’s information in
 * read-only mode.
 *
 * @see wxGridCellNumberEditor
 * @see wxGridCellChoiceEditor
 * @see ShowTeamEditGrid()
 */
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
    grid->SetCellValue(kRowInMatchNum, 0, std::to_string(team.matchNum));
    grid->SetCellValue(kRowOverall, 0, std::to_string(team.overall));
    grid->SetCellValue(kRowHangAttempt, 0, ( team.hangAttempt ) ? "Y" : "N");
    grid->SetCellValue(kRowHangSuccess, 0, ( team.hangSuccess ) ? "Y" : "N");
    grid->SetCellValue(kRowRobotCycleSpeed, 0, std::to_string(team.robotCycleSpeed));
    grid->SetCellValue(kRowCoralPoints, 0, std::to_string(team.coralPoints));
    grid->SetCellValue(kRowDefense, 0, std::to_string(team.defense));
    grid->SetCellValue(kRowAutonomousPoints, 0, std::to_string(team.autonomousPoints));
    grid->SetCellValue(kRowDriverSkill, 0, std::to_string(team.driverSkill));
    grid->SetCellValue(kRowPenaltys, 0, std::to_string(team.penaltys));
    grid->SetCellValue(kRowRankingPoints, 0, std::to_string(team.rankingPoints));

    grid->SetCellEditor(kRowTeamNum, 0, new wxGridCellNumberEditor(0, 20000));
    grid->SetCellEditor(kRowInMatchNum, 0, new wxGridCellNumberEditor(0, 1000));
    grid->SetCellEditor(kRowOverall, 0, new wxGridCellNumberEditor(0, 100));
    grid->SetCellEditor(kRowHangAttempt, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowHangSuccess, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowRobotCycleSpeed, 0, new wxGridCellNumberEditor());
    grid->SetCellEditor(kRowCoralPoints, 0, new wxGridCellNumberEditor());
    grid->SetCellEditor(kRowDefense, 0, new wxGridCellNumberEditor(0, 100));
    grid->SetCellEditor(kRowAutonomousPoints, 0, new wxGridCellNumberEditor());
    grid->SetCellEditor(kRowDriverSkill, 0, new wxGridCellNumberEditor(0, 100));
    grid->SetCellEditor(kRowPenaltys, 0, new wxGridCellNumberEditor());
    grid->SetCellEditor(kRowRankingPoints, 0, new wxGridCellNumberEditor());

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

/**
 * @brief Prompts the user to edit details of a given match in an editable grid.
 *
 * This function displays a grid for editing match details, populating it with values from the
 * provided match object. The user can either view or modify the match details based on the
 * current edit mode setting.
 *
 * @param match The match object containing the match details to be displayed and edited.
 *
 * @note This function updates grid cell values with match data such as the match number,
 *       teams participating, and match results (e.g., red and blue team wins).
 *       It also sets up cell editors for numeric values and choice cells (e.g., "Y" or "N").
 *       The grid is displayed with appropriate titles and descriptions indicating whether
 *       the user is in edit or view mode.
 */
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
    grid->SetCellValue(kRowRedWin, 0, ( match.redWin ) ? "Y" : "N");
    grid->SetCellValue(kRowBlueWin, 0, ( match.blueWin ) ? "Y" : "N");
    grid->SetCellValue(kRowRed1, 0, std::to_string(match.Team1().teamNum));
    grid->SetCellValue(kRowRed2, 0, std::to_string(match.Team2().teamNum));
    grid->SetCellValue(kRowRed3, 0, std::to_string(match.Team3().teamNum));
    grid->SetCellValue(kRowBlue4, 0, std::to_string(match.Team4().teamNum));
    grid->SetCellValue(kRowBlue5, 0, std::to_string(match.Team5().teamNum));
    grid->SetCellValue(kRowBlue6, 0, std::to_string(match.Team6().teamNum));

    // Set grid editor
    grid->SetCellEditor(kRowMatchNum, 0, new wxGridCellNumberEditor(0, 20000));
    grid->SetCellEditor(kRowRedWin, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowBlueWin, 0, new wxGridCellChoiceEditor(2, new wxString[]{ "Y", "N" }));
    grid->SetCellEditor(kRowRed1, 0, new wxGridCellNumberEditor(0, 20000));
    grid->SetCellEditor(kRowRed2, 0, new wxGridCellNumberEditor(0, 20000));
    grid->SetCellEditor(kRowRed3, 0, new wxGridCellNumberEditor(0, 20000));
    grid->SetCellEditor(kRowBlue4, 0, new wxGridCellNumberEditor(0, 20000));
    grid->SetCellEditor(kRowBlue5, 0, new wxGridCellNumberEditor(0, 20000));
    grid->SetCellEditor(kRowBlue6, 0, new wxGridCellNumberEditor(0, 20000));

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
 * @brief Resets the grid by clearing row labels and cell values.
 *
 * This function retrieves the grid object using the identifier `kEditItemGrid`. If the grid is found, it iterates
 * over all the rows and clears the values in the first column (cell values) and resets the row labels to a default
 * placeholder ("...").
 */
void MainFrame::DefaultEditGrid() {
    // get grid object
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    if ( !grid )
        return;

    // Remove any existing data in row labels
    for ( int i = 0; i < grid->GetNumberRows(); i++ ) {
        grid->SetCellValue(i, 0, "");
        grid->SetRowLabelValue(i, "");
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
    wxMenuItem* exportTeamDataCSV = new wxMenuItem(NULL, kExportTeamDataCSV, "Team Data As CSV and QR Code");
    wxMenuItem* exportMatchDataCSV = new wxMenuItem(NULL, kExportMatchDataCSV, "Match Data As CSV and QR Code");

    Bind(wxEVT_MENU, &MainFrame::OnExportTeamDataCSV, this, kExportTeamDataCSV);
    Bind(wxEVT_MENU, &MainFrame::OnExportMatchDataCSV, this, kExportMatchDataCSV);

    // Add options to main exportCSV menu
    menuExport->Append(exportTeamDataCSV);
    menuExport->Append(exportMatchDataCSV);

    /// JSON
    wxMenu* exportJSON = new wxMenu;
    wxMenuItem* exportTeamDataJSON = new wxMenuItem(NULL, kExportTeamDataJSON, "Team Data As JSON File");
    wxMenuItem* exportMatchDataJSON = new wxMenuItem(NULL, kExportMatchDataJSON, "Match Data As JSON File");

    Bind(wxEVT_MENU, &MainFrame::OnExportTeamDataJSON, this, kExportTeamDataJSON);
    Bind(wxEVT_MENU, &MainFrame::OnExportMatchDataJSON, this, kExportMatchDataJSON);

    // Add options to main exportCSV menu
    menuExport->Append(exportTeamDataJSON);
    menuExport->Append(exportMatchDataJSON);

    // TODO: Import options
    wxMenuItem* importTeamDataCSV = new wxMenuItem(NULL, kImportTeamDataCSV, "Import Team Data From CSV");
    wxMenuItem* importMatchDataCSV = new wxMenuItem(NULL, kImportMatchDataCSV, "Import Match Data From CSV");

    Bind(wxEVT_MENU, &MainFrame::OnImportTeamDataCSV, this, kImportTeamDataCSV);
    Bind(wxEVT_MENU, &MainFrame::OnImportMatchDataCSV, this, kImportMatchDataCSV);

    menuImport->Append(importTeamDataCSV);
    menuImport->Append(importMatchDataCSV);

    // Setup Menu Bar
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuExport, "&Export");
    menuBar->Append(menuImport, "&Import");

    SetMenuBar(menuBar);
    
    wxColour col = menuBar->GetBackgroundColour();
    if ( col == wxColour(240, 240, 240) )
        m_darkModeTheme = false;
    else if ( col == wxColour(32, 32, 32) )
        m_darkModeTheme = true;

    return menuBar;
}

/**
 * @brief Retrieves a Team object corresponding to a specific row in the team list view.
 *
 * This function retrieves the unique identifier (UID) for the team from the specified row
 * and queries the database to return the corresponding Team object.
 *
 * @param row The row index from which to retrieve the team.
 *
 * @return A Team object corresponding to the row, or an empty Team object if the row index is invalid.
 */
const Team MainFrame::GetTeamFromRow(int row) {
    if ( row > this->m_displayedTeamCount || !m_dataBase )
        return {};

    DataBase* db = reinterpret_cast< DataBase* >( m_dataBase );
    int uid = m_teamListView->GetItemData(row);

    return db->GetTeam(uid);
}

/**
 * @brief Retrieves a Match object corresponding to a specific row in the match list view.
 *
 * This function extracts the match number from the specified row and queries the database
 * to return the corresponding Match object.
 *
 * @param row The row index from which to retrieve the match.
 *
 * @return A Match object corresponding to the row, or an empty Match object if the row index is invalid.
 */
const Match MainFrame::GetMatchFromRow(int row) {
    if ( row > this->m_displayedMatchCount || !m_dataBase )
        return {};

    DataBase* db = reinterpret_cast< DataBase* >( m_dataBase );

    wxString colText = m_matchListView->GetItemText(row);
    int matchNumber = std::stoi(colText.ToStdString());

    return db->GetMatch(matchNumber);
}

const int MainFrame::GetSelectedRowMatchNum() {
    wxString colText = m_matchListView->GetItemText(m_selectedMatchRow, 0);
    if ( !colText.IsNumber() )
        return 0;

    return std::stoi(colText.ToStdWstring());
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

    m_teamListView->EnsureVisible(itemId);

    // Set each column value in the item
    FillTeamRow(itemId, team);

    m_teamListView->Bind(wxEVT_LIST_ITEM_SELECTED, &MainFrame::OnTeamRowLeftClicked, this);
    m_teamListView->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &MainFrame::OnTeamRowRightClicked, this);

    m_selectedTeamRow = m_displayedTeamCount;
    m_teamListView->SetItemData(itemId, team.uid);

    UpdateStatusBar();
    
    // change the background colour of the 
    // row depending on itemId for readability
    if ( itemId % 2 == 0 ) {
        wxColour colour = ( m_darkModeTheme ) ? DARK_GRAY_2 : LIGHT_GRAY_ACCENT_1;
        m_teamListView->SetItemBackgroundColour(itemId, colour);
    }
    else {
        wxColour colour = ( m_darkModeTheme ) ? DARK_GRAY_3 : LIGHT_GRAY_ACCENT_2;
        m_teamListView->SetItemBackgroundColour(itemId, colour);
    }
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

    m_matchListView->EnsureVisible(itemId);

    // Set each column value in the item
    FillMatchRow(itemId, match);

    m_matchListView->Bind(wxEVT_LIST_ITEM_SELECTED, &MainFrame::OnMatchRowLeftClicked, this);
    m_matchListView->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &MainFrame::OnMatchRowRightClicked, this);

    m_selectedMatchRow = m_displayedMatchCount;

    UpdateStatusBar();

    // change the background colour of the 
    // row depending on itemId for readability
    if ( itemId % 2 == 0 ) {
        wxColour colour = ( m_darkModeTheme ) ? DARK_GRAY_2 : LIGHT_GRAY_ACCENT_1;
        m_matchListView->SetItemBackgroundColour(itemId, colour);
    }
    else {
        wxColour colour = ( m_darkModeTheme ) ? DARK_GRAY_3 : LIGHT_GRAY_ACCENT_2;
        m_matchListView->SetItemBackgroundColour(itemId, colour);
    }
}

/**
 * @brief Refreshes the row displaying a team's data in the team list view.
 *
 * This function retrieves the latest data for a given team from the database
 * and updates the corresponding row in the team list view.
 *
 * @param uid The UID of the team to refresh
 *
 * @note If the team is not found in the displayed list, the function exits without making changes.
 *
 * @see FillTeamRow
 */
void MainFrame::RefreshTeamRow(int uid) {
    if ( !m_teamListView || !m_dataBase )
        return;

    DataBase* db = reinterpret_cast< DataBase* >( m_dataBase );
    const Team team = db->GetTeam(uid);

    // Find the row with the team number
    int row = -1;
    for ( int tempRow = 0; tempRow < m_displayedTeamCount; tempRow++ ) {
        // check item data (saved team uid) is equal to team uid to update
        if ( m_teamListView->GetItemData(tempRow) == uid ) {
            row = tempRow;
            break;
        }
    }

    if ( row == -1 )
        return;

    // Update the row with the new team data
    FillTeamRow(row, team);
}

/**
 * @brief Refreshes the row displaying a match's data in the match list view.
 *
 * This function retrieves the latest data for a given match from the database
 * and updates the corresponding row in the match list view.
 *
 * @param matchNum The match number to refresh.
 *
 * @note If the match is not found in the displayed list, the function exits without making changes.
 *
 * @see FillMatchRow
 */
void MainFrame::RefreshMatchRow(int matchNum) {
    if ( !m_matchListView || !m_dataBase )
        return;

    DataBase* db = reinterpret_cast< DataBase* >( m_dataBase );
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

/**
 * @brief Fills a row in the team list view with data from a Team object.
 *
 * This function updates the list view with details such as team number, performance metrics,
 * match statistics, and ranking points.
 *
 * @param row The row index to update in the team list view.
 * @param team The Team object containing the data to populate the row.
 *
 * @note If the list view is not initialized, the function exits without making changes.
 */
void MainFrame::FillTeamRow(int row, const Team& team) {
    if ( !m_teamListView )
        return;

    m_teamListView->SetItem(row, kRowTeamNum, std::to_string(team.teamNum));
    m_teamListView->SetItem(row, kRowInMatchNum, std::to_string(team.matchNum));
    m_teamListView->SetItem(row, kRowOverall, std::to_string(team.overall));
    m_teamListView->SetItem(row, kRowHangAttempt, ( team.hangAttempt ) ? "Y" : "N");
    m_teamListView->SetItem(row, kRowHangSuccess, ( team.hangSuccess ) ? "Y" : "N");
    m_teamListView->SetItem(row, kRowRobotCycleSpeed, std::to_string(team.robotCycleSpeed));
    m_teamListView->SetItem(row, kRowCoralPoints, std::to_string(team.coralPoints));
    m_teamListView->SetItem(row, kRowDefense, std::to_string(team.defense));
    m_teamListView->SetItem(row, kRowAutonomousPoints, std::to_string(team.autonomousPoints));
    m_teamListView->SetItem(row, kRowDriverSkill, std::to_string(team.driverSkill));
    m_teamListView->SetItem(row, kRowPenaltys, std::to_string(team.penaltys));
    m_teamListView->SetItem(row, kRowRankingPoints, std::to_string(team.rankingPoints));
}

/**
 * @brief Fills a row in the match list view with data from a Match object.
 *
 * This function updates the list view with details such as match number, teams, and match outcomes.
 *
 * @param row The row index to update in the match list view.
 * @param match The Match object containing the data to populate the row.
 *
 * @note If the list view is not initialized, the function exits without making changes.
 */
void MainFrame::FillMatchRow(int row, const Match& match) {
    if ( !m_matchListView )
        return;

    m_matchListView->SetItem(row, 0, std::to_string(match.matchNum));
    m_matchListView->SetItem(row, 1, ( match.redWin ) ? "Y" : "N");
    m_matchListView->SetItem(row, 2, ( match.blueWin ) ? "Y" : "N");
    m_matchListView->SetItem(row, 3, std::to_string(match.Team1().teamNum));
    m_matchListView->SetItem(row, 4, std::to_string(match.Team2().teamNum));
    m_matchListView->SetItem(row, 5, std::to_string(match.Team3().teamNum));
    m_matchListView->SetItem(row, 6, std::to_string(match.Team4().teamNum));
    m_matchListView->SetItem(row, 7, std::to_string(match.Team5().teamNum));
    m_matchListView->SetItem(row, 8, std::to_string(match.Team6().teamNum));
}