#include <iostream>
#include <sstream>
#include <vector>

#include "../msgpack.hpp"
#include "flatjson/flatjson.hpp"

#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

enum DataType
{
    DataTypeBool,
    DataTypeBinary,
    DataTypeExt,
    DataTypeNull,
    DataTypeString,
    DataTypeMap,
    DataTypeArray,
    DataTypeTimestamp,
    DataTypeNumber,
};

typedef struct
{
    std::string name;
    DataType type;
    std::vector<uint8_t> data;
    bool bool_value;
    int64_t number_value;
    double number_value_float;
} test_data_t;

std::vector<test_data_t> extract_tests();

void print_vec(std::vector<uint8_t> &data)
{
    for (auto &e : data)
    {
        std::cout << std::hex << (uint16_t)e << " ";
    }
    std::cout << std::endl;
}

TEST_CASE("Test Vectors")
{
    auto tests = extract_tests();

    for (auto &t : tests)
    {
        MsgPack *reader;
        switch (t.type)
        {
        case DataTypeBool:
            reader = new MsgPack(t.data);
            REQUIRE(reader->objects[0]->m_bool == t.bool_value);
            break;
        case DataTypeNumber:
            reader = new MsgPack(t.data);
            if (reader->objects[0]->is_float32())
            {
                REQUIRE(reader->objects[0]->m_float32 == t.number_value_float);
            }
            else if (reader->objects[0]->is_float64())
            {
                REQUIRE(reader->objects[0]->m_float64 == t.number_value_float);
            }
            else if (reader->objects[0]->is_signed())
            {
                REQUIRE(reader->objects[0]->as_int64() == t.number_value);
            }
            else if (reader->objects[0]->is_unsigned())
            {
                REQUIRE(reader->objects[0]->as_uint64() == t.number_value);
            }
            break;
        default:
            break;
        }
    }
}

TEST_CASE("Test Zero Types")
{
    std::vector<unsigned char> msg = {0x00,
                                      0xcc, 0x00,
                                      0xcd, 0x00, 0x00,
                                      0xce, 0x00, 0x00, 0x00, 0x00,
                                      0xcf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0xd0, 0x00,
                                      0xd1, 0x00, 0x00,
                                      0xd2, 0x00, 0x00, 0x00, 0x00,
                                      0xd3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0xca, 0x00, 0x00, 0x00, 0x00,
                                      0xcb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    auto *reader = new MsgPack(msg);

    REQUIRE(reader->objects[0]->type == MsgpackType::POSITIVE_FIXINT);
    REQUIRE(reader->objects[1]->type == MsgpackType::UINT8);
    REQUIRE(reader->objects[2]->type == MsgpackType::UINT16);
    REQUIRE(reader->objects[3]->type == MsgpackType::UINT32);
    REQUIRE(reader->objects[4]->type == MsgpackType::UINT64);
    REQUIRE(reader->objects[5]->type == MsgpackType::INT8);
    REQUIRE(reader->objects[6]->type == MsgpackType::INT16);
    REQUIRE(reader->objects[7]->type == MsgpackType::INT32);
    REQUIRE(reader->objects[8]->type == MsgpackType::INT64);
    REQUIRE(reader->objects[9]->type == MsgpackType::FLOAT32);
    REQUIRE(reader->objects[10]->type == MsgpackType::FLOAT64);
}

uint8_t from_hex(std::string str)
{
    uint8_t x;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> x;
    return x;
}

std::vector<test_data_t> extract_tests()
{

    using namespace flatjson;

    // Tests will be stored in here
    std::vector<test_data_t> tests;

    std::ifstream t("./test_suite/dist/msgpack-test-suite.json");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string test_vectors_json = buffer.str();

    parser parser;
    flatjson::fjson json(test_vectors_json.c_str(), test_vectors_json.c_str() + test_vectors_json.size());

    assert(json.is_valid());
    assert(json.is_object());

    auto beg = json.begin();
    assert(beg->is_object());

    auto end = json.end();
    for (auto test = ++beg; test != end; ++test)
    {
        // std::cout << test->key() << ":" << test->type_name() << " -> " << test->value() << std::endl;
        // std::cout << "Processing " << test->key() << std::endl;

        // Get just this element
        char buffer[1024];
        std::stringstream ss(test->key().data());
        ss.get((char *)&buffer, test->key().size() + 1); // +1 for reasons
        std::string name = buffer;

        fjson subjson = json.at(test);
        auto subtest = subjson.begin();
        auto aend = subjson.end();
        for (++subtest; subtest != aend; ++subtest)
        {
            // std::cout << "  " << subtest->type_name() << " -> " << subtest->value() << std::endl;

            auto subjson = json.at(subtest);
            auto oit = subjson.begin();
            auto oend = subjson.end();

            DataType type = DataTypeNull;
            bool bool_value = false;
            int64_t number_value = -123456;
            double number_value_float = 0.12345;

            for (++oit; oit != oend; ++oit)
            {
                //   std::cout << "      " << oit->key() << ":" << oit->type_name() << " -> " << oit->value() << std::endl;

                if (oit->key() == "ext")
                {
                    type = DataTypeExt;
                }
                else if (oit->key() == "nil")
                {
                    type = DataTypeNull;
                }
                else if (oit->key() == "string")
                {
                    type = DataTypeString;
                }
                else if (oit->key() == "number")
                {
                    type = DataTypeNumber;
                    number_value = std::strtoul(oit->value().data(), nullptr, 10);
                    number_value_float = std::strtod(oit->value().data(), nullptr);
                    // std::cout << "      " << oit->key() << ":" << oit->type_name() << " -> " << oit->value() << std::endl;
                }
                else if (oit->key() == "timestamp")
                {
                    type = DataTypeTimestamp;
                }
                else if (oit->key() == "array")
                {
                    type = DataTypeArray;
                }
                else if (oit->key() == "map")
                {
                    type = DataTypeMap;
                }
                else if (oit->key() == "bool")
                {
                    type = DataTypeBool;
                    if (oit->value() == "true")
                    {
                        bool_value = true;
                    }
                    else
                    {
                        bool_value = false;
                    }
                }
                else if (oit->key() == "binary")
                {
                    type = DataTypeBinary;
                }
                else if (oit->key() == "msgpack")
                {
                    // This is an array of data strings

                    fjson arrayjson = json.at(oit);
                    auto arrayjson_element = arrayjson.begin();
                    auto arrayjson_end = arrayjson.end();
                    for (++arrayjson_element; arrayjson_element != arrayjson_end; ++arrayjson_element)
                    {
                        //std::cout << "  " << arrayjson_element->type_name() << " -> " << arrayjson_element->value() << std::endl;

                        // For some reason this ss includes all remaining data in the array
                        std::stringstream ss(arrayjson_element->value().data());

                        // Get just this element
                        char buffer[128 * 1024];
                        ss.get((char *)&buffer, arrayjson_element->value().size() + 1); // +1 for reasons

                        // Create a new stringstream for doing the data extraction
                        std::stringstream element(buffer);

                        // Turn the text in to data
                        std::vector<uint8_t> bytes;
                        std::string tmp;
                        while (getline(element, tmp, '-'))
                        {
                            bytes.push_back(std::strtoul(tmp.c_str(), nullptr, 16));
                        }

                        tests.push_back({name, type, bytes, bool_value, number_value, number_value_float});
                    }
                }
                else if (oit->key() == "bignum")
                {
                    // pass
                }
                else
                {
                    std::cout << "      " << oit->key() << ":" << oit->type_name() << " -> " << oit->value() << std::endl;
                }
            }
        }
    }

    return tests;
}