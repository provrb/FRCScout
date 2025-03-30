#include "frontend/app.h"
#include "frontend/mainframe.h"

/**
 * @brief Initializes the wxWidgets application.
 *
 * This function is called when the application starts. It creates the
 * main application window, sets the icon, adjusts the window size
 * to fit the display, and enforces a minimum size constraint.
 *
 * @return true Always returns true to indicate successful initialization.
 */
bool App::OnInit() {
    bool darkMode = MSWEnableDarkMode();
    
    // Create the main application window
    MainFrame* mainFrame = new MainFrame(APP_NAME, darkMode);
    mainFrame->Show(true);
    // mainFrame->SetIcon(wxICON(IDI_ICON1));

    // Get display information for positioning and sizing
    wxDisplay display(wxDisplay::GetFromWindow(mainFrame));
    wxRect dimensions = display.GetClientArea();

    // Maximize the window and set constraints
    mainFrame->Maximize();
    mainFrame->SetClientSize(dimensions.GetWidth(), dimensions.GetHeight());
    //mainFrame->SetMinClientSize(wxSize(800, 600));

    return true;
}