#include <team.h>
#include <iostream> // cout

/**
 * @brief Creates a Team object from an SQLite database statement.
 *
 * Extracts team statistics from the given SQLite statement, including performance metrics
 * such as hang attempts, cycle speed, defense, and ranking points.
 *
 * @param stmt Pointer to an SQLite statement containing team data.
 * @return Team object populated with data from the database.
 */
Team Team::FromSQLStatment(sqlite3_stmt* stmt) {
    Team team = {};
    team.uid = sqlite3_column_int(stmt, 0);
    team.teamNum = sqlite3_column_int(stmt, 1);
    team.eliminated = sqlite3_column_int(stmt, 2);
    team.hangAttempt = sqlite3_column_int(stmt, 3);
    team.hangSuccess = sqlite3_column_int(stmt, 4);
    team.robotCycleSpeed = sqlite3_column_int(stmt, 5);
    team.coralPoints = sqlite3_column_int(stmt, 6);
    team.defense = sqlite3_column_int(stmt, 7);
    team.autonomousPoints = sqlite3_column_int(stmt, 8);
    team.driverSkill = sqlite3_column_int(stmt, 9);
    team.fouls = sqlite3_column_int(stmt, 10);
    team.overall = sqlite3_column_int(stmt, 11);
    team.rankingPoints = sqlite3_column_int(stmt, 12);
    team.ppm = sqlite3_column_int(stmt, 13);

    return team;
}

/**
 * @brief Prints team details to the console for debugging purposes.
 *
 * Displays various team statistics, including match performance, defense,
 * fouls, and ranking metrics.
 */
void Team::DebugPrint() const {
    std::cout << "uid: " << this->uid << std::endl;
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
