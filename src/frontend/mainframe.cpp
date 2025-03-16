#include "frontend/mainframe.h"
#include "frontend/wxids.h" // WinId enum

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
    wxListCtrl* teamListView = ( wxListCtrl* ) FindWindow(kTeamListView);
    wxListCtrl* matchListView = ( wxListCtrl* ) FindWindow(kMatchListView);
    const int teamListWidth = teamListView->GetSize().GetWidth();
    const int matchListWidth = matchListView->GetSize().GetWidth();

    // Add team list columns
    teamListView->AppendColumn("Team #", wxLIST_FORMAT_CENTER, teamListWidth * 0.055);
    teamListView->AppendColumn("OVR", wxLIST_FORMAT_CENTER, teamListWidth * 0.05);
    teamListView->AppendColumn("Out", wxLIST_FORMAT_CENTER, teamListWidth * 0.05);
    teamListView->AppendColumn("Hang Attempt", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    teamListView->AppendColumn("Hang Success", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    teamListView->AppendColumn("Robot Cycle Speed", wxLIST_FORMAT_CENTER, teamListWidth * 0.12);
    teamListView->AppendColumn("Coral Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.085);
    teamListView->AppendColumn("Defense", wxLIST_FORMAT_CENTER, teamListWidth * 0.06);
    teamListView->AppendColumn("Auto. Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.085);
    teamListView->AppendColumn("Driver Skill", wxLIST_FORMAT_CENTER, teamListWidth * 0.08);
    teamListView->AppendColumn("Fouls", wxLIST_FORMAT_CENTER, teamListWidth * 0.06);
    teamListView->AppendColumn("Ranking Points", wxLIST_FORMAT_CENTER, teamListWidth * 0.1);
    teamListView->AppendColumn("PPM", wxLIST_FORMAT_CENTER, teamListWidth * 0.06); // points per match

    // Add match list columns
    matchListView->AppendColumn("Match #", wxLIST_FORMAT_CENTER, matchListWidth * 0.08);
    matchListView->AppendColumn("Finished", wxLIST_FORMAT_CENTER, matchListWidth * 0.08);
    matchListView->AppendColumn("Red Win", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    matchListView->AppendColumn("Blue Win", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    matchListView->AppendColumn("Team 1 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    matchListView->AppendColumn("Team 2 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    matchListView->AppendColumn("Team 3 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    matchListView->AppendColumn("Team 4 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    matchListView->AppendColumn("Team 5 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);
    matchListView->AppendColumn("Team 6 #", wxLIST_FORMAT_CENTER, matchListWidth * 0.07);

    mainSizer->Add(leftSizer, 0, wxEXPAND | wxLEFT | wxTOP | wxBOTTOM, 10);
    panel->SetSizer(mainSizer);
    this->Layout();

    // Add menu bar
    CreateMenuBar();
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
    int listWidth = this->GetSize().GetWidth() * 1.2;

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

