#ifndef _USING_UI // Console tests

#include "data.h"
#include "team.h"
#include "match.h"
#include <iostream>

int main() {
    DataBase db(DB_PATH);

    std::cout << "Testing database operations...\n";

    // Create test teams
    Team team1{ 101, 0, false, true, false, 50, 30, 40, 20, 60, 2, 85, 5, 15 };
    Team team2{ 102, 0, false, false, false, 45, 25, 35, 18, 55, 1, 80, 4, 14 };
    Team team3{ 103, 0, false, true, true, 55, 35, 45, 22, 65, 3, 90, 6, 16 };
    Team team4{ 104, 0, false, false, true, 40, 20, 30, 15, 50, 2, 75, 3, 13 };
    Team team5{ 105, 0, false, false, false, 60, 40, 50, 25, 70, 4, 95, 7, 17 };
    Team team6{ 106, 0, false, true, false, 48, 28, 38, 19, 58, 2, 83, 5, 14 };

    // Add teams
    db.AddTeam(team1);
    db.AddTeam(team2);
    db.AddTeam(team3);
    db.AddTeam(team4);
    db.AddTeam(team5);
    db.AddTeam(team6);

    std::cout << "Teams added.\n";
    db.ExportTableToCSV(TEAM_TABLE, "output.csv");

    // Verify if teams exist
    if ( db.TeamExists(101) && db.TeamExists(106) ) {
        std::cout << "Teams exist in the database.\n";
    }
    else {
        std::cout << "Error: Teams not found in database.\n";
    }

    // Create a match
    Match match1;
    match1.matchNum = 201;
    match1.played = false;
    match1.redWin = false;
    match1.blueWin = false;
    match1.teamCount = 6;
    match1.teams = { team1, team2, team3, team4, team5, team6 };

    db.AddMatch(match1);
    std::cout << "Match added.\n";

    // Verify match existence
    if ( db.MatchExists(201) ) {
        std::cout << "Match exists in the database.\n";
    }
    else {
        std::cout << "Error: Match not found in database.\n";
    }

    // Verify teams are in the match
    for ( const Team& team : match1.teams ) {
        if ( !db.TeamInMatch(team.teamNum, 201) ) {
            std::cout << "Error: Team " << team.teamNum << " not found in match.\n";
        }
    }

    // Modify a team
    team1.robotCycleSpeed = 75;
    db.UpdateTeam(team1);

    // Retrieve and verify modification
    Team updatedTeam = db.GetTeam(101);
    if ( updatedTeam.robotCycleSpeed == 75 ) {
        std::cout << "Team 101 updated successfully.\n";
    }
    else {
        std::cout << "Error: Team update failed.\n";
    }

    // Remove a team from the match
    db.RemoveTeamFromMatch(101, 201);
    std::cout << "Updated team 1: " << db.GetMatch(201).teams.at(0).teamNum << std::endl;
    if ( !db.TeamInMatch(101, 201) ) {
        std::cout << "Team 101 successfully removed from match.\n";
    }
    else {
        std::cout << "Error: Team removal from match failed.\n";
    }

    // Remove match
    db.RemoveMatch(201);
    if ( !db.MatchExists(201) ) {
        std::cout << "Match successfully removed.\n";
    }
    else {
        std::cout << "Error: Match removal failed.\n";
    }

    // Remove teams
    db.RemoveTeam(101);
    db.RemoveTeam(102);
    db.RemoveTeam(103);
    db.RemoveTeam(104);
    db.RemoveTeam(105);
    db.RemoveTeam(106);

    if ( !db.TeamExists(101) && !db.TeamExists(106) ) {
        std::cout << "Teams successfully removed.\n";
    }
    else {
        std::cout << "Error: Team removal failed.\n";
    }

    std::cout << "All tests completed.\n";
    return 0;
}

#endif