#include "Twitch.h"
#include "Randomizer.h"




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

double Twitch::GetAvialibility()
{
    RealRandomizer randomizer = RealRandomizer(0,1);
    
    double value = randomizer.GetEvenRandomNumber();
    if (value <= 0.17) //17% range 0 - 0.1
        return RealRandomizer(0, 0.1).GetEvenRandomNumber();
    else if (value <= 0.38) // 21+17 = 38
        return RealRandomizer(0.1, 0.2).GetEvenRandomNumber();
    else if (value <= 0.55) // 38% + 17%
        return RealRandomizer(0.2, 0.3).GetEvenRandomNumber();
    else if (value <= 0.66)
        return RealRandomizer(0.3, 0.4).GetEvenRandomNumber();
    else if (value <= 0.73)
        return RealRandomizer(0.4, 0.5).GetEvenRandomNumber();
    else if (value <= 0.78)
        return RealRandomizer(0.5, 0.6).GetEvenRandomNumber();
    else if (value <= 0.83)
        return RealRandomizer(0.6, 0.7).GetEvenRandomNumber();
    else if (value <= 0.87)
        return RealRandomizer(0.7, 0.8).GetEvenRandomNumber();
    else if (value <= 0.90)
        return RealRandomizer(0.8, 0.9).GetEvenRandomNumber();
    return RealRandomizer(0.9, 1).GetEvenRandomNumber();
}

double Twitch::GetReception()
{
    RealRandomizer randomizer = RealRandomizer(0, 1);
    double value = randomizer.GetEvenRandomNumber();

    if (value <= 0.64)
        return RealRandomizer(0, 0.1).GetEvenRandomNumber();
    else if (value <= 0.80)
        return RealRandomizer(0.1, 0.2).GetEvenRandomNumber();
    else if (value <= 0.86)
        return RealRandomizer(0.2, 0.3).GetEvenRandomNumber();
    else if (value <= 0.90)
        return RealRandomizer(0.3, 0.4).GetEvenRandomNumber();
    else if (value <= 0.92)
        return RealRandomizer(0.4, 0.5).GetEvenRandomNumber();
    else if (value <= 0.93)
        return RealRandomizer(0.5, 0.6).GetEvenRandomNumber();
    else if (value <= 0.94)
        return RealRandomizer(0.6, 0.7).GetEvenRandomNumber();
    else if (value <= 0.95)
        return RealRandomizer(0.7, 0.8).GetEvenRandomNumber();
    else if (value <= 0.96)
        return RealRandomizer(0.8, 0.9).GetEvenRandomNumber();
    return RealRandomizer(0.9, 1).GetEvenRandomNumber();
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
