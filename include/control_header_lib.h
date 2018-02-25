#ifndef CONTROL_HANDLER_LIB_H_
#define CONTROL_HANDLER_LIB_H_

#define CNTRL_HEADER_SIZE 8
#define CNTRL_RESP_HEADER_SIZE 8

#define PACKET_USING_STRUCT // Comment this out to use alternate packet crafting technique

#ifdef PACKET_USING_STRUCT
    struct __attribute__((__packed__)) CONTROL_HEADER
    {
        uint32_t dest_ip_addr;
        uint8_t control_code;
        uint8_t response_time;
        uint16_t payload_len;
    };
    struct __attribute__((__packed__)) CONTROL_RESPONSE_HEADER
    {
        uint32_t controller_ip_addr;
        uint8_t control_code;
        uint8_t response_code;
        uint16_t payload_len;
    };
    struct __attribute__((__packed__)) ROUTER_DATA
    {
        uint16_t id;
        uint16_t port;
        uint16_t data_port;
        uint16_t cost;
        uint32_t ip;
    }router[5];
    struct __attribute__((__packed__)) CONTROL_PAYLOAD_INIT
    {
        uint16_t no_of_routers;
        uint16_t update_interval;
        struct ROUTER_DATA router[5];
    };
    struct __attribute__((__packed__)) ROUTER_UPDATE
    {
       uint32_t ip;
       uint16_t port;
       uint16_t padding;
       uint16_t id;
       uint16_t cost;
    };
    struct __attribute__((__packed__)) INIT_RESPONSE_PAYLOAD
    {
      uint16_t update_fields;
      uint16_t port;
      uint32_t ip;
      struct ROUTER_UPDATE router[5];
    }temp1;
    struct __attribute__((__packed__)) RESPONSE_PAYLOAD
    {
      uint16_t id1;
      uint16_t padding;
      uint16_t id;
      uint16_t cost;
      uint16_t old_cost;
    };
    struct __attribute__((__packed__)) UPDATE_RESPONSE_PAYLOAD
    {
      uint32_t ip;
      uint16_t port;
      uint16_t padding;
      uint16_t id;
      uint16_t cost;
    };
    struct __attribute__((__packed__)) UPDATE_VECTOR
    {
      uint16_t update_fields;
      uint16_t port;
      uint32_t ip;
      struct UPDATE_RESPONSE_PAYLOAD temp[5];
    };
    struct __attribute__((__packed__)) DATA_PACKET
    {
      uint32_t ip;
      uint8_t tid;
      uint8_t ttl;
      uint16_t seq_num;
      uint32_t padding;
    };
    struct RESPONSE_PAYLOAD route[5];
#endif

char* create_response_header(int sock_index, uint8_t control_code, uint8_t response_code, uint16_t payload_len);

#endif
