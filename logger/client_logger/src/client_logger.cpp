#include <not_implemented.h>

#include "../include/client_logger.h"

/*std::map<std::string,
        std::pair<std::ofstream*, int>>
        client_logger::streams;*/

client_logger::client_logger(std::map<std::string, std::vector<int>> _files, const std::string &_format) {
    for (auto &[file_name, severity] : files){
        //проверяем есть ли поток и задан ли северети
        if (streams.find(file_name) != streams.end() && streams[file_name].second != 0){
            files[file_name] = severity;
        }
        else{//открываем новый поток
            streams[file_name].first->open(file_name);
            if (!streams[file_name].first->is_open()){
                throw std::runtime_error("Failed to open streams. \n");
            }
            files[file_name] = severity;
        }
        (streams[file_name].second)++;
    }
    format = _format;
}

client_logger::client_logger(
    client_logger const &other)
    : files(other.files),
    format(other.format)
{
    for (auto &[key, pair] : streams){
        pair.second++;//увеличиваем счетчик пользователей для каждого потока, т.к. появился новый объект-логгер, использующий этот поток.
    }
}

client_logger &client_logger::operator=(
    client_logger const &other)
{
    if (this != &other){
        files = other.files;
        format = other.format;
    }
    return *this;
}

client_logger::client_logger(
    client_logger &&other) noexcept
{
    files = std::move(other.files);
    format = std::move(other.format);
}

client_logger &client_logger::operator=(
    client_logger &&other) noexcept
{
    if (this != &other){
        files = std::move(other.files);
        format = std::move(other.format);
    }
    return *this;
}

client_logger::~client_logger() noexcept
{
    files.clear();
    format = "";
}

logger const *client_logger::log(
    const std::string &text,
    logger::severity severity) const noexcept
{
    throw not_implemented("logger const *client_logger::log(const std::string &text, logger::severity severity) const noexcept", "your code should be here...");
}




