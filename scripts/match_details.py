import requests
import base64
import json
import os
from dotenv import load_dotenv
import string

load_dotenv()

url = "https://frc-api.firstinspires.org/v3.0/2024/matches/ONWAT?tournamentLevel=Playoff"

username = os.getenv("API_USERNAME")
api_key = os.getenv("API_KEY")

creds = f"{username}:{api_key}"
encodedCreds = base64.b64encode(creds.encode()).decode()

headers = {
    "Authorization": f"Basic {encodedCreds}",
    "If-Modified-Since": "" 
}


response = requests.get(url, headers=headers).json()

# convert json values to import values that can be used to train ml model
# match #, red win, blue win, red 1, red 2, red 3, blue....

for match in response["Matches"]:
    # statistics
    redScore = match["scoreRedFinal"]
    blueScore = match["scoreBlueFinal"]
    if redScore == None or blueScore == None:
        continue
    
    redWin = int(redScore > blueScore)
    blueWin = int(blueScore > redScore)
    
    # info
    matchNum = match["matchNumber"]

    # teams
    teams = match["teams"]
    redTeams = []
    blueTeams = []
    for team in teams:
        teamNumber = team["teamNumber"]
        if "Red" in team["station"]:
            redTeams.append(teamNumber)
        elif "Blue" in team["station"]:
            blueTeams.append(teamNumber)

    matchInfo = [matchNum, redWin, blueWin, *redTeams, *blueTeams]
    print(matchInfo)
    
