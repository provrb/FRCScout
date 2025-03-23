import requests
from bs4 import BeautifulSoup
import json

# URL for the qualification rounds of FRC
url = "https://frc-events.firstinspires.org/2025/ONWAT/qualifications"

response = requests.get(url)

if response.status_code != 200:
    return

soup = BeautifulSoup(response.content, "html.parser")
match_results = soup.select_one('table#matches tbody#match-results')
    
if match_results:
    match_data = []

    matches = match_results.find_all('tr')
        
    for match in matches:
        match_info = {}
        match_info['match_id'] = match.get('id')

        teams = {}
        for i in range(1, 7):
            id = f'team{i}_{match_info["match_id"][5:]}'
            id = id.rstrip("a")
            team_cell = match.find('td', {'id': id})

            if team_cell:
                team_link = team_cell.find('a')
                if team_link:
                    teams[f'team_{i}'] = team_link.text.strip()
                else:
                    teams[f'team_{i}'] = None
            else:
                teams[f'team_{i}'] = None
            
            
        blue_score = match.find_all('td')[-1].text.strip()
        red_score = match.find_all('td')[-2].text.strip()
        if blue_score > red_score:
            match_info["blue_win"] = 1
            match_info["red_win"] = 0
        elif red_score > blue_score:
            match_info["blue_win"] = 0
            match_info["red_win"] = 1

        match_info['teams'] = teams
        match_data.append(match_info)
    for match in match_data:
        print(match['match_id'].strip("match").strip("a"), end=",")
        print(match['red_win'], end=",")
        print(match['blue_win'], end=",")
        for team, team_number in match['teams'].items():
            print(team_number, end=",")
        print()
else:
    print("No match results found.")

