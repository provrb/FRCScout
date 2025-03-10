#include <backend/data.h>
#include <iostream>    

int main() {    
    DataBase db(DB_PATH);
    Team team = {};
    team.teamNum = 10;
    team.matchNum = 2;
    team.eliminated = false;
    team.hangAttempt = true;
    team.hangSuccess = true;
    team.robotCycleSpeed = 42;
    team.coralPoints = 33;
    team.defense = 14;
    team.autonomousPoints = 66;
    team.driverSkill = 2;
    team.fouls = 2;
    team.overall = 28;
    team.rankingPoints = 39;
    team.ppm = 59;

    //db.AddTeam(team);

    Match match = {};
    match.blueWin = false;
    match.matchNum = 1;
    match.played = false;

    //db.EditTeam(team);
    //db.AddMatch(match);
    db.AddTeamToMatch(10, 1);
    //db.GetTeam(10).DebugPrint();
    //Team fromSQL = db.GetTeam(2);

    //std::cout << "\n\n" << std::endl;

    //std::cout << "Team from SQL: " << std::endl;


    return 0;
}
