#include <Arduino.h>
#include "Serial_print.h"


/*
void debugPrint(const char msg[])
{
    Serial.print(msg);
    debug_print_alternative(msg);
}

void debugPrintln(const char msg[])
{
    Serial.println(msg);
    debug_println_alternative(msg);
}

void debugPrint(const __FlashStringHelper *msg)
{
    Serial.println(msg);
    char buffer_msg_str[100];

    snprintf(buffer_msg_str, 100, "%s", msg);
    debug_print_alternative(buffer_msg_str);
}

void debugPrintln(const __FlashStringHelper *msg)
{
    Serial.println(msg);

    char buffer_msg_str[100];
    snprintf(buffer_msg_str, 100, "%s\n", msg);
    debug_println_alternative(buffer_msg_str);
}

void debugPrint(int msg)
{
    Serial.print(msg);

    char buffer_msg_str[10];
    itoa(msg, buffer_msg_str, 10);
    debug_print_alternative(buffer_msg_str);
}

void debugPrintln(int msg)
{
    Serial.println(msg);

    char buffer_msg_str[10];
    itoa(msg, buffer_msg_str, 10);
    debug_println_alternative(buffer_msg_str);
}

void debugPrint(float msg, unsigned char decimalPlaces)
{
    Serial.print(msg, decimalPlaces);

    char buffer_msg_str[10];
    String(msg, decimalPlaces).toCharArray(buffer_msg_str, 10);
    debug_print_alternative(buffer_msg_str);
}

void debugPrintln(float msg, unsigned char decimalPlaces)
{
    Serial.println(msg, decimalPlaces);

    char buffer_msg_str[10];
    String(msg, decimalPlaces).toCharArray(buffer_msg_str, 10);
    debug_println_alternative(buffer_msg_str);
}
*/

////////////////////////////////////////////////////////////////////////////////

void serial_print_dashes(unsigned char num_of_dashes)
{
    for (unsigned char i = 0; i < num_of_dashes; i++)
    {
        Serial.print("-");
    }
    Serial.print(" ");
}

void serial_print_stars(unsigned char num_of_stars)
{
    for (unsigned char i = 0; i < num_of_stars; i++)
    {
        Serial.print("*");
    }
    Serial.print(" ");
}

