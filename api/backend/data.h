#pragma once

// Frontend
#include "frontend/mainframe.h" // MainFrame class

#include <team.h>    // Team struct definition
#include <match.h>   // Match struct definition
#include <sqlite3.h> // sqlite3_prepare_v2, sqlite3_exec, sqlite3_column_int, sqlite3_bind_int...
#include <vector>    // std::vector
#include <string>    // std::string

#define DB_PATH     "data.db" // Path to connect and save database file
#define TEAM_TABLE  "Teams"   // Name of the Teams table to save Team info in
#define MATCH_TABLE "Matches" // Name of the Matches table to save Match info in

/**
 * @class DataBase
 * @brief This class manages the interactions with an SQLite database that stores information about teams and matches.
 *
 * The DataBase class provides methods to add, remove, and update teams and matches, as well as retrieve information about
 * teams and matches stored in the database. It also includes helper functions to check for the existence of records
 * and perform necessary SQL operations.
 *
 * The class handles establishing a connection to the database, ensuring the required tables exist, and ensuring data
 * integrity through the various add/remove/update functions. 
 *
 * @see Team
 * @see Match
 */
class DataBase {
public:
    explicit DataBase(const std::string& dbPath, MainFrame* mainFrame);
    ~DataBase();

    // Add/remove/update/get
    void AddTeam(Team& team); // Add a team from Team struct to SQL DB
    void AddMatch(const Match& match); // Add a match from Match struct to SQL DB
    void AddTeamToMatch(int uid, int matchNum); // Add a team with teamNum to teams of match with matchNum
    void RemoveTeamFromMatch(int teamNum, int matchNum); // Remove a team with teamNum from teams of match with matchNum
    void RemoveTeam(int uid); // Remove a team with teamNum from SQL DB
    void RemoveMatch(int matchNum); // Remove a match with matchNum from SQL DB
    void UpdateTeam(const Team& team); // Update a team with teamNum from SQL DB
    void UpdateMatch(const Match& match); // Update a match with matchNum from SQL DB 
    Team GetTeam(int uid); // Get a Team struct from SQL DB of teamNum
    Match GetMatch(int matchNum); // Get a Match struct from SQL DB of matchNum
    std::vector<Team> GetTeams();
    std::vector<Match> GetMatches();
    double GetTeamWinRate(int teamNum);

    // Generate a unique ID for a new team that is not in use
    int GetNextTeamUID(); 

    // Conditional/exists/item1 in item2
    bool TeamExistsUID(int uid);
    bool TeamExists(int teamNum); // check if a team with teamNum is in the SQL DB 
    bool MatchExists(int matchNum); // check if a match with matchNum is in the SQL DB
    bool TeamInMatch(int teamNum, int matchNum); // check if a team is in a match with matchNum
    bool TeamInMatch(int teamNum, const Match& match); // check if a team is in a match struct
    
    // Exporting
    void ExportTableToJSON(const std::string& tableName, const std::string& outputFilename);
    void ExportTableToCSV(const std::string& tableName, const std::string& outputFilename);
    void ExportTOQRCode(const std::string& content, const std::string& outputFilename);
    
    // Importing
    void ImportTableFromCSV(const std::string& tableName, const std::string& inputFilename);
private:
    void Connect(); // Connect to the SQL database
    void Disconnect(); // Disconnect from the SQL database
    
    bool TableExists(const std::string& tableName); // check if an SQL table exists
    void CreateTables(); // create all required and used SQL tables
    void NewTeamTable(); // create blank Team SQL table
    void NewMatchesTable(); // create blank Matches SQL Table
    void AddQueryToHistory(sqlite3_stmt* stmt);
    void AddQueryToHistory(std::string query);
    
    sqlite3* m_db; // SQL database
    std::vector<std::string> m_queryHistory = {}; // list of SQL querys for debugging purposes
    const std::string m_dbPath; // Path to the .db file. Set when DataBase is constructed
    bool m_connected; // If the database is connected
    MainFrame* m_mainFrame; // connect backend to frontend
};
