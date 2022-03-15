#pragma once

#include "ops.h"

struct _op {
	const char* name;
	int length;
};

void dbg_print_cpu_data(struct cpu*, u8);
void dbg_stack_view(struct cpu*);