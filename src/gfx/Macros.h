#ifndef MACROS_H
#define MACROS_H

#include <ostream>
#include <sstream>

#include "../Logger.h"

#define PRINT_ADDRESS
//#define PRINT_ADDRESS " at " << &rhs <<

#define OSTREAM_FRIEND(klass) friend std::ostream& operator<<(std::ostream&, const klass&)

#define OSTREAM_IMPL(klass)                                                                        \
    inline std::ostream& operator<<(std::ostream& out, const klass& rhs)                           \
    {                                                                                              \
        out << "<" << #klass << PRINT_ADDRESS ">";                                                 \
        return out;                                                                                \
    }

#define OSTREAM_IMPL_1(klass, member)                                                              \
    inline std::ostream& operator<<(std::ostream& out, const klass& rhs)                           \
    {                                                                                              \
        out << "<" << #klass << ":" << rhs.member << PRINT_ADDRESS ">";                            \
        return out;                                                                                \
    }

#define OSTREAM_IMPL_2(klass, member1, member2)                                                    \
    inline std::ostream& operator<<(std::ostream& out, const klass& rhs)                           \
    {                                                                                              \
        out << "<" << #klass << ":" << rhs.member1 << ":" << rhs.member2 << PRINT_ADDRESS ">";     \
        return out;                                                                                \
    }

template <typename T>
std::string to_string(const T& x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

#define LOG_CREATED LOG_INFO("Created {}", to_string(*this))

#define LOG_RELEASED LOG_INFO("Released {}", to_string(*this))

#endif
