#include "data.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>

DataBase::DataBase(const std::string& dbPath)
{
    if ( !std::filesystem::exists(dbPath) ) {
        std::cout << "File with path " << dbPath << " doesn't exist. Creating it" << std::endl;
        std::ofstream file(dbPath);
        if ( !file.is_open() ) {
            std::cout << "Failed to create file. Aborting." << std::endl;
            exit(-1);
        }
    
        std::cout << "File created successfully" << std::endl;
        CreateTables();
        return;
    }
    else {
        std::cout << "File with path " << dbPath << " exists." << std::endl;
    }

    Connect(dbPath);
}

DataBase::~DataBase()
{
}

void DataBase::EditTeam(const Team& team)
{
}

void DataBase::EditMatch(const Match& match)
{
}

void DataBase::AddTeam(const Team& team)
{
}

void DataBase::AddMatch(const Match& match)
{
}

void DataBase::RemoveTeam(int teamNum)
{
}

void DataBase::RemoveMatch(int matchNum)
{
}

std::vector<Team> DataBase::GetTeams()
{
    return std::vector<Team>();
}

std::vector<Match> DataBase::GetMatches()
{
    return std::vector<Match>();
}

void DataBase::CreateTables()
{
    std::cout << "Creating tables" << std::endl;
    
}

void DataBase::Connect(const std::string& dbPath)
{
    std::cout << "Connecting to SQL DB. Path: " << dbPath << std::endl;
}
