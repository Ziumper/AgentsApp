#include "Twitch.h"




std::vector<TwitchUser> Twitch::ReadTwitchUserData(std::vector<std::string_view>& streamsDataPath, std::string_view& userDataPath)
{
    return std::vector<TwitchUser>();
}

std::vector<TwitchUser> Twitch::ReadTwitchUserData(std::string_view& userDataPath)
{
    std::vector<TwitchUser> result;
    CSVReader reader = CSVReader();
    auto data = reader.ReadCSV(userDataPath);

    for (size_t i = 1; i < data.size(); i++) {
        auto row = data[i];
        result.push_back(TwitchUser(row));
    }

    return result;
}

TwitchUser::TwitchUser(std::vector<std::string>& data)
{
    Id = std::stoi(data[0]);
    Rank = std::stoi(data[1]);
    MinutesStreamed = std::stoi(data[2]);
    AverageViewiersCount = std::stoi(data[3]);
    MaxViewiersCount = std::stoi(data[4]);
    HoursWatched = std::stoi(data[5]);
    Followers = std::stoi(data[6]);
    FollowersTotal = std::stoi(data[8]);//ignore views 7
    Login = data[10];
}
