#pragma once


#define NULLFS extern "C"

#define FUNCTION_EXIT goto function_exit;
#define FUNCTION_EXIT_WITH(s) s; goto function_exit;
