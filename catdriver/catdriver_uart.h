#pragma once
#include "catdriver.h"

#define UART_CH_ESP (FuriHalSerialIdUsart)
#define UART_CH_GPS (FuriHalSerialIdLpuart)

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

void catdriver_uart_init(Context* ctx);
void catdriver_uart_deinit(Context* ctx);
