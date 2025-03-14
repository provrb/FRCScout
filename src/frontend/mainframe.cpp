#include "frontend/mainframe.h"
#include "frontend/winids.h" // WinId enum

// WX Components
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) // Initial window size
{
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);

    int listWidth = this->GetSize().GetWidth() * 0.9; 

    // Function to create a list view with buttons above it
    auto CreateListPanel = [&](wxWindow* parent, int listId) -> wxBoxSizer* {
        wxBoxSizer* listSizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

        // Buttons
        wxButton* addButton = new wxButton(parent, wxID_ANY, "Add", wxDefaultPosition, wxSize(listWidth / 3, 30));
        wxButton* deleteButton = new wxButton(parent, wxID_ANY, "Delete", wxDefaultPosition, wxSize(listWidth / 3, 30));

        // Buttons should touch the edges
        buttonSizer->Add(addButton, 1, wxEXPAND);
        buttonSizer->Add(deleteButton, 1, wxEXPAND);

        // List view
        wxListCtrl* listCtrl = new wxListCtrl(parent, listId, wxDefaultPosition, wxSize(listWidth, 200), wxLC_REPORT);

        // Add columns
        listCtrl->InsertColumn(0, "Team #", wxLIST_FORMAT_LEFT, listWidth * 0.25);
        listCtrl->InsertColumn(1, "Stat 1", wxLIST_FORMAT_LEFT, listWidth * 0.2);
        listCtrl->InsertColumn(2, "Stat 2", wxLIST_FORMAT_LEFT, listWidth * 0.2);
        listCtrl->InsertColumn(3, "Stat 3", wxLIST_FORMAT_LEFT, listWidth * 0.2);
        listCtrl->InsertColumn(4, "Stat 4", wxLIST_FORMAT_LEFT, listWidth * 0.2);

        // Example row
        long index = listCtrl->InsertItem(0, "Team 1");
        listCtrl->SetItem(index, 1, "100");
        listCtrl->SetItem(index, 2, "90");
        listCtrl->SetItem(index, 3, "85");
        listCtrl->SetItem(index, 4, "80");

        // Add buttons and list to the sizer
        listSizer->Add(buttonSizer, 0, wxEXPAND | wxBOTTOM, 5);
        listSizer->Add(listCtrl, 1, wxEXPAND);

        return listSizer;
        };

    // Create two list panels
    leftSizer->Add(CreateListPanel(panel, kTeamListView), 1, wxEXPAND | wxALL, 10);
    leftSizer->Add(CreateListPanel(panel, kTeamListView + 1), 1, wxEXPAND | wxALL, 10);

    mainSizer->Add(leftSizer, 0, wxEXPAND | wxLEFT | wxTOP | wxBOTTOM, 10);
    panel->SetSizer(mainSizer);
    this->Layout();
}
