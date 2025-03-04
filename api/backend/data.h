#pragma once

#include <sqlite3.h>
#include <vector>
#include <array>
#include <memory>
#include <string>

#define DB_PATH "data.db"
#define TEAM_TABLE "Teams"
#define MATCH_TABLE "Matches"
#define MATCH_TEAMS_TABLE "MatchTeams"

struct Team {
    int teamNum;
    bool eliminated;

    bool hangAttempt;
    bool hangSuccess;

    // Statistics
    uint8_t robotCycleSpeed; // 1-100
    uint8_t coralPoints;
    uint8_t defense; // 1-100
    uint8_t autonomousPoints;
    uint8_t driverSkill; // 1-100
    uint8_t fouls; 
    uint8_t overall; // 1-100
    uint16_t rankingPoints;
    uint16_t ppm; // Points per match

    /// Create a new Team struct from SQL DB
    Team FromSQLStatment(sqlite3_stmt* stmt);
};

struct Match {
    std::array<Team*, 6> teams; // teams facing each other
    int matchNum;
    bool played;
    
    // Match Results
    bool redWin;
    bool blueWin;

    const bool IsTie() const { return (redWin == true && blueWin == true); }
    const bool RedWon() const { return ( redWin == true && blueWin == false ); }
    const bool BlueWon() const { return ( redWin == false && blueWin == true ); }
};

class DataBase {
public:
    DataBase(const std::string& dbPath);
    ~DataBase();

    // find the fields that are different and make an sql query to update them specifically
    void EditTeam(const Team& team); 
    void EditMatch(const Match& match);

    void AddTeam(const Team& team);
    void AddMatch(const Match& match);
    void AddTeamToMatch(int teamNum, int matchNum);
    void RemoveTeam(int teamNum);
    void RemoveMatch(int matchNum);

    bool TeamExists(int teamNum);
    bool MatchExists(int matchNum);
    bool TeamInMatch(int teamNum, int matchNum);

    // Note: to modify a team or match, use EditTeam and call GetTeams for maximum safety
    const std::vector<Team> GetTeams();
    const std::vector<Match> GetMatches();
private:
    void Connect(); // Connect to the SQL database
    void Disconnect(); // Disconnect from the SQL database

    bool TableExists(const std::string& tableName); // check if an SQL table exists
    void CreateTables(); // create all required and used SQL tables
    void NewTeamTable(); // create blank Team SQL table
    void NewMatchesTable(); // create blank Matches SQL Table
    void NewMatchTeamsTable(); // consists of teams in each match
    sqlite3_stmt* MakeQuery(const std::string& query);
    void SQLFatalError(uint8_t exitCode, const char* errMsg); // Exit program with an error message

    sqlite3* db; // SQL database
    const std::string dbPath; // Path to the .db file
    bool m_Connected; // If the database is connected
};
