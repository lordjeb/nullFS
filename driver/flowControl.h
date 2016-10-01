#pragma once


#define FUNCTION_EXIT goto function_exit;
#define FUNCTION_EXIT_WITH(s) { s; goto function_exit; }
#define TRY_EXIT goto try_exit;
#define TRY_EXIT_WITH(s) { s; goto try_exit; }
