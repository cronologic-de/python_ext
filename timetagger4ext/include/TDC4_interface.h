//
// Header file containing structs and #defines commonly used by header files of
// derived TDC4 device classes. The current driver version for xTDC4/TimeTagger4
// devices is %VERSION%
//

/*! \file
 *  \brief The functions provided by the API are declared in
 *  xTDC4_interface.h/TimeTagger4_interface.h.
 *
 *  The API is a DLL with C linkage.
 */
#ifndef TDC4_INTERFACE_H
#define TDC4_INTERFACE_H

#include "crono_interface.h"

// current version of the API
#define TDC4_API_VERSION 1

/**
 * current version of data structures used by the interface
 * When the definition of the structure is changed the version number is
 * incremented.
 */
#define TDC4_STATIC_INFO_VERSION 2
#define TDC4_FAST_INFO_VERSION 2
#define TDC4_PARAM_INFO_VERSION 1
#define TDC4_DEVICE_CONFIG_VERSION 2

/*! \brief the number of analog input channels
 *
 * used by
 * - @link confstruct xtdc4_configuration::dc_offset @endlink
 * - @link confstruct xtdc4_configuration::channel @endlink
 */
#define TDC4_TDC_CHANNEL_COUNT 4

/*! \brief the number of digital input channels
 *
 * used by
 * - @link confstruct xtdc4_configuration::lowres_channel @endlink
 */
#define TDC4_LOWRES_CHANNEL_COUNT 3

/*! \brief the number of timing generators
 *
 * used by
 * - @link confstruct xtdc4_configuration::tiger_block[] @endlink
 */
#define TDC4_TIGER_COUNT 5

/*! \brief the number of triggers.
 *
 *  Two per analog input, one per digital input plus some specials.
 *
 *  used by
 *  - @link confstruct xtdc4_configuration::trigger[] @endlink
 */
#define TDC4_TRIGGER_COUNT 16 // including "config" for AUTO and ONE

/*! \brief Valid values for tdc4_init_parameters.rclk_sel.
 */
#define TDC4_RCLK_37M5 0
#define TDC4_RCLK_75M 1
#define TDC4_RCLK_150M 2

/**
 * Fixed length of calibration date string.
 * calibration date format: YYYY-MM-DD hh:mm
 */
#define TDC4_CALIBRATION_DATE_LEN 20

/**
 * Fixed length of bitstream date string.
 * bitstream date format: YYYY-MM-DD hh:mm:ss
 */
#define TDC4_BITSTREAM_DATE_LEN 20

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifdef XTDC4_DRIVER_EXPORTS
#define XTDC4_API __declspec(dllexport)
#else
#define XTDC4_API __declspec(dllimport)
#endif
#else
#define XTDC4_API
#endif

#define crono_bool_t uint8_t

/*! \ingroup initparamsstruct
 *  \brief struct for the initialization of the xTDC4/TimeTagger4
 *
 *  this structure MUST be completely INITIALIZED
 */
typedef struct {
    /**
     * The version number is increased when the definition of the structure is
     * changed. The increment can be larger than one to match driver version
     * numbers or similar. Set to 0 for all versions up to first release.
     */
    /*! \brief the version number
     *
     *  must be set to @link apiversion XTDC4_API_VERSION @endlink
     */
    int version;

    /*! \brief The index in the list of xTDC4/TimeTagger4 boards that should be
     *  initialized.
     *
     *  There might be multiple boards in the system that are handled by
     *  this driver as reported by @link initialization xtdc4_count_devices()
     *  @endlink. This index selects one of them. Boards are enumerated
     *  depending on the PCIe slot. The lower the bus number and the lower the
     *  slot number the lower the card index.
     */
    int card_index;

    /*! \brief the global index in all cronologic devices
     *
     *  This 8 bit number is filled into each packet created by the
     *  board and is useful if data streams of multiple boards will be merged.
     *  If only xTDC4/TimeTagger4 cards are used this number can be set to the
     *  @link card_index card_index @endlink. If boards of different
     *  types that use a compatible data format are used in a system each
     *  board should get a unique id.
     */
    int board_id;

    /*! \brief The minimum size of the DMA buffer.
     *
     *  If set to 0 the default size of 16MB is used.
     *  For the xTDC4/TimeTagger4 only the first entry is used.
     */
    int64_t buffer_size[8];

    /*! \brief The type of buffer
     *
     *  Can be either allocated (only option currently) or physical.
     *  Please refer to group @link buffertype #defines of buffer_type
     *@endlink.
     */
    int buffer_type;

    /*! \brief The start address of the reserved memory.
     *
     *  The buffers will be allocated with the sizes given above. Make
     *  sure that the memory is large enough.
     */
    uint64_t buffer_address;

    /*! \brief A variant, for reconfiguring the chip for future extension
     *
     */
    int variant;

    /*! \brief A constant for the different devices of cronologic
     *  CRONO_DEVICE_*.
     *
     *  Initialized by @link initialization
     *  xtdc4_get_default_init_parameters()
     *  @endlink. Must be left unchanged. Please refer to @link devicetype
     *  #define for device_type @endlink.
     */
    int device_type;

    /*! \brief The update delay of the writing pointer after a packet has
     *been send over PCIe.
     *
     *  The base unit is 16 to 32 ns.
     */
    int dma_read_delay;

    /*! \brief Select external 10 MHz reference.
     *
     *  If set to 1 use external 10 MHz reference.
     *  If set to 0 use internal reference.
     */
    int use_ext_clock;

    /*! \brief Set THS788 RClk frequency, default is 150 MHz.
     *
     * 0: 37.5 MHz
     * 1: 75 MHz
     * 2: 150 MHz
     */
    int rclk_sel;
} tdc4_init_parameters;

/*! \ingroup staticinfo
 *  \brief Structure contains static information
 *
 *  This structure contains information about the board that does not change
 *  during run time. It is provided by the function @link statfuncts
 *  xtdc4_get_static_info() @endlink.
 */
typedef struct {

    int size; //!< The number of bytes occupied by the structure
    /**
     * A version number that is increased when the definition of the
     * structure is changed. The increment can be larger than one to match
     * driver version numbers or similar. Set to 0 for all versions up to
     * first release.
     */
    int version; //!< The version number
    /*! \brief ID of the board.
     *
     *  This value is passed to the constructor. It is reflected in the
     *  output data.
     */
    int board_id;
    /*! \brief encoded version number
     *
     *  The lower three bytes contain a triple level hierarchy of
     *  version numbers. E.g. 0x010103 codes version 1.1.3.
     *
     *  A change in the first digit generally requires
     *  a recompilation of user applications. Change in the second digit
     *  denote significant improvements or changes that don't break
     *  compatibility and the third digit changes with minor bugfixes and the
     *  like.
     */
    int driver_revision;
    /*! \brief SVN revision of the driver build
     */
    int driver_build_revision;
    /*  This can be read from a register.
     */
    int firmware_revision; //!< Revision number of the FPGA configuration.
    /*! \brief board revision number
     *
     *  The board revision number can be read from a register. It is a
     *  four bit number that changes when the schematic of the board is
     *  changed.
     *  - 0: Experimental first board Version. Labeled "Rev. 1"
     *  - 2: First commercial Version. Labeled "Rev. 2"
     */
    int board_revision;
    /*! \brief Describes the schematic configuration of the board.
     *
     *  The same board schematic can be populated in multiple variants.
     *  This is a eight bit code that can be read from a register.
     */
    int board_configuration;
    /* A number to track builds of the firmware in more detail than the
     * firmware revision. It changes with every change in the firmware, even
     * if there is no visible effect for the user. The subversion revision
     * number can be read from a register.
     */
    int subversion_revision; //!< Subversion revision id of the FPGA
                             //!< configuration.
    /* This is the chipID as read from the 16 bit ths788 chip id register
     * at SPI address 0x83. This value should be cached.
     */
    int chip_id; //!< 16bit factory ID of the TDC chip.
    /*! \brief Serial number
     *
     *  with year and running number in 8.24 format. The number is
     *identical to the one printed on the silvery sticker on the board.
     */
    int board_serial;
    unsigned int flash_serial_low;  //!< low 32 bits of 64 bit manufacturer
                                    //!< serial number of the flash chip
    unsigned int flash_serial_high; //!< high 32 bits of 64 bit manufacturer
                                    //!< serial number of the flash chip
    /*! \brief Flash data is valid and in use
     *
     *  If not 0 the driver found valid calibration data in the flash on
     *  the board and is using it.
     */
    int flash_valid;

    /*! \brief calibration date
     *
     *  DIN EN ISO 8601 string YYYY-MM-DD HH:DD describing the time when the
     *  card was calibrated.
     */
    char calibration_date[TDC4_CALIBRATION_DATE_LEN];
    /*! \brief bitstream date
     *  Bitstream creation date
     *
     *  DIN EN ISO 8601 string YYYY-MM-DD HH:DD:SS describing the time when the
     *  bitstream was created.
     */
    char bitstream_date[TDC4_BITSTREAM_DATE_LEN];
    /*! \brief Bin size of delay in ps
     *  The increment of the delay_config.delay field for TimeTagger4
     */
    double delay_bin_size;
    /*! \brief Auto trigger clock frequency
     *  The clock frequency of the auto trigger in Hz
     *  used for calculating the auto_trigger_period.
     */
    double auto_trigger_ref_clock;
    /*! \brief The number of bins in a rollover period
     *  This is a power of two (the maximum value of a hit timestamp
     *  is this value minus -1)
     */
    uint32_t rollover_period;
} tdc4_static_info;

/*! \ingroup fastinfo
 *  \brief contains fast dynamic information
 *
 *  This call returns a structure that contains dynamic information that can
 *  be obtained within a few microseconds.
 */
typedef struct {
    int size; //!< The number of bytes occupied by the structure
    /* A version number that is increased when the definition of the
     * structure is changed. The increment can be larger than one to match
     * driver version numbers or similar. Set to 0 for all versions up to first
     * release.
     */
    int version; //!< The version number
    /*! \brief Speed of the TDC fan.
     *
     *  Reports 0, if no fan is present
     */
    int tdc_rpm;
    /*! \brief Speed of the FPGA fan.
     *
     * Reports 0, if no fan is present.
     */
    int fpga_rpm;
    /*! \brief Alert bits from temperature sensor and the system monitor
     *
     *  - bit 0: THS788 over temperature alarm
     */
    int alerts;
    /*! \brief organizes power supply of PCIe lanes.
     */
    int pcie_pwr_mgmt;
    /*! \brief Current PCIe link speed.
     */
    int pcie_link_speed;
    /*! \brief Number of PCIe lanes the card uses.
     */
    int pcie_link_width;
    /*! \brief Maximum size for a single PCIe transaction
     *  in bytes. Depends on system configuration.
     */
    int pcie_max_payload;
} tdc4_fast_info;

/*! \ingroup paraminfo
 *  \brief contains configuration changes
 *
 *  Structure filled by @link statfuncts xtdc4_get_param_info() @endlink.
 *  This structure contains information that change indirectly due to
 *  configuration changes.
 */
typedef struct {
    int size; //!< The number of bytes occupied by the structure
    /* is increased when the definition of the structure is changed.
     * The increment can be larger than one to match driver version numbers or
     * similar. Set to 0 for all versions up to first release.
     */
    int version; //!< The version number
    /*! \brief Binsize (in ps) of the measured TDC data.
     *
     *  The TDC main clk is running at a frequency of 76.8 GHz
     *  resulting in a binsize of ~13.0208 ps for xTDC4
     *
     *  Timetagger4 bin size depends on the card.
     */
    double binsize;
    /*! \brief Board ID
     *
     *  The board uses this ID to identify itself in the output data
     *  stream. The ID takes values between 0 and 255.
     */
    int board_id;
    /*! \brief Number of channels in the current TDC mode
     *
     * Currently fixed at 4.
     */
    int channels;
    /*! \brief bit assignment of each enabled input channel.
     *
     *  Mask assigns a certain bit to each enabled input channel.
     */
    int channel_mask;
    int64_t total_buffer; //!< The total amount of DMA buffer in bytes.

    /*! \brief Binsize (in ps) of the packet timestamp
     * For xTDC4 this is 1666.6ps
     * For TimeTagger4 packet bin size depends on the generation of the board. with 500 ps for Gen1 and 100ps for Gen2
     * boards.
     */
    double packet_binsize;

    /*! \brief Quantization or the measurement resolution in ps
     * For xTDC4 this is ~13.0208 ps.
     * For TimeTagger4 the quantization depends on the card type and can range between 100 and 1000 ps.
     * 1G: 1000ps, 2G: 500ps, 1.25G: 800ps, 2.5G: 400ps, 5G: 200ps, 10G: 100ps
     * This means, that for 1.25G the lower 3 bits in the timestamps are zero.
     */
    double quantization;

} tdc4_param_info;

/*! \ingroup channel
 *  \brief Contains TDC channel settings
 */
typedef struct {
    crono_bool_t enabled;        //!< Enable TDC channel.
    crono_bool_t rising;         //!< Set whether to record rising or falling edges.
    crono_bool_t cc_enable;      //!< Enable carry chain TDC as backup
    crono_bool_t cc_same_edge;   //!< Set whether the carry chain TDC records the same
                                 //!< edge as THS788 (as backup) or opposite edge
    crono_bool_t ths788_disable; //!< Disable THS788 timestamps

    /*! \brief Veto function
     *
     *  only timestamps >= start are recorded.
     */
    uint32_t start;
    /*! \brief Veto function
     *
     *  only timestamps <= stop are recorded.
     */
    uint32_t stop;

} tdc4_channel;

/*! \ingroup lowres_channel
 *  \brief Contains digital channel settings
 */
typedef struct {
    crono_bool_t enabled; //!< Enable TDC channel.
    /*! \brief Veto function
     *
     *  only timestamps >= start are recorded.
     */
    uint32_t start;
    /*! \brief Veto function
     *
     *  only timestamps <= stop are recorded.
     */
    uint32_t stop;
} tdc4_lowres_channel;

/*! \ingroup delay_config
 *  \brief Contains configurable delay value for TimeTagger4
 */
typedef struct {
    /*! \brief delay in static_info.delay_bin_size (currently 200 ps)
     bins for a channel
     * must be >= 0 , maximum value of 1023 (204 ns)
     */
    uint32_t delay;
} tdc4_delay_config;

/*! \ingroup tiger
 *  \brief contains settings of timing generator
 */
typedef struct {
    crono_bool_t enable; //!< activates timing generator
    /*! \brief inverts output polarity
     *
     * default is set to false.
     */
    crono_bool_t negate;
    /*! \brief enables/disables retrigger setting
     *
     *  Default is set to false. If retriggering is enabled the timer is
     *  reset to the value of the start parameter, whenever the input signal
     *  is set while waiting to reach the stop time.
     */
    crono_bool_t retrigger;
    // default is true
    crono_bool_t extend;             //!< not implemented
    crono_bool_t enable_lemo_output; //!< enables the LEMO output
    /*! \brief Precursor
     *
     *  Number of 6.6ns clock cycles before the tiger output goes active
     *  relative to the trigger signal.
     */
    uint32_t start;
    /*! \brief postcursor
     *
     *  Number of 6.6ns clock cycles before the tiger output goes inactive
     *  relative to the trigger signal.
     */
    uint32_t stop;
    /*! \brief mask for choosing the trigger source
     *
     *  A bit mask with a bit set for all trigger sources that can
     *  trigger this channel. Default is XTDC4_TRIGGER_SOURCE_S. One can
     *  choose a from a source
     *  @link deftriggersource here @endlink.
     */
    int sources;
} tdc4_tiger_block;

/* low res trigger config */
/*!\ingroup trigger
 *  \brief contains trigger settings
 */
typedef struct {
    crono_bool_t falling; //!< triggers on falling edges
    crono_bool_t rising;  //!< triggers on rising edges
} tdc4_trigger;

/*! \ingroup confstruct Structure xtdc4_configuration
 *  \brief contains configuration information
 *
 *  This structure contains the configuration information. It is used in
 *  conjunction with
 *  @link conffuncts xtdc4_get_default_configuration(),
 *  xtdc4_get_current_configuration() and xtdc4_configure() @endlink
 *
 *  Internally it uses the structures @link trigger xtdc4_trigger @endlink
 *  and @link tiger xtdc4_tiger_block @endlink
 */
typedef struct {
    /*! \brief The number of bytes occupied by the structure
     */
    int size; //!< The number of bytes occupied by the structure
    /*! \brief A version number
     *
     *  that is increased when the definition of the structure is changed.
     *  The increment can be larger than one to match driver version numbers
     *  or similar. Set to 0 for all versions up to first release.
     */
    int version;
    /*! \brief TDC mode
     *
     *  can be @link tdcmode grouped or continous @endlink. Currently
     *  supported: grouped.
     */
    int tdc_mode;
    /*! \brief rising or falling edge trigger
     *
     *  whether to sync the TDC on the rising or falling edge
     */
    crono_bool_t start_rising;
    /*! \brief Set DAC channels for S, A - D.
     *
     *  dc_offset[0]     : Start
     *
     *  dc_offset[1 - 4] : A - D
     *
     *  Set to a value between -1.65V and +0.85V. This should be close
     *  to 50% of the height of your pulses on the inputs. Examples for
     *  various signaling standards are defined in @link defdcoffset #defines
     *  for dc_offset @endlink. The inputs are AC coupled. This means that for
     *  pulse inputs the absolute voltage is not important. it is rather the
     *  relative pulse amplitude that causes the input circuits to switch.
     *  dc_offset for an input must be set to the relative switching voltage
     *  for the input standard in use. If the pulses are negative, a negative
     *  switching threshold must be set and vice versa.
     */
    double dc_offset[TDC4_TDC_CHANNEL_COUNT + 1];
    tdc4_trigger trigger[TDC4_TRIGGER_COUNT];                      //!< Configuration of external
                                                                   //!< trigger sources
    tdc4_tiger_block tiger_block[TDC4_TIGER_COUNT];                //!< configuration of the
                                                                   //!< timing generator
    tdc4_channel channel[TDC4_TDC_CHANNEL_COUNT];                  //!< configure polaritiy, type and
                                                                   //!< threshold for the TDC channels
    tdc4_lowres_channel lowres_channel[TDC4_LOWRES_CHANNEL_COUNT]; //!< configure polaritiy,
                                                                   //!< type and threshold
                                                                   //!< for the digital
                                                                   //!< channels
    /** \brief component to create a trigger either periodically or
     *  randomly.
     *
     *  To be exact, there are two parameters M = @link auto_trigger_period
     *  auto_trigger_period @endlink and N = @link
     *  auto_trigger_random_exponent auto_trigger_random_exponent @endlink
     *  that result in a distance between triggers of
     *
     *          T = 1 + M + [1...2^N]
     *
     *  clock cycles.
     *
     *          0 <= M < 2^32
     *
     *          0 <= N < 32
     *
     *  there is no enable or reset as the usage of this trigger can be
     *  configured in the channels.
     *  The clock frequency is specified in static_info.auto_trigger_ref_clock
     */
    ///@{
    uint32_t auto_trigger_period;
    /** \brief component to create a trigger either periodically or
     *  randomly.
     *
     *  To be exact, there are two parameters M = @link auto_trigger_period
     *  auto_trigger_period @endlink and N = @link
     *  auto_trigger_random_exponent auto_trigger_random_exponent @endlink
     *  that result in a interval between triggers of
     *
     *          T = M + [1...2^N] - 1
     *
     *  clock cycles.
     *
     *          10 <= M < 2^31
     *
     *          0 <= N < 32
     *
     *  there is no enable or reset as the usage of this trigger can be
     *  configured in the channels.
     */
    uint32_t auto_trigger_random_exponent;
    ///@}
    /** \brief configurable delay of input channels. (currently only used by TimeTagger4)
     * index 0 is start channel, 1-4 are A-D
     */
    tdc4_delay_config delay_config[TDC4_TDC_CHANNEL_COUNT + 1];
    /*! \brief ignore packets which do not contain hits. These packets are not written to the ringbuffer.
     */
    uint32_t ignore_empty_packets;

} tdc4_configuration;

/*! \ingroup readin
 *  \brief The parameters of the read commands
 */
typedef struct {
    crono_bool_t acknowledge_last_read; //!< xtdc4/timetagger_read automatically
                                        //!< acknowledges packets from the last read
} tdc4_read_in;

/*! \ingroup strucreadout
 *  \brief struct for the read out of the packets
 */
typedef struct {
    /*! \brief pointer to the first packet
     *
     *  that was captured by the call of @link readout xtdc4_read
     *@endlink
     */
    volatile crono_packet *first_packet;
    /*! \brief Address of header of the last packet in the buffer.
     */
    volatile crono_packet *last_packet;
    /*! \brief error code
     *
     *  The assignments of the error codes can be found @link deferror here
     *
     *@endlink.
     */
    int error_code;
    /*! \brief The last error in human readable form, possibly with
     *  additional information on the error.
     */
    const char *error_message;
} tdc4_read_out;
#ifdef __cplusplus
}
#endif

#endif
