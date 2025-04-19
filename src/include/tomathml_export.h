#pragma once

// Define TOMATHML_STATIC when building or linking against the static library version
#if defined(TOMATHML_STATIC)
    #define TOMATHML_API
#else
    // Platform-specific visibility handling
    #if defined _WIN32 || defined __CYGWIN__
        #ifdef TOMATHML_EXPORTS
            #define TOMATHML_API __declspec(dllexport)
        #else
            #define TOMATHML_API __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define TOMATHML_API __attribute__ ((visibility ("default")))
    #else
        #define TOMATHML_API
    #endif
#endif
