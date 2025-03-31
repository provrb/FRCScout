// Frontend
#include "frontend/mainframe.h"

/**
 * @brief Logs a message to the SQL history text box with a specified text color.
 *
 * This function retrieves the SQL history text box, checks if it is valid, and logs the provided message to it.
 * The message is displayed in the specified text color. If the message is empty or if the text box is not found,
 * the function exits without performing any further actions. After appending the message, the text color is reset
 * to the default style of the text box.
 *
 * @param msg The message to be logged and displayed in the text box.
 * @param colour The color in which the message text will be displayed.
 */
void MainFrame::LogMessage(std::string& msg, wxColour colour) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return;

    if ( msg.empty() )
        return;

    static const wxTextAttr defaultAttr = SQLHistoryTextBox->GetDefaultStyle();

    if ( !m_darkModeTheme )
        SQLHistoryTextBox->SetDefaultStyle(wxTextAttr(colour)); // change text colour to red
    else // dark mode output will always be white. other colous look terrible
        SQLHistoryTextBox->SetDefaultStyle(wxTextAttr(LIGHT_GRAY_ACCENT_2));

    SQLHistoryTextBox->AppendText(msg);
    SQLHistoryTextBox->SetDefaultStyle(defaultAttr); // reset text colour
}

/**
 * @brief Updates the SQL query history in the text box.
 *
 * This function appends an SQL query that was ran by the backend, prefixed by
 * "SQL> ". Used to display all executed SQL queries for debugging purposes.
 *
 * @param query The SQL query string to append.
 */
void MainFrame::LogSQLQuery(std::string query) {
    query = "SQL> " + query + "\n\n";
    LogMessage(query);

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
void MainFrame::LogErrorMessage(std::string errorMsg) {
    errorMsg = "ERROR> " + errorMsg + "\n\n";
    LogMessage(errorMsg, *wxRED);
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
    msg = "MSG> " + msg + "\n\n";
    LogMessage(msg, *wxBLUE);
}

/*
 * @brief Clears the output in the SQL history text box.
 *
 * This function finds the SQL history text box and clears its contents.
 *
 * @param Unused wxCommandEvent so this function can be
 * bound to WX events like button clicks.
 */
void MainFrame::ClearOutput(wxCommandEvent&) {
    wxTextCtrl* SQLHistoryTextBox = ( wxTextCtrl* ) FindWindow(kSQLHistoryTextBox);
    if ( !SQLHistoryTextBox )
        return;
    SQLHistoryTextBox->Clear();
}