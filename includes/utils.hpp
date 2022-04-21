#ifndef DA_PROJ1_UTILS_H
#define DA_PROJ1_UTILS_H

#define ENUM(name, ...) enum class name { BEGIN, __VA_ARGS__, END }
#define FOR_ENUM(type, name)                                                   \
    for (type name = static_cast<type>(static_cast<int>(type::BEGIN) + 1);     \
         name != type::END;                                                    \
         name = static_cast<type>(static_cast<int>(name) + 1))

#include <string>
#include <vector>

/**
 * @brief Exception thrown when the user wants to exit the program.
 */
class Exit {};

/**
 * @brief Splits a string by a character.
 *
 * @param str The string to be split.
 * @param sep The character to use as the separator.
 * @return A vector with all the words in the string that were separated by the
 *         given character.
 */
std::vector<std::string> split(std::string str, char sep);

/**
 * @brief Normalizes a string.
 *
 * @details Replaces all tabs with spaces, removes duplicate spaces and trims
 *          spaces from the start and the end.
 *
 * @param input The string to normalize.
 */
void normalizeInput(std::string &input);

#endif
