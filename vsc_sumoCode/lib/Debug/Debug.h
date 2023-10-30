#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <Arduino.h>
#include <Configs.h>

namespace debug
{
    template<bool B = DEBUG>
    void serial_println(const String& str){
        if constexpr (B){
            Serial.println(str);
        }
    }

    template<bool B = DEBUG>
    void serial_print(const String& str){
        if constexpr (B){
            Serial.print(str);
        }
    }

} // namespace debug


#endif