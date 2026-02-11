#ifndef LOGFILEPARSER_H
#define LOGFILEPARSER_H

#include <string>
#include <map>
#include <stdint.h>
#include <ardupilotmega/mavlink.h>

class LogFileParser
{
public:
    explicit LogFileParser(const std::string &filename);

    /// @brief Получение списка сообщений с указанным идентификатором
    /// @param msgid - идентификатор соощения
    /// @return Список сообщений с указанным идентификатором
    std::map<uint64_t, mavlink_message_t> &getMessagesByID(uint32_t msgid) noexcept;

private:
    /// @brief Разбор данных из лог-файла
    /// @param name - имя файла
    /// @param size - размер файла
    void parseData(const std::string &name, size_t size);

    std::map<uint32_t, std::map<uint64_t, mavlink_message_t>> m_messages;   ///< Сообщения из лога
};

#endif // LOGFILEPARSER_H
