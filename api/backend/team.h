#pragma once

#include <cstdint>
#include <sqlite3.h>

struct Team {
    int teamNum; // unique team number used to identify the team
    int matchNum; // match the team is in, if any

    bool eliminated; // if the team is eliminated from the tournanment
    bool hangAttempt; // did the team attempt to hang at the end of the match
    bool hangSuccess; // if the team attempted to hang, did they succeed

    // Statistics
    uint16_t robotCycleSpeed; // 1-100
    uint16_t coralPoints;
    uint16_t defense; // 1-100
    uint16_t autonomousPoints;
    uint16_t driverSkill; // 1-100
    uint16_t fouls;
    uint16_t overall; // 1-100
    uint16_t rankingPoints;
    uint16_t ppm; // Points per match

    static Team FromSQLStatment(sqlite3_stmt* stmt); // Create a new Team struct from SQL DB
    void DebugPrint() const; // print out all fields of a Team
};