#pragma once

#include "team.h"
#include "match.h"

#include <sqlite3.h>
#include <vector>
#include <string>

#define DB_PATH "data.db"
#define TEAM_TABLE "Teams"
#define MATCH_TABLE "Matches"

class DataBase {
public:
    DataBase(const std::string& dbPath);
    ~DataBase();

    // find the fields that are different and make an sql query to update them specifically
    void UpdateTeam(const Team& team); 
    void UpdateMatch(const Match& match);

    void AddTeam(const Team& team);
    void AddMatch(const Match& match);
    void AddTeamToMatch(int teamNum, int matchNum);
    void RemoveTeamFromMatch(int teamNum, int matchNum);
    void RemoveTeam(int teamNum);
    void RemoveMatch(int matchNum);
    Team GetTeam(int teamNum);
    Match GetMatch(int matchNum);

    bool TeamExists(int teamNum); // check if a team with teamNum is in the SQL DB 
    bool MatchExists(int matchNum); // check if a match with matchNum is in the SQL DB
    bool TeamInMatch(int teamNum, int matchNum); // check if a team is in a match with matchNum
    bool TeamInMatch(int teamNum, const Match& match); // check if a team is in a match struct

    // Note: to modify a team or match, use UpdateTeam and call GetTeams for maximum safety
    std::vector<Team> GetTeams();
    std::vector<Match> GetMatches();
private:
    void Connect(); // Connect to the SQL database
    void Disconnect(); // Disconnect from the SQL database

    int BindMatchTeams(sqlite3_stmt*, const Match& match);
    bool TableExists(const std::string& tableName); // check if an SQL table exists
    void CreateTables(); // create all required and used SQL tables
    void NewTeamTable(); // create blank Team SQL table
    void NewMatchesTable(); // create blank Matches SQL Table
    void SQLFatalError(uint8_t exitCode, const char* errMsg); // Exit program with an error message

    sqlite3* db; // SQL database
    const std::string dbPath; // Path to the .db file
    bool m_Connected; // If the database is connected
};
