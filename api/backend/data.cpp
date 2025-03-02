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

void DataBase::AddTeam(const Team& team) {
    std::string query = 
        "INSERT OR REPLACE INTO Teams "
        "(teamNum, eliminated, hangAttempt, "
        "hangSuccess, robotSpeed, shooterAccuracy, "
        "defense, autonomous, driverSkill, strategy, "
        "fouls, overall, rankingPoints, ppm) "
        "VALUES ("
        + std::to_string(team.teamNum) + ", "
        + std::to_string(team.eliminated) + ", "
        + std::to_string(team.hangAttempt) + ", "
        + std::to_string(team.hangSuccess) + ", "
        + std::to_string(team.robotSpeed) + ", "
        + std::to_string(team.shooterAccuracy) + ", "
        + std::to_string(team.defense) + ", "
        + std::to_string(team.autonomous) + ", "
        + std::to_string(team.driverSkill) + ", "
        + std::to_string(team.strategy) + ", "
        + std::to_string(team.fouls) + ", "
        + std::to_string(team.overall) + ", "
        + std::to_string(team.rankingPoints) + ", "
        + std::to_string(team.ppm) + ");";
    std::cout << "Query: \n" << query << std::endl;
}

void DataBase::AddMatch(const Match& match) {

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
    if ( TableExists("Teams") )
        return;

    char* errorMsg;
    const char* query =
        "CREATE TABLE IF NOT EXISTS Teams ("
        "teamNum INTEGER PRIMARY KEY, "
        "eliminated INTEGER, "
        "hangAttempt INTEGER, "
        "hangSuccess INTEGER, "
        "robotSpeed INTEGER, "
        "shooterAccuracy INTEGER, "
        "defense INTEGER, "
        "autonomous INTEGER, "
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

void DataBase::Connect() {
    std::cout << "Connecting to SQL DB. Path: " << dbPath << std::endl;
    if ( connected ) {
        std::cout << "Already connected to SQL DB" << std::endl;
        return;
    }

    int res = sqlite3_open(dbPath.c_str(), &db);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to connect to SQL DB. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Connected to SQL DB" << std::endl;
    connected = true;
}

void DataBase::Disconnect() {
    std::cout << "Disconnecting from SQL DB" << std::endl;

    sqlite3_close(db);
    connected = false;
}
