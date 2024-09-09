#include <stdbool.h>

// Struct for emuContext
typedef struct
{
	bool paused;
	bool running;
	uint64_t ticks;
} emuContext_t;

// Function prototypes
emuContext_t* getEmuContext();
