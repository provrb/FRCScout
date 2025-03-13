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

/**
 * @brief Constructs a DataBase object and initializes the database.
 *
 * This constructor checks if the specified database file exists. If the file
 * does not exist, it creates a new one. After ensuring the database file is
 * available, it establishes a connection and initializes the necessary tables.
 *
 * @param path The file path to the SQLite database.
 *
 * @note If file creation fails, the program exits with an error code.
 */
DataBase::DataBase(const std::string& path) : m_dbPath(path) {
    if ( !std::filesystem::exists(m_dbPath) ) {
        std::cout << "File with path " << m_dbPath << " doesn't exist. Creating it" << std::endl;
        std::ofstream file(m_dbPath);
        if ( !file.is_open() ) {
            std::cout << "Failed to create file. Aborting." << std::endl;
            exit(-1);
        }
    
        std::cout << "File created successfully" << std::endl;
    }

    Connect();
    CreateTables();
}

/**
 * @brief Destroys the DataBase object and disconnects from the database.
 *
 * This destructor ensures that the database connection is properly closed
 * before the object is destroyed.
 */
DataBase::~DataBase() {
    Disconnect();
}


/**
 * @brief Creates the necessary tables for the database.
 *
 * This function creates the tables for teams and matches in the database if they do
 * not already exist.
 */
void DataBase::CreateTables() {
    if ( !m_Connected ) {
        std::cout << "Not connected to database. Failed to created initial tables." << std::endl;
        return;
    }

    NewTeamTable();
    NewMatchesTable();
}

/**
 * @brief Connects to the SQLite database.
 *
 * This function opens a connection to the SQLite database if one is not already established.
 * It ensures that the database is ready for further interactions.
 */
void DataBase::Connect() {
    if ( m_Connected )
        return;

    int res = sqlite3_open(m_dbPath.c_str(), &m_db);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to connect to SQL DB. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Connected to SQL DB" << std::endl;
    m_Connected = true;
}

/**
 * @brief Disconnects from the SQLite database.
 *
 * This function closes the connection to the SQLite database.
 */
void DataBase::Disconnect() {
    std::cout << "Disconnecting from SQL DB" << std::endl;

    sqlite3_close(m_db);
    m_Connected = false;
}

/**
 * @brief Creates the teams table in the database.
 *
 * This function creates a table to store information about teams. The table is created
 * only if it does not already exist.
 */
void DataBase::NewTeamTable() {
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

    int res = sqlite3_exec(m_db, query, NULL, 0, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Created blank team table." << std::endl;
}

/**
 * @brief Creates the matches table in the database.
 *
 * This function creates a table to store information about matches. The table is created
 * only if it does not already exist.
 */
void DataBase::NewMatchesTable() {
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

    int res = sqlite3_exec(m_db, query, NULL, 0, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Created blank matches table." << std::endl;
}

/**
 * @brief Updates an existing team's information in the database.
 *
 * This function updates specific fields of a team entry in the database if the
 * team exists. It binds the values of the given `Team` object to an SQL
 * `UPDATE` statement and executes it.
 *
 * @param team The `Team` object containing the updated data.
 *
 * @note If the team does not exist in the database, an error message is printed,
 *       and the function returns without making changes.
 * @note If the SQL statement fails to execute, the program exits with an error code.
 */
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
    if ( sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK ) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
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

/**
 * @brief Updates an existing match's information in the database.
 *
 * This function updates specific fields of a match entry in the database if the
 * match exists. It binds the values of the given `Match` object to an SQL
 * `UPDATE` statement and executes it.
 *
 * @param match The `Match` object containing the updated data.
 *
 * @note If the match does not exist in the database, an error message is printed,
 *       and the function returns without making changes.
 * @note If the SQL statement fails to execute, the program exits with an error code.
 */
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
    if ( sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK ) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
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

/**
 * @brief Checks if a team exists in the database.
 *
 * This function queries the database to check if a team with the specified
 * team number exists in the `Teams` table. It returns true if the team is found,
 * and false otherwise.
 *
 * @param teamNum The team number to search for in the database.
 *
 * @return `true` if the team exists in the database, `false` otherwise.
 *
 * @note If the `Teams` table does not exist, the function immediately returns false.
 * @note If an error occurs while preparing or executing the SQL query, the program will
 *       exit with an error message.
 */
bool DataBase::TeamExists(int teamNum) {
    std::string query = 
        "SELECT 1 FROM " TEAM_TABLE " WHERE teamNum = " 
        + std::to_string(teamNum);

    // The reason why sqlite3_prepare_v2 is used here instead
    // of sqlite3_exec is to avoid the use of a callback function.
    // 
    // By using sqlite3_prepare_v2, we can use sqlite3_step to see
    // if there was a row as a result of our query rather than
    // having to set a bool in a callback variable depending 
    // if the row exists. TLDR: Simpler.
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Error preparing query. Aborting." << std::endl;
        exit(-1);
    }

    // If we step and find a row, that means there is a row where
    // team number is equal to 'teamNum'
    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}

/**
 * @brief Checks if a match exists in the database.
 *
 * This function queries the database to check if a match with the specified
 * match number exists in the `Matches` table. It returns true if the match is found,
 * and false otherwise.
 *
 * @param matchNum The match number to search for in the database.
 *
 * @return `true` if the match exists in the database, `false` otherwise.
 *
 * @note If the `Matches` table does not exist, the function immediately returns false.
 * @note If an error occurs while executing the SQL query, the program will
 *       exit with an error message.
 */
bool DataBase::MatchExists(int matchNum) {
    std::string query =
        "SELECT 1 FROM " MATCH_TABLE " WHERE matchNum = "
        + std::to_string(matchNum);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
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

/**
 * @brief Checks if a team is participating in a specified match.
 *
 * This function iterates through the list of teams in a given match and compares
 * each team's number to the specified `teamNum`. If a team with the given number
 * is found, the function returns `true`, indicating the team is participating in the match.
 * Otherwise, it returns `false`.
 *
 * @param teamNum The team number to search for in the match.
 * @param match The match object that contains the list of teams participating.
 *
 * @return `true` if the team is found in the match, `false` otherwise.
 */
bool DataBase::TeamInMatch(int teamNum, const Match& match) {
    // iterate through each team comparing the 
    // team numbers to the one were looking for
    
    for ( const Team& team : match.teams )
        if ( team.teamNum == teamNum )
           return true; // team is in match
    
    return false;
}

/**
 * @brief Checks if a team is participating in a match by match number.
 *
 * This function first verifies if the match with the given `matchNum` exists. If the match exists,
 * it retrieves the match details from the database and checks if the specified `teamNum` is listed among
 * the teams in the match. If no teams are associated with the match (i.e., `teamCount` is 0), it returns `false`.
 * If the match exists and contains teams, it calls another `TeamInMatch` function to check the participation.
 *
 * @param teamNum The team number to search for.
 * @param matchNum The match number to check against.
 *
 * @return `true` if the team is part of the match, `false` if the team is not part of the match or if the match doesn't exist.
 */
bool DataBase::TeamInMatch(int teamNum, int matchNum) {
    if ( !MatchExists(matchNum) ) {
        std::cout << "Cannot check if team is in match. Match doesn't exist." << std::endl;
        return false;
    }

    const Match& match = GetMatch(matchNum); // get match struct from sql db
    if ( match.teamCount == 0 ) // if no teams, automatically, team isnt in match
        return false;

    return TeamInMatch(teamNum, match);
}

/**
 * @brief Adds a new team to the database.
 *
 * This function checks if the team already exists in the database based on the team number. If the team exists,
 * it outputs a message indicating that the team already exists and suggests editing the team instead. If the team
 * does not exist, it prepares and executes an `INSERT OR REPLACE` SQL statement to add the team to the database.
 *
 * @param team The `Team` object containing all relevant information for the team to be added to the database.
 */
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
    int res = sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
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

/**
 * @brief Adds a team to a match in the database.
 *
 * Checks if the match and team exist, if the team is already in the match, and if the match is full.
 * If all conditions are met, adds the team to the match and updates the match in the database.
 *
 * @param teamNum The team number to add.
 * @param matchNum The match number to add the team to.
 */
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

    // Add team and update match
    match.AddCompetitor(team);
    UpdateMatch(match);
}

/**
 * @brief Adds or updates a match in the database.
 *
 * This function inserts a new match or updates an existing match with the provided `matchNum`.
 * It binds the match data, such as the match number, results (red and blue wins), and the six
 * teams involved, and then executes the SQL query to insert or replace the match in the database.
 *
 * @param match The `Match` object containing the match details to be inserted.
 */
void DataBase::AddMatch(const Match& match) {
    const char* query =
        "INSERT OR REPLACE INTO " MATCH_TABLE " "
        "(matchNum, played, redWin, blueWin, "
        "team1, team2, team3, team4, team5, team6) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to prepare statement" << std::endl;
        return;
    }

    // Bind each field of Match to stmt
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

/**
 * @brief Removes a team from a match in the database.
 *
 * This function removes the specified team from the match with the given `matchNum`.
 * It checks if the match and team exist and are properly related before proceeding
 * with the removal. After modifying the match, it updates the match in the database.
 *
 * @param teamNum The team number of the team to be removed.
 * @param matchNum The match number from which the team will be removed.
 */
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

/**
 * @brief Removes a team from the database and all associated matches.
 *
 * This function deletes the specified team from the database. It then iterates over all
 * matches and removes the team from any match in which it is a competitor, replacing
 * the team number with 0. The matches are updated accordingly.
 *
 * @param teamNum The team number of the team to be removed.
 */
void DataBase::RemoveTeam(int teamNum) {
    std::string query = std::format("DELETE FROM {} WHERE teamNum = {}", TEAM_TABLE, teamNum);

    int res = sqlite3_exec(m_db, query.c_str(), NULL, 0, NULL);
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

/**
 * @brief Removes a match from the database.
 *
 * This function deletes the specified match from the database based on the provided
 * match number.
 *
 * @param matchNum The match number of the match to be removed.
 */
void DataBase::RemoveMatch(int matchNum) {
    std::string query = std::format("DELETE FROM {} WHERE matchNum = {}", MATCH_TABLE, matchNum);

    int res = sqlite3_exec(m_db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to execute query. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Removed match with match number: " << matchNum << std::endl;
}

/**
 * @brief Retrieves a team from the database.
 *
 * This function queries the database to retrieve the team information based on the provided
 * team number. If the team number is invalid or the team does not exist, an empty `Team`
 * object is returned.
 *
 * @param teamNum The team number of the team to retrieve.
 * @return A `Team` object containing the team's information.
 */
Team DataBase::GetTeam(int teamNum) {
    Team team = {};
    
    if ( teamNum == 0 || !TeamExists(teamNum) ) {
        std::cout << "Team with team number " << teamNum << " doesn't exist." << std::endl;
        return team;
    }

    std::string query = std::format("SELECT * from {} WHERE teamNum = {}", TEAM_TABLE, teamNum);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        std::cout << "Error preparing query. Aborting." << std::endl;
        exit(-1);
    }

    if ( sqlite3_step(stmt) == SQLITE_ROW )
        team = Team::FromSQLStatment(stmt);

    sqlite3_finalize(stmt);

    return team;
}

/**
 * @brief Retrieves a match from the database.
 *
 * This function queries the database to retrieve a match's details based on the provided
 * match number. The function will also populate the teams participating in the match
 * if they exist in the database.
 *
 * @param matchNum The match number of the match to retrieve.
 * @return A `Match` object containing the match's information, including teams.
 */
Match DataBase::GetMatch(int matchNum) {
    Match match = {};

    if ( !MatchExists(matchNum) ) {
        std::cout << "Match with match number " << matchNum << " doesn't exist." << std::endl;
        return match;
    }

    std::string query = std::format("SELECT * from {} WHERE matchNum = {}", MATCH_TABLE, matchNum);
    
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
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

// TODO
std::vector<Team> DataBase::GetTeams() {
    return std::vector<Team>();
}

// TODO
std::vector<Match> DataBase::GetMatches() {
    return std::vector<Match>();
}

/**
 * @brief Checks if a table exists in the database.
 *
 * This function queries the SQLite master table to check whether a table with the
 * specified name exists in the database.
 *
 * @param tableName The name of the table to check for existence.
 * @return `true` if the table exists, `false` otherwise.
 */
bool DataBase::TableExists(const std::string& tableName) {
    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';";
    sqlite3_stmt* stmt;

    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
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
