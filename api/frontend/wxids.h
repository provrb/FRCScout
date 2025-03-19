// IDs for WX menu items, windows, etc

#pragma once
#ifdef _USING_UI

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
    kExportCSVMenuItem = 0xB0,
    kExportTeamDataJSON,
    kExportMatchDataJSON,
    kExportTeamDataCSV,
    kExportMatchDataCSV,
    kSQLHistoryTextBox,
    kEditingDataTitle, // e.g "Editing Team #1" title
    kEditingDataDesc, // e.g "Modify values for Team #1" description
    kEditItemGrid, // grid to input new values for an item
    kEditModeButton,
};

enum TeamGridRowIds {
    kRowTeamNum = 0,
    kRowOverall = 1,
    kRowEliminated = 2,
    kRowHangAttempt = 3,
    kRowHangSuccess = 4,
    kRowRobotCycleSpeed = 5,
    kRowCoralPoints = 6,
    kRowDefense = 7,
    kRowAutonomousPoints = 8,
    kRowDriverSkill = 9,
    kRowFouls = 10,
    kRowRankingPoints = 11,
    kRowPPM = 12,
};

enum MatchGridRowIds {
    kRowMatchNum = 0,
    kRowPlayed = 1,
    kRowRedWin = 2,
    kRowBlueWin = 3,
    kRowTeam1 = 4,
    kRowTeam2 = 5,
    kRowTeam3 = 6,
    kRowTeam4 = 7,
    kRowTeam5 = 8,
    kRowTeam6 = 9,
};

#endif // _USING_UI
