#pragma once

#define LEAVE_IF_NOT_SUCCESS(r) \
    if (!NT_SUCCESS(r)) __leave;

#define LEAVE_WITH(s) \
    { s; __leave; }

#define LEAVE() __leave;