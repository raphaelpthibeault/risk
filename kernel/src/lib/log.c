#include <log.h>

void 
log_(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);

    write_serial_str(buf);

    va_end(args);
}

