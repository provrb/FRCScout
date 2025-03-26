# qualification match data
data = [
    [1, 1, 0, 6875, 4678, 3683, 6992, 6070, 3739],
    [2, 0, 1, 8089, 7022, 2702, 4903, 4617, 6978],
    [3, 1, 0, 9659, 5024, 4476, 6162, 9562, 1360],
    [4, 0, 1, 3756, 9062, 4917, 2609, 5912, 772],
    [5, 0, 1, 3739, 6070, 6992, 2702, 7722, 8089],
    [6, 0, 0, 1360, 9562, 6162, 4917, 9062, 3756],
    [7, 1, 0, 6875, 4678, 3683, 4903, 4617, 6978],
    [8, 1, 0, 9659, 5024, 4476, 2609, 5912, 772],
    [9, 0, 1, 6978, 4617, 4903, 4917, 9062, 3756],
    [10, 1, 0, 772, 5912, 2609, 2702, 7022, 8089],
    [11, 1, 0, 6875, 4678, 3683, 4476, 5024, 9659],
    [12, 0, 1, 772, 5912, 2609, 4917, 9062, 3756],
    [13, 1, 0, 9659, 5024, 4476, 4917, 9062, 3756],
    [14, 0, 1, 6875, 4678, 3683, 4476, 5024, 9663],
    [15, 0, 1, 6875, 4678, 3683, 4476, 5024, 9659],
]

teams = {}

# get a dict of all teams in the tournament
for match in data:
    redWin = match[1]
    redTeams = match[3:6]
    blueTeams = match[6:9]
    
    for team in redTeams + blueTeams:
        if team not in teams:
            teams[team] = {"wins": 0, "win_rate": 0}

        if redWin == 1 and team in redTeams:
            teams[team]["wins"] += 1
        elif redWin == 0 and team in blueTeams:
            teams[team]["wins"] += 1

for teamNum, team in teams.items():
    team["win_rate"] = round(((team["wins"] / 12) * 100), 2)

for match in data:
    redTeams = match[3:6]
    blueTeams = match[6:9]
    for team in redTeams + blueTeams:
        match.append(teams[team]["win_rate"])

    print(*match, sep=",")