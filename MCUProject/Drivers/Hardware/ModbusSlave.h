#ifndef __MODBUSSLAVE_H
#define __MODBUSSLAVE_H

//ModBus ������

#define ILLEGAL_FUNC 	0x01
#define ILLEGAL_ADDR 	0x02
#define ILLEGAL_VALE 	0x03
#define SLAVE_ERROR 	0x04
#define SLAVE_CONFM 	0x05
#define SLAVE_BUSY		0x06


void ModbusSlave(int p);//ModBusRTUЭ����մ���


#endif /*__MODBUSSLAVE_H*/


