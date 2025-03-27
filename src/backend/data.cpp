#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "data.h"
#include "team.h" // Team struct
#include "match.h" // Match struct

#include <filesystem> // filesystem::exists
#include <iostream> // cout
#include <fstream> // std::ofstream
#include <string> // std::string
#include <sqlite3.h> 
#include <json.hpp> // json
#include <qrcodegen.hpp>
#include <stb_image_write.h>

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
DataBase::DataBase(const std::string& path, MainFrame* mainFrame) : m_dbPath(path), m_mainFrame(mainFrame) {
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
    if ( !m_connected ) {
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
    if ( m_connected )
        return;

    int res = sqlite3_open(m_dbPath.c_str(), &m_db);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to connect to SQL DB. Aborting." << std::endl;
        exit(-1);
    }

    std::cout << "Connected to SQL DB" << std::endl;
    m_connected = true;
}

/**
 * @brief Disconnects from the SQLite database.
 *
 * This function closes the connection to the SQLite database.
 */
void DataBase::Disconnect() {
    std::cout << "Disconnecting from SQL DB" << std::endl;

    sqlite3_close(m_db);
    m_connected = false;
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
        "uid INTEGER, "
        "teamNum INTEGER, "
        "matchNum INTEGAR, "
        "hangAttempt INTEGER, "
        "hangSuccess INTEGER, "
        "robotCycleSpeed INTEGER, "
        "coralPoints INTEGER, "
        "defense INTEGER, "
        "autonomousPoints INTEGER, "
        "driverSkill INTEGER, "
        "penaltys INTEGER, "
        "overall INTEGER, "
        "rankingPoints INTEGER, "
        "PRIMARY KEY (uid, teamNUm)"
        ");";

    int res = sqlite3_exec(m_db, query, NULL, 0, nullptr);
    if ( res != SQLITE_OK ) {
        std::cout << "Failed to create table. Aborting." << std::endl;
        exit(-1);
    }

    AddQueryToHistory(query);
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

    AddQueryToHistory(query);
    std::cout << "Created blank matches table." << std::endl;
}

/**
 * @brief Adds an expanded SQL query to the query history.
 *
 * This function retrieves the fully expanded SQL query (with bound values
 * substituted) from the given prepared SQLite statement and stores it in
 * the `m_QueryHistory` vector for debugging purposes if 'stmt' is not 'nullptr'
 *
 * @param stmt A pointer to the prepared SQLite statement.
 * @note Requires SQLite 3.14+ for `sqlite3_expanded_sql`.
 */
void DataBase::AddQueryToHistory(sqlite3_stmt* stmt) {
    if ( !stmt )
        return;

    const char* expanded = sqlite3_expanded_sql(stmt);
    if ( !expanded )
        return;

    this->m_queryHistory.push_back(expanded);
    this->m_mainFrame->LogSQLQuery(expanded);
}

/**
 * @brief Adds an SQL query to the query history.
 *
 * This function stores a given SQL query string in the `m_QueryHistory` vector
 * for debugging purposes. If the input query is `nullptr`, the function does nothing.
 *
 * @param query A pointer to a null-terminated SQL query string.
 */
void DataBase::AddQueryToHistory(std::string query) {
    this->m_queryHistory.push_back(query);
    this->m_mainFrame->LogSQLQuery(query);
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
    const char* query =
        "UPDATE " TEAM_TABLE " SET "
        "teamNum = ?, matchNum = ?, hangAttempt = ?, hangSuccess = ?, robotCycleSpeed = ?, "
        "coralPoints = ?, defense = ?, autonomousPoints = ?, driverSkill = ?, penaltys = ?, overall = ?, "
        "rankingPoints = ? "
        "WHERE uid = ?";
    
    sqlite3_stmt* stmt;
    if ( sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK ) {
        std::cout << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    // Bind each field of 'team' to sql query 'query'
    sqlite3_bind_int(stmt, 1, team.teamNum);
    sqlite3_bind_int(stmt, 2, team.matchNum);
    sqlite3_bind_int(stmt, 3, team.hangAttempt);
    sqlite3_bind_int(stmt, 4, team.hangSuccess);
    sqlite3_bind_int(stmt, 5, team.robotCycleSpeed);
    sqlite3_bind_int(stmt, 6, team.coralPoints);
    sqlite3_bind_int(stmt, 7, team.defense);
    sqlite3_bind_int(stmt, 8, team.autonomousPoints);
    sqlite3_bind_int(stmt, 9, team.driverSkill);
    sqlite3_bind_int(stmt, 10, team.penaltys);
    sqlite3_bind_int(stmt, 11, team.overall);
    sqlite3_bind_int(stmt, 12, team.rankingPoints);
    sqlite3_bind_int(stmt, 13, team.uid);

    AddQueryToHistory(stmt);

    int res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        m_mainFrame->LogErrorMessage("There was an error updating a team. Try again or delete the team and retry.");
        return;
    }

    sqlite3_finalize(stmt);
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
        "redWin = ?, blueWin = ?, "
        "team1 = ?, team2 = ?, team3 = ?, team4 = ?, team5 = ?, "
        "team6 = ? WHERE matchNum = ?";

    sqlite3_stmt* stmt;
    if ( sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return;
    }

    // Bind each field of 'team' to sql query 'query'
    sqlite3_bind_int(stmt, 1, match.redWin);
    sqlite3_bind_int(stmt, 2, match.blueWin);
    sqlite3_bind_int(stmt, 3, match.Team1().teamNum);
    sqlite3_bind_int(stmt, 4, match.Team2().teamNum);
    sqlite3_bind_int(stmt, 5, match.Team3().teamNum);
    sqlite3_bind_int(stmt, 6, match.Team4().teamNum);
    sqlite3_bind_int(stmt, 7, match.Team5().teamNum);
    sqlite3_bind_int(stmt, 8, match.Team6().teamNum);
    sqlite3_bind_int(stmt, 9, match.matchNum);

    AddQueryToHistory(stmt);

    int res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        m_mainFrame->LogErrorMessage(
            "There was an error updating a match. Try again or delete the match and retry."
        );
        return;
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
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db)) 
        );
        return false;
    }

    AddQueryToHistory(stmt);

    // If we step and find a row, that means there is a row where
    // team number is equal to 'teamNum'
    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}

bool DataBase::TeamExistsUID(int uid) {
    std::string query =
        "SELECT 1 FROM " TEAM_TABLE " WHERE uid = "
        + std::to_string(uid);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return false;
    }

    AddQueryToHistory(stmt);

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
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return false;
    }

    AddQueryToHistory(stmt);

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
void DataBase::AddTeam(Team& team) {
    const char* query =
        "INSERT OR REPLACE INTO " TEAM_TABLE " " // INSERT OR REPLACE INTO Teams
        "(uid, teamNum, matchNum, hangAttempt, hangSuccess, robotCycleSpeed, "
        "coralPoints, defense, autonomousPoints, driverSkill, penaltys, overall, "
        "rankingPoints) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return;
    }

    // Bind each field of 'team' to sql query 'query'
    sqlite3_bind_int(stmt, 1, team.uid);
    sqlite3_bind_int(stmt, 2, team.teamNum);
    sqlite3_bind_int(stmt, 3, team.matchNum);
    sqlite3_bind_int(stmt, 4, team.hangAttempt);
    sqlite3_bind_int(stmt, 5, team.hangSuccess);
    sqlite3_bind_int(stmt, 6, team.robotCycleSpeed);
    sqlite3_bind_int(stmt, 7, team.coralPoints);
    sqlite3_bind_int(stmt, 8, team.defense);
    sqlite3_bind_int(stmt, 9, team.autonomousPoints);
    sqlite3_bind_int(stmt, 10, team.driverSkill);
    sqlite3_bind_int(stmt, 11, team.penaltys);
    sqlite3_bind_int(stmt, 12, team.overall);
    sqlite3_bind_int(stmt, 13, team.rankingPoints);

    AddQueryToHistory(stmt);

    res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        m_mainFrame->LogErrorMessage("Failed to add the team to team database.");
        return;
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
void DataBase::AddTeamToMatch(int uid, int matchNum) {
    if ( !MatchExists(matchNum) ) {
        std::cout << "Match with match number " << matchNum << " already exists." << std::endl;
        return;
    }

    Team team = GetTeam(uid);
    if ( TeamInMatch(team.teamNum, matchNum) ) {
        std::cout << "Team is already in match. Cannot add" << std::endl;
        return;
    }

    Match match = GetMatch(matchNum);
    if ( match.teamCount >= 6 ) {
        std::cout << "Match is full. Cannot add more teams." << std::endl;
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
        "(matchNum, redWin, blueWin, "
        "team1, team2, team3, team4, team5, team6) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return;
    }

    // Bind each field of Match to stmt
    sqlite3_bind_int(stmt, 1, match.matchNum);
    sqlite3_bind_int(stmt, 2, match.redWin);
    sqlite3_bind_int(stmt, 3, match.blueWin);
    sqlite3_bind_int(stmt, 4, match.Team1().teamNum);
    sqlite3_bind_int(stmt, 5, match.Team2().teamNum);
    sqlite3_bind_int(stmt, 6, match.Team3().teamNum);
    sqlite3_bind_int(stmt, 7, match.Team4().teamNum);
    sqlite3_bind_int(stmt, 8, match.Team5().teamNum);
    sqlite3_bind_int(stmt, 9, match.Team6().teamNum);

    AddQueryToHistory(stmt);

    res = sqlite3_step(stmt); // execute
    if ( res != SQLITE_DONE ) {
        m_mainFrame->LogErrorMessage("Failed to add the match to match database.");
        return;
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
void DataBase::RemoveTeam(int uid) {
    int teamNum = GetTeam(uid).teamNum;

    std::string query = std::format("DELETE FROM {} WHERE uid = {}", TEAM_TABLE, uid);

    int res = sqlite3_exec(m_db, query.c_str(), NULL, 0, NULL);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage("Failed to delete the team from team database.");
        return;
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
        m_mainFrame->LogErrorMessage("Failed to remove the match from match database.");
        return;
    }

    AddQueryToHistory(query.c_str());
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
Team DataBase::GetTeam(int uid) {
    Team team = {};

    std::string query = std::format("SELECT * from {} WHERE uid = {}", TEAM_TABLE, uid);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return team;
    }

    AddQueryToHistory(stmt);

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

    std::string query = std::format("SELECT * from {} WHERE matchNum = {}", MATCH_TABLE, matchNum);
    
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db)) 
        );
        return match;
    }

    AddQueryToHistory(stmt);

    if ( sqlite3_step(stmt) == SQLITE_ROW )
        match = Match::FromSQLStatment(stmt);
    
    sqlite3_finalize(stmt);

    // No team infromation needs to be processed and checked
    if ( match.teamCount == 0 )
        return match;

    return match;
}

/**
 * @brief Retrieves all teams from the database.
 *
 * This function queries the database to select all rows from the `TEAM_TABLE`. It then iterates over each row
 * in the result set, creating `Team` objects using the `Team::FromSQLStatement` function. These objects are
 * added to a vector, which is returned to the caller.
 *
 * If there is an error in preparing the SQL statement, an error is logged, and an empty vector is returned.
 *
 * @return std::vector<Team> A vector containing all the teams retrieved from the database.
 */
std::vector<Team> DataBase::GetTeams() {
    std::vector<Team> teams = {};
    std::string query = std::format("SELECT * from {}", TEAM_TABLE);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return teams;
    }

    AddQueryToHistory(stmt);

    while ( sqlite3_step(stmt) == SQLITE_ROW ) {
        Team team = Team::FromSQLStatment(stmt);
        teams.push_back(team);
    }

    sqlite3_finalize(stmt);
    
    this->m_mainFrame->LogBackendMessage("Found " + std::to_string(teams.size()) + " Teams");

    return teams;
}

/**
 * @brief Retrieves all matches from the database.
 *
 * This function queries the database to select all rows from the `MATCH_TABLE`. It then iterates over each row
 * in the result set, creating `Match` objects using the `Match::FromSQLStatement` function. These objects are
 * added to a vector, which is returned to the caller.
 *
 * If there is an error in preparing the SQL statement, an error is logged, and an empty vector is returned.
 *
 * @return std::vector<Match> A vector containing all the matches retrieved from the database.
 */
std::vector<Match> DataBase::GetMatches() {
    std::vector<Match> matches = {};
    std::string query = std::format("SELECT * from {}", MATCH_TABLE);

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, NULL);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return matches;
    }

    AddQueryToHistory(stmt);

    while ( sqlite3_step(stmt) == SQLITE_ROW ) {
        Match match = Match::FromSQLStatment(stmt);
        matches.push_back(match);
    }

    sqlite3_finalize(stmt);

    this->m_mainFrame->LogBackendMessage("Found " + std::to_string(matches.size()) + " Matches");

    return matches;
}

/**
 * @brief Calculates the win rate of a team based on their performance in matches.
 *
 * This function checks all matches in which the specified team participated,
 * counts the number of wins and losses, and calculates the win rate as a percentage.
 *
 * @param uid The unique identifier of the team.
 * @return The win rate of the team as a percent (e.g. 75% win rate).
 */
double DataBase::GetTeamWinRate(int teamNum) {
    const std::vector<Match> matches = GetMatches();
    
    // statistics
    double wins = 0;
    int matchesPlayed = 0;

    // check all matches team is in
    for ( const Match& match : matches ) {
        // team did not compete in match
        if ( !TeamInMatch(teamNum, match) )
            continue;

        // team competed in match
        matchesPlayed++;

        // team won their match if they are on red alliance and red won
        // otherwise, they lost
        if ( match.RedWon() && match.RedAllianceTeam(teamNum) )
            wins++;
    }

    // catch divide by zero error
    if ( matchesPlayed == 0 )
        return 0.0;

    return (wins / matchesPlayed) * 100;
}

/**
 * @brief Generates a unique team UID (User Identifier) that does not already exist.
 *
 * This function generates a random 4-digit integer UID (between 1000 and 9999) using a random number generator.
 * It checks if the generated UID already exists using the `TeamExistsUID` function. If the UID exists, the process
 * repeats until a unique UID is found. The unique UID is then returned.
 *
 * @return int The unique team UID.
 */
int DataBase::GetNextTeamUID() {
    int uid = 0;

    do {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 9999);
        uid = dis(gen);
    } while ( TeamExistsUID(uid) );

    return uid;
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
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db)) 
        );
        return false;
    }

    AddQueryToHistory(stmt);

    if ( sqlite3_step(stmt) == SQLITE_ROW ) {
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}

/**
 * @brief Exports the contents of a specified table to a JSON file.
 *
 * This function retrieves all rows and columns from a given table in the SQLite database and exports
 * the data to a JSON file. The data is stored as an array of JSON objects, where each object represents
 * a row in the table with column names as keys and column values as values.
 *
 * @param tableName The name of the table to export from the database.
 * @param outputFilename The name of the output JSON file where the data will be saved.
 *
 * @note The output file is overwritten if it already exists.
 *
 * @return void This function does not return a value.
 *
 * @example
 * DataBase db("path_to_database.db");
 * db.ExportTableToJSON("teams", "teams_data.json");
 */
void DataBase::ExportTableToJSON(const std::string& tableName, const std::string& outputFilename) {    
    using json = nlohmann::json;

    std::ofstream outFile(outputFilename);
    if ( !outFile )
        return;

    sqlite3_stmt* stmt;
    std::string query = "SELECT * FROM " + tableName + ";";
    int res = sqlite3_prepare_v2(this->m_db, query.c_str(), -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db))
        );
        return;
    }

    AddQueryToHistory(stmt);

    json records = json::array();
    while ( sqlite3_step(stmt) == SQLITE_ROW ) {
        json row;
        for ( int i = 0; i < sqlite3_column_count(stmt); i++ ) {
            std::string colName = sqlite3_column_name(stmt, i);
            const unsigned char* value = sqlite3_column_text(stmt, i);
            if ( value )
                row[colName] = reinterpret_cast< const char* >( value );
            else
                row[colName] = nullptr;
        }
        records.push_back(row);
    }

    if ( outFile ) {
        outFile << records.dump(4);
        outFile.close();
        this->m_mainFrame->LogBackendMessage("JSON data exported to " + outputFilename);
    }

    sqlite3_finalize(stmt);
}

/**
 * @brief Exports the contents of a specified table to a CSV file.
 *
 * This function retrieves all rows and columns from a given table in the SQLite database and exports
 * the data to a CSV file. The first row in the CSV file contains the column names (headers), and each
 * subsequent row represents the data in the table. NULL values are represented as "NULL" in the CSV file.
 *
 * @param tableName The name of the table to export from the database.
 * @param outputFilename The name of the output CSV file where the data will be saved.
 *
 * @note The output file is overwritten if it already exists.
 */
void DataBase::ExportTableToCSV(const std::string& tableName, const std::string& outputFilename) {
    sqlite3_stmt* stmt;
    std::string query = "SELECT * FROM " + tableName + ";";

    int res = sqlite3_prepare_v2(this->m_db, query.c_str(), -1, &stmt, nullptr);
    if ( res != SQLITE_OK ) {
        m_mainFrame->LogErrorMessage(
            std::string("Failed to prepare SQL Statement") + std::string(sqlite3_errmsg(m_db)) 
        );
        return;
    }

    AddQueryToHistory(stmt);

    std::ofstream csvfile(outputFilename);
    if ( !csvfile.is_open() )
        return;

    int columnCount = sqlite3_column_count(stmt);

    while ( sqlite3_step(stmt) == SQLITE_ROW ) {

        // write each column from the row
        int i = 0;
        if ( tableName == TEAM_TABLE )
            i = 1; // start at 1 so we don't write the team uid to file

        for (; i < columnCount; i++ ) {
            const char* text = ( const char* ) sqlite3_column_text(stmt, i);
            if ( text )
                csvfile << text;
            else 
                csvfile << "NULL";

            // write comma
            if ( i < columnCount - 1 )
                csvfile << ",";
        }

        csvfile << std::endl; // newline
    }

    sqlite3_finalize(stmt);
}

// Not my code
/**
 * @brief Generates a QR code from the provided content and saves it as a PNG file.
 *
 * This function takes a string of content and generates a QR code with low error correction. It then creates an
 * image representing the QR code in RGB format, scales it, and saves it to the specified file in PNG format. If
 * the QR code image cannot be written to the file, an error is logged. A success message is logged once the QR
 * code is successfully generated and saved.
 *
 * @param content The content to be encoded in the QR code.
 * @param outputFilename The file path where the generated QR code image will be saved.
 */
void DataBase::ExportTOQRCode(const std::string& content, const std::string& outputFilename) {
    // Create a QR code
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(content.c_str(), qrcodegen::QrCode::Ecc::LOW);
    
    int size = qr.getSize();
    int scale = 5;
    int width = size * scale;
    int height = size * scale;

    unsigned char* image = new unsigned char[width * height * 3]; // RGB values

    for ( int y = 0; y < height; ++y ) {
        for ( int x = 0; x < width; ++x ) {
            int qrX = x / scale;
            int qrY = y / scale;

            unsigned char color = qr.getModule(qrX, qrY) ? 0 : 255; // Black for 1, white for 0
            image[( y * width + x ) * 3 + 0] = color; // R
            image[( y * width + x ) * 3 + 1] = color; // G
            image[( y * width + x ) * 3 + 2] = color; // B
        }
    }

    if ( !stbi_write_png(outputFilename.c_str(), width, height, 3, image, width * 3) ) {
        m_mainFrame->LogErrorMessage("Failed to write QR code to file.");
    }

    m_mainFrame->LogBackendMessage("QR code generated and saved to " + outputFilename);

    delete[] image;
}

void DataBase::ImportTableFromCSV(const std::string& tableName, const std::string& inputFilename) {
    std::ifstream csvfile(inputFilename);
    
    if ( !csvfile.is_open() ) {
        m_mainFrame->LogErrorMessage("Failed to open CSV file for import.");
        return;
    }
    
    std::string line;
    
    auto SplitString = [](std::string& s, char split) -> std::vector<std::string> {
        std::vector<std::string> result;
        std::string token;
        for ( char ch : s ) {
            if ( ch == split ) {
                result.push_back(token);
                token.clear();
            }
            else
                token += ch;
        }

        if ( !token.empty() )
            result.push_back(token);
        
        return result;
    };

    while ( std::getline(csvfile, line) ) {
        std::vector<std::string> values = SplitString(line, ',');

        if ( tableName == MATCH_TABLE ) {
            Match match = {};
            match.matchNum = std::stoi(values[0]);
            match.redWin = std::stoi(values[1]);
            match.blueWin = std::stoi(values[2]);
            match.teams[0].teamNum = std::stoi(values[3]);
            match.teams[1].teamNum = std::stoi(values[4]);
            match.teams[2].teamNum = std::stoi(values[5]);
            match.teams[3].teamNum = std::stoi(values[6]);
            match.teams[4].teamNum = std::stoi(values[7]);
            match.teams[5].teamNum = std::stoi(values[8]);

            AddMatch(match);
        }
        else if ( tableName == TEAM_TABLE ) {
            Team team = {};
            team.uid = GetNextTeamUID();
            team.teamNum = std::stoi(values[0]);
            team.matchNum = std::stoi(values[1]);
            team.overall = std::stoi(values[2]);
            team.hangAttempt = std::stoi(values[3]);
            team.hangSuccess = std::stoi(values[4]);
            team.robotCycleSpeed = std::stoi(values[5]);
            team.coralPoints = std::stoi(values[6]);
            team.defense = std::stoi(values[7]);
            team.autonomousPoints = std::stoi(values[8]);
            team.driverSkill = std::stoi(values[9]);
            team.penaltys = std::stoi(values[10]);
            team.rankingPoints = std::stoi(values[11]);

            AddTeam(team);
        }
        else {
            m_mainFrame->LogBackendMessage("Invalid table for import.");
            return;
        }
    }
    m_mainFrame->LogBackendMessage("Data imported from " + inputFilename + " to " + tableName);
}

