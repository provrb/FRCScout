#include "data.h"
#include "team.h"
#include "match.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <format>
#include <vector>
#include <iosfwd>

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

void DataBase::UpdateTeam(const Team& team) {
    if ( !TeamExists(team.teamNum) ) {
        std::cout << "Team with team number " << team.teamNum << " doesn't exist. "
            "Cannot edit." << std::endl;
        return;
    }

    const char* query =
        "UPDATE " TEAM_TABLE " SET "
        "eliminated = ?, hangAttempt = ?, hangSuccess = ?, robotCycleSpeed = ?, "
        "coralPoints = ?, defense = ?, autonomousPoints = ?, driverSkill = ?, fouls = ?, overall = ?, "
        "rankingPoints = ?, ppm = ? "
        "WHERE teamNum = ?";
    
    sqlite3_stmt* stmt;
    if ( sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK ) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind each field of 'team' to sql query 'query'
    sqlite3_bind_int(stmt, 1, team.eliminated);
    sqlite3_bind_int(stmt, 2, team.hangAttempt);
    sqlite3_bind_int(stmt, 3, team.hangSuccess);
    sqlite3_bind_int(stmt, 4, team.robotCycleSpeed);
    sqlite3_bind_int(stmt, 5, team.coralPoints);
    sqlite3_bind_int(stmt, 6, team.defense);
    sqlite3_bind_int(stmt, 7, team.autonomousPoints);
    sqlite3_bind_int(stmt, 8, team.driverSkill);
    sqlite3_bind_int(stmt, 9, team.fouls);
    sqlite3_bind_int(stmt, 10, team.overall);
    sqlite3_bind_int(stmt, 11, team.rankingPoints);
    sqlite3_bind_int(stmt, 12, team.ppm);
    sqlite3_bind_int(stmt, 13, team.teamNum);

    int res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        std::cout << "Failed to execute statement." << std::endl;
        exit(-1);
    }

    sqlite3_finalize(stmt);

    std::cout << "Updated team with team number: " << team.teamNum << std::endl;

    // TODO: Update matches team is in if any. change old team number in match to new
}

void DataBase::UpdateMatch(const Match& match) {
    if ( !MatchExists(match.matchNum) ) {
        std::cout << "Match doesn't exist. Cannot update." << std::endl;
        return;
    }
        
    const char* query =
        "UPDATE " MATCH_TABLE " SET "
        "matchNum = ?, played = ?, redWin = ?, blueWin = ?, "
        "team1 = ?, team2 = ?, team3 = ?, team4 = ?, team5 = ?, "
        "team6 = ? WHERE matchNum = ?";

    sqlite3_stmt* stmt;
    if ( sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK ) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind each field of 'team' to sql query 'query'
    sqlite3_bind_int(stmt, 1, match.matchNum);
    sqlite3_bind_int(stmt, 2, match.played);
    sqlite3_bind_int(stmt, 3, match.redWin);
    sqlite3_bind_int(stmt, 4, match.blueWin);
    sqlite3_bind_int(stmt, 5, match.Team1().teamNum);
    sqlite3_bind_int(stmt, 6, match.Team2().teamNum);
    sqlite3_bind_int(stmt, 7, match.Team3().teamNum);
    sqlite3_bind_int(stmt, 8, match.Team4().teamNum);
    sqlite3_bind_int(stmt, 9, match.Team5().teamNum);
    sqlite3_bind_int(stmt, 10, match.Team6().teamNum);
    sqlite3_bind_int(stmt, 11, match.matchNum);

    int res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        std::cout << "Failed to execute statement." << std::endl;
        exit(-1);
    }

    sqlite3_finalize(stmt);

    std::cout << "Updated match with match number: " << match.matchNum << std::endl;
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

bool DataBase::TeamInMatch(int teamNum, const Match& match) {
    // iterate through each team comparing the 
    // team numbers to the one were looking for

    
    for ( const Team& team : match.teams )
        if ( team.teamNum == teamNum )
           return true; // team is in match
    
    return false;
}

bool DataBase::TeamInMatch(int teamNum, int matchNum) {
    if ( !MatchExists(matchNum) ) {
        std::cout << "Cannot check if team is in match. Match doesn't exist." << std::endl;
        return false;
    }

    const Match& match = GetMatch(matchNum); // get match struct from sql db
    std::cout << "Match " << match.matchNum << std::endl;
    for ( const Team& team : match.teams ) {
        std::cout << "\tTeam " << team.teamNum << std::endl;
    }
    if ( match.teamCount == 0 ) // if no teams, automatically, team isnt in match
        return false;

    return TeamInMatch(teamNum, match);
}

void DataBase::AddTeam(const Team& team) {
    if ( TeamExists(team.teamNum) ) {
        std::cout << "Team with team number " << team.teamNum << " already exists. " 
            "Perhaps try editing the team instead." << std::endl;
        return;
    }

    const char* query =
        "INSERT OR REPLACE INTO " TEAM_TABLE " " // INSERT OR REPLACE INTO Teams
        "(teamNum, eliminated, hangAttempt, hangSuccess, robotCycleSpeed, "
        "coralPoints, defense, autonomousPoints, driverSkill, fouls, overall, "
        "rankingPoints, ppm) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind each field of 'team' to sql query 'query'
    sqlite3_bind_int(stmt, 1, team.teamNum);
    sqlite3_bind_int(stmt, 2, team.eliminated);
    sqlite3_bind_int(stmt, 3, team.hangAttempt);
    sqlite3_bind_int(stmt, 4, team.hangSuccess);
    sqlite3_bind_int(stmt, 5, team.robotCycleSpeed);
    sqlite3_bind_int(stmt, 6, team.coralPoints);
    sqlite3_bind_int(stmt, 7, team.defense);
    sqlite3_bind_int(stmt, 8, team.autonomousPoints);
    sqlite3_bind_int(stmt, 9, team.driverSkill);
    sqlite3_bind_int(stmt, 10, team.fouls);
    sqlite3_bind_int(stmt, 11, team.overall);
    sqlite3_bind_int(stmt, 12, team.rankingPoints);
    sqlite3_bind_int(stmt, 13, team.ppm);

    res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        std::cout << "Failed to execute statement." << std::endl;
        exit(-1);
    }

    std::cout << "Added team to teams table." << std::endl;
}

void DataBase::AddMatch(const Match& match) {
    const char* query =
        "INSERT OR REPLACE INTO " MATCH_TABLE " "
        "(matchNum, played, redWin, blueWin, "
        "team1, team2, team3, team4, team5, team6) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to prepare statement" << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, match.matchNum);
    sqlite3_bind_int(stmt, 2, match.played);
    sqlite3_bind_int(stmt, 3, match.redWin);
    sqlite3_bind_int(stmt, 4, match.blueWin);
    sqlite3_bind_int(stmt, 5, match.Team1().teamNum);
    sqlite3_bind_int(stmt, 6, match.Team2().teamNum);
    sqlite3_bind_int(stmt, 7, match.Team3().teamNum);
    sqlite3_bind_int(stmt, 8, match.Team4().teamNum);
    sqlite3_bind_int(stmt, 9, match.Team5().teamNum);
    sqlite3_bind_int(stmt, 10, match.Team6().teamNum);

    res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        std::cout << "Failed to execute statement." << std::endl;
        exit(-1);
    }

    std::cout << "Added match to matches table." << std::endl;
}

void DataBase::RemoveTeamFromMatch(int teamNum, int matchNum) {
    if ( !MatchExists(matchNum) ) {
        std::cout << "Match with match number " << matchNum << " already exists." << std::endl;
        return;
    }

    if ( !TeamInMatch(teamNum, matchNum) ) {
        std::cout << "Team not in match already. Cannot remove" << std::endl;
        return;
    }   

    Match match = GetMatch(matchNum);
    match.RemoveCompetitor(teamNum);
    
    UpdateMatch(match);
}

void DataBase::AddTeamToMatch(int teamNum, int matchNum) {
    if ( !MatchExists(matchNum) ) {
        std::cout << "Match with match number " << matchNum << " already exists." << std::endl;
        return;
    }

    if ( TeamInMatch(teamNum, matchNum) ) {
        std::cout << "Team is already in match. Cannot add" << std::endl;
        return;
    }
        
    Match match = GetMatch(matchNum);
    if ( match.teamCount >= 6 ) {
        std::cout << "Match is full. Cannot add more teams." << std::endl;
        return;
    }
    
    Team team = GetTeam(teamNum);
    if ( team.teamNum == 0 ) {
        std::cout << "Team with team number " << teamNum << " doesn't exist. Cannot add to match." << std::endl;
        return;
    }

    match.AddCompetitor(team);
    UpdateMatch(match);
}

void DataBase::RemoveTeam(int teamNum) {
    std::string query = std::format("DELETE FROM {} WHERE teamNum = {}", TEAM_TABLE, teamNum);

    int res = sqlite3_exec(db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }

    // Remove team from matches if any, replace the teamnum with 0 in each match
    for ( Match& match : GetMatches() ) {
        if ( !TeamInMatch(teamNum, match) )
            continue;
        
        // Remove and update
        match.RemoveCompetitor(teamNum);
        UpdateMatch(match);
    }

    std::cout << "Removed team with team number: " << teamNum << std::endl;
}

void DataBase::RemoveMatch(int matchNum) {
    std::string query = std::format("DELETE FROM {} WHERE matchNum = {}", MATCH_TABLE, matchNum);

    int res = sqlite3_exec(db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Removed match with match number: " << matchNum << std::endl;
}

Team DataBase::GetTeam(int teamNum) {
    Team team = {};
    
    if ( teamNum == 0 || !TeamExists(teamNum) ) {
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

    std::string query = std::format("SELECT * from {} WHERE matchNum = {}", MATCH_TABLE, matchNum);
    
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Error preparing query. Aborting." << std::endl;
        exit(-1);
    }

    if ( sqlite3_step(stmt) == SQLITE_ROW )
        match = Match::FromSQLStatment(stmt);
    
    sqlite3_finalize(stmt);

    // No team infromation needs to be processed and checked
    if ( match.teamCount == 0 )
        return match;

    // Fill out each team information
    for ( Team& team : match.teams ) {
        if ( team.teamNum == 0 )
            continue;

        // if the team exists add the new comptitor from sql db
        if ( TeamExists(team.teamNum) ) {
            team = GetTeam(team.teamNum);
        }
    }

    return match;
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
        std::cout << "Error preparing statement to check if table exists. Aborting." << std::endl;
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
}

void DataBase::NewTeamTable() {
    if ( TableExists(TEAM_TABLE) )
        return;

    const char* query =
        "CREATE TABLE IF NOT EXISTS " TEAM_TABLE " ("
        "teamNum INTEGER PRIMARY KEY, "
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
    
    int res = sqlite3_exec(db, query, NULL, 0, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Created blank team table." << std::endl;
}

void DataBase::NewMatchesTable() {
    if ( TableExists(MATCH_TABLE) )
        return;

    const char* query =
        "CREATE TABLE IF NOT EXISTS " MATCH_TABLE " ("
        "matchNum INTEGER PRIMARY KEY, "
        "played INTEGER, "
        "redWin INTEGER, "
        "blueWin INTEGER, "
        "team1 INTEGAR, "
        "team2 INTEGAR, "
        "team3 INTEGAR, "
        "team4 INTEGAR, "
        "team5 INTEGAR, "
        "team6 INTEGAR"
        ");";

    Connect();

    int res = sqlite3_exec(db, query, NULL, 0, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Created blank matches table." << std::endl;
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

int DataBase::BindMatchTeams(sqlite3_stmt* stmt, const Match& match) {
    // add each team to match
    int i = 0;
    for ( ; i < 6; i++ ) {
        // if the team in Match struct doesnt exist then
        // just skip it and put 0 as the team number
        // team will needed to be added to Teams table
        // first through AddTeam
        if ( match.teams[i].teamNum != 0 && !TeamExists(match.teams[i].teamNum) ) {
            std::cout << "Team in match (" << match.teams[i].teamNum << ") hasn't been added to Teams table yet. Skipping." << std::endl;
            sqlite3_bind_int(stmt, i + 5, 0);
            continue;
        }

        std::cout << "Adding team: " << match.teams[i].teamNum << std::endl;
        sqlite3_bind_int(stmt, i + 5, match.teams[i].teamNum);
    }
    return i;
}
