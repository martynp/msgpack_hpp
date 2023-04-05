#include <iostream>
#include <sstream>
#include <vector>

#include "../msgpack.hpp"

int main(void)
{
    std::vector<uint8_t> msg = {
        0xde, 0x00, 0x03,                                                                  // Map of 3 elements
        0xa1, 0x61, 0x01,                                                                  // Element 1
        0xa1, 0x62, 0x02,                                                                  // Element 2
        0xa1, 0x63, 0xdf, 0x00, 0x00, 0x00, 0x01, 0xa2, 0x63, 0x61, 0xa3, 0x61, 0x62, 0x63 // Element 3 (Map)
    };
    auto *reader = new MsgPack(msg);

    auto map = reader->objects[0]->as_str_map();
    std::cout << map["a"]->as_int32() << std::endl;

    delete reader;

    return 0;
}
