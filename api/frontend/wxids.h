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

enum TeamGridColIds {
    kColTeamNum = 0,
    kColOverall = 1,
    kColEliminated = 2,
    kColHangAttempt = 3,
    kColHangSuccess = 4,
    kColRobotCycleSpeed = 5,
    kColCoralPoints = 6,
    kColDefense = 7,
    kColAutonomousPoints = 8,
    kColDriverSkill = 9,
    kColFouls = 10,
    kColRankingPoints = 11,
    kColPPM = 12,
};

enum MatchGridColIds {
    kColMatchNum = 0,
    kColPlayed = 1,
    kColRedWin = 2,
    kColBlueWin = 3,
    kColTeam1 = 4,
    kColTeam2 = 5,
    kColTeam3 = 6,
    kColTeam4 = 7,
    kColTeam5 = 8,
    kColTeam6 = 9,
};

#endif // _USING_UI
