
#include "../msgpack.hpp"

#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

int Factorial(int number)
{
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
