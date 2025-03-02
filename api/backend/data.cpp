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
        CreateTables();
        return;
    }
    else {
        std::cout << "File with path " << dbPath << " exists." << std::endl;
    }

    Connect();
}

DataBase::~DataBase() {
    Disconnect();
}

void DataBase::EditTeam(const Team& team) {

}

void DataBase::EditMatch(const Match& match) {

}

void DataBase::AddTeam(const Team& team) {

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

void DataBase::CreateTables() {
    std::cout << "Creating tables" << std::endl;
    Connect();
    NewTeamTable();
}

void DataBase::NewTeamTable() {
    std::cout << "Creating blank team table." << std::endl;

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
