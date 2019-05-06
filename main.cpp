//
// Created by rsbat on 5/6/19.
//

#include <cstdint>
#include <sys/mman.h>
#include <cstring>
#include <iostream>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

constexpr size_t code_size = 97;
uint8_t code[] = {0x55, 0x48, 0x89, 0xe5, 0x53, 0x48, 0x89, 0xf8, 0x48, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x74, 0x30,
                  0x48, 0x3d, 0x01, 0x00, 0x00, 0x00, 0x74, 0x37, 0x48, 0xff, 0xc8, 0x50, 0x48, 0x89, 0xc7, 0xe8,
                  0xdc, 0xff, 0xff, 0xff, 0x48, 0x89, 0xc3, 0x58, 0x48, 0xff, 0xc8, 0x50, 0x48, 0x89, 0xc7, 0xe8,
                  0xcc, 0xff, 0xff, 0xff, 0x48, 0x01, 0xc3, 0x58, 0x48, 0x89, 0xd8, 0xe9, 0x1e, 0x00, 0x00, 0x00,
                  0x48, 0xb8, /**/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,/**/ 0xe9, 0x0f, 0x00, 0x00, 0x00, 0x48,
                  0xb8, /**/0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,/**/ 0xe9, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x5d, 0xc3};

struct jit_function {
    void* memory;
    bool initialized;

    explicit jit_function() : initialized(false) {
        memory = mmap(nullptr, code_size, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (memory == MAP_FAILED) {
            return;
        }

        memcpy(memory, code, code_size);

        if (mprotect(memory, code_size, PROT_READ | PROT_EXEC) == -1) {
            return;
        }

        initialized = true;
    }

    int64_t operator()(int64_t n) {
        if (initialized) {
            return reinterpret_cast<int64_t (*)(int64_t)>(memory)(n);
        } else {
            return 0;
        }
    }

    ~jit_function() {
        if (munmap(memory, code_size) == -1) {
            cerr << "Could not unmap memory: " << strerror(errno) << endl;
        }
    }
};

void print_usage() {
    cout << "Usage: os_jit [<value of f(0)> <value of f(1)>]" << endl;
}

int main(int argc, char* argv[]) {
    if (argc == 3) {
        try {
            *reinterpret_cast<uint64_t*>(code + 0x42) = std::stoul(argv[1]);
            *reinterpret_cast<uint64_t*>(code + 0x51) = std::stoul(argv[2]);
        } catch (std::logic_error&) {
            print_usage();
            return 0;
        }
    } else if (argc == 1) {
        cout << "No starting values given, calculating Fibonacci sequence" << endl;
    } else {
        print_usage();
        return 0;
    }

    jit_function function;
    if (!function.initialized) {
        cerr << "Could not init function: " << strerror(errno) << endl;
    }

    cout << "Enter number in sequence to calculate" << endl;
    int64_t n;
    cin >> n;

    int64_t res = function(n);
    cout << "Number in sequence #" << n << " is " << res << endl;

    return 0;
}