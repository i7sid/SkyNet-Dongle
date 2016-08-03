#define malloc_count()    {}
#define free_count()      {}

/*
#define MAC_CONST_aMinMPDUOverhead          (9)
#define MAC_CONST_aUnitBackoffPeriod        (20)
#define MAC_CONST_aMaxMPDUUnsecuredOverhead (25)
#define MAC_CONST_aMaxSIFSFrameSize         (18)
//#define MAC_CONST_aMaxMACPayloadSize        (  )
#define MAC_CONST_aBaseSlotDuration         (60)
*/
#ifndef SKYMAC_H_
#define SKYMAC_H_

//#define MAC_CONST_SYMBOL_LENGTH             (10) // TODO: correct value in us?
#define MAC_CONST_SYMBOL_LENGTH             (2000)
//

#define MAC_CONST_A_UNIT_BACKOFF_PERIOD     (20) // in "symbols"
#define MAC_CONST_MAX_BE                    (8)
#define MAC_CONST_MIN_BE                    (3)
#define MAC_CONST_MAX_CSMA_BACKOFFS         (4)
#define MAC_CONST_ACK_WAIT_DURATION         (100000) // in "symbols"


typedef enum mac_extheader_types {
  EXTHDR_NO       = 0,  ///<@brief No header following. (prev was last)
  EXTHDR_ETHER_TYPE   = 1,  ///<@brief Specifies ether_type of payload.
  EXTHDR_SENSOR_VALUES  = 2,  ///<@brief Specifies which sensor data is contained in payload.
  EXTHDR_DONGLE_CMD   = 3,  ///<@brief Contains a command to the dongle.
  EXTHDR_DONGLE_CMD_ANSWER= 4,  ///<@brief Contains an answer to a command to the dongle.
  EXTHDR_TTL        = 5   ///<@brief Time to live for multihop packets.
} mac_extheader_types;


union typelength_union {
  uint8_t raw;
  struct type_length {
    mac_extheader_types   type  : 4;
    uint8_t         length  : 4;
  } type_length;
};

typedef struct mac_extheader {
  union typelength_union  typelength_union;
  uint8_t         data[16];
  struct mac_extheader* next;
} __attribute__((aligned(1),packed)) mac_extheader;



inline void mac_extheader_init(mac_extheader* hdr) {
  memset(hdr, 0, sizeof(mac_extheader));
}


typedef struct mac_frame_data {
    struct {
        uint8_t frame_control[2];
        uint8_t seq_no;
        uint8_t dest_pan_id[2];
        uint8_t dest_address[8];
        uint8_t src_pan_id[2];
        uint8_t src_address[8];
        // uint8_t aux_security_header[0];      // for future usage?
    } __attribute__((aligned(1),packed)) mhr;
    mac_extheader* extheader;
    uint8_t* payload;
    uint8_t fcs[2];
    uint16_t payload_size;
} __attribute__((aligned(1),packed)) mac_frame_data;




/**
 * @brief Allocates the memory for a frame and initializes memory.
 */
void* mac_frame_data_calloc(void);

/**
 * @brief Frees the memory internally used by a frame (payload and headers).
 *
 * This does not free the frame itself!
 * (Important! May ba allocated statically.)
 */
void mac_frame_data_free_contents(mac_frame_data *frame);


//size_t mac_frame_data_get_size(mac_frame_data *frame);

/**
 * @brief Estimates the maximum size of the size needed by a frame.
 *
 * Can be used to allocate a raw buffer used to pack frame into.
 */
size_t mac_frame_data_estimate_size(mac_frame_data *frame);


/**
 * @brief Initializes the given frame by setting all field to zero.
 *
 * Some fields are initialized with default values.
 * See source for more information.
 */
void mac_frame_data_init(mac_frame_data *frame);


/**
 * @brief Writes packet into buffer that can be passed to phy_transmit.
 * @param frame Pointer to frame in RAM that should be sent.
 * @param buffer  Pointer to buffer in RAM, must be allocated large enough!
 *
 * @return  Exact number of bytes written.
 */
uint16_t mac_frame_data_pack(mac_frame_data *frame, uint8_t *buffer);

/**
 * @brief Parses the content of a packet from a buffer into a \ref mac_frame_data structure.
 * @param frame Data structure to write data to.
 * @param buffer  Data buffer from for example PHY to read data from.
 * @param length  Total bytes received (important for correct detection of payload size)
 *
 * @return  Exact number of bytes read.
 *
 * @warning After data was processed you have to call \ref mac_frame_data_free_contents()
 *      to clean up dynamically allocated memory.
 */
uint16_t mac_frame_data_unpack(mac_frame_data *frame, uint8_t *buffer, uint16_t length);

/**
 * @brief Frees the allocated memory for extheaders.
 */
void mac_frame_extheaders_free(mac_extheader* hdr);



typedef enum mac_frame_types {
    MAC_FRAME_BEACON    = 0x0,
    MAC_FRAME_DATA      = 0x1,
    MAC_FRAME_ACK       = 0x2,
    MAC_FRAME_MAC_CMD   = 0x3
} mac_frame_types;

typedef enum mac_addr_modes {
    MAC_ADDR_MODE_NOT        = 0x0,
    MAC_ADDR_MODE_RESERVED   = 0x1,
    MAC_ADDR_MODE_SHORT      = 0x2,
    MAC_ADDR_MODE_LONG       = 0x3
} mac_addr_modes;


#define MHR_FC_GET_FRAME_TYPE(x)       ({ (x[0] >> 5) & 0x7;  })
#define MHR_FC_SET_FRAME_TYPE(x, n)     ({ x[0] = (x[0] & 0x1F) | ((n & 0x7) << 5); })
#define MHR_FC_GET_SECURITY_ENABLED(x)    ({ (x[0] >> 4) & 0x1;  })
#define MHR_FC_SET_SECURITY_ENABLED(x, n)   ({ x[0] = (x[0] & 0xEF) | ((n & 0x1) << 4); })
#define MHR_FC_GET_FRAME_PENDING(x)     ({ (x[0] >> 3) & 0x1;  })
#define MHR_FC_SET_FRAME_PENDING(x, n)    ({ x[0] = (x[0] & 0xF7) | ((n & 0x1) << 3); })
#define MHR_FC_GET_ACK_REQUEST(x)       ({ (x[0] >> 2) & 0x1;  })
#define MHR_FC_SET_ACK_REQUEST(x, n)    ({ x[0] = (x[0] & 0xFB) | ((n & 0x1) << 2); })
#define MHR_FC_GET_PAN_ID_COMPRESSION(x)  ({ (x[0] >> 1) & 0x1;  })
#define MHR_FC_SET_PAN_ID_COMPRESSION(x, n) ({ x[0] = (x[0] & 0xFD) | ((n & 0x1) << 1); })

#define MHR_FC_GET_DEST_ADDR_MODE(x)    ({ (x[1] >> 4) & 0x3;  })
#define MHR_FC_SET_DEST_ADDR_MODE(x, n)   ({ x[1] = (x[1] & 0xCF) | ((n & 0x3) << 4);  })
#define MHR_FC_GET_FRAME_VERSION(x)     ({ (x[1] >> 2) & 0x3;  })
#define MHR_FC_SET_FRAME_VERSION(x, n)    ({ x[1] = (x[1] & 0xF3) | ((n & 0x3) << 2);  })
#define MHR_FC_GET_SRC_ADDR_MODE(x)     ({ (x[1]) & 0x3;  })
#define MHR_FC_SET_SRC_ADDR_MODE(x, n)    ({ x[1] = (x[1] & 0xFC) | (n & 0x3);  })



typedef struct mac_frame_control {
    uint8_t     frame_type          : 3;
    uint8_t     security_enabled    : 1;
    uint8_t     frame_pending       : 1;
    uint8_t     ack_request         : 1;
    uint8_t     pan_id_compression  : 1;
    uint8_t     reserved            : 3;
    uint8_t     dest_addr_mode      : 2;
    uint8_t     frame_version       : 2;
    uint8_t     source_addr_mode    : 2;

} __attribute__((aligned(1),packed))  mac_frame_control;



/**
 * This is not transmitted in frames!
 * (Given implicitly by which headers are sent.)
 */
typedef enum mac_payload_type {
  NONE        = 0,  ///<@brief No type specified. Normally ignore.
  ETHERFRAME      = 1,  ///<@brief Tunneled ethernet frame
  BASE_SENSOR_DATA  = 2,  ///<@brief Sensor data from base station
  DONGLE_CMD      = 3,  ///<@brief Command to dongle
  DONGLE_CMD_ANSWER = 4   ///<@brief Answer from dongle (to command)
} mac_payload_type;


typedef enum base_sensor_data_type {
  SENSOR_DATE     = 0x1,  ///<@brief date of the sensor node
  SENSOR_POSITION   = 0x2,  ///<@brief gps position data
  SENSOR_COMPASS    = 0x4,  ///<@brief compass orientation
  SENSOR_WIND_SPEED   = 0x8,  ///<@brief wind speed
  SENSOR_WIND_DIR   = 0x10, ///<@brief wind direction (compensated by compass)
  SENSOR_WIND_DIR_RAW = 0x20  ///<@brief wind direction (uncompensated)
} base_sensor_data_type;


typedef enum dongle_command {
  IGNORE        = 0,  ///<@brief Nothing. Just ignore.
  TEST        = 1,  ///<@brief Nothing. Just ACK.
  CALIB_COMPASS   = 2,  ///<@brief Start the compass calibration.
  CALIB_COMPASS_STOP  = 3,  ///<@brief Stop the compass calibration.
  BASE_START_SENDING  = 4,  ///<@brief Start sending regular updates.
  BASE_STOP_SENDING = 5,  ///<@brief Stop sending regular updates.
  RF_SET_PARAMETERS = 6   ///<@brief Set parameters of RF communication.
} dongle_command;


/**
 * @brief  Initializes the mac layer. (i.e. create first random sequence number)
 */
void mac_init(void);

/**
 * @brief Tries to send a packet over PHY using \ref max_transmit_data.
 *
 * @return  True, if sent successfully, false otherwise.
 */
bool mac_transmit_packet(mac_frame_data* frame, bool new_seq);

/**
 * @brief Returns a random integer strict smaller than max.
 */
int mac_random(int max);


/**
 * @brief Generates and transmits an acknolodgement packet with given sequence number.
 */
bool mac_transmit_ack(uint8_t seq_no);


#endif // SKYMAC_H_
