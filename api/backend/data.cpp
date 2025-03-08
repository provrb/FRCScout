#include "data.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <format>
#include <string_view>

Team Team::FromSQLStatment(sqlite3_stmt* stmt) {
    Team team = {};
    team.teamNum = sqlite3_column_int(stmt, 0);
    team.eliminated = sqlite3_column_int(stmt, 1);
    team.hangAttempt = sqlite3_column_int(stmt, 2);
    team.hangSuccess = sqlite3_column_int(stmt, 3);
    team.robotCycleSpeed = sqlite3_column_int(stmt, 4);
    team.coralPoints = sqlite3_column_int(stmt, 5);
    team.defense = sqlite3_column_int(stmt, 6);
    team.autonomousPoints = sqlite3_column_int(stmt, 7);
    team.driverSkill = sqlite3_column_int(stmt, 8);
    team.fouls = sqlite3_column_int(stmt, 9);
    team.overall = sqlite3_column_int(stmt, 10);
    team.rankingPoints = sqlite3_column_int(stmt, 11);
    team.ppm = sqlite3_column_int(stmt, 12);

    return team;
}

void Team::DebugPrint() const {
    std::cout << "Team Number: " << this->teamNum << std::endl;
    std::cout << "Eliminated: " << this->eliminated << std::endl;
    std::cout << "Hang Attempt: " << this->hangAttempt << std::endl;
    std::cout << "Hang Success: " << this->hangSuccess << std::endl;
    std::cout << "Robot Cycle Speed: " << this->robotCycleSpeed << std::endl;
    std::cout << "Coral Points: " << this->coralPoints << std::endl;
    std::cout << "Defense: " << this->defense << std::endl;
    std::cout << "Autonomous Points: " << this->autonomousPoints << std::endl;
    std::cout << "Driver Skill: " << this->driverSkill << std::endl;
    std::cout << "Fouls: " << this->fouls << std::endl;
    std::cout << "Overall: " << this->overall << std::endl;
    std::cout << "Ranking Points: " << this->rankingPoints << std::endl;
    std::cout << "PPM: " << this->ppm << std::endl;
}

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
        "SELECT * from " TEAM_TABLE " WHERE teamNum = " 
        + std::to_string(teamNum);

    std::cout << "Query: " << query << std::endl;

    sqlite3_stmt* stmt = MakeQuery(query);  
    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        sqlite3_finalize(stmt);
        std::cout << "Team with team number: " << teamNum << " exists." << std::endl;
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

void DataBase::AddTeam(const Team& team) {
    if ( TeamExists(team.teamNum) ) {
        std::cout << "Team with team number " << team.teamNum << " already exists." << std::endl;
        return;
    }        

    std::string q = std::format(
        "INSERT OR REPLACE INTO {} " // INSERT OR REPLACE INTO Teams
        "(teamNum, matchNum, eliminated, hangAttempt, hangSuccess, robotCycleSpeed, "
        "coralPoints, defense, autonomousPoints, driverSkill, fouls, overall, "
        "rankingPoints, ppm) "
        "VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {});",
        TEAM_TABLE, team.teamNum, team.matchNum, team.eliminated, team.hangAttempt, team.hangSuccess,
        team.robotCycleSpeed, team.coralPoints, team.defense, team.autonomousPoints, team.driverSkill,
        team.fouls, team.overall, team.rankingPoints, team.ppm
    );

    std::cout << "New query: " << q << std::endl;

    std::string query = 
        "INSERT OR REPLACE INTO Teams "
        "(teamNum, matchNum, eliminated, hangAttempt, "
        "hangSuccess, robotCycleSpeed, coralPoints, "
        "defense, autonomousPoints, driverSkill, "
        "fouls, overall, rankingPoints, ppm) "
        "VALUES ("
        + std::to_string(team.teamNum) + ", "
        + std::to_string(team.matchNum) + ", "
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

    int res = sqlite3_exec(db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Added team to teams table." << std::endl;
}

void DataBase::AddMatch(const Match& match) {
    std::string query =
        "INSERT OR REPLACE INTO " MATCH_TABLE
        " (matchNum, blueWin, played, "
        "redWin) VALUES ("
        + std::to_string(match.matchNum) + ", "
        + std::to_string(match.blueWin) + ", "
        + std::to_string(match.played) + ", "
        + std::to_string(match.redWin) + ");";

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

Team DataBase::GetTeam(int teamNum) {
    Team team = {};
    
    if ( !TeamExists(teamNum) ) {
        std::cout << "Team with team number " << teamNum << " doesn't exist." << std::endl;
        return team;
    }

    std::string query =
        "SELECT * from " TEAM_TABLE " WHERE teamNum = "
        + std::to_string(teamNum);

    sqlite3_stmt* stmt = MakeQuery(query);
    if ( sqlite3_step(stmt) == SQLITE_ROW )
        team = Team::FromSQLStatment(stmt);

    sqlite3_finalize(stmt);

    return team;
}

const std::vector<Team> DataBase::GetTeams() {

    return std::vector<Team>();
}

const std::vector<Match> DataBase::GetMatches() {

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

    sqlite3_finalize(stmt);
    return false;
}

void DataBase::CreateTables() {
    Connect();
    NewTeamTable();
    NewMatchesTable();
    NewMatchTeamsTable();
}

void DataBase::NewTeamTable() {
    if ( TableExists(TEAM_TABLE) )
        return;

    char* errorMsg;
    const char* query =
        "CREATE TABLE IF NOT EXISTS " TEAM_TABLE " ("
        "teamNum INTEGER PRIMARY KEY, "
        "matchNum INTEGAR, "
        "eliminated INTEGER, "
        "hangAttempt INTEGER, "
        "hangSuccess INTEGER, "
        "robotCycleSpeed INTEGER, "
        "coralPoints INTEGER, "
        "defense INTEGER, "
        "autonomousPoints INTEGER, "
        "driverSkill INTEGER, "
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
    if ( TableExists("Matches") )
        return;

    char* errorMsg;
    const char* query =
        "CREATE TABLE IF NOT EXISTS " MATCH_TABLE " ("
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
    if ( TableExists(MATCH_TEAMS_TABLE) )
        return;
    
    char* errorMsg;

    // each team will have a table describing what match they belong to
    const char* query =
        "CREATE TABLE IF NOT EXISTS " MATCH_TEAMS_TABLE " ("
        "matchNum INTEGER, "
        "team1 INTEGER, "
        "team2 INTEGAR, "
        "team3 INTEGER, "
        "team4 INTEGER, "
        "team5 INTEGER, "
        "team6 INTEGER);";

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
    if ( m_Connected )
        return;

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
