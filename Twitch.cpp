#include "Twitch.h"




std::vector<TwitchUser> Twitch::ReadTwitchUserData(std::vector<std::string>& streamsDataPath, std::string& userDataPath)
{
    std::vector<TwitchUser> users = this->ReadTwitchUserData(userDataPath);
    std::vector<std::vector<TwitchStream>> streamsData; 

    for (std::string& streamPath : streamsDataPath) {
        streamsData.push_back(this->ReadTwitchStreams(streamPath));
    }

    for (TwitchUser& user : users) {
        //load a stream 
        for (std::vector<TwitchStream>& streamCollection : streamsData) {
            for (TwitchStream& stream : streamCollection) {
                if (user.Login == stream.Login) {
                    user.Streams.push_back(stream);
                }
           }
        }
    }

    return users;
}

std::vector<TwitchUser> Twitch::ReadTwitchUserData(std::string& userDataPath)
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

std::vector<TwitchStream> Twitch::ReadTwitchStreams(std::string& path)
{
    std::vector<TwitchStream> result;
    CSVReader reader = CSVReader();
    auto data = reader.ReadCSV(path);
    for (size_t i= 1; i < data.size(); i++) {
        auto row = data[i];
        result.push_back(TwitchStream(row));
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

TwitchStream::TwitchStream(std::vector<std::string>& data)
{
    GameId = std::stoi(data[4]);
    ViewiersCount = std::stoi(data[8]);
    GameName = data[5];
    Login = data[2];
}
