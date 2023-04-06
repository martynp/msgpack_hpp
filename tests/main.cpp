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
    uint32_t map_length;
    int64_t number_value;
    uint64_t number_value_unsigned;
    double number_value_float;
    std::string string_value;
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
        case DataTypeMap:
            reader = new MsgPack(t.data);
            // TODO Map tests are recursive, need to refactor the map YML decoder in order to run these tests correctly
            for (auto e : reader->objects)
            {
                REQUIRE(e->is_map() == true);
            }
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
                REQUIRE(reader->objects[0]->as_uint64() == t.number_value_unsigned);
            }
            break;
        case DataTypeString:
            reader = new MsgPack(t.data);
            REQUIRE(reader->objects[0]->as_string() == t.string_value);
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

TEST_CASE("Complex Maps")
{
    /*
    {
        "a": 1,
        "b": 2,
        "c": {
            "ca": "abc"
        }
    }
    */

    std::vector<uint8_t> msg = {
        0xde, 0x00, 0x03, // Map of 3 elements
        0xa1, 0x61, 0x01, // Element 1
        0xa1, 0x62, 0x02, // Element 2
        0xa1, 0x63, 0xdf, 0x00, 0x00, 0x00, 0x01, 0xa2, 0x63, 0x61, 0xa3, 0x61, 0x62, 0x63 // Element 3 (Map)
    };
    auto *reader = new MsgPack(msg);

    REQUIRE(reader->objects[0]->type == MsgpackType::MAP);
    auto map = reader->objects[0]->as_str_map();
    REQUIRE(map["a"]->as_int32() == 1);
    REQUIRE(map["b"]->as_int32() == 2);
    REQUIRE(map["c"]->is_map());
    auto sub_map = map["c"]->as_str_map();
    REQUIRE(sub_map["ca"]->as_string() == "abc");

}

TEST_CASE("Arrays")
{
    std::vector<uint8_t> msg = {
        0x92, 0x00, 0x01, 
    };
    auto *reader = new MsgPack(msg);

    REQUIRE(reader->objects[0]->type == MsgpackType::ARRAY);
    auto array = reader->objects[0]->as_vector();
    REQUIRE(array[0]->as_int32() == 0);
    REQUIRE(array[1]->as_int32() == 1);

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
            uint32_t map_size = 10000;
            int64_t number_value = -123456;
            uint64_t number_value_unsigned = 123456;
            double number_value_float = 0.12345;
            std::string string_value = "foobarbaz";

            for (++oit; oit != oend; ++oit)
            {
                // std::cout << "      " << oit->key() << ":" << oit->type_name() << " -> " << oit->value() << std::endl;

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
                    char str_buffer[128 * 1024];
                    std::stringstream ss(oit->value().data());
                    ss.get((char *)&str_buffer, oit->value().size() + 1);
                    string_value = str_buffer;
                }
                else if (oit->key() == "number")
                {
                    type = DataTypeNumber;
                    number_value = std::strtol(oit->value().data(), nullptr, 10);
                    number_value_unsigned = std::strtoul(oit->value().data(), nullptr, 10);
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

                    /*
                    std::cout << "      " << oit->key() << ":" << oit->type_name() << " -> " << oit->value() << std::endl;

                    auto mapjson = json.at(oit);
                    auto mit = mapjson.begin();
                    auto mend = mapjson.end();

                    for (++mit; mit != mend; ++mit)
                    {
                        std::cout << "            " << mit->key() << ":" << mit->type_name() << " -> " << mit->value() << std::endl;
                    }
                    */
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
                        // std::cout << "  " << arrayjson_element->type_name() << " -> " << arrayjson_element->value() << std::endl;

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

                        tests.push_back({name, type, bytes, bool_value, map_size, number_value, number_value_unsigned, number_value_float, string_value});
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