#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <Arduino.h>
#include <Configs.h>

namespace debug
{
    template <bool B = DEBUG, typename T>
    void serial_println(const T str)
    {
        if constexpr (B)
        {
            Serial.println(str);
        }
    }

    template <bool B = DEBUG, typename T>
    void serial_print(const T str)
    {
        if constexpr (B)
        {
            Serial.print(str);
        }
    }

    template <bool B = DEBUG, typename Fn>
    void debug_fn(Fn fn)
    {
        if constexpr (B)
        {
            fn();
        }
    }

} // namespace debug

#endif