#include "portaudio.h"
#include "record.c"
#include "menu.h"
#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/StreamingClient.h>


int         recordStream(bandData band);
