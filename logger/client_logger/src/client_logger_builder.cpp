#include <not_implemented.h>

#include "../include/client_logger_builder.h"


client_logger_builder::client_logger_builder(
    client_logger_builder const &other):
    files(other.files),
    format(other.format)
{
}

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder const &other)
{
    if (this != &other){
        files.clear();
        files = other.files;
        format = other.format;
    }
    return *this;
}

client_logger_builder::client_logger_builder(
    client_logger_builder &&other) noexcept
    :files(std::move(other.files)), format(std::move(other.format))
{

}

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder &&other) noexcept
{
    if (this != &other){
        files.clear();
        files = std::move(other.files);
        format = std::move(other.format);
    }
    return *this;
}

client_logger_builder::~client_logger_builder() noexcept
{
    files.clear();
    format = "";
}

logger_builder *client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    //получаем асболютны путь к файлу
    std::string abs_file_path = std::filesystem::absolute(stream_file_path).string();
    if (abs_file_path.empty()){//проверяем, пустой ли 
        throw std::logic_error("Path can't be empty");
    }
    auto i = files.find(abs_file_path);
    if (i != files.end()){
        //провеярем, задан ли sevetiry
        if(i->second[static_cast<int>(severity)]) return this;
        i->second[static_cast<int>(severity)] = true;
        return this;
    }
    // если элемент не найден, добавляем новый с уровнем ошибки
    std::vector<bool> severity_flags(count_severity, false); // инициализация вектора
    severity_flags[static_cast<int>(severity)] = true; // устанавливаем уровень ошибки
    files.emplace(abs_file_path, severity_flags);
    return this;
}

logger_builder *client_logger_builder::add_console_stream(
    logger::severity severity)
{
    auto i = files.find("cerr");
    if (i != files.end()){
        //провеярем, задан ли sevetiry
        if(i->second[static_cast<int>(severity)]) return this;
        i->second[static_cast<int>(severity)] = true;
        return this;
    }
    // если элемент не найден, добавляем новый с уровнем ошибки
    std::vector<bool> severity_flags(count_severity, false); // инициализация вектора
    severity_flags[static_cast<int>(severity)] = true; // устанавливаем уровень ошибки
    files.emplace("cerr", severity_flags);
    return this;
}

logger_builder* client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,//это путь к файлу конфигурации
    std::string const &configuration_path)//а это путь к конфигурации внутри json
{
    std::ifstream config_file(configuration_file_path);

    if(!config_file.is_open()){
        throw std::runtime_error("Configuration file doesn't exist\n");
    }

    if(config_file.peek()==EOF){
        throw std::runtime_error("Can't find configuration path\n");
    }

    nlohmann::json config_data;

    config_file>>config_data;

    //проверка наличия указанного пути в конфигурации
    if(config_data.find(configuration_file_path) == config_data.end()){
        throw std::runtime_error("Can't find configuration path\n");
    }

    std::string file_path;

    //проходим по каждому файлу в конфигурации
    for (auto & file : config_data[configuration_path]) {
        file_path = file["path"];//извлечение значения по ключу "path"
        auto severity = file["severity"];
        for (auto &s : severity){
            //добавление потока с уровнем серезности
            this->add_file_stream(file_path, string_to_severity(s));
        }
    }

    //установка формата из конфигурации
    format = config_data["format"];

    config_file.close();

    return this;
}

logger_builder *client_logger_builder::clear()
{
    for (auto &file : files){
        file.second.clear();
    }
    files.clear();
    format = "";
    return this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(files,format);
}


