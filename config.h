#ifndef __NSENSOR_CONFIG__
#define __NSENSOR_CONFIG__

#define PIPE_FOR_REQUEST_ID                     0x0000000001LL
#define WORKSCHEDULER_BROASTCASTING_CYCLE_TIME  10000UL
#define PIPE                                    0xE8E8F0F0E1LL // địa chỉ thu của server
#define ROUTER_ID								1		   // router id
#define MAX_CLIENT_NUMBER						50
#define DEBUG									1

//PROTOCOL DEFINE
#define PROTOCOL_REQUEST_TYPE					1
#define PROTOCOL_ACCEPT_TYPE					2
#define PROTOCOL_SENSOR_TYPE					3
#define PROTOCOL_DEVICE_TYPE					4

#endif