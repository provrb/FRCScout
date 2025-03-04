#include <backend/data.h>
    
int main() {    
    DataBase db(DB_PATH);
    Team team = { 1, false, true, false, 5, 100, 5, 5, 5, 5, 0, 10, 0};
    db.AddTeam(team);
    db.TeamExists(1);
    
    return 0;
}
