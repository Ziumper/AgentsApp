#pragma once
#include <string>
#include <vector>


class TwitchStream {
public:
	int ViewiersCount;
	int GameId;
	std::string_view GameName;
};

class TwitchUser {
public:
	int Id;
	int Followers;
	int AverageViewiersCount;
	std::string_view Login;
	std::vector<TwitchStream> Streams;
};




class Twitch
{
public:
	std::vector<TwitchUser> ReadTwitchUserData(
		std::vector<std::string_view>* streamsDataPath,
		std::string_view* userDataPath);
	std::vector<TwitchUser> ReadTwitchUserData(std::string_view* userDataPath);
};

