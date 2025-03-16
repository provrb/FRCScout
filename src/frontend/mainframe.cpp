#include "frontend/mainframe.h"
#include "frontend/wxids.h" // WinId enum
#include <wx/grid.h>

// WX Components

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) // Initial window size
{
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);

    // Create two list panels
    leftSizer->Add(CreateListPanel(panel, kTeamListView, "Teams", "View and edit specific fields of any team."), 1, wxEXPAND | wxALL, 10);
    leftSizer->Add(CreateListPanel(panel, kMatchListView, "Matches", "View and modify individual fields of a match."), 1, wxEXPAND | wxALL, 10);

    // Get list views
    m_TeamListView = ( wxListCtrl* ) FindWindow(kTeamListView);
    m_MatchListView = ( wxListCtrl* ) FindWindow(kMatchListView);
    const int teamListWidth = m_TeamListView->GetSize().GetWidth();
    const int matchListWidth = m_MatchListView->GetSize().GetWidth();

    // Add team list columns
    m_TeamListView->AppendColumn("Team #", wxLIST_FORMAT_CENTER, teamListWidth * 0.055);
    m_TeamListView->AppendColumn("OVR", wxLIST_FORMAT_CENTER, teamListWidth * 0.05);
    m_TeamListView->AppendColumn("Out", wxLIST_FORMAT_CENTER, teamListWidth * 0.05);
    m_TeamListView->AppendColumn("Hang Attempt", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    m_TeamListView->AppendColumn("Hang Success", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    m_TeamListView->AppendColumn("Robot Cycle Speed", wxLIST_FORMAT_CENTER, teamListWidth * 0.12);
    m_TeamListView->AppendColumn("Coral Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.085);
    m_TeamListView->AppendColumn("Defense", wxLIST_FORMAT_CENTER, teamListWidth * 0.06);
    m_TeamListView->AppendColumn("Auto. Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.085);
    m_TeamListView->AppendColumn("Driver Skill", wxLIST_FORMAT_CENTER, teamListWidth * 0.08);
    m_TeamListView->AppendColumn("Fouls", wxLIST_FORMAT_CENTER, teamListWidth * 0.06);
    m_TeamListView->AppendColumn("Ranking Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    m_TeamListView->AppendColumn("PPM", wxLIST_FORMAT_CENTER, teamListWidth * 0.06); // points per match

    // Add match list columns
    m_MatchListView->AppendColumn("Match #", wxLIST_FORMAT_CENTER, matchListWidth * 0.08);
    m_MatchListView->AppendColumn("Finished", wxLIST_FORMAT_CENTER, matchListWidth * 0.08);
    m_MatchListView->AppendColumn("Red Win", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_MatchListView->AppendColumn("Blue Win", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_MatchListView->AppendColumn("Team 1 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_MatchListView->AppendColumn("Team 2 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_MatchListView->AppendColumn("Team 3 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_MatchListView->AppendColumn("Team 4 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_MatchListView->AppendColumn("Team 5 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    m_MatchListView->AppendColumn("Team 6 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);

    // Create the grid panel on the right side
    wxPanel* rightPanel = new wxPanel(panel, wxID_ANY);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);

    // Title and description above the grid
    wxStaticText* gridTitle = new wxStaticText(rightPanel, wxID_ANY, "Editing ___", wxPoint(0, 10), wxDefaultSize, 0);
    gridTitle->SetFont(wxFontInfo(18).Bold());

    wxStaticText* gridDesc = new wxStaticText(rightPanel, wxID_ANY, "Modify fields of ", wxPoint(0, 41), wxDefaultSize, 0);
    gridDesc->SetFont(wxFontInfo(10));

    wxGrid* grid = CreateEditingGrid(rightPanel);
    wxTextCtrl* sqlHistory = new wxTextCtrl(rightPanel, wxID_ANY, wxEmptyString, wxPoint(425, 65), wxSize(400, 900), wxTE_MULTILINE | wxTE_READONLY);

    wxFont font(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Consolas");
    font.Scale(.9);
    sqlHistory->SetFont(font);

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
}

wxGrid* MainFrame::CreateEditingGrid(wxPanel* panel) {
    // Create the grid
    wxGrid* grid = new wxGrid(panel, wxID_ANY, wxPoint(0, 65), wxSize(400, 900));
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
    grid->SetRowLabelValue(0, "Overall (0-100)");
    grid->SetRowLabelValue(1, "Eliminated (Y/N)");
    grid->SetRowLabelValue(2, "Hang Attempt (Y/N)");
    grid->SetRowLabelValue(3, "Hang Success (Y/N)");
    grid->SetRowLabelValue(4, "Robot Cycle Speed");
    grid->SetRowLabelValue(5, "Coral Points");
    grid->SetRowLabelValue(6, "Defense (0-100)");
    grid->SetRowLabelValue(7, "Auto. Points");
    grid->SetRowLabelValue(8, "Driver Skill");
    grid->SetRowLabelValue(9, "Fouls");
    grid->SetRowLabelValue(10, "Ranking Points");
    grid->SetRowLabelValue(11, "Points Per Match");

    return grid;
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

    // Buttons on the right side
    wxButton* addButton = new wxButton(parent, wxID_ANY, "+", wxDefaultPosition, wxSize(50, 30));
    wxButton* deleteButton = new wxButton(parent, wxID_ANY, "-", wxDefaultPosition, wxSize(50, 30));

    // Set button fonts
    deleteButton->SetFont(wxFontInfo(18));
    addButton->SetFont(wxFontInfo(18));

    // Add elements to the horizontal top sizer
    topSizer->Add(textSizer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);  // Add title + desc stack
    topSizer->AddSpacer(10);  // Some space before buttons
    topSizer->Add(addButton, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);
    topSizer->Add(deleteButton, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT);

    // List view
    wxListCtrl* listCtrl = new wxListCtrl(parent, listId, wxDefaultPosition, wxSize(listWidth, 200), wxLC_REPORT);

    // Add topSizer and list view to listSizer
    listSizer->Add(topSizer, 0, wxEXPAND | wxBOTTOM, 5);
    listSizer->Add(listCtrl, 1, wxEXPAND);


    return listSizer;
}