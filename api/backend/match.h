#pragma once

#include "team.h"
#include <array>
#include <cstdint>

struct Match {
    std::array<Team, 6> teams = { 0 }; // teams facing each other
    uint8_t teamCount; // number of teams in the match

    int matchNum;
    bool played;

    // Match Results
    bool redWin;
    bool blueWin;

    const bool IsTie() const { return ( redWin == true && blueWin == true ); }
    const bool RedWon() const { return ( redWin == true && blueWin == false ); }
    const bool BlueWon() const { return ( redWin == false && blueWin == true ); }

    static Match FromSQLStatment(sqlite3_stmt* stmt); // New Match struct from SQL db
    bool TeamInMatch(int teamNum); // return true or false whether or not the team number is in 'teams'
    void AddCompetitor(Team team); // Add a team to the match
    void RemoveCompetitor(int teamNum); // Remove a team from the match
};
