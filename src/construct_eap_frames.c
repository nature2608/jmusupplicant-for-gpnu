// 
//=========================================================================
//
//  Filename: construct_frames.c
//  
//  Description:
//
//    Version:  1.0
//    Created:  04/30/2018 10:08:13 P.M.
//    Revision:  none
//    Compiler:  gcc
//
//    Author: codingstory, codingstory.tw@gmail.com
//    
//    1、构造 ethernet frame 头部数据
//    2、构造 EAPOL START frame
//    3、构造 EAP RESPONSE IDENTITY frame
//    4、构造 EAP RESPONSE MD5 CHALLENGE frame 
//    5、构造 heartbeat frame  
//  
//=========================================================================
//

#include "construct_eap_frames.h"

extern NIC_STRUCT nic_info;
extern int errno;
static uint32_t net_order_ip;
static uint32_t net_order_netmask;
static uint32_t net_order_gateway;
static struct ether_header *ethernet_header;


//
//------function----------------------------------------------------------
// name: 
//    construct_ethernet_frame_header
// description:
//    构造 Ethernet frame header
//    Data Fields
//      u_char 	  ether_dhost [6]
// 	          Destination MAC address.
//      u_char 	  ether_shost [6]
// 	          Source MAC address.
//      u_short   ether_type
// 	          Protocol type.
//
//------------------------------------------------------------------------
//
static void construct_ethernet_frame_header(uint8_t *packet, const uint8_t *src_MAC, const uint8_t *dst_MAC, const uint16_t ether_type)
{
  uint8_t eapol_ethernet_header[ETHERNET_HEADER_LENGTH];
  struct ether_header *ethernet_header = (struct ether_header*)eapol_ethernet_header;
//  const uint8_t ethernet_type[] = {0x88, 0x8e};

  memcpy(ethernet_header->ether_dhost, dst_MAC, ETHER_ADDR_LEN);  // filling FujianSt_00:00:03 MAC address to ether header struct
  memcpy(ethernet_header->ether_shost, src_MAC, ETHER_ADDR_LEN); // filling current host network interface card MAC address to ether header struct
  ethernet_header->ether_type = ether_type; // filling Ethernet type to ether header struct

  memcpy(packet, eapol_ethernet_header, ETHERNET_HEADER_LENGTH); // filling Ethernet header to eapol start packet

  return;
}

//
//------function----------------------------------------------------------
// name: 
//    construct_eapol_start_frame
// description:
//    构造 EAPOL Start frame。
//    抓包分析Ruijie客户端发送给服务器的,用于Ruijie认证第一
//    步的 EAPOL Start frame 格式和内容。用代码仿造出一个几
//    乎一模一样(服务器不验证的字段可以不改)的 EAPOL Start 
//    frame。
//
//    函数中使用了大量的 const value。这些 value 有一些代表
//    认证信息在 EAPOL Start frame 中的起始位置, 例如 ipaddr
//    的起始位置, netmask 的起始位置等等。还有一些代表认证信
//    息的 size, 用于 memcpy() 。
//
//    ethernet_padding[] 和 ethernet_trailer[]是从抓到的 frame 中复制来的。
//    一些关键的认证数据嵌在这两个数组中,需要用代码去替换。
//------------------------------------------------------------------------
//

void construct_eapol_start_frame(const uint8_t *frame)
{
  const uint16_t EAPOL_START_PACKET_LENGTH  = 524;  // send to server packet length
  extern uint8_t eapol_start[];
  const uint8_t eapol_version[] = {0x01};
  const uint8_t eapol_type[] = {0x01}; // type: eapol start
  const uint8_t eapol_start_dst_MAC[] = {0x01, 0xd0, 0xf8, 0x00, 0x00, 0x03};
  const uint8_t eapol_frame_length[] = {0x00, 0x00};
  const uint8_t ethernet_padding[] = {0xff, 0xff, 0x37, 0x77, 0x7f, 0xff, /* ....7w.. */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0x53, 0xfa, /* ......S. */
0xc7, 0x00, 0x00, 0x13, 0x11, 0x38, 0x30, 0x32, /* .....802 */
0x31, 0x78, 0x2e, 0x65, 0x78, 0x65, 0x00, 0x00, /* 1x.exe.. */
0x00, 0x00, 0x00, 0x00
    };
    const uint8_t ethernet_trailer[] = {0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0x01, /* ........ */
0x02, 0x00, 0x00, 0x00, 0x13, 0x11, 0x01, 0xb4, /* ........ */
0x1a, 0x0c, 0x00, 0x00, 0x13, 0x11, 0x18, 0x06, /* ........ */
0x00, 0x00, 0x00, 0x01, 0x1a, 0x0e, 0x00, 0x00, /* ........ */
0x13, 0x11, 0x2d, 0x08, 0x1c, 0x39, 0x47, 0x9a, /* ..-..9G. */
0xc1, 0xeb, 0x1a, 0x08, 0x00, 0x00, 0x13, 0x11, /* ........ */
0x2f, 0x02, 0x1a, 0x08, 0x00, 0x00, 0x13, 0x11, /* /....... */
0x76, 0x02, 0x1a, 0x09, 0x00, 0x00, 0x13, 0x11, /* v....... */
0x35, 0x03, 0x03, 0x1a, 0x18, 0x00, 0x00, 0x13, /* 5....... */
0x11, 0x36, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, /* .6...... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x1a, 0x18, 0x00, 0x00, 0x13, /* ........ */
0x11, 0x38, 0x12, 0xfe, 0x80, 0x00, 0x00, 0x00, /* .8...... */
0x00, 0x00, 0x00, 0x35, 0xec, 0xa9, 0xaa, 0xf1, /* ...5.... */
0xa2, 0x64, 0xf7, 0x1a, 0x18, 0x00, 0x00, 0x13, /* .d...... */
0x11, 0x4e, 0x12, 0x20, 0x01, 0x0d, 0xa8, 0x20, /* .N. ...  */
0x06, 0xb5, 0x0a, 0xe1, 0x65, 0x84, 0xaa, 0xf2, /* ....e... */
0x07, 0x03, 0x46, 0x1a, 0x88, 0x00, 0x00, 0x13, /* ..F..... */
0x11, 0x4d, 0x82, 0x63, 0x65, 0x64, 0x66, 0x65, /* .M.cedfe */
0x62, 0x31, 0x64, 0x37, 0x66, 0x37, 0x31, 0x34, /* b1d7f714 */
0x35, 0x34, 0x36, 0x65, 0x35, 0x37, 0x63, 0x61, /* 546e57ca */
0x32, 0x33, 0x62, 0x35, 0x36, 0x65, 0x31, 0x38, /* 23b56e18 */
0x38, 0x38, 0x62, 0x31, 0x63, 0x35, 0x37, 0x36, /* 88b1c576 */
0x37, 0x38, 0x30, 0x62, 0x36, 0x38, 0x64, 0x61, /* 780b68da */
0x32, 0x37, 0x36, 0x34, 0x62, 0x32, 0x64, 0x61, /* 2764b2da */
0x39, 0x62, 0x61, 0x64, 0x66, 0x36, 0x38, 0x66, /* 9badf68f */
0x65, 0x37, 0x39, 0x31, 0x33, 0x30, 0x37, 0x34, /* e7913074 */
0x30, 0x39, 0x37, 0x35, 0x39, 0x36, 0x35, 0x32, /* 09759652 */
0x33, 0x32, 0x63, 0x35, 0x61, 0x35, 0x65, 0x33, /* 32c5a5e3 */
0x36, 0x62, 0x30, 0x30, 0x63, 0x30, 0x39, 0x34, /* 6b00c094 */
0x30, 0x31, 0x64, 0x30, 0x66, 0x64, 0x38, 0x31, /* 01d0fd81 */
0x31, 0x37, 0x39, 0x61, 0x33, 0x31, 0x38, 0x62, /* 179a318b */
0x36, 0x36, 0x35, 0x38, 0x37, 0x66, 0x66, 0x39, /* 66587ff9 */
0x65, 0x34, 0x39, 0x61, 0x61, 0x39, 0x38, 0x66, /* e49aa98f */
0x35, 0x64, 0x30, 0x1a, 0x28, 0x00, 0x00, 0x13, /* 5d0.(... */
0x11, 0x39, 0x22, 0xd1, 0xa7, 0xc9, 0xfa, 0x57, /* .9"....W */
0x65, 0x62, 0xb0, 0xfc, 0xd4, 0xc2, 0xbc, 0xc6, /* eb...... */
0xb7, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x1a, 0x48, 0x00, 0x00, 0x13, /* ....H... */
0x11, 0x54, 0x42, 0x30, 0x30, 0x30, 0x30, 0x30, /* .TB00000 */
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, /* 00000000 */
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x00, /* 0000001. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x1a, 0x08, 0x00, 0x00, 0x13, /* ........ */
0x11, 0x55, 0x02, 0x1a, 0x09, 0x00, 0x00, 0x13, /* .U...... */
0x11, 0x62, 0x03, 0x00, 0x1a, 0x09, 0x00, 0x00, /* .b...... */
0x13, 0x11, 0x70, 0x03, 0x40, 0x1a, 0x1e, 0x00, /* ..p.@... */
0x00, 0x13, 0x11, 0x6f, 0x18, 0x52, 0x47, 0x2d, /* ...o.RG- */
0x53, 0x55, 0x20, 0x46, 0x6f, 0x72, 0x20, 0x4c, /* SU For L */
0x69, 0x6e, 0x75, 0x78, 0x20, 0x56, 0x31, 0x2e, /* inux V1. */
0x33, 0x30, 0x00, 0x1a, 0x09, 0x00, 0x00, 0x13, /* 30...... */
0x11, 0x79, 0x03, 0x02                          /* .y.. */
    };
  extern uint8_t *service_name;
  extern unsigned int midnight_mode_change_tail_data;

  ethernet_header = (struct ether_header*)(frame); // extract ethernet header values

  // The start address of
  // the eapol version value
  // in eapol start frame
  const int EAPOL_VERSION_ADDR = ETHERNET_HEADER_LENGTH;
  // The start address of
  // the eapol type value
  // in eapol start frame
  const int EAPOL_TYPE_ADDR = EAPOL_VERSION_ADDR + EAPOL_VERSION_LENGTH;
  // The start address of
  // the eapol frame length value
  // in eapol start frame
  const int EAPOL_FRAME_LENGTH_ADDR = EAPOL_TYPE_ADDR + EAPOL_TYPE_LENGTH;
  // The start address of
  // the ethernet padding array
  // in eapol start frame
  const int ETHERNET_PADDING_ADDR = EAPOL_FRAME_LENGTH_ADDR + EAPOL_FRAME_LENGTH;
  // The start address of
  // the ethernet trailer array
  // in eapol start frame
  const int ETHERNET_TRAILER_ADDR =  ETHERNET_PADDING_ADDR + ETHERNET_PADDING_LENGTH;
 

  net_order_netmask = htonl(nic_info.netmask.s_addr);
  net_order_gateway = htonl(nic_info.gateway.s_addr);


// Combine the Ethernet header,  eapol protocol version, eapol protocol
// packet type, eapol frame length, ethernet padding and ethernet
// trailer into a contiguous block
  memset(eapol_start, 0, EAPOL_START_PACKET_LENGTH);
// Filling eapol header
  construct_ethernet_frame_header(eapol_start, nic_info.nic_MAC, eapol_start_dst_MAC, htons(0x888e));
// Filling eapol protocol version
  memcpy(eapol_start + EAPOL_VERSION_ADDR,
         eapol_version, EAPOL_VERSION_LENGTH);
// Filling eapol protocol packet type
  memcpy(eapol_start + EAPOL_TYPE_ADDR,
         eapol_type, EAPOL_TYPE_LENGTH);
// Filling eapol frame length
  memcpy(eapol_start + EAPOL_FRAME_LENGTH_ADDR,
         eapol_frame_length, EAPOL_FRAME_LENGTH);
// Filling ethernet padding
  memcpy(eapol_start + ETHERNET_PADDING_ADDR,
         ethernet_padding, ETHERNET_PADDING_LENGTH);
// Filling network byte order ip addr
  memcpy(eapol_start + EAPOL_START_IP_ADDR,
         encodeIP(nic_info.ipaddr.s_addr), sizeof(uint32_t));
// Filling network byte order net mask
  memcpy(eapol_start + EAPOL_START_NETMASK_ADDR,
         (uint8_t *)&net_order_netmask, sizeof(uint32_t));
// Filling network byte order gateway
  memcpy(eapol_start + EAPOL_START_GATEWAY_ADDR,
         (uint8_t *)&net_order_gateway, sizeof(uint32_t));
  memcpy(eapol_start + EAPOL_START_SECONDRY_MAC_ADDR, nic_info.nic_MAC, 6);//新加mac二次填充
// Filling ethernet trailer
  memcpy(eapol_start + ETHERNET_TRAILER_ADDR,
         ethernet_trailer, ETHERNET_TRAILER_LENGTH);
// Filling service name
  memcpy(eapol_start + EAPOL_START_SERVICE_NAME_ADDR,
         service_name, SERVICE_NAME_LENGTH);

  if(midnight_mode_change_tail_data == 1) {
    // replace tail data in relogin process when midnight mode is on
    memcpy(eapol_start + TAIL_DATA_ADDRESS_IN_START, SWITCH_SERVICE_TAIL_DATA_START, SWITCH_SERVICE_TAIL_DATA_START_LENGTH);
  }

#if defined(DEBUG)
  fprintf(stdout, "The login ip in start frame %s\n", inet_ntoa(nic_info.ipaddr));
#endif


  return;
}


//
//------function----------------------------------------------------------
// name: 
//    construct_eap_response_identity_frame
// description:
//    构造 EAP RESPONSE IDENTITY frame。
//    抓包分析Ruijie客户端发送给服务器的,用于Ruijie认证第二
//    步的 EAP RESPONSE IDENTITY frame 格式和内容。用代码仿造出一个几
//    乎一模一样(服务器不验证的字段可以不改)的 EAP RESPONSE IDENTITY frame。
//
//    函数中使用了大量的 const value。这些 value 有一些代表
//    认证信息在 EAP RESPONSE IDENTITY frame 中的起始位置, 例如 ipaddr
//    的起始位置, netmask 的起始位置等等。还有一些代表认证信
//    息的 size, 用于 memcpy() 。
//
//    ethernet_padding[] 和 ethernet_trailer[]是从抓到的 frame 中复制来的。
//    一些关键的认证数据嵌在这两个数组中,需要用代码去替换。
//
//------------------------------------------------------------------------
//
//void construct_eap_response_identity_frame(const uint8_t *dst_MAC, const uint8_t eap_id)
void construct_eap_response_identity_frame(const uint8_t *frame)
{
  const uint16_t EAP_RESPONSE_IDENTITY_LENGTH  = 542;  // send to server packet length
  extern uint8_t eap_response_identity[];
  const uint8_t eapol_version[] = {0x01};
  const uint8_t eapol_type[] = {0x00}; // type: eapol start
  const uint8_t eapol_frame_length[] = {0x00, 0x12};
// const uint8_t eap_header[] = {0x02, 0x01, 0x00, 0x11}; 
  const uint8_t eap_id = frame[EAP_ID_ADDRESS_IN_EAP_IDENTITY_REQUEST_FRAME];
  const uint8_t eap_header[] = {0x02, eap_id, 0x00, 0x12};
  const uint8_t eap_type[] = {0x01};  // type: eap packet
  extern uint8_t *service_name;
  extern char *username;
  extern unsigned int username_length;
  uint8_t *eap_data = NULL;
  uint8_t *dst_MAC = NULL;
//  uint8_t eap_id = 0x01; // default 0x01, 详见 RFC 3748
    const uint8_t ethernet_padding[] = {0xff, 0xff, 0x37, 0x77, /* 4121..7w */
0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ........ */
0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, /* ........ */
0x53, 0xfa, 0xc7, 0x00
    };

    const uint8_t ethernet_trailer[] = {0x00, 0x13, 0x11, 0x38, /* S......8 */
0x30, 0x32, 0x31, 0x78, 0x2e, 0x65, 0x78, 0x65, /* 021x.exe */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* ........ */
0x1f, 0x01, 0x02, 0xd0, 0x00, 0x00, 0x13, 0x11, /* ........ */
0x01, 0xb4, 0x1a, 0x0c, 0x00, 0x00, 0x13, 0x11, /* ........ */
0x18, 0x06, 0x00, 0x00, 0x00, 0x01, 0x1a, 0x0e, /* ........ */
0x00, 0x00, 0x13, 0x11, 0x2d, 0x08, 0x1c, 0x39, /* ....-..9 */
0x47, 0x9a, 0xc1, 0xeb, 0x1a, 0x08, 0x00, 0x00, /* G....... */
0x13, 0x11, 0x2f, 0x02, 0x1a, 0x08, 0x00, 0x00, /* ../..... */
0x13, 0x11, 0x76, 0x02, 0x1a, 0x09, 0x00, 0x00, /* ..v..... */
0x13, 0x11, 0x35, 0x03, 0x03, 0x1a, 0x18, 0x00, /* ..5..... */
0x00, 0x13, 0x11, 0x36, 0x12, 0x00, 0x00, 0x00, /* ...6.... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x18, 0x00, /* ........ */
0x00, 0x13, 0x11, 0x38, 0x12, 0xfe, 0x80, 0x00, /* ...8.... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0xec, 0xa9, /* .....5.. */
0xaa, 0xf1, 0xa2, 0x64, 0xf7, 0x1a, 0x18, 0x00, /* ...d.... */
0x00, 0x13, 0x11, 0x4e, 0x12, 0x20, 0x01, 0x0d, /* ...N. .. */
0xa8, 0x20, 0x06, 0xb5, 0x0a, 0xe1, 0x65, 0x84, /* . ....e. */
0xaa, 0xf2, 0x07, 0x03, 0x46, 0x1a, 0x88, 0x00, /* ....F... */
0x00, 0x13, 0x11, 0x4d, 0x82, 0x63, 0x65, 0x64, /* ...M.ced */
0x66, 0x65, 0x62, 0x31, 0x64, 0x37, 0x66, 0x37, /* feb1d7f7 */
0x31, 0x34, 0x35, 0x34, 0x36, 0x65, 0x35, 0x37, /* 14546e57 */
0x63, 0x61, 0x32, 0x33, 0x62, 0x35, 0x36, 0x65, /* ca23b56e */
0x31, 0x38, 0x38, 0x38, 0x62, 0x31, 0x63, 0x35, /* 1888b1c5 */
0x37, 0x36, 0x37, 0x38, 0x30, 0x62, 0x36, 0x38, /* 76780b68 */
0x64, 0x61, 0x32, 0x37, 0x36, 0x34, 0x62, 0x32, /* da2764b2 */
0x64, 0x61, 0x39, 0x62, 0x61, 0x64, 0x66, 0x36, /* da9badf6 */
0x38, 0x66, 0x65, 0x37, 0x39, 0x31, 0x33, 0x30, /* 8fe79130 */
0x37, 0x34, 0x30, 0x39, 0x37, 0x35, 0x39, 0x36, /* 74097596 */
0x35, 0x32, 0x33, 0x32, 0x63, 0x35, 0x61, 0x35, /* 5232c5a5 */
0x65, 0x33, 0x36, 0x62, 0x30, 0x30, 0x63, 0x30, /* e36b00c0 */
0x39, 0x34, 0x30, 0x31, 0x64, 0x30, 0x66, 0x64, /* 9401d0fd */
0x38, 0x31, 0x31, 0x37, 0x39, 0x61, 0x33, 0x31, /* 81179a31 */
0x38, 0x62, 0x36, 0x36, 0x35, 0x38, 0x37, 0x66, /* 8b66587f */
0x66, 0x39, 0x65, 0x34, 0x39, 0x61, 0x61, 0x39, /* f9e49aa9 */
0x38, 0x66, 0x35, 0x64, 0x30, 0x1a, 0x28, 0x00, /* 8f5d0.(. */
0x00, 0x13, 0x11, 0x39, 0x22, 0xd1, 0xa7, 0xc9, /* ...9"... */
0xfa, 0x57, 0x65, 0x62, 0xb0, 0xfc, 0xd4, 0xc2, /* .Web.... */
0xbc, 0xc6, 0xb7, 0xd1, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x48, 0x00, /* ......H. */
0x00, 0x13, 0x11, 0x54, 0x42, 0x30, 0x30, 0x30, /* ...TB000 */
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, /* 00000000 */
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, /* 00000000 */
0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 1....... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x08, 0x00, /* ........ */
0x00, 0x13, 0x11, 0x55, 0x02, 0x1a, 0x09, 0x00, /* ...U.... */
0x00, 0x13, 0x11, 0x62, 0x03, 0x00, 0x1a, 0x09, /* ...b.... */
0x00, 0x00, 0x13, 0x11, 0x70, 0x03, 0x40, 0x1a, /* ....p.@. */
0x1e, 0x00, 0x00, 0x13, 0x11, 0x6f, 0x18, 0x52, /* .....o.R */
0x47, 0x2d, 0x53, 0x55, 0x20, 0x46, 0x6f, 0x72, /* G-SU For */
0x20, 0x4c, 0x69, 0x6e, 0x75, 0x78, 0x20, 0x56, /*  Linux V */
0x31, 0x2e, 0x33, 0x30, 0x00, 0x1a, 0x09, 0x00, /* 1.30.... */
0x00, 0x13, 0x11, 0x79, 0x03, 0x02              /* ...y.. */
    };

 ethernet_header = (struct ether_header*)(frame);  // extract ethernet header values
 dst_MAC = ethernet_header->ether_shost;
// eap_id = frame[EAP_ID_ADDRESS];

// The start address of
// the eapol version value
// in eap response identity frame
  const int EAPOL_VERSION_ADDR = ETHERNET_HEADER_LENGTH;
// The start address of
// the eapol type value
// in eap response identity frame
  const int EAPOL_TYPE_ADDR = EAPOL_VERSION_ADDR + EAPOL_VERSION_LENGTH;
// The start address of
// the eapol frame length value
// in eap response identity frame
  const int EAPOL_FRAME_LENGTH_ADDR = EAPOL_TYPE_ADDR + EAPOL_TYPE_LENGTH;
// The start address of
// the eap header array
// in eap response identity frame
  const int EAP_HEADER_ADDR = EAPOL_FRAME_LENGTH_ADDR + EAPOL_FRAME_LENGTH;
// The start address of
// the eap type value
// in eap response identity frame
  const int EAP_TYPE_ADDR = EAP_HEADER_ADDR + EAP_HEADER_LENGTH;
// The start address of
// the username array
// in eap response identity frame
  const int USERNAME_ADDR = EAP_TYPE_ADDR + EAP_TYPE_LENGTH;
// The start address of
// the ethernet padding array
// in eap response identity frame
  const int ETHERNET_PADDING_ADDR = USERNAME_ADDR + username_length;
// The start address of
// the ethernet trailer array
// in eap response identity frame
  const int ETHERNET_TRAILER_ADDR = ETHERNET_PADDING_ADDR + EAP_IDENTITY_PADDING_LENGTH;

  extern unsigned int midnight_mode_change_tail_data;

// Filling ethernet header
  construct_ethernet_frame_header(eap_response_identity, nic_info.nic_MAC,
                                  dst_MAC, htons(0x888e));
// Filling eapol protocol version
  memcpy(eap_response_identity + EAPOL_VERSION_ADDR,
         eapol_version, EAPOL_VERSION_LENGTH);
// Filling eapol protocol packet type
  memcpy(eap_response_identity + EAPOL_TYPE_ADDR,
         eapol_type, EAPOL_TYPE_LENGTH);
// Filling eapol frame length
  memcpy(eap_response_identity + EAPOL_FRAME_LENGTH_ADDR,
         eapol_frame_length, EAPOL_FRAME_LENGTH);
// Filling eap header
  memcpy(eap_response_identity + EAP_HEADER_ADDR,
         eap_header, EAP_HEADER_LENGTH);
// Filling eap type
  memcpy(eap_response_identity + EAP_TYPE_ADDR,
         eap_type, EAP_TYPE_LENGTH);
#if defined(DEBUG)
  printf("eap data is : %s\n", eap_data);
#endif
// Filling eap data i.e. username
  memcpy(eap_response_identity + USERNAME_ADDR,
         username, username_length);
// Filling eap padding data
  memcpy(eap_response_identity + ETHERNET_PADDING_ADDR,
         ethernet_padding, EAP_IDENTITY_PADDING_LENGTH);
memcpy(eap_response_identity + EAP_IDENTITY_RESPONSE_SECONDRY_MAC_ADDR, nic_info.nic_MAC, 6);//新加mac二次填充
// Filling eap trailer data
  memcpy(eap_response_identity + ETHERNET_TRAILER_ADDR,
         ethernet_trailer, EAP_IDENTITY_TRAILER_LENGTH);
// Filling service name
  memcpy(eap_response_identity + EAP_IDENTITY_RESPONSE_SERVICE_NAME_ADDR,
         service_name, SERVICE_NAME_LENGTH);

  if(midnight_mode_change_tail_data == 1) {
//   memcpy(eap_response_identity + TAIL_DATA_ADDRESS_IN_IDENTITY,
//       SWITCH_SERVICE_TAIL_DATA_IDENTITY_AND_MD5_CHALLENGE, SWITCH_SERVICE_TAIL_DATA_IDENTITY_AND_MD5_CHALLENGE_LENGTH);
    memcpy(eap_response_identity + EAP_IDENTITY_RESPONSE_IP_ADDR,
           encodeIP(nic_info.ipaddr.s_addr), sizeof(uint32_t)); // filling network byte order ip addr
    memcpy(eap_response_identity + EAP_IDENTITY_RESPONSE_NETMASK_ADDR,
           (uint8_t *)&net_order_netmask, sizeof(uint32_t)); // filling network byte order net mask
    memcpy(eap_response_identity + EAP_IDENTITY_RESPONSE_GATEWAY_ADDR,
           (uint8_t *)&net_order_gateway, sizeof(uint32_t)); // filling network byte order gateway

  }



  return;
}

//void  construct_eap_response_md5_challenge_frame(const uint8_t *dst_MAC, const uint8_t *eap_request_md5_value, const uint8_t *eap_md5_value_size, const uint8_t *frame)
//
//------function----------------------------------------------------------
// name: 
//    construct_eap_response_md5_challenge_frame
// description:
//    构造 EAP RESPONSE MD5 CHALLENGE frame。
//    抓包分析Ruijie客户端发送给服务器的,用于Ruijie认证第三
//    步的 EAP RESPONSE MD5 CHALLENGE frame。 格式和内容。用代码仿造出一个几
//    乎一模一样(服务器不验证的字段可以不改)的 EAP RESPONSE IDENTITY frame。
//
//    函数中使用了大量的 const value。这些 value 有一些代表
//    认证信息在 EAP RESPONSE IDENTITY frame 中的起始位置, 例如 ipaddr
//    的起始位置, netmask 的起始位置等等。还有一些代表认证信
//    息的 size, 用于 memcpy() 。
//
//    ethernet_padding[] 和 ethernet_trailer[]是从抓到的 frame 中复制来的。
//    一些关键的认证数据嵌在这两个数组中,需要用代码去替换。
//
//    Ruijie V4 algorithm的计算结果填充在这个frame中, 该算法用 
//    request md5 challenge frame 中的 md5-value 和 md5-value大小 0x10,配合4种
//    hash-algorithms 和 Ampheck 操作, 分成 5 种情况算出一个 size == 0x80 的 
//    hex stream, 填充进 EAP RESPONSE CHALLENGE frame。该算法移植自 华中科技大
//    学 hyrathb。感谢！
//------------------------------------------------------------------------
//
void construct_eap_response_md5_challenge_frame(const uint8_t *frame)
{
  const uint16_t EAP_RESPONSE_MD5_CHALLENGE_LENGTH  = 575;  // send to server packet length
  extern uint8_t eap_response_md5_challenge[];
  const uint8_t eapol_version[] = {0x01};
  const uint8_t eapol_type[] = {0x00}; // type: eapol start
  const uint8_t eapol_frame_length[] = {0x00, 0x23};
  const uint8_t eap_id = frame[EAP_ID_ADDRESS_IN_EAP_MD5_REQUEST_FRAME];
  const uint8_t eap_header[] = {0x02, eap_id, 0x00, 0x23};
  const uint8_t eap_type[] = {0x04};  // type: eap packet
// const uint8_t eap_md5_value_size[] = {0x10}; // eap.md5.value_size
//uint8_t eap_response_md5_value[EAP_MD5_VALUE_LENGTH]; // md5 calculation result
  uint8_t eap_response_md5_value[EAP_MD5_VALUE_LENGTH]; // md5 calculation result
  uint8_t *dst_MAC = NULL;
  uint8_t eap_request_md5_value[EAP_MD5_VALUE_LENGTH]; // 该hex数组从服务器发送的 request-md5-challenge frame 中提取
  uint8_t eap_md5_value_size[1];

  extern char *username;
  extern uint8_t *service_name;
  extern unsigned int username_length;
  extern unsigned int midnight_mode_change_tail_data;

    const uint8_t ethernet_padding[] = {0xff, 0xff, 0x37, 0x77, 0x7f, 0xaf, 0x6c};
    const uint8_t ethernet_trailer[] = {0x73, 0x06, 0x00, 0x00, /* w..ls... */
0xe0, 0xff, 0xaf, 0x6c, 0xf3, 0x7f, 0x01, 0xff, /* ...l.... */
0xff, 0x53, 0xe0, 0xb3, 0x00, 0x00, 0x13, 0x11, /* .S...... */
0x38, 0x30, 0x32, 0x31, 0x78, 0x2e, 0x65, 0x78, /* 8021x.ex */
0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* e....... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x01, 0x1f, 0x01, 0x02, 0xd0, 0x00, 0x00, 0x13, /* ........ */
0x11, 0x01, 0xc4, 0x1a, 0x0c, 0x00, 0x00, 0x13, /* ........ */
0x11, 0x18, 0x06, 0x00, 0x00, 0x00, 0x01, 0x1a, /* ........ */
0x0e, 0x00, 0x00, 0x13, 0x11, 0x2d, 0x08, 0x1c, /* .....-.. */
0x39, 0x47, 0x9a, 0xc1, 0xeb, 0x1a, 0x18, 0x00, /* 9G...... */
0x00, 0x13, 0x11, 0x2f, 0x12, 0x14, 0x1d, 0x21, /* .../...! */
0xea, 0x2b, 0x54, 0x12, 0x2e, 0x3d, 0xea, 0xb6, /* .+T..=.. */
0xf1, 0x02, 0x60, 0xc8, 0xc8, 0x1a, 0x08, 0x00, /* ..`..... */
0x00, 0x13, 0x11, 0x76, 0x02, 0x1a, 0x09, 0x00, /* ...v.... */
0x00, 0x13, 0x11, 0x35, 0x03, 0x03, 0x1a, 0x18, /* ...5.... */
0x00, 0x00, 0x13, 0x11, 0x36, 0x12, 0x00, 0x00, /* ....6... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x18, /* ........ */
0x00, 0x00, 0x13, 0x11, 0x38, 0x12, 0xfe, 0x80, /* ....8... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0xec, /* ......5. */
0xa9, 0xaa, 0xf1, 0xa2, 0x64, 0xf7, 0x1a, 0x18, /* ....d... */
0x00, 0x00, 0x13, 0x11, 0x4e, 0x12, 0x20, 0x01, /* ....N. . */
0x0d, 0xa8, 0x20, 0x06, 0xb5, 0x0a, 0xe1, 0x65, /* .. ....e */
0x84, 0xaa, 0xf2, 0x07, 0x03, 0x46, 0x1a, 0x88, /* .....F.. */
0x00, 0x00, 0x13, 0x11, 0x4d, 0x82, 0x35, 0x32, /* ....M.52 */
0x34, 0x37, 0x34, 0x31, 0x65, 0x38, 0x32, 0x31, /* 4741e821 */
0x37, 0x39, 0x62, 0x36, 0x65, 0x37, 0x64, 0x64, /* 79b6e7dd */
0x62, 0x33, 0x66, 0x39, 0x64, 0x30, 0x37, 0x38, /* b3f9d078 */
0x61, 0x37, 0x34, 0x31, 0x33, 0x30, 0x31, 0x61, /* a741301a */
0x39, 0x61, 0x63, 0x61, 0x66, 0x36, 0x34, 0x66, /* 9acaf64f */
0x37, 0x64, 0x63, 0x37, 0x66, 0x35, 0x30, 0x36, /* 7dc7f506 */
0x37, 0x35, 0x32, 0x36, 0x39, 0x61, 0x62, 0x35, /* 75269ab5 */
0x62, 0x34, 0x37, 0x37, 0x65, 0x65, 0x39, 0x33, /* b477ee93 */
0x39, 0x63, 0x31, 0x66, 0x66, 0x39, 0x61, 0x62, /* 9c1ff9ab */
0x35, 0x37, 0x33, 0x35, 0x65, 0x39, 0x31, 0x37, /* 5735e917 */
0x39, 0x66, 0x37, 0x63, 0x63, 0x35, 0x33, 0x62, /* 9f7cc53b */
0x34, 0x34, 0x37, 0x32, 0x33, 0x61, 0x34, 0x35, /* 44723a45 */
0x65, 0x39, 0x65, 0x64, 0x31, 0x34, 0x35, 0x36, /* e9ed1456 */
0x34, 0x34, 0x64, 0x36, 0x64, 0x61, 0x31, 0x31, /* 44d6da11 */
0x37, 0x36, 0x61, 0x62, 0x32, 0x32, 0x35, 0x63, /* 76ab225c */
0x62, 0x63, 0x61, 0x32, 0x32, 0x38, 0x1a, 0x28, /* bca228.( */
0x00, 0x00, 0x13, 0x11, 0x39, 0x22, 0xd1, 0xa7, /* ....9".. */
0xc9, 0xfa, 0x57, 0x65, 0x62, 0xb0, 0xfc, 0xd4, /* ..Web... */
0xc2, 0xbc, 0xc6, 0xb7, 0xd1, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x48, /* .......H */
0x00, 0x00, 0x13, 0x11, 0x54, 0x42, 0x30, 0x30, /* ....TB00 */
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, /* 00000000 */
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, /* 00000000 */
0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 01...... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x08, /* ........ */
0x00, 0x00, 0x13, 0x11, 0x55, 0x02, 0x1a, 0x09, /* ....U... */
0x00, 0x00, 0x13, 0x11, 0x62, 0x03, 0x00, 0x1a, /* ....b... */
0x09, 0x00, 0x00, 0x13, 0x11, 0x70, 0x03, 0x40, /* .....p.@ */
0x1a, 0x1e, 0x00, 0x00, 0x13, 0x11, 0x6f, 0x18, /* ......o. */
0x52, 0x47, 0x2d, 0x53, 0x55, 0x20, 0x46, 0x6f, /* RG-SU Fo */
0x72, 0x20, 0x4c, 0x69, 0x6e, 0x75, 0x78, 0x20, /* r Linux  */
0x56, 0x31, 0x2e, 0x33, 0x30, 0x00, 0x1a, 0x09, /* V1.30... */
0x00, 0x00, 0x13, 0x11, 0x79, 0x03, 0x02        /* ....y.. */
    };
  // The start address of
  // the eapol version value
  // in eap response md5 challenge frame
  const int EAPOL_VERSION_ADDR = ETHERNET_HEADER_LENGTH;
  // The start address of
  // the eapol type value
  // in eap response md5 challenge frame
  const int EAPOL_TYPE_ADDR = EAPOL_VERSION_ADDR + EAPOL_VERSION_LENGTH;
  // The start address of
  // the eapol frame length value
  // in eap response md5 challenge frame
  const int EAPOL_FRAME_LENGTH_ADDR = EAPOL_TYPE_ADDR + EAPOL_TYPE_LENGTH;
  // The start address of
  // the eap header array
  // in eap response md5 challenge frame
  const int EAP_HEADER_ADDR = EAPOL_FRAME_LENGTH_ADDR + EAPOL_FRAME_LENGTH;
  // The start address of
  // the eap type value
  // in eap response md5 challenge frame
  const int EAP_TYPE_ADDR = EAP_HEADER_ADDR + EAP_HEADER_LENGTH;
  // The start address of
  // the eap md5 value size value
  // in eap response md5 challenge frame
  const int EAP_MD5_VALUE_SIZE_ADDR = EAP_TYPE_ADDR + EAP_TYPE_LENGTH;
  // The start address of
  // the eap response md5 value
  // in eap response md5 challenge frame
  const int EAP_MD5_VALUE_ADDR = EAP_MD5_VALUE_SIZE_ADDR + EAP_MD5_VALUE_SIZE_LENGTH;
  // The start address of
  // the username array
  // in eap response md5 challenge frame
  const int USERNAME_ADDR = EAP_MD5_VALUE_ADDR + EAP_MD5_VALUE_LENGTH ;
  // The start address of
  // the ethernet padding array
  // in eap response md5 challenge frame
  const int ETHERNET_PADDING_ADDR = USERNAME_ADDR + username_length;
  // The start address of
  // the ethernet trailer array
  // in eap response md5 challenge frame
  const int ETHERNET_TRAILER_ADDR = ETHERNET_PADDING_ADDR + EAP_MD5_CHALLENGE_PADDING_LENGTH;

  

  ethernet_header = (struct ether_header*)(frame); // extract ethernet header values
  dst_MAC = ethernet_header->ether_shost;
  strncpy(eap_request_md5_value, frame+24, EAP_MD5_VALUE_LENGTH);
  memcpy(eap_md5_value_size, (frame + 0x17), 1);
  
  memcpy(eap_response_md5_value,
         calculate_the_eap_md5_value_in_response_md5_challenge_frame( frame[EAP_ID_ADDRESS_IN_EAP_MD5_REQUEST_FRAME],
             eap_request_md5_value,
             frame[EAP_MD5_VALUE_SIZE_ADDRESS_IN_EAP_MD5_REQUEST_FRAME] ),
         EAP_MD5_VALUE_LENGTH);


// Filling ethernet header
  construct_ethernet_frame_header(eap_response_md5_challenge, nic_info.nic_MAC,
                                  dst_MAC, htons(0x888e));
// Filling eapol protocol version
  memcpy(eap_response_md5_challenge + EAPOL_VERSION_ADDR,
         eapol_version, EAPOL_VERSION_LENGTH);
// Filling eapol protocol packet type
  memcpy(eap_response_md5_challenge + EAPOL_TYPE_ADDR,
         eapol_type, EAPOL_TYPE_LENGTH);
// Filling eapol frame length
  memcpy(eap_response_md5_challenge + EAPOL_FRAME_LENGTH_ADDR,
         eapol_frame_length, EAPOL_FRAME_LENGTH);
// Filling eap header
  memcpy(eap_response_md5_challenge + EAP_HEADER_ADDR,
         eap_header, EAP_HEADER_LENGTH);
// Filling eap type
  memcpy(eap_response_md5_challenge + EAP_TYPE_ADDR,
         eap_type, EAP_TYPE_LENGTH);
// Filling EAP-MD5 Value-Size
  memcpy(eap_response_md5_challenge + EAP_MD5_VALUE_SIZE_ADDR,
         eap_md5_value_size, EAP_MD5_VALUE_SIZE_LENGTH);
// Filling EAP-MD5 value
  memcpy(eap_response_md5_challenge + EAP_MD5_VALUE_ADDR,
         eap_response_md5_value, EAP_MD5_VALUE_LENGTH);
// Filling EAP-MD5 Extra Data
  memcpy(eap_response_md5_challenge + USERNAME_ADDR,
         username, username_length);
// Filling eap padding data
  memcpy(eap_response_md5_challenge + ETHERNET_PADDING_ADDR,
         ethernet_padding, EAP_MD5_CHALLENGE_PADDING_LENGTH);

// Filling eap trailer data
  memcpy(eap_response_md5_challenge + ETHERNET_TRAILER_ADDR,
         ethernet_trailer, EAP_MD5_CHALLENGE_TRAILER_LENGTH);

// Filling network byte order ip addr
  memcpy(eap_response_md5_challenge + EAP_MD5_RESPONSE_IP_ADDR, encodeIP(nic_info.ipaddr.s_addr), sizeof(uint32_t));
// Filling network byte order net mask
  memcpy(eap_response_md5_challenge + EAP_MD5_RESPONSE_NETMASK_ADDR, (uint8_t *)&net_order_netmask, sizeof(uint32_t));
// Filling network byte order gateway
  memcpy(eap_response_md5_challenge + EAP_MD5_RESPONSE_GATEWAY_ADDR, (uint8_t *)&net_order_gateway, sizeof(uint32_t));
memcpy(eap_response_md5_challenge + EAP_MD5_RESPONSE_SECONDRY_MAC_ADDR, nic_info.nic_MAC, 6);//新加mac二次填充
// Filling mentohust code
  memcpy(eap_response_md5_challenge + EAP_MD5_RESPONSE_COMPUTE_PASSWORD_ADDR ,
         computePwd(eap_request_md5_value), 0x10);
// Filling V4 algorithm result
  memcpy(eap_response_md5_challenge + EAP_MD5_RESPONSE_COMPUTE_V4_ADDR ,
         computeV4(eap_request_md5_value, 16), 0x80);
// Filling service name
  memcpy(eap_response_md5_challenge + EAP_MD5_RESPONSE_SERVICE_NAME_ADDR,
         service_name, SERVICE_NAME_LENGTH);

  if(midnight_mode_change_tail_data == 1) {
    // replace tail data in relogin process if midnight_mode is on
    memcpy(eap_response_md5_challenge + TAIL_DATA_ADDRESS_IN_MD5_CHALLENGE,
           SWITCH_SERVICE_TAIL_DATA_MD5_CHALLENGE, SWITCH_SERVICE_TAIL_DATA_MD5_CHALLENGE_LENGTH);
  }

#if defined(DEBUG)
  fprintf(stdout, "The login ip in md5-challenge frame %s\n", inet_ntoa(nic_info.ipaddr));
#endif

  return;
}

//void construct_eapol_heartbeat_frame(const uint8_t *dst_MAC, const uint8_t *frame)
void construct_eapol_heartbeat_frame(const uint8_t *frame)
{
  const uint16_t EAP_EAPOL_HEARTBEAT_LENGTH  = 45;  // send to server packet length
  extern uint8_t eapol_heartbeat[];
  const uint8_t eapol_version[] = {0x01};
  const uint8_t eapol_type[] = {0xbf}; // type: eapol start
  const uint8_t eapol_frame_length[] = {0x00, 0x1e};
  const uint8_t eapol_heartbeat_data[] = {0xff, 0xff, 0x37, 0x77, 0x7f, 0x9f, 0x7d, 0x0f, 0x03, 0x39, 0xff, 0xff, 0x37, 0x77, 0x7f, 0x9f, 0xff, 0xff, 0xf7, 0x4b, 0xff, 0xff, 0x37, 0x77, 0x7f, 0x3f, 0xff};
  uint8_t *dst_MAC = NULL;

// The start address of
// the eapol version value
// in eapol heartbeat frame
  const int EAPOL_VERSION_ADDR = ETHERNET_HEADER_LENGTH;
// The start address of
// the eapol type value
// in eapol heartbeat frame
  const int EAPOL_TYPE_ADDR = EAPOL_VERSION_ADDR + EAPOL_VERSION_LENGTH;
// The start address of
// the eapol frame length value
// in eapol heartbeat frame
  const int EAPOL_FRAME_LENGTH_ADDR  = EAPOL_TYPE_ADDR + EAPOL_TYPE_LENGTH;
// The start address of
// the eapol heartbeat data array
// in eapol heartbeat frame
  const int EAPOL_HEARTBEAT_DATA_ADDR = EAPOL_FRAME_LENGTH_ADDR + EAPOL_FRAME_LENGTH;
  
  ethernet_header = (struct ether_header*)(frame); // extract ethernet header values
  dst_MAC = ethernet_header->ether_shost;
  construct_ethernet_frame_header(eapol_heartbeat, nic_info.nic_MAC,
                                  dst_MAC, htons(0x888e)); // Filling ethernet header

  memcpy(eapol_heartbeat + EAPOL_VERSION_ADDR,
         eapol_version, EAPOL_VERSION_LENGTH);  // Filling eapol protocol version
  memcpy(eapol_heartbeat + EAPOL_TYPE_ADDR,
         eapol_type, EAPOL_TYPE_LENGTH);  // Filling eapol protocol packet type
  memcpy(eapol_heartbeat + EAPOL_FRAME_LENGTH_ADDR,
         eapol_frame_length, EAPOL_FRAME_LENGTH); // Filling eapol frame length
  memcpy(eapol_heartbeat + EAPOL_HEARTBEAT_DATA_ADDR,
         eapol_heartbeat_data, EAPOL_HEARTBEAT_DATA_LENGTH); // Filling eapol heartbeat data length
  getEchoKey(frame);
  fillEchoPacket(eapol_heartbeat);

  return;
}


