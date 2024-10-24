#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H

#include <logger.h>
#include "client_logger_builder.h"
#include <map>
#include <string>
#include "vector"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

class client_logger final:
    public logger
{
private:
    std::map<std::string, std::vector<bool>> files;
    static std::map<std::string, std::pair<std::ofstream *, int>> streams;
    std::string format;

    client_logger(std::map<std::string, std::vector<bool>> _files, const std::string& _format);

    friend class client_logger_builder;

public:

    client_logger(
        client_logger const &other);

    client_logger &operator=(
        client_logger const &other);

    client_logger(
        client_logger &&other) noexcept;

    client_logger &operator=(
        client_logger &&other) noexcept;

    ~client_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

    void close_streams();

    std::string string_format(std::string const &message, logger::severity severity) const;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H