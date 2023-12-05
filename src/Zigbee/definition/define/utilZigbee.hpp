#ifndef INC_DEFINE_UTIL_ZIGBEE_HPP_
#define INC_DEFINE_UTIL_ZIGBEE_HPP_

namespace ZigbeeNamespace {

	enum UTILCommandsT {
		UTIL_GET_DEVICE_INFO = 0x00,
		UTIL_GET_NV_INFO,
		UTIL_SET_PANID,
		UTIL_SET_CHANNELS,
		UTIL_SET_SECLEVEL,
		UTIL_SET_PRECFGKEY,
		UTIL_CALLBACK_SUB_CMD,
		UTIL_KEY_EVENT,
		UTIL_TIME_ALIVE = 0x09,
		UTIL_LED_CONTROL,
		UTIL_LOOPBACK = 0x10,
		UTIL_DATA_REQ,
		UTIL_SRC_MATCH_ENABLE = 0x20,
		UTIL_SRC_MATCH_ADD_ENTRY,
		UTIL_SRC_MATCH_DEL_ENTRY,
		UTIL_SRC_MATCH_CHECK_SRC_ADDR,
		UTIL_SRC_MATCH_ACK_ALL_PENDING,
		UTIL_SRC_MATCH_CHECK_ALL_PENDING,
		UTIL_ADDRMGR_EXT_ADDR_LOOKUP = 0x40,
		UTIL_ADDRMGR_NWK_ADDR_LOOKUP,
		UTIL_APSME_LINK_KEY_DATA_GET = 0x44,
		UTIL_APSME_LINK_KEY_NV_ID_GET,
		UTIL_ASSOC_COUNT = 0x48,
		UTIL_ASSOC_FIND_DEVICE,
		UTIL_ASSOC_GET_WITH_ADDRESS,
		UTIL_APSME_REQUEST_KEY_CMD,
		UTIL_SRNG_GEN,
		UTIL_BIND_ADD_ENTRY,
		UTIL_ASSOC_REMOVE = 0x63,/*Custom*/
		UTIL_ASSOC_ADD,/*Custom*/
		UTIL_ZCL_KEY_EST_INIT_EST = 0x80,
		UTIL_ZCL_KEY_EST_SIGN,
		UTIL_SYNC_REQ = 0xE0,
		UTIL_ZCL_KEY_ESTABLISH_IND
	};

};

#endif /* INC_DEFINE_UTIL_ZIGBEE_HPP_ */
