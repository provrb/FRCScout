#include <match.h> 
#include <iostream> // cout, endl

/**
 * @brief Creates a Match object from an SQLite database statement.
 *
 * Extracts match details from the given SQLite statement, including the match number,
 * whether it has been played, and the teams involved.
 *
 * @param stmt Pointer to an SQLite statement containing match data.
 * @return Match object populated with data from the database.
 *
 * @note This function initializes only the team numbers. Other team details
 *       must be retrieved separately.
 */
Match Match::FromSQLStatment(sqlite3_stmt* stmt) {
    Match match = {};
    match.matchNum = sqlite3_column_int(stmt, 0);
    match.redWin = sqlite3_column_int(stmt, 1);
    match.blueWin = sqlite3_column_int(stmt, 2);

    for ( int i = 0; i < 6; i++ ) {
        int teamNum = sqlite3_column_int(stmt, i + 3);
        if ( teamNum == 0 ) // Skip if there is no team number
            continue;

        match.teams.at(i).teamNum = teamNum;
        match.teamCount++;
    }

    return match;
}

/**
 * @brief Checks if a given team is part of the match.
 *
 * @param teamNum The team number to check.
 * @return true if the team is in the match, false otherwise.
 */
bool Match::TeamInMatch(int teamNum) {
    if ( teamCount == 0 )
        return false;

    for ( const Team& team : this->teams )
        if ( team.teamNum == teamNum )
            return true;

    return false;
}

/**
 * @brief Adds a team to the match.
 *
 * If the match already has 6 teams, the function prints an error message
 * and does not add the team.
 *
 * @param team The team to be added.
 */
void Match::AddCompetitor(Team team) {
    if ( this->teamCount == 6 ) {
        std::cout << "Match is full. Cannot add more teams." << std::endl;
        return;
    }

    this->teams.at(this->teamCount) = team;
    this->teamCount++;
}

/**
 * @brief Removes a team from the match by its team number.
 *
 * If the team is found, it is removed and the total team count is decreased.
 *
 * @param teamNum The number of the team to remove.
 */
void Match::RemoveCompetitor(int teamNum) {
    if ( this->teamCount == 0 ) {
        std::cout << "Match is empty. Cannot remove teams." << std::endl;
        return;
    }

    for ( Team& team : this->teams ) {
        if ( team.teamNum == teamNum ) {
            team = { 0 };
            this->teamCount--;
            break;
        }
    }
}

/**
 * @brief Checks if the given team is on the red alliance.
 *
 * This function checks if the specified team (identified by its team number)
 * is part of the red alliance. In the match structure, the red alliance is
 * assumed to be represented by the first three teams in the `teams` vector.
 *
 * @param teamNum The team number to check.
 * @return true if the team is on the red alliance, false otherwise.
 */
bool Match::RedAllianceTeam(int teamNum) const {
    // Check if the team is on the red alliance (first 3 teams)
    return std::any_of(this->teams.begin(), this->teams.begin() + 3, [&teamNum](const Team& t) {
        return teamNum == t.teamNum;
    });
}

/**
 * @brief Checks if the red alliance won the match.
 *
 * @return true if the red alliance won, false otherwise.
 */
const bool Match::RedWon() const {
    return redWin == true && blueWin == false;
}

/**
 * @brief Checks if the blue alliance won the match.
 *
 * @return true if the blue alliance won, false otherwise.
 */
const bool Match::BlueWon() const {
    return redWin == false && blueWin == true;
}

/**
 * @brief Checks if the match resulted in a tie.
 *
 * @return true if the match was a tie, false otherwise.
 */
const bool Match::IsTie() const {
    return redWin == true && blueWin == true;
}

/**
 * @brief Retrieves the first team in the match.
 * @return Reference to the first team.
 */
const Team& Match::Team1() const { return this->teams.at(0); }

/**
 * @brief Retrieves the second team in the match.
 * @return Reference to the second team.
 */
const Team& Match::Team2() const { return this->teams.at(1); }

/**
 * @brief Retrieves the third team in the match.
 * @return Reference to the third team.
 */
const Team& Match::Team3() const { return this->teams.at(2); }

/**
 * @brief Retrieves the fourth team in the match.
 * @return Reference to the fourth team.
 */
const Team& Match::Team4() const { return this->teams.at(3); }

/**
 * @brief Retrieves the fifth team in the match.
 * @return Reference to the fifth team.
 */
const Team& Match::Team5() const { return this->teams.at(4); }

/**
 * @brief Retrieves the sixth team in the match.
 * @return Reference to the sixth team.
 */
const Team& Match::Team6() const { return this->teams.at(5); }
