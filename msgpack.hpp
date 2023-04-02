#ifndef _MSGPACK_HPP_
#define _MSGPACK_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <iostream>

typedef enum e_MsgpackType
{
    POSITIVE_FIXINT,
    NEGATIVE_FIXINT,
    NIL,
    BOOL,
    BIN,
    EXT,
    FLOAT32,
    FLOAT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    INT8,
    INT16,
    INT32,
    INT64,
    STR,
    ARRAY,
    MAP,
} MsgpackType;

class MsgPackObj
{

public:
    bool is_nil() { return type == MsgpackType::NIL; }
    bool is_bool() { return type == MsgpackType::BOOL; }
    bool is_bin() { return type == MsgpackType::BIN; }
    bool is_ext() { return type == MsgpackType::EXT; }
    bool is_float32() { return type == MsgpackType::FLOAT32; }
    bool is_float64() { return type == MsgpackType::FLOAT64; }
    bool is_uint8() { return type == MsgpackType::UINT8 || type == MsgpackType::POSITIVE_FIXINT; }
    bool is_uint16() { return type == MsgpackType::UINT16; }
    bool is_uint32() { return type == MsgpackType::UINT32; }
    bool is_uint64() { return type == MsgpackType::UINT64; }
    bool is_int8() { return type == MsgpackType::INT8 || type == MsgpackType::POSITIVE_FIXINT || type == MsgpackType::NEGATIVE_FIXINT; }
    bool is_int16() { return type == MsgpackType::INT16; }
    bool is_int32() { return type == MsgpackType::INT32; }
    bool is_int64() { return type == MsgpackType::INT64; }
    bool is_str() { return type == MsgpackType::STR; }
    bool is_array() { return type == MsgpackType::ARRAY; }
    bool is_map() { return type == MsgpackType::MAP; }
    bool is_str_map() { return type == MsgpackType::MAP; }

    bool is_unsigned() { return type == MsgpackType::UINT16 || type == MsgpackType::UINT32 || type == MsgpackType::UINT64 || type == MsgpackType::UINT8; }
    bool is_signed() { return !is_unsigned(); }

    int32_t as_int32()
    {
        return as_int64();
    }

    int32_t as_uint32()
    {
        return as_uint64();
    }

    int64_t as_int64()
    {
        switch (type)
        {
        case MsgpackType::POSITIVE_FIXINT:
        case MsgpackType::NEGATIVE_FIXINT:
        case MsgpackType::INT8:
            return m_int8;
            break;
        case MsgpackType::UINT8:
            return m_uint8;
            break;
        case MsgpackType::INT16:
            return m_int16;
            break;
        case MsgpackType::UINT16:
            return m_uint16;
            break;
        case MsgpackType::INT32:
            return m_int32;
            break;
        case MsgpackType::UINT32:
            return m_uint32;
            break;
        case MsgpackType::INT64:
            return m_int64;
            break;
        case MsgpackType::UINT64:
            return m_uint64;
            break;
        default:
            return 0;
            break;
        }
    }

    uint64_t as_uint64()
    {
        switch (type)
        {
        case MsgpackType::POSITIVE_FIXINT:
        case MsgpackType::NEGATIVE_FIXINT:
        case MsgpackType::INT8:
            return m_int8;
            break;
        case MsgpackType::UINT8:
            return m_uint8;
            break;
        case MsgpackType::INT16:
            return m_int16;
            break;
        case MsgpackType::UINT16:
            return m_uint16;
            break;
        case MsgpackType::INT32:
            return m_int32;
            break;
        case MsgpackType::UINT32:
            return m_uint32;
            break;
        case MsgpackType::INT64:
            return m_int64;
            break;
        case MsgpackType::UINT64:
            return m_uint64;
            break;
        default:
            return 0;
            break;
        }
    }

    std::string as_string()
    {
        if (type == MsgpackType::STR)
        {
            return m_str;
        }
        return "";
    }

    std::unordered_map<std::string, std::shared_ptr<MsgPackObj>> as_str_map()
    {
        if (type == MsgpackType::MAP)
        {
            return m_map_string;
        }

        throw "That went wrong";
    }
    void to_raw(std::vector<char> &buffer);

    MsgpackType type;
    bool m_bool;
    std::shared_ptr<std::vector<unsigned char>> m_bin;
    // ext
    float m_float32;
    double m_float64;
    uint8_t m_uint8;
    uint16_t m_uint16;
    uint32_t m_uint32;
    uint64_t m_uint64;
    int8_t m_int8;
    int16_t m_int16;
    int32_t m_int32;
    int64_t m_int64;
    std::string m_str;
    std::vector<MsgPackObj> m_array;
    std::unordered_map<std::string, std::shared_ptr<MsgPackObj>> m_map_string;

    MsgPackObj()
    {
        type = MsgpackType::NIL;
    }

    MsgPackObj(bool value)
    {
        type = MsgpackType::BOOL;
        m_bool = value;
    }

    MsgPackObj(int8_t value, bool fixed_positive, bool fixed_negative)
    {
        if (fixed_positive)
        {
            type = MsgpackType::POSITIVE_FIXINT;
            m_int8 = value & 0x7f;  // Enforce 7 bit size
            m_uint8 = value & 0x7f; // Both are valid
        }
        else if (fixed_negative)
        {
            type = MsgpackType::NEGATIVE_FIXINT;
            m_int8 = value | 0xE0; // Enforce first three bits as 1's
        }
        else
        {
            type = MsgpackType::INT8;
            m_int8 = value;
        }
    }
    MsgPackObj(int16_t value)
    {
        type = MsgpackType::INT16;
        m_int16 = value;
    }

    MsgPackObj(int32_t value)
    {
        type = MsgpackType::INT32;
        m_int32 = value;
    }

    MsgPackObj(int64_t value)
    {
        type = MsgpackType::INT64;
        m_int64 = value;
    }

    MsgPackObj(uint8_t value)
    {

        type = MsgpackType::UINT8;
        m_uint8 = value;
    }

    MsgPackObj(uint16_t value)
    {
        type = MsgpackType::UINT16;
        m_uint16 = value;
    }

    MsgPackObj(uint32_t value)
    {
        type = MsgpackType::UINT32;
        m_uint32 = value;
    }

    MsgPackObj(uint64_t value)
    {
        type = MsgpackType::UINT64;
        m_uint64 = value;
    }

    MsgPackObj(float value)
    {
        type = MsgpackType::FLOAT32;
        m_float32 = value;
    }

    MsgPackObj(double value)
    {
        type = MsgpackType::FLOAT64;
        m_float64 = value;
    }

    MsgPackObj(std::string value)
    {
        type = MsgpackType::STR;
        m_str = value;
    }

    MsgPackObj(std::shared_ptr<std::vector<unsigned char>> value)
    {
        type = MsgpackType::BIN;
        m_bin = value;
    }

    MsgPackObj(std::unordered_map<std::string, std::shared_ptr<MsgPackObj>> value)
    {
        type = MsgpackType::MAP;
        for (const auto &n : value)
        {
            m_map_string[n.first] = n.second;
        }
    }

    ~MsgPackObj()
    {
        if (type == MsgpackType::MAP)
        {
            /*for (const auto &n : m_map_string)
            {
                delete n.second;
            }*/
        }
    }

    void print(bool new_line = true)
    {
        switch (type)
        {
        case MsgpackType::POSITIVE_FIXINT:
            std::cout << "POSITIVE_FIXINT(" << (uint16_t)m_int8 << ")";
            break;
        case MsgpackType::INT8:
            std::cout << "INT8(" << (uint16_t)m_int8 << ")";
            break;
        case MsgpackType::INT16:
            std::cout << "INT16(" << m_int16 << ")";
            break;
        case MsgpackType::INT32:
            std::cout << "INT32(" << m_int32 << ")";
            break;
        case MsgpackType::INT64:
            std::cout << "INT64(" << m_int64 << ")";
            break;
        case MsgpackType::UINT8:
            std::cout << "UINT8(" << (uint16_t)m_uint8 << ")";
            break;
        case MsgpackType::UINT16:
            std::cout << "UINT16(" << m_uint16 << ")";
            break;
        case MsgpackType::UINT32:
            std::cout << "UINT32(" << m_uint32 << ")";
            break;
        case MsgpackType::UINT64:
            std::cout << "UINT64(" << m_uint64 << ")";
            break;
        case MsgpackType::FLOAT32:
            std::cout << "FLOAT32(" << m_float32 << ")";
            break;
        case MsgpackType::FLOAT64:
            std::cout << "FLOAT64(" << m_float64 << ")";
            break;
        case MsgpackType::BIN:
            std::cout << "BIN(";
            for (const auto &n : *m_bin)
            {
                std::cout << "0x" << std::hex << (int)n << std::dec << ",";
            }

            std::cout << ")";
            break;
        case MsgpackType::STR:
            std::cout << "STR(" << m_str << ")";
            break;
        case MsgpackType::MAP:
            std::cout << "MAP(";
            for (const auto &n : m_map_string)
            {
                std::cout << n.first << " : ";
                n.second->print(false);
                std::cout << ", ";
            }
            std::cout << ")";
            break;
        default:
            std::cout << "??" << type;
            break;
        }

        if (new_line)
        {
            std::cout << std::endl;
        }
    }
};

class MsgPack
{
private:
    bool m_little_endian;

public:
    std::vector<std::shared_ptr<MsgPackObj>> objects;
    size_t consumed = 0;

    MsgPack(std::vector<unsigned char> raw, int limit = -1)
    {

        char num = 1;
        if (*(char *)&num == 1)
        {
            m_little_endian = true;
        }
        else
        {
            m_little_endian = false;
        }

        size_t current = 0;
        while (current < raw.size())
        {
            if ((uint8_t)raw[current] <= 0x7f)
            {
                objects.push_back(std::make_shared<MsgPackObj>((uint8_t)raw[current], true, false));
                current += 1;
            }

            else if (raw[current] == 0xc0) // NIL
            {
                objects.push_back(std::make_shared<MsgPackObj>());
                current += 1;
            }
            else if (raw[current] == 0xc2) // FALSE
            {
                objects.push_back(std::make_shared<MsgPackObj>(false));
                current += 1;
            }
            else if (raw[current] == 0xc3) // TRUE
            {
                objects.push_back(std::make_shared<MsgPackObj>(true));
                current += 1;
            }
            else if (raw[current] == 0xc4) // BIN8
            {
                if (current + 1 >= raw.size())
                {
                    // Exception
                }
                uint8_t size = (uint8_t)raw[current + 1];

                if (current + 1 + size >= raw.size())
                {
                    // Exception
                }
                std::cout << (int)size << std::endl;
                std::shared_ptr<std::vector<unsigned char>> value = std::make_shared<std::vector<unsigned char>>();
                value->reserve(size);
                for (int ind = 0; ind < size; ind++)
                {
                    value->push_back((unsigned char)raw[current + 2 + ind]);
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 2 + size;
            }
            else if (raw[current] == 0xca) // FLOAT
            {
                check_size(current, 4, raw.size());

                float value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 4];
                    *(v_ptr + 1) = raw[current + 3];
                    *(v_ptr + 2) = raw[current + 2];
                    *(v_ptr + 3) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                    *(v_ptr + 2) = raw[current + 3];
                    *(v_ptr + 3) = raw[current + 4];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 5;
            }
            else if (raw[current] == 0xcb) // DOUBLE
            {
                check_size(current, 8, raw.size());

                double value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 8];
                    *(v_ptr + 1) = raw[current + 7];
                    *(v_ptr + 2) = raw[current + 6];
                    *(v_ptr + 3) = raw[current + 5];
                    *(v_ptr + 4) = raw[current + 4];
                    *(v_ptr + 5) = raw[current + 3];
                    *(v_ptr + 6) = raw[current + 2];
                    *(v_ptr + 7) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                    *(v_ptr + 2) = raw[current + 3];
                    *(v_ptr + 3) = raw[current + 4];
                    *(v_ptr + 4) = raw[current + 5];
                    *(v_ptr + 5) = raw[current + 6];
                    *(v_ptr + 6) = raw[current + 7];
                    *(v_ptr + 7) = raw[current + 8];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 9;
            }
            else if (raw[current] == 0xcc) // UINT8
            {
                if (current + 1 >= raw.size())
                {
                    // Exception
                }
                objects.push_back(std::make_shared<MsgPackObj>((uint8_t)raw[current + 1]));
                current += 2;
            }
            else if (raw[current] == 0xcd) // UINT16
            {

                if (current + 2 >= raw.size())
                {
                    // Exception
                }

                uint16_t value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 2];
                    *(v_ptr + 1) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 3;
            }
            else if (raw[current] == 0xce) // UINT32
            {

                if (current + 4 >= raw.size())
                {
                    // Exception
                }

                uint32_t value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 4];
                    *(v_ptr + 1) = raw[current + 3];
                    *(v_ptr + 2) = raw[current + 2];
                    *(v_ptr + 3) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                    *(v_ptr + 2) = raw[current + 3];
                    *(v_ptr + 3) = raw[current + 4];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 5;
            }
            else if (raw[current] == 0xcf) // UINT64
            {
                check_size(current, 8, raw.size());

                uint64_t value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 8];
                    *(v_ptr + 1) = raw[current + 7];
                    *(v_ptr + 2) = raw[current + 6];
                    *(v_ptr + 3) = raw[current + 5];
                    *(v_ptr + 4) = raw[current + 4];
                    *(v_ptr + 5) = raw[current + 3];
                    *(v_ptr + 6) = raw[current + 2];
                    *(v_ptr + 7) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                    *(v_ptr + 2) = raw[current + 3];
                    *(v_ptr + 3) = raw[current + 4];
                    *(v_ptr + 4) = raw[current + 5];
                    *(v_ptr + 5) = raw[current + 6];
                    *(v_ptr + 6) = raw[current + 7];
                    *(v_ptr + 7) = raw[current + 8];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 9;
            }

            else if (raw[current] == 0xd0) // INT8
            {
                check_size(current, 1, raw.size());
                objects.push_back(std::make_shared<MsgPackObj>((int8_t)raw[current + 1], false, false));
                current += 2;
            }
            else if (raw[current] == 0xd1) // INT16
            {

                check_size(current, 2, raw.size());

                int16_t value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 2];
                    *(v_ptr + 1) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 3;
            }
            else if (raw[current] == 0xd2) // INT32
            {

                check_size(current, 4, raw.size());

                int32_t value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 4];
                    *(v_ptr + 1) = raw[current + 3];
                    *(v_ptr + 2) = raw[current + 2];
                    *(v_ptr + 3) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                    *(v_ptr + 2) = raw[current + 3];
                    *(v_ptr + 3) = raw[current + 4];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 5;
            }
            else if (raw[current] == 0xd3) // INT64
            {
                check_size(current, 8, raw.size());

                int64_t value;
                uint8_t *v_ptr = (uint8_t *)&value;
                if (m_little_endian)
                {
                    *(v_ptr) = raw[current + 8];
                    *(v_ptr + 1) = raw[current + 7];
                    *(v_ptr + 2) = raw[current + 6];
                    *(v_ptr + 3) = raw[current + 5];
                    *(v_ptr + 4) = raw[current + 4];
                    *(v_ptr + 5) = raw[current + 3];
                    *(v_ptr + 6) = raw[current + 2];
                    *(v_ptr + 7) = raw[current + 1];
                }
                else
                {
                    *(v_ptr) = raw[current + 1];
                    *(v_ptr + 1) = raw[current + 2];
                    *(v_ptr + 2) = raw[current + 3];
                    *(v_ptr + 3) = raw[current + 4];
                    *(v_ptr + 4) = raw[current + 5];
                    *(v_ptr + 5) = raw[current + 6];
                    *(v_ptr + 6) = raw[current + 7];
                    *(v_ptr + 7) = raw[current + 8];
                }
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 9;
            }

            else if ((raw[current] & 0xE0) == 0xA0) // Fixed string
            {

                uint8_t size = raw[current] & 0x1F;
                if (current + size >= raw.size())
                {
                    // Exception
                }
                std::string value;
                value.reserve(size);

                for (size_t index = current + 1; index <= current + size; index++)
                    value += (char)raw[index];
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 1 + size;
            }
            else if (raw[current] == 0xd9) // STR8
            {
                if (current + 1 >= raw.size())
                {
                    // Exception
                }
                uint8_t size = (uint8_t)raw[current + 1];

                if (current + 1 + size >= raw.size())
                {
                    // Exception
                }
                std::string value;
                value.reserve(size);

                for (size_t index = current + 2; index <= current + 1 + size; index++)
                    value += (char)raw[index];
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 2 + size;
            }
            else if (raw[current] == 0xda) // STR16
            {
                if (current + 2 >= raw.size())
                {
                    // Exception
                }
                uint16_t size = (((uint16_t)raw[current + 1]) << 8) | (uint16_t)raw[current + 2];

                if (current + 2 + size >= raw.size())
                {
                    // Exception
                }
                std::string value;
                value.reserve(size);

                for (size_t index = current + 3; index <= current + 2 + size; index++)
                    value += (char)raw[index];
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 3 + size;
            }
            else if (raw[current] == 0xdb) // STR32
            {
                if (current + 4 >= raw.size())
                {
                    // Exception
                }
                uint32_t size = (((uint32_t)raw[current + 1]) << 24) | (((uint32_t)raw[current + 2]) << 16) | (((uint32_t)raw[current + 3]) << 8) | (uint32_t)raw[current + 4];

                if (current + 4 + size >= raw.size())
                {
                    // Exception
                }
                std::string value;
                value.reserve(size);

                for (size_t index = current + 5; index <= current + 4 + size; index++)
                    value += (char)raw[index];
                objects.push_back(std::make_shared<MsgPackObj>(value));
                current += 5 + size;
            }
            else if ((raw[current] & 0xF0) == 0x80) // FIXMAP
            {
                uint8_t elements = raw[current] & 0x0F;
                std::unordered_map<std::string, std::shared_ptr<MsgPackObj>> pairs;
                size_t consumed = 0;

                // Get string:object pair
                for (int object_index = 0; object_index < elements; object_index++)
                {
                    // Get a pair from the vector
                    MsgPack *pair = new MsgPack(std::vector<unsigned char>(raw.begin() + current + 1 + consumed, raw.end()), 2);

                    if (pair->objects.size() != 2)
                    {
                        std::cout << "Expected 2" << std::endl;
                    }

                    std::shared_ptr<MsgPackObj> value = pair->objects[1];
                    pairs[pair->objects[0]->as_string()] = value;

                    consumed += pair->consumed; // Keep track of how many bytes from the input buffer we have used

                    delete pair;
                }

                objects.push_back(std::make_shared<MsgPackObj>(pairs));
                current += 1 + consumed;
            }
            else if (raw[current] >= 0xe0 || raw[current] <= 0xff) // NEGATIVE FIXINT
            {
                objects.push_back(std::make_shared<MsgPackObj>((int8_t)raw[current], false, true));
                current += 1;
            }
            else
            {
                std::cout << "Unknown: " << std::hex << (int)raw[current] << std::endl;
                current += 1;
            }

            if (limit > 0 && (size_t)limit == objects.size())
                break;
        }

        consumed = current;

        /*
        for (size_t i = 0; i < objects.size(); i++)
        {
            objects[i]->print(true);
        }
        */
    }

    ~MsgPack()
    {
    }

    template <typename T>
    T get(std::string path);

    std::unordered_map<std::string, std::shared_ptr<MsgPackObj>> as_str_map()
    {

        if (objects.size() == 0)
        {
            // Exception
        }

        if (!objects[0]->is_str_map())
        {
            // Exception
        }

        return objects[0]->as_str_map();
    }

private:
    bool check_size(size_t current, size_t required, size_t size)
    {
        return true;
    }
};

template <>
std::string MsgPack::get<std::string>(std::string path)
{
    for (const auto &n : objects)
    {
        if (n->is_str_map())
        {
            for (const auto &m : n->as_str_map())
            {
                if (m.first == path)
                {
                    if (m.second->is_str())
                        return m.second->as_string();
                    else
                    {
                        // Exception...
                        return "";
                    }
                }
            }
        }
    }

    return "";
}

template <>
uint32_t MsgPack::get<uint32_t>(std::string path)
{

    for (const auto &n : objects)
    {
        if (n->is_str_map())
        {
            for (const auto &m : n->as_str_map())
            {
                if (m.first == path)
                {
                    if (m.second->is_int32())
                        return m.second->as_int32();
                    else
                    {
                        // Exception...
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}

#endif