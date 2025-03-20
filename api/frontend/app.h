#pragma once

#include <wx/app.h>
#include <wx/display.h>

/**
 * @brief The main application class for the wxWidgets UI.
 *
 * This class initializes and manages the application lifecycle
 * when the UI is enabled.
 */
class App : public wxApp {
public:
    /**
     * @brief Initializes the wxWidgets application.
     *
     * Called when the application starts. This function is responsible
     * for setting up the main event loop and initializing UI components.
     *
     * @return true if initialization succeeds, false otherwise.
     */
    bool OnInit();
};

wxIMPLEMENT_APP(App);