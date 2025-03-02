#include <backend/data.h>
    
const char* DB_PATH = "data.db";

int main() {    
    DataBase db(DB_PATH);
    
    return 0;
}
