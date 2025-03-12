#include "match.h"
#include "team.h"

#include <iostream>

Match Match::FromSQLStatment(sqlite3_stmt* stmt) {
    Match match = {};
    match.matchNum = sqlite3_column_int(stmt, 0);
    match.played = sqlite3_column_int(stmt, 1);
    match.redWin = sqlite3_column_int(stmt, 2);
    match.blueWin = sqlite3_column_int(stmt, 3);
    
    for ( int i = 0; i < 6; i++ ) {
        int teamNum = sqlite3_column_int(stmt, i + 4);
        if ( teamNum == 0 ) // skip if there is no team number
            continue;
        
        // Note: This function does not initialize team values
        // but only the team number for each. if a team number is
        // 0 that means there is no team.
        match.teams.at(i).teamNum = teamNum;
        match.teamCount++;
    }

    return match;
}

bool Match::TeamInMatch(int teamNum) {
    if ( teamCount == 0 )
        return false;

    for ( const Team& team : this->teams )
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

    for ( Team& team : this->teams ) {
        if ( team.teamNum == teamNum ) {
            team = {0};
            this->teamCount--;
            break;
        }
    }
}

const Team& Match::Team1() const { return this->teams.at(0); }
const Team& Match::Team2() const { return this->teams.at(1); }
const Team& Match::Team3() const { return this->teams.at(2); }
const Team& Match::Team4() const { return this->teams.at(3); }
const Team& Match::Team5() const { return this->teams.at(4); }
const Team& Match::Team6() const { return this->teams.at(5); }
