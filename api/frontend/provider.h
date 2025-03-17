#include <vector>

#include "backend/team.h"

class ITeamDataProvider {
public:
    virtual ~ITeamDataProvider() = default;

    virtual std::vector<Team> GetTeams() = 0;
};
