#include "match.h"
#include "team.h"

#include <iostream>

Match Match::FromSQLStatment(sqlite3_stmt* stmt) {

    return Match();
}

bool Match::TeamInMatch(int teamNum) {
    if ( teamCount == 0 )
        return false;

    for ( Team team : this->teams )
        if ( team.teamNum == teamNum )
            return true;

    return false;
}

void Match::AddCompetitor(Team team) {
    if ( this->teamCount == 6 ) {
        std::cout << "Match is full. Cannot add more teams." << std::endl;
        return;
    }

    this->teams.at(this->teamCount) = team;
    this->teamCount++;
}

void Match::RemoveCompetitor(int teamNum) {
    if ( this->teamCount == 0 ) {
        std::cout << "Match is empty. Cannot remove teams." << std::endl;
        return;
    }

    for ( Team team : this->teams ) {
        if ( team.teamNum == teamNum ) {
            team = {0};
            this->teamCount--;
            break;
        }
    }
}
