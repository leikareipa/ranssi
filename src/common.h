/*
 * 2019 Tarpeeksi Hyvae Soft /
 * ranssi
 *
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef NDEBUG
    #error "NDEBUG disables assertions. Assertions are required by design."
#endif
#include <cassert>

#define k_assert(condition, error_string) assert(condition && error_string)

#endif
