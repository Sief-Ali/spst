#include "logger.h"

#include <stddef.h>

#include "serial.h"

/* UART prefixes used by the project logger for each severity tag. */
static const char *log_tag_prefix[] =
{
    [LOG_BOOT]    = "[BOOT]",
    [LOG_EVENT]   = "[EVT]",
    [LOG_INFO]   = "[INFO]",
    [LOG_ERROR] = "[ERROR]",
};

/* Initializes the serial layer used by the logger. */
void Logger_Init(void)
{
    Serial_Init(9600);
}

/* 
  logging style:
    [TAG]:[message]
  example: 
    [BOOT]: System=boot

*/
/* Writes one formatted log line to the serial maintenance terminal. */
void Logger_Log(
    log_tag_t tag,
    const char *message)
{
    if ((tag > LOG_ERROR) || (message == NULL))
    {
        return;
    }

    // logs tag / type D:Debug I:Info W:Warning E:Error
    Serial_Write(log_tag_prefix[tag]);
    Serial_Write(": ");

    // finally the message contains code:message
    Serial_Write(message);

    Serial_Write("\r\n");
}
