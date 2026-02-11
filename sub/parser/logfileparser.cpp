#include "logfileparser.h"

#include <fstream>
#include <filesystem>
#include <array>
#include <algorithm>

LogFileParser::LogFileParser(const std::string &filename)
{
    if (!std::filesystem::exists(filename))
        throw std::runtime_error("Файл не найден.");

    size_t filesize = std::filesystem::file_size(filename);
    if (filesize == 0)
        throw std::runtime_error("Файл пуст.");

    parseData(filename, filesize);
}

std::map<uint64_t, mavlink_message_t> &LogFileParser::getMessagesByID(uint32_t msgid) noexcept
{
    return m_messages[msgid];
}

union Timestamp
{
    uint64_t val;
    std::array<char,8> bytes;
};

void LogFileParser::parseData(const std::string &name, size_t size)
{
    size_t readsize = 0;
    std::ifstream in(name, std::ios::binary);
    while (!in.eof() && !in.fail() && !in.bad() && readsize < size)
    {
        Timestamp timestamp;
        size_t timestampreadsize = 0;
        while (!in.eof() && !in.fail() && !in.bad() && readsize < size && timestampreadsize < sizeof(Timestamp))
        {
            size_t s = in.readsome(&timestamp.bytes[timestampreadsize], sizeof(Timestamp) - timestampreadsize);
            timestampreadsize+= s;
            readsize+= s;
        }

        if (timestampreadsize != sizeof(Timestamp))
            throw std::runtime_error("Файл повреждён или неполон.");

        if (static_cast<uint8_t>(timestamp.bytes[0]) == MAVLINK_STX ||
            static_cast<uint8_t>(timestamp.bytes[0]) == MAVLINK_STX_MAVLINK1)
            in.seekg(-8, std::ios::cur);

        std::reverse(timestamp.bytes.begin(), timestamp.bytes.end());

        char bytebuf;
        mavlink_message_t msg;
        while (in.readsome(&bytebuf, 1) == 1) // Чтение одного байта
        {
            readsize++;
            mavlink_status_t st;
            if (mavlink_parse_char(MAVLINK_COMM_1, static_cast<uint8_t>(bytebuf), &msg, &st) == 1) // Распознано MAVLink сообщение
            {
                m_messages[msg.msgid][timestamp.val] = std::move(msg);
                break;
            }
        }
    }
    in.close();
}
