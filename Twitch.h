#pragma once
#include <string>
#include <vector>
#include "CSVReader.h"


class TwitchStream {
public:
	int ViewiersCount = 0;
	int GameId = 0;
	std::string GameName = "";
	std::string Login = "";

	TwitchStream(std::vector<std::string>& data);
};

class TwitchUser {
public:
	int Id = 0;
	int Rank = 0;
	int MinutesStreamed = 0;
	int AverageViewiersCount = 0;
	int MaxViewiersCount = 0;
	int HoursWatched = 0;
	int Followers = 0;
	int FollowersTotal = 0;
	std::string Login  = "";
	std::vector<TwitchStream> Streams;

	TwitchUser(std::vector<std::string>& data);
};


class Twitch
{
public:
	std::vector<TwitchUser> ReadTwitchUserData(
		std::vector<std::string>& paths,
		std::string& path);
	std::vector<TwitchUser> ReadTwitchUserData(std::string& path);
	std::vector<TwitchStream> ReadTwitchStreams(std::string& path);
	static double GetAvialibility();
	static double GetReception();
};

