#ifndef __RDT3_H__
#define __RDT3_H__

#define MAX_SEND_TIMES 10
#define MAX_WAIT_TIME 1
#include "pak.h"
//rdt3.0 :停等机制，超时重传
bool stopWaitsend(packet&a, packet& b);
#endif