#include "data.h"
#include "team.h"
#include "match.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <format>

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
    if ( !TeamExists(team.teamNum) ) {
        std::cout << "Team with team number " << team.teamNum << " doesn't exist. "
            "Cannot edit." << std::endl;
        return;
    }

    std::string query = std::format(
        "UPDATE {} SET "
        "matchNum = {}, eliminated = {}, hangAttempt = {}, hangSuccess = {}, robotCycleSpeed = {}, "
        "coralPoints = {}, defense = {}, autonomousPoints = {}, driverSkill = {}, fouls = {}, overall = {}, "
        "rankingPoints = {}, ppm = {} "
        "WHERE teamNum = {}",
        TEAM_TABLE, team.matchNum, team.eliminated, team.hangAttempt, team.hangSuccess,
        team.robotCycleSpeed, team.coralPoints, team.defense, team.autonomousPoints, team.driverSkill,
        team.fouls, team.overall, team.rankingPoints, team.ppm, team.teamNum
    );
    
    int res = sqlite3_exec(db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Updated team with team number: " << team.teamNum << std::endl;
}

void DataBase::EditMatch(const Match& match) {

}

bool DataBase::TeamExists(int teamNum) {
    if (!TableExists(TEAM_TABLE))
        return false;

    std::string query = 
        "SELECT * from " TEAM_TABLE " WHERE teamNum = " 
        + std::to_string(teamNum);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Error preparing query. Aborting." << std::endl;
        exit(-1);
    }

    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}

bool DataBase::MatchExists(int matchNum) {
    if ( !TableExists(MATCH_TABLE) )
        return false;

    std::string query =
        "SELECT * from " MATCH_TABLE " WHERE matchNum = "
        + std::to_string(matchNum);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Error preparing query. Aborting." << std::endl;
        exit(-1);
    }

    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);

    return false;
}

void DataBase::AddTeam(const Team& team) {
    if ( TeamExists(team.teamNum) ) {
        std::cout << "Team with team number " << team.teamNum << " already exists. " 
            "Perhaps try editing the team instead." << std::endl;
        return;
    }

    std::string query = std::format(
        "INSERT OR REPLACE INTO {} " // INSERT OR REPLACE INTO Teams
        "(teamNum, matchNum, eliminated, hangAttempt, hangSuccess, robotCycleSpeed, "
        "coralPoints, defense, autonomousPoints, driverSkill, fouls, overall, "
        "rankingPoints, ppm) "
        "VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {});",
        TEAM_TABLE, team.teamNum, team.matchNum, team.eliminated, team.hangAttempt, team.hangSuccess,
        team.robotCycleSpeed, team.coralPoints, team.defense, team.autonomousPoints, team.driverSkill,
        team.fouls, team.overall, team.rankingPoints, team.ppm
    );

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
    if ( !TeamExists(teamNum) ) {
        std::cout << "Team with team number " << teamNum << " doesn't exist. "
            "Cannot remove." << std::endl;
        return;
    }
    
    std::string query = std::format("DELETE FROM {} WHERE teamNum = {}", TEAM_TABLE, teamNum);

    int res = sqlite3_exec(db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }
    std::cout << "Removed team with team number: " << teamNum << std::endl;
}

void DataBase::RemoveMatch(int matchNum) {

}

Team DataBase::GetTeam(int teamNum) {
    Team team = {};
    
    if ( !TeamExists(teamNum) ) {
        std::cout << "Team with team number " << teamNum << " doesn't exist." << std::endl;
        return team;
    }

    std::string query = std::format("SELECT * from {} WHERE teamNum = {}", TEAM_TABLE, teamNum);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Error preparing query. Aborting." << std::endl;
        exit(-1);
    }

    if ( sqlite3_step(stmt) == SQLITE_ROW )
        team = Team::FromSQLStatment(stmt);

    sqlite3_finalize(stmt);

    return team;
}

Match DataBase::GetMatch(int matchNum) {
    Match match = {};

    if ( !MatchExists(matchNum) ) {
        std::cout << "Match with match number " << matchNum << " doesn't exist." << std::endl;
        return match;
    }

    return Match();
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
        exit(-1);
    }

    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        sqlite3_finalize(stmt);
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
    if ( TableExists(MATCH_TABLE) )
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
