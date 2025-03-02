#include <backend/data.h>
    
const char* DB_PATH = "data.db";

int main() {    
    DataBase db(DB_PATH);
    Team team = { 1, false, true, false, 5, 100, 5, 5, 5, 5, 0, 10, 0, 0 };
    db.AddTeam(team);
    
    return 0;
}
