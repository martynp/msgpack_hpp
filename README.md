
## Basic Usage

For a JSON type message structure:

``` json
{
    "hello": "world",
    "arr": [0, 1, 2, 3, 4, 5],
    "records": [
        {
            "name": "Bob",
            "location": "home"
        },
        {
            "name": "Fred",
            "location": "work"
        }
    ]
}
```

The following example code

``` c++
#include <iostream>
#include <vector>

#include "../msgpack.hpp"

int main(void)
{

    std::vector<uint8_t> msg = {
        0x83, 0xa5, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0xa5, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0xa3, 0x61, 0x72, 0x72, 0x96, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xa7, 0x72, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x73, 0x92, 0x82, 0xa4, 0x6e, 0x61, 0x6d, 0x65, 0xa3, 0x42, 0x6f, 0x62, 0xa8, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0xa4, 0x68, 0x6f, 0x6d, 0x65, 0x82, 0xa4, 0x6e, 0x61, 0x6d, 0x65, 0xa4, 0x46, 0x72, 0x65, 0x64, 0xa8, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0xa4, 0x77, 0x6f, 0x72, 0x6b};

    // Parse the file
    auto *reader = new MsgPack(msg);

    // Extract the content (map type structure)
    auto map = reader->objects[0]->as_str_map();

    std::cout << "hello: " << map["hello"]->as_string() << std::endl;

    std::cout << "array: ";
    for (auto &value : map["arr"]->as_vector())
    {
        std::cout << value->as_int32() << ' ';
    }
    std::cout << std::endl;


    auto records = map["records"]->as_str_map();
    for (auto &r : records)
    {
        std::cout << "Record: " << std::endl;
        auto record = r->as_str_map();
        std::cout << "  Name: " << record["name"]->as_string() << std::endl;
        std::cout << "  Location: " << record["location"]->as_string() << std::endl;
    }

    delete reader;

    return 0;
}
```

Generates the output:

```
hello: world
array: 0 1 2 3 4 5 
Record: 
  Name: Bob
  Location: home
Record: 
  Name: Fred
  Location: work
```