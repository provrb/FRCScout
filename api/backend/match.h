#pragma once

#include "team.h"
#include <array>

struct Match {
    std::array<Team*, 6> teams; // teams facing each other
    int matchNum;
    bool played;

    // Match Results
    bool redWin;
    bool blueWin;

    const bool IsTie() const { return ( redWin == true && blueWin == true ); }
    const bool RedWon() const { return ( redWin == true && blueWin == false ); }
    const bool BlueWon() const { return ( redWin == false && blueWin == true ); }

    static Match FromSQLStatment(sqlite3_stmt* stmt); // New Match struct from SQL db
    void AddCompetitor(Team* team); // Add a team to the match
    void RemoveCompetitor(int teamNum); // Remove a team from the match
};
