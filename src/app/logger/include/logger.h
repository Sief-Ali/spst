#ifndef LOGGER_H
#define LOGGER_H

typedef enum
{
    LOG_BOOT,
    LOG_EVENT,
    LOG_INFO,
    LOG_ERROR
} log_tag_t;

/* Initializes the serial logger at the project baud rate. */
void Logger_Init(void);

/* Writes one formatted log line. Invalid tags or NULL messages are ignored. */
void Logger_Log(
    log_tag_t tag,
    const char *message);

#endif
