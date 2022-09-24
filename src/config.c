#include <stdlib.h>

#include "include/config.h"

void Config_cleanup(Config *config) {
	free(config->logFile);
	free(config->deviceFile);
	config->logFile = NULL;
	config->deviceFile = NULL;
}
