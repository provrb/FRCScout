// IDs for WX menu items, windows, etc

#pragma once

/**
 * @brief Enumeration for window IDs used in the application.
 *
 * These IDs are used to uniquely identify various 
 * windows in the UI, such as the team and match list views.
 */
enum WinIds {
    kTeamListView = 0x30,
    kMatchListView,
};

/**
 * @brief Enumeration for item IDs used in menus and other UI components.
 *
 * These IDs are used to uniquely identify 
 * menu items, text boxes, and other components within the UI.
 */
enum ItemIds {
    kExportCSVMenuItem = 0xFBB,
    kExportTeamDataJSON,
    kExportMatchDataJSON,
    kExportTeamDataCSV,
    kExportMatchDataCSV,
    kImportTeamDataCSV,
    kImportMatchDataCSV,
    kSQLHistoryTextBox,
    kEditingDataTitle, // e.g "Editing Team #1" title
    kEditingDataDesc, // e.g "Modify values for Team #1" description
    kEditItemGrid, // grid to input new values for an item
    kEditModeButton,
    kClearOutputButton,
    kPredictMatch, // right click context menu button for predicting match outcome
};

/**
 * @brief Enum representing row indices for the team data grid.
 *
 * This enum defines the row positions for various team-related statistics
 * in the team data grid. Each row corresponds to a specific data field
 * that stores information about a team's performance.
 */
enum TeamGridRowIds {
    kRowTeamNum = 0,
    kRowInMatchNum, // the match the team participated in when being tracked
    kRowOverall,
    kRowHangAttempt,
    kRowHangSuccess,
    kRowRobotCycleSpeed,
    kRowCoralPoints,
    kRowDefense,
    kRowAutonomousPoints,
    kRowDriverSkill,
    kRowPenaltys,
    kRowRankingPoints,
};

/**
 * @brief Enum representing row indices for the match data grid.
 *
 * This enum defines the row positions for various match-related data fields
 * in the match data grid. Each row corresponds to a specific piece of match
 * information, including match number, results, and participating teams.
 */
enum MatchGridRowIds {
    kRowMatchNum = 0,
    kRowRedWin,
    kRowBlueWin,
    kRowRed1,
    kRowRed2,
    kRowRed3,
    kRowBlue4,
    kRowBlue5,
    kRowBlue6,
};