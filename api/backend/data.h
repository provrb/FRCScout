#pragma once

#include <sqlite3.h>
#include <vector>
#include <array>
#include <memory>
#include <string>

struct Team {
    int teamNum;
    bool eliminated;

    bool hangAttempt;
    bool hangSuccess;

    // Statistics
    uint8_t robotSpeed; // 1-5
    uint8_t shooterAccuracy; // % out of 100
    uint8_t defense; // 1-5
    uint8_t autonomous; // 1-5. How well their autonomous performance is
    uint8_t driverSkill; // 1-5
    uint8_t strategy; // 1-5
    uint8_t fouls; 
    uint8_t overall; // 1-10
    uint16_t rankingPoints;
    uint16_t ppm; // Points per match
};

struct Alliance {
    std::array<Team*, 3> teams;
    int allianceNum;
    bool eliminated;
    uint16_t score;
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
    void RemoveTeam(int teamNum);
    void RemoveMatch(int matchNum);

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
    bool connected; // If the database is connected
};
