# Scrape round and team information from the FRC
# website for qualification rounds to train LLM
# for this project

import requests
from bs4 import BeautifulSoup

# URL for the qualification rounds of FRC
url = "https://frc-events.firstinspires.org/2025/ONWAT/qualifications"

response = requests.get(url)

if response.status_code != 200:
    exit(1)

soup = BeautifulSoup(response.content, "html.parser")
matchResults = soup.select_one('table#matches tbody#match-results') # this is how matches are laid out in html
matchData = []
    
if not matchResults:
    exit(1)

matches = matchResults.find_all('tr')
    
for match in matches:
    matchInfo = {}
    matchInfo['match_id'] = match.get('id')

    teams = {}
    for i in range(1, 7):
        id = f'team{i}_{matchInfo["match_id"][5:]}'.rstrip("a") # id would be "teamx_yy" where x is team # and yy is match #
        teamData = match.find('td', {'id': id})

        if not teamData:
            teams[f'team_{i}'] = None
            continue

        teamNumber = teamData.find('a')
        teams[f'team_{i}'] = teamNumber.text.strip()
        
    blueScore = match.find_all('td')[-1].text.strip()
    redScore = match.find_all('td')[-2].text.strip()
    if blueScore > redScore:
        matchInfo["blue_win"] = 1
        matchInfo["red_win"] = 0
    elif redScore > blueScore:
        matchInfo["blue_win"] = 0
        matchInfo["red_win"] = 1

    matchInfo['teams'] = teams
    matchData.append(matchInfo)

# print out as csv format
for match in matchData:
    print(match['match_id'].strip("match").strip("a"), end=",")
    print(match['red_win'], end=",")
    print(match['blue_win'], end=",")
    for team, team_number in match['teams'].items():
        print(team_number, end=",")
    print()


