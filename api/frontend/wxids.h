// IDs for WX menu items, windows, etc

#pragma once
#ifdef _USING_UI

// Window IDs for WX
enum WinIds {
    kTeamListView = 0x30,
    kMatchListView,
};

// IDs for items like wxMenuItem
enum ItemIds {
    kExportCSVMenuItem = 0xB0,
    kExportTeamDataJSON,
    kExportMatchDataJSON,
    kExportTeamDataCSV,
    kExportMatchDataCSV,
    kSQLHistoryTextBox,
    kEditingDataTitle, // e.g "Editing Team #1" title
    kEditingDataDesc, // e.g "Modify values for Team #1" description
};

#endif // _USING_UI
