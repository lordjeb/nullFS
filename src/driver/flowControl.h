#pragma once

#define FUNCTION_EXIT goto function_exit
#define FUNCTION_EXIT_IF_NOT_SUCCESS(r) \
    if (!NT_SUCCESS(r))                 \
    {                                   \
        FUNCTION_EXIT;                  \
    }
#define FUNCTION_EXIT_WITH(s) \
    {                         \
        s;                    \
        FUNCTION_EXIT;        \
    }
#define TRY_EXIT goto try_exit
#define TRY_EXIT_WITH(s) \
    {                    \
        s;               \
        TRY_EXIT;        \
    }
