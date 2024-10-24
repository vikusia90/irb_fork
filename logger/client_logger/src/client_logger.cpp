#include <not_implemented.h>

#include "../include/client_logger.h"

std::map<std::string,
        std::pair<std::ofstream*, int>>
        client_logger::streams;

client_logger::client_logger(std::map<std::string, std::vector<bool>> _files, const std::string &_format)
        : format(_format), files(std::move(_files))
{
    for (auto &[file_name, severity] : files) {
        // Проверяем, есть ли поток и задан ли северети
        if (streams.find(file_name) != streams.end() && streams[file_name].first) {
            // Убедитесь, что поток действительно существует
            if (streams[file_name].second != 0) {
                // Возможно, не нужно пересваивать, если значения совпадают.
                files[file_name] = severity;
            }
        } else { // Открываем новый поток
            streams[file_name].first = new std::ofstream(); // Инициализация потока, если не инициализирован
            streams[file_name].first->open(file_name);
            if (!streams[file_name].first->is_open()) {
                throw std::runtime_error("Failed to open stream: " + file_name);
            }
            files[file_name] = severity;
        }
        (streams[file_name].second)++;
    }
}


client_logger::client_logger(
    client_logger const &other)
    : files(other.files),
    format(other.format)
{
    for (auto &[key, pair] : streams){
        pair.second++;//увеличиваем количество логеров для каждого потока, т.к. появился новый объект-логгер, использующий этот поток.
    }
}

client_logger &client_logger::operator=(
    client_logger const &other)
{
    if (this != &other){
        close_streams();
        files = other.files;
        format = other.format;
        for (auto &[key, pair] : streams){
            streams[key].second++;//увеличиваем количество логеров для потока, который копируется в данный момент
        }
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
        close_streams();
        files = std::move(other.files);
        format = std::move(other.format);
    }
    return *this;
}

client_logger::~client_logger() noexcept
{
    close_streams();
    files.clear();
    format = "";
}

std::string client_logger::string_format(const std::string &message, logger::severity severity) const{
    std::string result;

    //std::string time_data = current_datetime_to_string();

    for (int i = 0; i <format.size(); i++){
        if (format[i] == '%'){
            switch (format[i+1]) {
                case 'd':
                    result += current_datetime_to_string().substr(0,10);
                    result+= " ";
                    i++;
                    break;
                case 't':
                    result += current_datetime_to_string().substr(11, 18);
                    result += " ";
                    i++;
                    break;
                case 's':
                    result += severity_to_string(severity);
                    result += " ";
                    i++;
                    break;
                case 'm':
                    result += message;
                    result += " ";
                    i++;
                    break;
            }
        }
        else{
            result += format[i];
        }
    }
    return result;
}

logger const *client_logger::log(
    const std::string &text,
    logger::severity severity) const noexcept
{
    for (const auto & pair : files) {
        if (static_cast<int>(severity) < pair.second.size() && pair.second[static_cast<int>(severity)]) {
            std::string result_string;
            result_string = string_format(text, severity);

            if (pair.first == "cerr")
                std::cerr << result_string << std::endl;

            auto stream_entry = streams.find(pair.first);
            if (stream_entry != streams.end() && stream_entry->second.first) {
                // Проверка, что поток открыт и существует
                if (stream_entry->second.first->is_open()) {
                    *stream_entry->second.first << result_string << std::endl; // Запись в поток
                    //stream_entry->second.first->flush(); // Принудительная запись данных
                    std::cout << "Successfully wrote to " << pair.first << std::endl;
                } else {
                    std::cerr << "Error: Could not write to stream for " << pair.first << " (stream is not open)."
                              << std::endl;
                }
            } else {
                std::cerr << "Error: Stream for " << pair.first << " not found." << std::endl;
            }
        }
        else {
            std::cout << "Severity level not enabled for: " << pair.first << std::endl;
        }
    }
    return this;
}

void client_logger::close_streams() {
    for (auto & [file_name, severity] : files){//проходим по мапе
        if (--(streams[file_name].second)==0){//если это последний лог в потоке
            streams[file_name].first->close();//закрываем поток
            streams.erase(file_name);//удаляем запись, освобождая ресурсы
        }
    }
}







