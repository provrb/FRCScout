#include "data.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>

DataBase::DataBase(const std::string& path) : dbPath(path) {
    if ( !std::filesystem::exists(dbPath) ) {
        std::cout << "File with path " << dbPath << " doesn't exist. Creating it" << std::endl;
        std::ofstream file(dbPath);
        if ( !file.is_open() ) {
            std::cout << "Failed to create file. Aborting." << std::endl;
            exit(-1);
        }
    
        std::cout << "File created successfully" << std::endl;
    }
    else {
        std::cout << "File with path " << dbPath << " exists." << std::endl;
    }

    Connect();
    CreateTables();
}

DataBase::~DataBase() {
    Disconnect();
}

void DataBase::EditTeam(const Team& team) {

}

void DataBase::EditMatch(const Match& match) {

}

bool DataBase::TeamExists(int teamNum) {
    if (!TableExists(TEAM_TABLE))
        return false;

    std::string query = 
        "SELECT * from Teams";

    std::cout << "Query: " << query << std::endl;

    sqlite3_stmt* stmt = MakeQuery(query);  
    while ( sqlite3_step(stmt) == SQLITE_ROW ) {
        int team = sqlite3_column_int(stmt, 0);
        std::cout << "Team: " << team << std::endl;
        if ( team == teamNum ) {
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);

    return false;
}

void DataBase::AddTeam(const Team& team) {
    std::string query = 
        "INSERT OR REPLACE INTO Teams "
        "(teamNum, eliminated, hangAttempt, "
        "hangSuccess, robotCycleSpeed, coralPoints, "
        "defense, autonomousPoints, driverSkill, "
        "fouls, overall, rankingPoints, ppm) "
        "VALUES ("
        + std::to_string(team.teamNum) + ", "
        + std::to_string(team.eliminated) + ", "
        + std::to_string(team.hangAttempt) + ", "
        + std::to_string(team.hangSuccess) + ", "
        + std::to_string(team.robotCycleSpeed) + ", "
        + std::to_string(team.coralPoints) + ", "
        + std::to_string(team.defense) + ", "
        + std::to_string(team.autonomousPoints) + ", "
        + std::to_string(team.driverSkill) + ", "
        + std::to_string(team.fouls) + ", "
        + std::to_string(team.overall) + ", "
        + std::to_string(team.rankingPoints) + ", "
        + std::to_string(team.ppm) + ");";

    std::cout << "Adding team with team number " << team.teamNum << std::endl;

    int res = sqlite3_exec(db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Added team to teams table." << std::endl;
}

void DataBase::AddMatch(const Match& match) {
    std::string query =
        "INSERT OR REPLACE INTO Matches "
        "(matchNum, blueWin, played, "
        "redWin) VALUES ("
        + std::to_string(match.matchNum) + ", "
        + std::to_string(match.blueWin) + ", "
        + std::to_string(match.played) + ", "
        + std::to_string(match.redWin) + ");";

    std::cout << "Adding match with match number " << match.matchNum << std::endl;

    int res = sqlite3_exec(db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Added match to matches table." << std::endl;
}

void DataBase::RemoveTeam(int teamNum) {

}

void DataBase::RemoveMatch(int matchNum) {

}

std::vector<Team> DataBase::GetTeams() {

    return std::vector<Team>();
}

std::vector<Match> DataBase::GetMatches() {

    return std::vector<Match>();
}

bool DataBase::TableExists(const std::string& tableName) {
    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';";
    sqlite3_stmt* stmt;

    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to prepare statement \"" << query << "\" Aborting." << std::endl;
        exit(-1);
    }

    // Found a table with name tableName
    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        std::cout << sqlite3_column_int(stmt, 0) << std::endl;
        sqlite3_finalize(stmt);
        std::cout << "Table with name " << tableName << " exists." << std::endl;
        return true;
    }

    std::cout << "Table with name " << tableName << " doesn't exist." << std::endl;

    return false;
}

void DataBase::CreateTables() {
    std::cout << "Creating tables" << std::endl;
    Connect();
    NewTeamTable();
    NewMatchesTable();
    NewMatchTeamsTable();
}

void DataBase::NewTeamTable() {
    std::cout << "Creating blank team table." << std::endl;
    if ( TableExists(TEAM_TABLE) )
        return;

    char* errorMsg;
    const char* query =
        "CREATE TABLE IF NOT EXISTS Teams ("
        "teamNum INTEGER PRIMARY KEY, "
        "eliminated INTEGER, "
        "hangAttempt INTEGER, "
        "hangSuccess INTEGER, "
        "robotCycleSpeed INTEGER, "
        "coralPoints INTEGER, "
        "defense INTEGER, "
        "autonomousPoints INTEGER, "
        "driverSkill INTEGER, "
        "strategy INTEGER, "
        "fouls INTEGER, "
        "overall INTEGER, "
        "rankingPoints INTEGER, "
        "ppm INTEGER"
        ");";

    Connect();
    
    int res = sqlite3_exec(db, query, NULL, 0, &errorMsg);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting. Error message: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        exit(-1);
    }

    std::cout << "Created blank team table." << std::endl;
}

void DataBase::NewMatchesTable() {
    std::cout << "Creating blank matches table." << std::endl;
    if ( TableExists("Matches") )
        return;

    char* errorMsg;
    const char* query =
        "CREATE TABLE IF NOT EXISTS Matches ("
        "matchNum INTEGER PRIMARY KEY, "
        "played INTEGER, "
        "redWin INTEGER, "
        "blueWin INTEGER"
        ");";

    Connect();

    int res = sqlite3_exec(db, query, NULL, 0, &errorMsg);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting. Error message: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        exit(-1);
    }

    std::cout << "Created blank matches table." << std::endl;
}

void DataBase::NewMatchTeamsTable() {
    std::cout << "Creating blank match teams table." << std::endl;
    if ( TableExists("MatchTeams") )
        return;
    
    char* errorMsg;

    // each team will have a table describing what match they belong to
    const char* query =
        "CREATE TABLE IF NOT EXISTS MatchTeams ("
        "matchNum INTEGER, "
        "teamNum INTEGER, "
        "PRIMARY KEY(matchNum, teamNum),"
        "FOREIGN KEY (matchNum) REFERENCES Matches(matchNum), "
        "FOREIGN KEY (teamNum) REFERENCES Teams(teamNum)"
        ");";
    
    Connect();
    
    int res = sqlite3_exec(db, query, NULL, 0, &errorMsg);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting. Error message: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        exit(-1);
    }

    std::cout << "Created blank match teams table." << std::endl;
}

sqlite3_stmt* DataBase::MakeQuery(const std::string& query) {
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to prepare statement \"" << query << "\" Aborting." << std::endl;
        exit(-1);
    }

    return stmt;
}

void DataBase::Connect() {
    std::cout << "Connecting to SQL DB. Path: " << dbPath << std::endl;
    if ( m_Connected ) {
        std::cout << "Already connected to SQL DB" << std::endl;
        return;
    }

    int res = sqlite3_open(dbPath.c_str(), &db);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to connect to SQL DB. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Connected to SQL DB" << std::endl;
    m_Connected = true;
}

void DataBase::Disconnect() {
    std::cout << "Disconnecting from SQL DB" << std::endl;

    sqlite3_close(db);
    m_Connected = false;
}
