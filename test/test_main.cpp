/* Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "serial_io.h"


int main( int argc, char* argv[] ) {
    
    // global setup...
    if( loader_port_serial_init(0) != ESP_LOADER_SUCCESS ) {
        std::cout << "Serial initialization failed";
        return 0;
    }

    int result = Catch::Session().run( argc, argv );

    // global clean-up...
    loader_port_serial_deinit();

    return result;
}