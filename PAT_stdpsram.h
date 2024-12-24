// Copyright (c) 2023, Pourya Afshintabar (PAT). All rights reserved.
// This project is licensed under the BSD 3-Clause License, which can be found in the LICENSE file of this repository.
//
// Project Overview:
// This code demonstrates how to use the C++ Standard Library containers such as std::vector, std::list, std::map,
// and std::string with PSRAM support on the ESP32 microcontroller. PSRAM (Pseudo Static RAM) is an external memory
// that can be used to extend the available memory on ESP32 devices, particularly helpful for memory-intensive operations.
//
// In platformio.ini, add the following build flags to use the PSRAM:
// build_flags =
//   -DBOARD_HAS_PSRAM
//   -mfix-esp32-psram-cache-issue

#ifndef PAT_STDPSRAM_H
#define PAT_STDPSRAM_H

#include <Arduino.h>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include <memory>

///////////////////////////////////////////////////
// PSRAMAllocator: Custom allocator for PSRAM memory
// This allocator uses ps_malloc and free for memory allocation 
// and deallocation in PSRAM.
template <typename T>
class PSRAMAllocator {
public:
    using value_type = T;

    // Default constructor
    PSRAMAllocator() noexcept = default;

    // Copy constructor for different types
    template <typename U>
    PSRAMAllocator(const PSRAMAllocator<U>&) noexcept {}

    // Allocate memory for n objects of type T
    T* allocate(std::size_t n) {
        if (n > std::size_t(-1) / sizeof(T)) {
            throw std::bad_alloc();
        }
        // Using ps_malloc for PSRAM allocation
        T* ptr = static_cast<T*>(ps_malloc(n * sizeof(T)));
        if (!ptr) {
            throw std::bad_alloc();
        }
        return ptr;
    }

    // Deallocate memory for a single object of type T
    void deallocate(T* ptr, std::size_t) noexcept {
        if (ptr) {
            free(ptr);
        }
    }

    // Construct an object of type T at ptr with value
    void construct(T* ptr, const T& value) {
        new (ptr) T(value);
    }

    // Destroy an object of type T at ptr
    void destroy(T* ptr) noexcept {
        ptr->~T();
    }

    // Rebind allocator to another type
    template <typename U>
    struct rebind {
        using other = PSRAMAllocator<U>;
    };
};

///////////////////////////////////////////////////
// Namespace stdpsram: Defines containers using PSRAMAllocator
namespace stdpsram {
    // Vector with PSRAMAllocator
    template <typename T>
    using vector = std::vector<T, PSRAMAllocator<T>>;

    // List with PSRAMAllocator
    template <typename T>
    using list = std::list<T, PSRAMAllocator<T>>;

    // Map with PSRAMAllocator
    template <typename Key, typename Value>
    using map = std::map<Key, Value, std::less<Key>, PSRAMAllocator<std::pair<const Key, Value>>>;

    // String with PSRAMAllocator
    using string = std::basic_string<char, std::char_traits<char>, PSRAMAllocator<char>>;

    // Tuple (unchanged)
    template <typename... Types>
    using tuple = std::tuple<Types...>;
}

///////////////////////////////////////////////////
#endif // PAT_STDPSRAM_H

