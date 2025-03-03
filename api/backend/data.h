#pragma once

#include <sqlite3.h>
#include <vector>
#include <array>
#include <memory>
#include <string>

const std::string DB_PATH = "data.db";
const std::string TEAM_TABLE = "Teams";
const std::string MATCH_TABLE = "Matches";
const std::string MATCH_TEAMS_TABLE = "MatchTeams";

struct Team {
    int teamNum;
    bool eliminated;

    bool hangAttempt;
    bool hangSuccess;

    // Statistics
    uint8_t robotCycleSpeed; // 1-100
    uint8_t coralPoints;
    uint8_t defense; // 1-5
    uint8_t autonomousPoints;
    uint8_t driverSkill; // 1-100
    uint8_t fouls; 
    uint8_t overall; // 1-10
    uint16_t rankingPoints;
    uint16_t ppm; // Points per match
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

    std::vector<Team> GetTeams();
    std::vector<Match> GetMatches();
private:
    // Create tables if they don't exist
    bool TableExists(const std::string& tableName);
    void CreateTables();

    // Create blank SQL tables
    void NewTeamTable();
    void NewMatchesTable(); // consists of matches
    void NewMatchTeamsTable(); // consists of teams in each match

    // Connect to the SQL database
    void Connect();
    void Disconnect();

    // SQL database
    sqlite3* db;
    const std::string dbPath; // Path to the .db file
    bool m_Connected; // If the database is connected
};
