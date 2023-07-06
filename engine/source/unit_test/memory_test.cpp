#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv)
{
    std::cout << "Memory Test" << std::endl;

    {
        // allocator for integer values
        std::allocator<int> my_allocator;

        // allocate space for five ints
        int* arr = my_allocator.allocate(5);

        // construct arr[0] and arr[3]
        // my_allocator.construct(arr, 100); // no longer allowed in C++20
        arr[0] = 100; // do this instead
        arr[3] = 10;

        std::cout << arr[3] << std::endl;
        std::cout << arr[0] << std::endl;

        // deallocate space for five ints
        my_allocator.deallocate(arr, 5);
    }

    {
        // allocator for string values
        std::allocator<std::string> my_allocator;

        // allocate space for three strings
        std::string* str = my_allocator.allocate(3);

        // construct these 3 strings
        my_allocator.construct(str, "Geeks");
        my_allocator.construct(str + 1, "for");
        my_allocator.construct(str + 2, "Geeks");

        std::cout << str[0] << str[1] << str[2] << std::endl;;

        // destroy these 3 strings
        my_allocator.destroy(str);
        my_allocator.destroy(str + 1);
        my_allocator.destroy(str + 2);

        // deallocate space for 3 strings
        my_allocator.deallocate(str, 3);
    }

    return 0;
}
