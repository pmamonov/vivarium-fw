#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include "stm32f4xx.h"

#define STDIN_BUFFER_SIZE 1024

#ifdef __NEWLIB_STUBS
volatile uint8_t stdin_buffer[STDIN_BUFFER_SIZE];
volatile uint16_t stdin_buffer_in=0;
volatile uint16_t stdin_buffer_len=0;
volatile uint16_t stdin_buffer_out=0;
#else
extern volatile uint8_t stdin_buffer[];
extern volatile uint16_t stdin_buffer_in;
extern volatile uint16_t stdin_buffer_len;
#endif

uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len);
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);

