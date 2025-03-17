#ifdef _USING_UI

// Frontend
#include "frontend/mainframe.h"
#include "frontend/wxids.h" // WinId enum

// Backend
#include "backend/data.h"
#include "backend/team.h"
#include "backend/match.h"

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) // Initial window size
{
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
    wxTextCtrl* sqlHistory = CreateSQLHistoryBox(rightPanel);

    // Add grid to the rightSizer
    rightSizer->Add(grid, 1, wxEXPAND | wxALL, 10);
    rightSizer->Add(sqlHistory, 1, wxEXPAND | wxALL, 10);  // Text box will share the same space and dimensions

    // Add the leftSizer (team list & match list) and rightSizer (grid) to the mainSizer
    mainSizer->Add(leftSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(rightPanel, 0, wxEXPAND | wxALL, 10);

    // Set sizer for the panel
    panel->SetSizer(mainSizer);
    this->Layout();

    // Add menu bar
    CreateMenuBar();

    DataBase db("data.db", this);
}

wxBoxSizer* MainFrame::CreateListPanel(wxWindow* parent, int listId, wxString titleName, wxString description) {
    int listWidth = this->GetSize().GetWidth() * 1.3;

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


    return listSizer;
}

void MainFrame::AddTeamListColumns() {
    if ( !m_teamListView )
        return;

    const int teamListWidth = m_teamListView->GetSize().GetWidth();

    // Add team list columns
    m_teamListView->SetFont(wxFontInfo(9).Bold());
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

void MainFrame::AddMatchListColumns() {
    if ( !m_matchListView )
        return;

    const int matchListWidth = m_matchListView->GetSize().GetWidth();

    // Add match list columns
    m_matchListView->SetFont(wxFontInfo(9).Bold()); // bold headers
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

wxGrid* MainFrame::CreateEditingGrid(wxPanel* panel) {
    // Create the grid
    wxGrid* grid = new wxGrid(panel, kEditItemGrid, wxPoint(0, 65), wxSize(400, 900));
    grid->CreateGrid(35, 1);
    grid->DisableColResize(0);
    grid->SetRowLabelSize(150);
    grid->SetColSize(0, 230);
    grid->SetLabelBackgroundColour(wxColour(255, 255, 255));
    grid->SetRowLabelAlignment(wxALIGN_LEFT, wxALIGN_TOP);
    grid->SetLabelFont(wxFontInfo(9).Bold());
    grid->SetRowSize(35, 31);

    for ( int i = 0; i < 36; i++ ) {
        grid->DisableRowResize(i);
        grid->SetRowSize(i, 25);
        grid->SetRowLabelValue(i, "...");
    }

    grid->SetColLabelValue(0, "Value");

    // Title and description above the grid
    wxStaticText* gridTitle = new wxStaticText(panel, kEditingDataTitle, "Edit Values", wxPoint(0, 10), wxDefaultSize, 0);
    gridTitle->SetFont(wxFontInfo(18).Bold());

    wxStaticText* gridDesc = new wxStaticText(panel, kEditingDataDesc, "Modifying fields for: ", wxPoint(0, 41), wxDefaultSize, 0);
    gridDesc->SetFont(wxFontInfo(10));

    return grid;
}

wxTextCtrl* MainFrame::CreateSQLHistoryBox(wxPanel* rightPanel) {
    wxTextCtrl* sqlHistory = new wxTextCtrl(rightPanel, kSQLHistoryTextBox, wxEmptyString, wxPoint(425, 65), wxSize(400, 900), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);

    wxFont font(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
    font.Scale(.9);
    sqlHistory->SetFont(font);
    return sqlHistory;
}

void MainFrame::ShowTeamEditGrid() {
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    grid->SetRowLabelValue(0, "Team #");
    grid->SetRowLabelValue(1, "Overall (0-100)");
    grid->SetRowLabelValue(2, "Eliminated (Y/N)");
    grid->SetRowLabelValue(3, "Hang Attempt (Y/N)");
    grid->SetRowLabelValue(4, "Hang Success (Y/N)");
    grid->SetRowLabelValue(5, "Robot Cycle Speed");
    grid->SetRowLabelValue(6, "Coral Points");
    grid->SetRowLabelValue(7, "Defense (0-100)");
    grid->SetRowLabelValue(8, "Auto. Points");
    grid->SetRowLabelValue(9, "Driver Skill");
    grid->SetRowLabelValue(10, "Fouls");
    grid->SetRowLabelValue(11, "Ranking Points");
    grid->SetRowLabelValue(12, "Points Per Match");
}

void MainFrame::ShowMatchEditGrid() {
    wxGrid* grid = ( wxGrid* ) FindWindow(kEditItemGrid);
    grid->SetRowLabelValue(0, "Match #");
    grid->SetRowLabelValue(1, "Finished (Y/N)");
    grid->SetRowLabelValue(2, "Red Win (Y/N)");
    grid->SetRowLabelValue(3, "Blue Win (Y/N)");
    grid->SetRowLabelValue(4, "Team 1 #");
    grid->SetRowLabelValue(5, "Team 2 #");
    grid->SetRowLabelValue(6, "Team 3 #");
    grid->SetRowLabelValue(7, "Team 4 #");
    grid->SetRowLabelValue(8, "Team 5 #");
    grid->SetRowLabelValue(9, "Team 6 #");
}

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

void MainFrame::CreateTeamRow(const Team& team) {
    const int itemId = m_displayedTeamCount++;
    
    wxListItem item;
    item.SetId(itemId);

    m_teamListView->InsertItem(item);
    m_teamListView->SetItemFont(itemId, wxFontInfo(9));
    m_teamListView->SetItem(itemId, 0, std::to_string(team.teamNum));
    m_teamListView->SetItem(itemId, 1, std::to_string(team.overall));
    m_teamListView->SetItem(itemId, 2, ( team.eliminated ) ? "Y" : "N");
    m_teamListView->SetItem(itemId, 3, ( team.hangAttempt ) ? "Y" : "N");
    m_teamListView->SetItem(itemId, 4, ( team.hangSuccess ) ? "Y" : "N");
    m_teamListView->SetItem(itemId, 5, std::to_string(team.robotCycleSpeed));
    m_teamListView->SetItem(itemId, 6, std::to_string(team.coralPoints));
    m_teamListView->SetItem(itemId, 7, std::to_string(team.defense));
    m_teamListView->SetItem(itemId, 8, std::to_string(team.autonomousPoints));
    m_teamListView->SetItem(itemId, 9, std::to_string(team.driverSkill));
    m_teamListView->SetItem(itemId, 10, std::to_string(team.fouls));
    m_teamListView->SetItem(itemId, 11, std::to_string(team.rankingPoints));
    m_teamListView->SetItem(itemId, 12, std::to_string(team.ppm));
    
    if ( itemId % 2 == 0 )
        m_teamListView->SetItemBackgroundColour(itemId, wxColour(245, 245, 245));
    else
        m_teamListView->SetItemBackgroundColour(itemId, wxColour(250, 250, 250));
}

void MainFrame::CreateMatchRow(const Match& match) {
    const int itemId = m_displayedMatchCount++;

    wxListItem item;
    item.SetId(itemId);

    m_matchListView->InsertItem(item);
    m_matchListView->SetItemFont(itemId, wxFontInfo(9));
    m_matchListView->SetItem(itemId, 0, std::to_string(match.matchNum));
    m_matchListView->SetItem(itemId, 1, ( match.played ) ? "Y" : "N");
    m_matchListView->SetItem(itemId, 2, ( match.redWin ) ? "Y" : "N");
    m_matchListView->SetItem(itemId, 3, ( match.blueWin ) ? "Y" : "N");
    m_matchListView->SetItem(itemId, 4, std::to_string(match.Team1().teamNum));
    m_matchListView->SetItem(itemId, 5, std::to_string(match.Team2().teamNum));
    m_matchListView->SetItem(itemId, 6, std::to_string(match.Team3().teamNum));
    m_matchListView->SetItem(itemId, 7, std::to_string(match.Team4().teamNum));
    m_matchListView->SetItem(itemId, 8, std::to_string(match.Team5().teamNum));
    m_matchListView->SetItem(itemId, 9, std::to_string(match.Team6().teamNum));

    if ( itemId % 2 == 0 )
        m_matchListView->SetItemBackgroundColour(itemId, wxColour(245, 245, 245));
    else
        m_matchListView->SetItemBackgroundColour(itemId, wxColour(250, 250, 250));
}

bool MainFrame::UpdateQueryHistory(std::string queryHistory) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return false;

    if ( queryHistory.empty() )
        return false;

    queryHistory = "SQL> " + queryHistory + "\n\n";
    SQLHistoryTextBox->AppendText(queryHistory);

    return true;
}

void MainFrame::LogSQLError(std::string errorMsg) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return;

    errorMsg = "ERROR> " + errorMsg + "\n\n";
    wxTextAttr defaultAttr = SQLHistoryTextBox->GetDefaultStyle();
    SQLHistoryTextBox->SetDefaultStyle(wxTextAttr(*wxRED));
    SQLHistoryTextBox->AppendText(errorMsg);
    SQLHistoryTextBox->SetDefaultStyle(defaultAttr);
}

void MainFrame::LogBackendMessage(std::string msg) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return;

    msg = "MSG> " + msg + "\n\n";

    wxTextAttr defaultAttr = SQLHistoryTextBox->GetDefaultStyle();
    SQLHistoryTextBox->SetDefaultStyle(wxTextAttr(*wxBLUE));
    SQLHistoryTextBox->AppendText(msg);
    SQLHistoryTextBox->SetDefaultStyle(defaultAttr);
}

#endif // _USING_UI