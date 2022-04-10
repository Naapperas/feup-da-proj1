#include <fstream>

#include "../includes/order.hpp"
#include "../includes/constants.hpp"
#include "../includes/utils.hpp"

Order::Order(int vol, int weight, int reward, int duration) : volume(vol), weight(weight), reward(reward), duration(duration) {};

int Order::getVolume() const { return this->volume; }
int Order::getWeight() const { return this->weight; }
int Order::getReward() const { return this->reward; }
int Order::getDuration() const { return this->duration; }

Order Order::from(const std::vector<std::string> &tokens) {

    int vol = std::stoi(tokens.at(0));
    int weight = std::stoi(tokens.at(1));
    int reward = std::stoi(tokens.at(2));
    int duration = std::stoi(tokens.at(3));

    return Order{vol, weight, reward, duration};
}

std::vector<Order> Order::processDataset() {

    std::ifstream dataset_file{ORDERS_FILE_PATH};

    if (!dataset_file.is_open()) return {};

    std::string line;
    std::vector<Order> result;

    std::getline(dataset_file, line); // ignore dataset header

    while (std::getline(dataset_file, line))
        result.push_back(Order::from(split(line, ' ')));

    return result;
}

std::ostream& operator<<(std::ostream& out, const Order& o) {

    out << "Order[ volume=" << o.volume << ", weight=" << o.weight << ", reward=" << o.reward << ", duration=" << o.duration << " ]";

    return out;
}