#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <iostream>
#include <string>
#include <Windows.h>

#define BG_BLACK "\x1b[40m"
#define BG_GREEN "\x1b[102m"
#define BG_BLUE "\x1b[104m"
#define BG_RED "\x1b[101m"

inline void Print(std::string str )   { std::cout << str; }
inline void Print(char ch)            { std::cout << ch; }
inline void Print(char* str)          { std::cout << str; }
inline void Print(const char* str)    { std::cout << str; }
inline void Print(int i)              { std::cout << i; }

inline void PrintLn(std::string str ) { std::cout << str << "\n"; }
inline void PrintLn(char ch)           { std::cout << ch << "\n"; }
inline void PrintLn(char* str)         { std::cout << str << "\n"; }
inline void PrintLn(const char* str)   { std::cout << str << "\n"; }
inline void PrintLn(int i)             { std::cout << i << "\n"; }

inline void SetColor(const char* color, const char* bgColor)
{
    Print(color);
    Print(bgColor);
}

// Math
inline void Clamp(int &value, int min, int max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
}

inline void SelfNormalize(sf::Vector2i &v)
{
    float norm = sqrt(v.x * v.x + v.y * v.y);
    if (norm != 0.f)
        v /= (int)norm;
}

#endif