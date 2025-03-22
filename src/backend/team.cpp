#include <team.h>
#include <iostream> // cout

/**
 * @brief Creates a Team object from an SQLite database statement.
 *
 * Extracts team statistics from the given SQLite statement, including performance metrics
 * such as hang attempts, cycle speed, defense, and ranking points.
 *
 * @param stmt Pointer to an SQLite statement containing team data.
 * @return Team object populated with data from the database.
 */
Team Team::FromSQLStatment(sqlite3_stmt* stmt) {
    Team team = {};
    team.uid = sqlite3_column_int(stmt, 0);
    team.teamNum = sqlite3_column_int(stmt, 1);
    team.matchNum = sqlite3_column_int(stmt, 2);
    team.hangAttempt = sqlite3_column_int(stmt, 3);
    team.hangSuccess = sqlite3_column_int(stmt, 4);
    team.robotCycleSpeed = sqlite3_column_int(stmt, 5);
    team.coralPoints = sqlite3_column_int(stmt, 6);
    team.defense = sqlite3_column_int(stmt, 7);
    team.autonomousPoints = sqlite3_column_int(stmt, 8);
    team.driverSkill = sqlite3_column_int(stmt, 9);
    team.penaltys = sqlite3_column_int(stmt, 10);
    team.overall = sqlite3_column_int(stmt, 11);
    team.rankingPoints = sqlite3_column_int(stmt, 12);

    return team;
}