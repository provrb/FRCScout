#include "match.h"

#include <iostream>

Match Match::FromSQLStatment(sqlite3_stmt* stmt) {

    return Match();
}

void Match::AddCompetitor(Team* team) {
    if ( this->teamCount == 6 ) {
        std::cout << "Match is full. Cannot add more teams." << std::endl;
        return;
    }

    this->teams.at(this->teamCount) = team;
}

void Match::RemoveCompetitor(int teamNum) {
    if ( this->teamCount == 0 ) {
        std::cout << "Match is empty. Cannot remove teams." << std::endl;
        return;
    }

    for ( Team* team : this->teams ) {
        if ( team->teamNum == teamNum ) {
            team = {0};
            break;
        }
    }
}
