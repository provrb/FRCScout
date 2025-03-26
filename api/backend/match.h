#pragma once

#include "team.h" // Team
#include <array> // array
#include <cstdint> // uint8_t

/**
 * @struct Match
 * @brief Represents a match in a robotics competition.
 *
 * This struct holds information about a match, including the teams participating,
 * match results, and utility functions for match management.
 *
 * @param teams      Array of up to 6 teams competing in the match.
 * @param teamCount  Number of teams in the match (should not exceed 6).
 * @param matchNum   Unique identifier for the match.
 * @param played     Indicates whether the match has been played.
 *
 * @param redWin     True if the red alliance won the match.
 * @param blueWin    True if the blue alliance won the match.
 *
 * @note If both `redWin` and `blueWin` are true, the match is considered a tie.
 *
 * @function IsTie()    Determines if the match resulted in a tie.
 * @function RedWon()   Determines if the red alliance won the match.
 * @function BlueWon()  Determines if the blue alliance won the match.
 *
 * @function FromSQLStatment()  Creates a `Match` object from an SQLite database statement.
 * @function TeamInMatch()      Checks if a given team number is part of the match.
 * @function AddCompetitor()    Adds a team to the match (if space allows).
 * @function RemoveCompetitor() Removes a team from the match.
 *
 * @function Team1() - Team6()  Access individual teams in the match.
 */
struct Match {
    std::array<Team, 6> teams = { 0 }; // teams facing each other
    uint8_t teamCount; // number of teams in the match

    int matchNum; // unique number or match index

    // Match Results. if both are true, the match was a tie
    bool redWin; // red alliance won the match
    bool blueWin; // blue alliance won the match.

    const bool RedWon() const; // Return true if red alliance won, otherwise false
    const bool BlueWon() const; // Return true if blue alliance won, otherwise false
    const bool IsTie() const; // Return true if both blueWin and redWin are true, meaning a tie happened

    static Match FromSQLStatment(sqlite3_stmt* stmt); // New Match struct from SQL db
    bool TeamInMatch(int teamNum); // return true or false whether or not the team number is in 'teams'
    void AddCompetitor(Team team); // Add a team to the match
    void RemoveCompetitor(int teamNum); // Remove a team from the match
    bool RedAllianceTeam(int teamNum) const; // Check if a team is on the red alliance

    const Team& Team1() const; // return Team from self.teams[0]
    const Team& Team2() const; // return Team from self.teams[1]
    const Team& Team3() const; // return Team from self.teams[2]
    const Team& Team4() const; // return Team from self.teams[3]
    const Team& Team5() const; // return Team from self.teams[4]
    const Team& Team6() const; // return Team from self.teams[5]
};
