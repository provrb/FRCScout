#pragma once

#include <cstdint> // uint16_t
#include <sqlite3.h> // sqlite3_stmt, sqlite3_column_int
#include <random>

/**
 * @struct Team
 * @brief Represents a robotics competition team and its performance metrics.
 *
 * This struct holds information about a team, including its unique identifier,
 * match participation, performance statistics, and other key attributes used
 * for ranking and analysis in a tournament setting.
 *
 * @param uid              Unique identifier for the team.
 * @param teamNum          Team number used to identify the team.
 * @param matchNum         Match number the team is currently competing in (if applicable).
 * @param eliminated       Indicates whether the team has been eliminated from the tournament.
 * @param hangAttempt      Specifies if the team attempted to hang at the end of a match.
 * @param hangSuccess      Indicates if the team's hang attempt was successful.
 *
 * @param robotCycleSpeed  Performance metric for robot cycle speed (1-100 scale).
 * @param coralPoints      Points scored by the team in the coral section.
 * @param defense          Defensive capability rating of the team (1-100 scale).
 * @param autonomousPoints Points scored during the autonomous period.
 * @param driverSkill      Driver's skill level, rated from 1 to 100.
 * @param fouls            Number of fouls committed by the team.
 * @param overall          Overall performance rating of the team (1-100 scale).
 * @param rankingPoints    Ranking points earned by the team.
 * @param ppm              Points per match (average performance metric).
 *
 * @note The struct provides functions to initialize a team from an SQLite database
 *       and to print debug information for analysis.
 */
struct Team {
    int uid; // unique id 

    int teamNum; // unique team number used to identify the team
    int matchNum; // match the team is in, if any

    bool hangAttempt; // did the team attempt to hang at the end of the match
    bool hangSuccess; // if the team attempted to hang, did they succeed

    // Statistics
    uint16_t robotCycleSpeed; // 1-100
    uint16_t coralPoints;
    uint16_t defense; // 1-100
    uint16_t autonomousPoints;
    uint16_t driverSkill; // 1-100
    uint16_t penaltys;
    uint16_t overall; // 1-100
    uint16_t rankingPoints;

    static Team FromSQLStatment(sqlite3_stmt* stmt); // Create a new Team struct from SQL DB
};
