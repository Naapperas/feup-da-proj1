#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../includes/constants.hpp"
#include "../includes/dataset.hpp"
#include "../includes/order.hpp"
#include "../includes/scenarios.hpp"
#include "../includes/ui.hpp"
#include "../includes/utils.hpp"
#include "../includes/van.hpp"

template <class T>
std::optional<T> UserInterface::optionsMenu(const Options<T> &options) {
    for (int i{1}; i < options.size(); ++i) {
        std::cout << "(" << i << ") " << options.at(i).first << std::endl;
    }

    std::cout << "(0) " << options.front().first << "\n\n";

    unsigned option = getUnsignedInput("Please insert option: ");

    if (option < options.size())
        return options.at(option).second;

    errorMessage = "Invalid option!\n";
    return {};
}

unsigned long UserInterface::getUnsignedInput(const std::string &prompt,
                                              unsigned long min,
                                              unsigned long max) {
    std::string input;
    unsigned long number;
    bool done = false;

    do {
        input = getStringInput(prompt);

        try {
            number = stoul(input);
            done = true;
        } catch (std::invalid_argument) {
            errorMessage = "Invalid input!\n";
            done = false;
        }
    } while (!done || !inRange(number, min, max));

    return number;
}

double UserInterface::getDoubleInput(const std::string &prompt, double min,
                                     double max) {
    std::string input;
    double number;
    bool done = false;

    do {
        input = getStringInput(prompt);

        try {
            number = stod(input);
            done = true;
        } catch (std::invalid_argument) {
            errorMessage = "Invalid input!\n";
            done = false;
        }
    } while (!done || !inRange(number, min, max));

    return number;
}

std::string UserInterface::getStringInput(const std::string &prompt) {
    std::cout << RED_TEXT << errorMessage << RESET_FORMATTING << prompt;
    errorMessage = "";

    std::string input{};

    std::getline(std::cin, input);
    normalizeInput(input);

    if (std::cin.eof())
        throw Exit();

    return input;
}

bool UserInterface::inRange(unsigned long n, unsigned long min,
                            unsigned long max) {
    bool b = (n <= max) && (n >= min);

    if (!b)
        errorMessage = "Value outside allowed range!\n";

    return b;
}

bool UserInterface::inRange(double n, double min, double max) {
    bool b = (n <= max) && (n >= min);

    if (!b)
        errorMessage = "Value outside allowed range!\n";

    return b;
}

void UserInterface::show(Dataset &dataset) {
    std::cout << CLEAR_SCREEN << PROGRAM_NAME << '\n' << std::endl;

    switch (currentMenu) {
    case MAIN:
        mainMenu();
        break;
    case SHOW_ORDERS:
        showOrdersMenu(dataset);
        break;
    case SHOW_VANS:
        showVansMenu(dataset);
        break;
    case CHOOSE_SCENARIO:
        chooseScenarioMenu();
        break;
    case CHOOSE_DATASET:
        chooseDatasetMenu(dataset);
        break;

    case SCENARIO_ONE:
        scenarioOneMenu(dataset);
        break;
    case SCENARIO_TWO:
        scenarioTwoMenu(dataset);
        break;
    case SCENARIO_THREE:
        scenarioThreeMenu(dataset);
        break;

    case RESULTS:
        resultsMenu();
        break;
    case RESULTS_VANS:
        resultsVansMenu();
        break;

    case EXIT:
    default:
        throw Exit();
        break;
    }
}

void UserInterface::exit() {
    std::cout << CLEAR_SCREEN << std::flush;
    std::cout << "Shutting down..." << std::endl;
}

void UserInterface::mainMenu() {
    auto menu = optionsMenu<Menu>({
        {"Exit", EXIT},
        {"Orders", SHOW_ORDERS},
        {"Vans", SHOW_VANS},
        {"Choose dataset", CHOOSE_DATASET},
        {"Choose scenario", CHOOSE_SCENARIO},
    });
    currentMenu = menu.value_or(currentMenu);
}

template <class T>
void UserInterface::paginatedMenu(const std::vector<T> &items) {
    static unsigned int page{0};

    unsigned int pages = ceil((float)items.size() / ITEMS_PER_PAGE);

    for (auto i{items.begin() + page * ITEMS_PER_PAGE},
         end{page == pages - 1 ? items.end()
                               : items.begin() + (page + 1) * ITEMS_PER_PAGE};
         i < end; ++i)
        std::cout << *i << std::endl;

    std::cout << "\nPage " << page + 1 << " of " << pages;
    auto option = getStringInput(
        "\nPress enter for next page, or 'q' to exit paginated view");

    if ((option.size() == 1 && tolower(option.at(0)) == 'q') ||
        (++page == pages)) {
        currentMenu = MAIN;
        page = 0;
    }
}

void UserInterface::showOrdersMenu(Dataset &dataset) {
    std::cout << "Volume\tWeight\tReward\tDuration\n";
    paginatedMenu(dataset.getOrders());
}

void UserInterface::showVansMenu(Dataset &dataset) {
    std::cout << "Volume\tWeight\tCost\n" << std::left;
    paginatedMenu(dataset.getVans());
}

void UserInterface::chooseScenarioMenu() {
    auto menu = optionsMenu<Menu>({
        {"Go back", MAIN},
        {"Scenario 1 - Minimize used vans", SCENARIO_ONE},
        {"Scenario 2 - Maximize profit", SCENARIO_TWO},
        {"Scenario 3 - Maximize express deliveries", SCENARIO_THREE},
    });
    currentMenu = menu.value_or(currentMenu);
}

void UserInterface::chooseDatasetMenu(Dataset &dataset) {
    Options<std::string> options{{"Go back", ""}};

    for (auto &p : std::filesystem::directory_iterator(DATASETS_PATH))
        if (p.is_directory())
            options.push_back({
                p.path().filename().string(),
                p.path().filename().string(),
            });

    const auto selection = optionsMenu(options);

    if (!selection.has_value())
        return;

    currentMenu = MAIN;

    if (selection == "")
        return;

    dataset = Dataset::load(selection.value());
}

void UserInterface::scenarioOneMenu(Dataset &dataset) {
    auto selection = optionsMenu<std::optional<Scenario1Strategy>>({
        {"Go back", {}},
        {"Optimize using volume - ascending", Scenario1Strategy::VOLUME_ASC},
        {"Optimize using volume - descending", Scenario1Strategy::VOLUME_DESC},
        {"Optimize using weight - ascending", Scenario1Strategy::WEIGHT_ASC},
        {"Optimize using weight - descending", Scenario1Strategy::WEIGHT_DESC},
        {"Optimize using area   - ascending", Scenario1Strategy::AREA_ASC},
        {"Optimize using area   - descending", Scenario1Strategy::AREA_DESC},
    });

    if (!selection.has_value()) // Error while getting option
        return;

    if (!selection.value().has_value()) { // User wants to go back
        currentMenu = CHOOSE_SCENARIO;
        return;
    }

    result = scenario1(dataset, selection.value().value());
    currentMenu = RESULTS;
}

void UserInterface::scenarioTwoMenu(Dataset &dataset) {
    auto selection = optionsMenu<std::optional<Scenario2Strategy>>({
        {"Go back", {}},
        {"Optimize using division", Scenario2Strategy::DIVIDE},
        {"Optimize using multiplication", Scenario2Strategy::MULTIPLY},
    });

    if (!selection.has_value()) // Error while getting option
        return;

    if (!selection.value().has_value()) { // User wants to go back
        currentMenu = CHOOSE_SCENARIO;
        return;
    }

    result = scenario2(dataset, selection.value().value());
    currentMenu = RESULTS;
}

void UserInterface::scenarioThreeMenu(Dataset &dataset) {
    result = scenario3(dataset);
    currentMenu = RESULTS;
}

void UserInterface::resultsMenu() {
    std::cout << "Used " << result.vans.size() << " vans\n"
              << "Dispatched " << result.ordersDispatched << " orders, "
              << result.remainingOrders.size() << " remain\n"
              << "Delivery efficiency: " << (result.efficiency * 100) << "%\n"
              << "Average delivery time: "
              << ((double)result.deliveryTime / result.ordersDispatched)
              << "s\n"
              << "Spent " << result.cost << "€ on vans\n"
              << "Received " << result.reward << "€ from orders\n"
              << "Total profit: " << result.profit << "€\n"
              << "Took " << result.runtime.count() << "µs\n\n";

    // if (result.vans.size())
    //     for (auto &van : result.vans)
    //         van.printStatistics(std::cout);

    getStringInput("Press enter to continue ");
    currentMenu = MAIN;
    // currentMenu = optionsMenu<Menu>({
    //     {"Continue", MAIN},
    //     {"See vans", RESULTS_VANS},
    // });
}

void UserInterface::resultsVansMenu() {
    std::cout << "Volume\tWeight\tCost\n" << std::left;
    paginatedMenu(result.vans);
}
