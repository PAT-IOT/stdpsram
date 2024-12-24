
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
//_____________________________________________________________________________________________________________________

#include <Arduino.h>
#include <PAT_stdpsram.h>

/// Prints the free heap and PSRAM memory to the serial console.
#define PRINT_FREE_HEAP_AND_PSRAM                                           \
      Serial.printf("line: %d --> free heap: %skb , free PSRAM: %skb \n",   \
                    __LINE__, String((ESP.getFreeHeap()) / 1000.0).c_str(), \
                    String((heap_caps_get_free_size(MALLOC_CAP_SPIRAM)) / 1000.0).c_str());

//_____________________________________________________________________________________________________________________
void setup()
{
      Serial.begin(115200);
      while (!Serial)
            ;
      //-----------------------------------------
      PRINT_FREE_HEAP_AND_PSRAM
      // Test std::vector with PSRAMAllocator
      stdpsram::vector<int>
          psramVector{1, 2, 3, 4, 5};
      Serial.println("Testing std::vector:");
      for (const auto &value : psramVector)
      {
            Serial.println(value);
      }
      //-----------------------------------------
      PRINT_FREE_HEAP_AND_PSRAM
      // Test std::list with PSRAMAllocator
      stdpsram::list<stdpsram::string> psramList{"Hello", "from", "PSRAM"};
      Serial.println("Testing std::list:");
      for (const auto &value : psramList)
      {
            Serial.println(value.c_str());
      }
      //-----------------------------------------
      PRINT_FREE_HEAP_AND_PSRAM
      // Test std::map with PSRAMAllocator
      stdpsram::map<int, stdpsram::string> psramMap;
      psramMap[1] = "One";
      psramMap[2] = "Two";
      psramMap[3] = "Three";
      Serial.println("Testing std::map:");
      for (const auto &pair : psramMap)
      {
            Serial.print(pair.first);
            Serial.print(": ");
            Serial.println(pair.second.c_str());
      }
      //-----------------------------------------
      PRINT_FREE_HEAP_AND_PSRAM
      // Test std::string with PSRAMAllocator
      stdpsram::string psramString = "Hello from PSRAM string!";
      Serial.println("Testing std::string:");
      Serial.println(psramString.c_str());
      //-----------------------------------------
      PRINT_FREE_HEAP_AND_PSRAM
      // Test std::tuple with PSRAMAllocator
      stdpsram::tuple<int, double, stdpsram::string> psramTuple = std::make_tuple(42, 3.14, "PSRAM Tuple");
      Serial.println("Testing std::tuple:");
      Serial.print("Int: ");
      Serial.println(std::get<0>(psramTuple));
      Serial.print("Double: ");
      Serial.println(std::get<1>(psramTuple), 2);
      Serial.print("String: ");
      Serial.println(std::get<2>(psramTuple).c_str());
      //-----------------------------------------
      PRINT_FREE_HEAP_AND_PSRAM
}

//_____________________________________________________________________________________________________________________
void loop()
{
      delay(1000);
}
