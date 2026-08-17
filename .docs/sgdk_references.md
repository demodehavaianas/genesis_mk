SGDK API Reference version 2.11
// Source: https://github.com/Stephane-D/SGDK
// This includes: headers (inc/), readme.md, changelog.txt
// Contains all API functions, structs, and Doxygen documentation

# File Summary

## Purpose
This file contains a packed representation of a subset of the repository's contents that is considered the most important context.
It is designed to be easily consumable by AI systems for analysis, code review,
or other automated processes.

## File Format
The content is organized as follows:
1. This summary section
2. Repository information
3. Directory structure
4. Repository files (if enabled)
5. Multiple file entries, each consisting of:
  a. A header with the file path (## File: path/to/file)
  b. The full contents of the file in a code block

## Usage Guidelines
- This file should be treated as read-only. Any changes should be made to the
  original repository files, not this packed version.
- When processing this file, use the file path to distinguish
  between different files in the repository.
- Be aware that this file may contain sensitive information. Handle it with
  the same level of security as you would the original repository.

## Notes
- Some files may have been excluded based on .gitignore rules and Repomix's configuration
- Binary files are not included in this packed representation. Please refer to the Repository Structure section for a complete list of file paths, including binary files
- Only files matching these patterns are included: inc/**/*.h, readme.md, changelog.txt
- Files matching patterns in .gitignore are excluded
- Files matching default ignore patterns are excluded
- Security check has been disabled - content may contain sensitive information
- Files are sorted by Git change count (files with more changes are at the bottom)

# Directory Structure
````
inc/
  ext/
    flash-save/
      flash.h
      saveman.h
    minimusic/
      minimus.h
    mw/
      16c550.h
      comm.h
      gamejolt.h
      jsmn.h
      json.h
      lsd.h
      megawifi.h
      mw-msg.h
      ssf_ed_pro.h
      ssf_ed_x7.h
    serial/
      buffer.h
      serial.h
    stb/
      stb_sprintf.h
    console.h
    everdrive.h
    fat16.h
    link_cable.h
  snd/
    pcm/
      snd_dpcm2.h
      snd_pcm.h
      snd_pcm4.h
      tab_vol.h
    smp_null_dpcm.h
    smp_null.h
    sound.h
    xgm.h
    xgm2.h
  asm.h
  bmp.h
  config.h
  dma.h
  genesis.h
  joy.h
  kdebug.h
  map.h
  mapper.h
  maths.h
  maths3D.h
  memory_base.h
  memory.h
  object.h
  pal.h
  pool.h
  psg.h
  sprite_eng_legacy.h
  sprite_eng.h
  sram.h
  string.h
  sys.h
  tab_cnv.h
  task_cst.h
  task.h
  timer.h
  tools.h
  types.h
  vdp_bg.h
  vdp_pal.h
  vdp_spr.h
  vdp_tile.h
  vdp.h
  vram.h
  ym2612.h
  z80_ctrl.h
changelog.txt
readme.md
````

# Files

## File: inc/ext/flash-save/flash.h
````c
/**
 * \file flash.h
 * \brief Low level NOR flash read/erase/write interface.
 * \author Jesus Alonso (doragasu)
 * \date 08/2022
 *
 * This module allows reading, erasing and writing data from/to the
 * cartridge flash chip. It should work with any flash chip compatible with
 * the AMD format, and implementing a CFI interface. It has been tested
 * successfully with S29GL032 and W29GL032C chips. Also it is required that
 * the cart has the #WE line connected to the flash chip (that should be the
 * case on most flash carts, but is not guaranteed).
 *
 * \note The module has some limitations: addresses must be even, data pointers
 * must be word aligned, and some functions (flash_init(), flash_sector_erase()
 * and flash_program()) must be run with interrupts disabled and Z80 stopped.
 * Please also take into account that flash suffers from wear, so if you use
 * this e.g. for saving data, avoid saving very frequently and/or implement
 * a wear leveling algorithm to avoid damaging the chip.
 *
 * \note Although it is not a requirement for this module to work, it is
 * strongly recommended that the cart you use also has the #RESET pin connected
 * to the console #RESET signal. Otherwise, if the user pushes the RESET
 * button while the flash chip is being erased/programmed, the machine will
 * most likely freeze, requiring another press of the RESET button or a power
 * cycle.
 */

#ifndef __FLASH_H__
#define __FLASH_H__

// I usually include stdint.h, but SGDK defines some stdint types here:
#include "types.h"

// Maximum number of flash regions. CFI standard allows up to 4
#define FLASH_REGION_MAX 4

/**
 * \brief Metadata of a flash region, consisting of several sectors.
 * \{/
 */
struct flash_region {
	uint32_t start_addr;    //< Sector start address
	uint16_t num_sectors;   //< Number of sectors
	uint16_t sector_len;	//< Sector length in 256 byte units
};
/** \} */

/**
 * \brief Metadata of a flash chip, describing memory layout.
 * \{
 */
struct flash_chip {
	uint32_t len;                  //< Length of chip in bytes
	uint16_t num_regions;          //< Number of regions in chip
	struct flash_region region[FLASH_REGION_MAX]; //< Region data array
};
/** \} */

/**
 * \brief Initialise flash chip. Call this function only once, before any
 * other function in the module.
 *
 * \warning This function must be called with interrupts disabled and
 * Z80 stopped.
 */
int16_t flash_init(void);

/**
 * \brief Deinitialise flash chip.
 */
void flash_deinit(void);

/**
 * \brief Get chip metadata.
 *
 * \return An immutable reference to the chip metadata.
 */
const struct flash_chip *flash_metadata_get(void);

/**
 * \brief Erases a flash sector. Erased sectors will be read as 0xFF.
 *
 * \param[in] addr Any even address contained in the sector to erase.
 *
 * \note This function erases a complete sector. Use flash_sector_limits()
 * function to obtain the sector limits that will be erased.
 * \warning If addr is not even, this function can lock the machine.
 * \warning This function must be called with interrupts disabled and
 * Z80 stopped.
 *
 * \return The address of the next sector to the one erased on success,
 * 0 on error.
 */
uint32_t flash_sector_erase(uint32_t addr);

/**
 * \brief Programs (writes) a data buffer to the specified flash address.
 *
 * \param[in] addr Address to which data will be programmed.
 * \param[in] data Buffer containing the data to program.
 * \param[in] len  Length of the data buffer to write.
 *
 * \return The number of written bytes on success, 0 on error.
 *
 * \warning Before using this function, make sure the address range to program
 * is erased. Otherwise this function can lock the machine.
 * \warning If addr is not even, this function can lock the machine.
 * \warning If data is not word aligned, this function can lock the machine.
 * \warning If data is not in work RAM, this function can lock the machine.
 * \warning If len is odd, the last byte will not be written.
 * \warning This function must be called with interrupts disabled and
 * Z80 stopped.
 */
uint16_t flash_program(uint32_t addr, const uint8_t *data, uint16_t len);

/**
 * \brief Reads data from the specified flash address.
 *
 * \param[in]  addr Address to read data from.
 * \param[out] data Buffer used to copy readed data.
 * \param[in]  len  Length of the data block to read.
 *
 * \return The number of read bytes.
 *
 * \warning addr must be even.
 * \warning data must be word aligned.
 * \warning If len is odd, the last byte will not be read.
 */
int16_t flash_read(uint32_t addr, uint8_t *data, uint16_t len);

/**
 * \brief Copy data from a flash region to other region also in flash.
 *
 * \param[in] dst Destination address to copy data to.
 * \param[in] src Source address to copy data from
 * \param[in] len Length of the data block to copy.
 *
 * \return The number of copied bytes.
 *
 * \warning dst and src must be even.
 * \warning If len is odd, the last byte will not be copied.
 * \warning Before using this function, make sure the dst region is erased.
 * Otherwise this function can lock the machine.
 * \warning This function must be called with interrupts disabled and
 * Z80 stopped.
 */
int16_t flash_copy(uint32_t dst, uint32_t src, uint16_t len);

/**
 * \brief Obtains the sector limits (start of sector, start of next sector)
 * corresponding to the specified address.
 *
 * \return 0 on success, -1 if input address does not fit in the chip.
 *
 * \param[in]  addr  Address corresponding to the sector to get info from.
 * \param[out] start Start address of the sector. Can be NULL if this
 *             parameter is not needed.
 * \param[out] next  Start address of the sector next to the current one.
 *                   Can be NULL if this parameter is not needed.
 */
int16_t flash_sector_limits(uint32_t addr, uint32_t *start, uint32_t *next);

#endif
````

## File: inc/ext/flash-save/saveman.h
````c
/**
 * \file saveman.h
 * \brief Save manager for flash memory chips supported by the flash module.
 * \author Jesus Alonso (doragasu)
 * \date 10/2022
 *
 * This module allows saving data to non volatile flash chips. It supports as
 * many save slots as required, and arbitrary data lengths can be written to
 * each slot. This allows defining for example a 5 slot layout that uses slot
 * 0 for the game configuration, slot 1 to keep high scores and slots 2 to 4 to
 * save games. Each time data is written to a slot, it replaces whatever was
 * written to the slot previously.
 *
 * The module uses two sectors from the flash chip in order to implement flash
 * wear leveling and safe save: the save file is not lost even if there is a
 * power cut during save operation. Note that typical flash chips used in carts
 * such as Krikkzz Flash Kit Cart use 64 KiB sectors at the end of the flash, so
 * if you want to use this module, it will use the top 128 KiB of the ROM
 * address space by default. Keep this in mind because if code/data falls there,
 * it will be erased by this module routines, breaking your game! There is also
 * a way to use 8 KiB sectors from these carts, in order to waste 16 KiB instead
 * of 128 KiB, but it is a bit tricky, so unless you are very tight on ROM
 * space, better use the default layout. Using the default 128 KiB layout has two
 * additional benefits: wear is reduced and bigger save lengths are possible.
 *
 * Note when you define the slot layout, you must make sure all of them fit in
 * a single sector (plus a bit more room for internal headers). So e.g. if you
 * use 64 KiB sectors and have 5 slots with 2 KiB each, that will fit without a
 * problem, but if you use 8 KiB sectors, that config will not fit (5 x 2 KiB =
 * 10 KiB, bigger than the 8 KiB available in the sector). Make sure your
 * sector configuration is correct, because if it isn't, the module will work
 * initially, but will fail when more slots are saved and they do not fit in
 * one sector.
 *
 * Typical usage of the module is as follows:
 *
 * 1. Call sm_init() to initialize the module. Make sure it is always called
 *    with the same parameters.
 * 2. Call sm_load() to load data, sm_save() to save data and sm_delete() to
 *    delete saved data.
 * 3. In case you need to perform a factory reset, call sm_clear().
 *
 * The module has been carefully written, but some of its logic is a bit
 * complex, so make sure you test it thoroughly just in case there is a bug
 * lurking inside!
 *
 * \note Excepting sm_load(), all module functions stop Z80 and disable
 * interrupts until they return. Thus you shall make sure no music or sound
 * is playing when you call them.
 * \warning Save/load buffers must be word aligned or you will get a
 * SM_STAT_INVALID_PARAM_ERR error. Variables using types with a length equal
 * or bigger than 2 bytes will be aligned (same as structs with at least one
 * variable obeying this rule), but if variable has a type with length 1 (such
 * as a char array), to make sure it is aligned, you can use the attribute:
 * __attribute__((aligned(2))) in the declaration.
 * \warning If any save write operation returns a SM_STAT_HW_ERR, something
 * weird has occurred. Your best chance if this happens is to either restart
 * the console or at least deinit and init this module to let the init routine
 * try correcting the mess and avoid further data loss.
 */

#ifndef _SAVEMAN_H_
#define _SAVEMAN_H_

#include "types.h"

/**
 * \brief Status code for some of the function calls
 * \{/
 */
enum {
	SM_STAT_PARAM_ERR = -3,   ///< Invalid parameter in function call
	SM_STAT_HW_ERR = -2,      ///< Flash chip hardware related error
	SM_STAT_ERR = -1,         ///< Generic error
	SM_STAT_OK = 0,           ///< Success
	SM_STAT_WARN_NO_DATA = 1, ///< Success, chip has no data saved
};
/** \} */

/**
 * \brief Initialise save manager module. Must be called once before invoking
 * any other function in the module.
 *
 * \param[in] num_slots Number of save slots to support.
 *
 * \return Status code:
 * - SM_STAT_OK if initialisation was successful and flash has data to load.
 * - SM_STAT_WARN_NO_DATA if initialisation was successful but there is no
 *   save data to load.
 * - SM_STAT_PARAM_ERR if function was called with invalid parameters.
 * - SM_STAT_HW_ERR if the flash chip failed to perform any operation.
 * - SM_ERR if other unrecoverable error was found.
 *
 * \note This function internally calls flash_init(), you must not call it
 * by yourself.
 * \note This function halts Z80 and disables interrupts until done.
 * \note This module uses the two adjacent sectors in the memory range
 * specified in the ROM header at offset 0x1B4. If the range does not exactly
 * match two flash adjacents sectors, function will return SM_STAT_PARAM_ERR.
 * Make sure you read and understand the save manager README.md file.
 * \warning The two flash sectors used by this module must not have code. If
 * there is code in there, it will be deleted. Don't make your game erase
 * itself!
 */
int16_t sm_init(uint8_t num_slots);

/**
 * \brief Deinitialises save manager module. Usually you do not have to use
 * this function.
 */
void sm_deinit(void);

/**
 * \brief Load previously saved data for the specified slot.
 *
 * \param[in]  slot Slot from which we want to load the data.
 * \param[out] save_data Buffer in which loaded data will be copied.
 * \param[in]  len Length of the data to load.
 *
 * \return A positive number with the loaded data length, or a negative
 * error status code:
 * - SM_STAT_PARAM_ERR: Invalid parameter in invocation.
 * - SM_STAT_ERR: The requested slot has no data available.
 *
 * \note Readed bytes can be less than len if there is an error (e.g. 0 if the
 * slot has no data, or less than len if less data than requested was saved).
 */
int16_t sm_load(uint8_t slot, void *save_data, uint16_t len);

/**
 * \brief Save data to the specified slot.
 *
 * \param[in] slot Slot to which we want to save the data.
 * \param[in] save_data Data buffer to save in the slot.
 * \param[in] len Length of the data to save.
 *
 * \return Status code:
 * - SM_STAT_OK: Save operation successful.
 * - SM_STAT_PARAM_ERR: Invalid parameter in invocation.
 * - SM_STAT_HW_ERR: Save operation failed due to flash error.
 * - SM_STAT_ERR: Other unrecoverable error.
 *
 * \note Readed bytes can be less than len if there is an error.
 * \note This function halts Z80 and disables interrupts until it done.
 */
int16_t sm_save(uint8_t slot, const void *save_data, uint16_t len);

/**
 * \brief Delete data from specified slot
 *
 * \param[in] slot Slot to delete.
 *
 * \return Status code:
 * - SM_STAT_OK: Delete operation successful.
 * - SM_STAT_PARAM_ERR: Invalid parameter in invocation.
 * - SM_STAT_HW_ERR: Delete operation failed due to flash error.
 * - SM_STAT_ERR: Other unrecoverable error.
 *
 * \note This function halts Z80 and disables interrupts until it done.
 */
int16_t sm_delete(uint8_t slot);

/**
 * \brief Deletes all save data from all save slots, and reinitializes the
 * module to support the requested number of save slots.
 *
 * \param[in] num_slots Number of slots to support after data clear.
 *
 * \return Status code:
 * - SM_STAT_OK: Clear operation successful.
 * - SM_STAT_HW_ERR: Flash chip failed to erase.
 *
 * \note This function halts Z80 and disables interrupts until it done.
 */
int16_t sm_clear(uint8_t num_slots);

#endif
````

## File: inc/ext/minimusic/minimus.h
````c
#ifndef MINIMUS_H_
#define MINIMUS_H_

// SGDK replacement for stdint.h
#include "types.h"

#define MINIMUSIC_MEMORY_BARRIER() asm volatile ("" : : : "memory")

static inline void MINIMUSIC_Z80_GUARD_BEGIN() {
   volatile uint16_t *port = (uint16_t*)(0xA11100);
   MINIMUSIC_MEMORY_BARRIER();
   *port = 0x100;
   MINIMUSIC_MEMORY_BARRIER();
}

static inline void MINIMUSIC_Z80_GUARD_END() {
   volatile uint16_t *port = (uint16_t*)(0xA11100);
   MINIMUSIC_MEMORY_BARRIER();
   *port = 0x000;
   MINIMUSIC_MEMORY_BARRIER();
}

#define MINIMUSIC_STATUS_BGM        0x01

void minimusic_init(const void *, uint16_t);
void minimusic_sendcmd(uint8_t);
uint8_t minimusic_get_status(void);

#endif
````

## File: inc/ext/mw/16c550.h
````c
/************************************************************************//**
 * \brief Simple 16C550 UART chip driver.
 *
 * \author Jesus Alonso (doragasu)
 * \date   2016
 * \defgroup 16C550 16c550
 * \{
 ****************************************************************************/

#ifndef _16C550_H_
#define _16C550_H_

#include "types.h"
#include "mw-msg.h"

/// 16C550 UART base address
#define UART_BASE		0xA130C1

/// Clock applied to 16C550 chip. Currently using 24 MHz crystal
#define UART_CLK		24000000LU

/// Desired baud rate. Maximum achievable baudrate with 24  MHz crystal
/// is 24000000/16 = 1.5 Mbps
#define UART_BR			1500000LU
//#define UART_BR			500000LU
//#define UART_BR			750000LU
//#define UART_BR			115200

/// Length of the TX FIFO in bytes
#define UART_TX_FIFO_LEN		16

#define UART_BUFLEN	1460

/// Division with one bit rounding, useful for divisor calculations.
#define DivWithRounding(dividend, divisor)	((((dividend)*2/(divisor))+1)/2)
/// Value to load on the UART divisor, high byte
#define UART_DLM_VAL	(DivWithRounding(UART_CLK, 16 * UART_BR)>>8)
//#define UART_DLM_VAL	((UART_CLK/16/UART_BR)>>8)
/// Value to load on the UART divisor, low byte
#define UART_DLL_VAL	(DivWithRounding(UART_CLK, 16 * UART_BR) & 0xFF)
//#define UART_DLL_VAL	((UART_CLK/16/UART_BR)&0xFF)

/** \addtogroup UartRegs UartRegs
 *  \brief 16C550 UART registers
 *  \note Do NOT access IER, FCR, LCR and MCR directly, use Set/Get functions.
 *        Remaining registers can be directly accessed, but meeting the
 *        read only/write only restrictions.
 *  \{
 */
/// Receiver holding register. Read only.
#define UART_RHR	(*((volatile uint8_t*)(UART_BASE +  0)))
/// Transmit holding register. Write only.
#define UART_THR	(*((volatile uint8_t*)(UART_BASE +  0)))
/// Interrupt enable register. Write only.
#define UART_IER	(*((volatile uint8_t*)(UART_BASE +  2)))
/// FIFO control register. Write only.
#define UART_FCR	(*((volatile uint8_t*)(UART_BASE +  4)))
/// Interrupt status register. Read only.
#define UART_ISR	(*((volatile uint8_t*)(UART_BASE +  4)))
/// Line control register. Write only.
#define UART_LCR	(*((volatile uint8_t*)(UART_BASE +  6)))
/// Modem control register. Write only.
#define UART_MCR	(*((volatile uint8_t*)(UART_BASE +  8)))
/// Line status register. Read only.
#define UART_LSR	(*((volatile uint8_t*)(UART_BASE + 10)))
/// Modem status register. Read only.
#define UART_MSR	(*((volatile uint8_t*)(UART_BASE + 12)))
/// Scratchpad register.
#define UART_SPR	(*((volatile uint8_t*)(UART_BASE + 14)))
/// Divisor latch LSB. Acessed only when LCR[7] = 1.
#define UART_DLL	(*((volatile uint8_t*)(UART_BASE +  0)))
/// Divisor latch MSB. Acessed only when LCR[7] = 1.
#define UART_DLM	(*((volatile uint8_t*)(UART_BASE +  2)))
/** \} */


/// Structure with the shadow registers.
typedef struct {
	uint8_t IER;	///< Interrupt Enable Register
	uint8_t FCR;	///< FIFO Control Register
	uint8_t LCR;	///< Line Control Register
	uint8_t MCR;	///< Modem Control Register
} UartShadow;

/// Uart shadow registers. Do NOT access directly!
extern UartShadow sh;

/** \addtogroup UartOuts UartOuts
 *  \brief Output pins controlled by the MCR UART
 *  register.
 *  \{ */
#define UART_MCR__DTR		0x01	///< Data Terminal Ready.
#define UART_MCR__RTS		0x02	///< Request To Send.
#define UART_MCR__OUT1		0x04	///< GPIO pin 1.
#define UART_MCR__OUT2		0x08	///< GPIO pin 2.
/** \} */

/** \addtogroup UartIns UartIns
 *  \brief Input pins readed in the MSR UART register.
 *  \{ */
#define UART_MSR__DSR		0x20	///< Data Set Ready
/** \} */


/** \addtogroup mw_ctrl_pins mw_ctrl_pins
 *  \brief Pins used to control WiFi module.
 *  \{ */
#define MW__RESET UART_MCR__DTR   ///< Reset out.
#define MW__PRG   UART_MCR__RTS   ///< Program out.
#define MW__DCD   UART_MSR__DSR   ///< Data request in.
/** \} */

/************************************************************************//**
 * \brief Initializes the driver. The baud rate is set to UART_BR, and the
 *        UART FIFOs are enabled. This function must be called before using
 *        any other API call.
 ****************************************************************************/
void uart_init(void);

/************************************************************************//**
 * \brief Check if uart is present.
 ****************************************************************************/
bool uart_is_present(void);

/************************************************************************//**
 * \brief Checks if UART transmit register/FIFO is ready. In FIFO mode, up to
 *        16 characters can be loaded each time transmitter is ready.
 *
 * \return TRUE if transmitter is ready, FALSE otherwise.
 ****************************************************************************/
bool uart_tx_ready();

/************************************************************************//**
 * \brief Checks if UART receive register/FIFO has data available.
 *
 * \return TRUE if at least 1 byte is available, FALSE otherwise.
 ****************************************************************************/
bool uart_rx_ready();

/************************************************************************//**
 * \brief Sends a character. Please make sure there is room in the transmit
 *        register/FIFO by calling uart_rx_ready() before using this function.
 *
 * \return Received character.
 ****************************************************************************/
void uart_putc(u8 c);

/************************************************************************//**
 * \brief Returns a received character. Please make sure data is available by
 *        calling uart_rx_ready() before using this function.
 *
 * \return Received character.
 ****************************************************************************/
u8 uart_getc();

/************************************************************************//**
 * \brief Sets a value in IER, FCR, LCR or MCR register.
 *
 * \param[in] reg Register to modify (IER, FCR, LCR or MCR).
 * \param[in] val Value to set in IER, FCR, LCR or MCR register.
 ****************************************************************************/
#define uart_set(reg, val)	do{sh.reg = (val);UART_##reg = (val);}while(0)

/************************************************************************//**
 * \brief Gets value of IER, FCR, LCR or MCR register.
 *
 * \param[in] reg Register to read (IER, FCR, LCR or MCR).
 * \return The value of the requested register.
 ****************************************************************************/
#define uart_get(reg)		(sh.reg)

/************************************************************************//**
 * \brief Sets bits in IER, FCR, LCR or MCR register.
 *
 * \param[in] reg Register to modify (IER, FCR, LCR or MCR).
 * \param[in] val Bits set in val, will be set in reg register.
 ****************************************************************************/
#define uart_set_bits(reg, val)	do{sh.reg |= (val);			\
	UART_##reg = sh.reg;}while(0)

/************************************************************************//**
 * \brief Clears bits in IER, FCR, LCR or MCR register.
 *
 * \param[in] reg Register to modify (IER, FCR, LCR or MCR).
 * \param[in] val Bits set in val, will be cleared in reg register.
 ****************************************************************************/
#define uart_clr_bits(reg, val)	do{sh.reg &= ~(val);			\
	UART_##reg = sh.reg;}while(0)

/************************************************************************//**
 * \brief Reset TX and RX FIFOs.
 ****************************************************************************/
void uart_reset_fifos();

/************************************************************************//**
 * \brief Test Connection with registers
 *
 * \param[in] reg Register to modify
 * \param[in] val Bits set in val, will be readed from reg register.
 ****************************************************************************/
#define uart_test(reg, val) reg = val; 
//
//  if (reg != val) return MW_ERR

void uart_reset(void);

void uart_start(void);

/************************************************************************//**
 * \brief Sends syncrhonization frame.
 *
 * This function sends a chunk of 0x55 bytes to help physical layer to
 * synchronize. It is usually not necessary to use this function, but might
 * help some UART chips to compute an accurate clock.
 ****************************************************************************/
void uart_line_sync(void);

#endif /*_16C550_H_*/

/** \} */
````

## File: inc/ext/mw/comm.h
````c
/************************************************************************
 * \brief COMM driver Interface.
 *
 * \author Juan Antonio Ruiz (PaCHoN)
 * \date   2024-2025
 * \defgroup COMM COMM
 * \brief
 *      Interface for COMM communication over Everdrive and EverdrivePro.
*************************************** */
#ifndef _COMM_H_
#define _COMM_H_

#include "types.h"
/**************************************** */

typedef struct CommDriver {
    void (*init)(void);
    bool (*is_present)(void);
    u8 (*read_ready)(void);
    u8 (*read)(void);
    u8 (*write_ready)(void);
    void (*write)(u8 data);
    
    u16 buff_length;
    u16 fifo_length;
    char* mode;
} CommDriver;

/**
 * \brief Get the current receive buffer length.
 * \return Number of bytes available to read in the receive buffer.
 */
u16 comm_get_buffer_length(void);

/**
 * \brief Get the current transmit FIFO length.
 * \return Number of bytes currently in the transmit FIFO.
 */
u16 comm_get_tx_fifo_length(void);

/**
 * \brief Initialize the COMM communication interface.
 * 
 * Must be called before using any other COMM functions.
 */
void comm_init(void);

/**
 * \brief Write a single byte to the COMM interface.
 * \param data The byte to transmit.
 */
void comm_write(u8 data);

/**
 * \brief Check if data is available to read.
 * \return TRUE if data is available in the receive buffer, FALSE otherwise.
 */
bool comm_read_ready(void);

/**
 * \brief Check if the COMM interface is ready to accept data for transmission.
 * \return TRUE if write buffer has space available, FALSE otherwise.
 */
bool comm_write_ready(void);

/**
 * \brief Read a single byte from the COMM interface.
 * \return The byte read from the receive buffer.
 * \note Call comm_read_ready() first to ensure data is available.
 */
u8 comm_read(void);

/**
 * \brief Get the current COMM driver.
 * \return A string describing the current communication mode.
 */
const CommDriver* comm_get_driver(void);

/**
 * \brief Get the list of available COMM drivers.
 * \return The number of available COMM drivers and a pointer to the array of CommDriver structures.
 */
size_t comm_get_drivers(const CommDriver** drivers);

/**
 * \brief Set the active COMM driver.
 * \param driver Pointer to the CommDriver structure to set as active.
 */
void comm_set_driver(const CommDriver* driver);


#endif /*_COMM_H_*/
````

## File: inc/ext/mw/gamejolt.h
````c
/************************************************************************//**
 * \file
 *
 * \brief GameJolt Game API implementation for MegaWiFi.
 *
 * \defgroup gamejolt gamejolt
 * \{
 *
 * \brief GameJolt Game API implementation for MegaWiFi.
 *
 * Implementation of the version 1.2 of the GameJolt Game API, supporting
 * scoreboards, trophies, friends, etc. For more information, read the original
 * GameJolt Game API documentation: https://gamejolt.com/game-api/doc
 *
 * \author Jesus Alonso (doragasu)
 * \date 2020
 *
 * \note This module requires setting MODULE_MEGAWIFI to 1 in config.h and
 * rebuilding the library (if you had to change them).
 * \note The module uses X Macros, making Doxygen documentation of some
 * elements a bit complicated. Sorry about that.
 ****************************************************************************/

#ifndef _GAMEJOLT_H_
#define _GAMEJOLT_H_

#include "types.h"

/// \brief Module error codes.
///
/// On success value is 0. On error all values are
/// negative, with the exception of the HTTP status error. In this case,
/// the value reported is the HTTP status code obtained (e.g. 500).
enum gj_error {
	GJ_ERR_NONE      =  0,	///< No error
	GJ_ERR_PARAM     = -1,	///< Missing or invalid parameter
	GJ_ERR_REQUEST   = -2,	///< Request failed (network or server down?)
	GJ_ERR_RECEPTION = -3,	///< Failed to receive response data
	GJ_ERR_RESPONSE  = -4,	///< Response has no success:"true" head
	GJ_ERR_PARSE     = -5	///< Error while parsing response data
};

/// \brief Difficulty to achieve the trophy
enum gj_trophy_difficulty {
	GJ_TROPHY_TYPE_BRONZE = 0,	///< Bronze trophy (easiest)
	GJ_TROPHY_TYPE_SILVER,		///< Silver trophy (medium)
	GJ_TROPHY_TYPE_GOLD,		///< Gold trophy (hard)
	GJ_TROPHY_TYPE_PLATINUM,	///< Platinum trophy (hardest)
	GJ_TROPHY_TYPE_UNKNOWN		///< Unknown, just for errors
};

/// \brief Operations supported by gj_data_store_update() function
enum gj_data_store_update_operation {
	GJ_OP_ADD = 0,	///< Adds the value to item
	GJ_OP_SUBTRACT,	///< Subtracs the value from item
	GJ_OP_MULTIPLY,	///< Multiplies value by item
	GJ_OP_DIVIDE,	///< Divides the item by value
	GJ_OP_APPEND,	///< Appends value to the data store
	GJ_OP_PREPEND,	///< Prepends value to the data store
	GJ_OP_MAX	///< Maximum enum value, do not use for operation
};

/// Reply fields to a trophy fetch request.
#define GJ_TROPHY_RESPONSE_TABLE(X_MACRO) \
	X_MACRO(id,          string,            char*) \
	X_MACRO(title,       string,            char*) \
	X_MACRO(difficulty,  trophy_difficulty, enum gj_trophy_difficulty) \
	X_MACRO(description, string,            char*) \
	X_MACRO(image_url,   string,            char*) \
	X_MACRO(achieved,    string,            char*)

/// Reply fields to a time fetch request.
#define GJ_TIME_RESPONSE_TABLE(X_MACRO) \
	X_MACRO(timestamp, string, char*) \
	X_MACRO(timezone,  string, char*) \
	X_MACRO(day,       string, char*) \
	X_MACRO(hour,      string, char*) \
	X_MACRO(minute,    string, char*) \
	X_MACRO(second,    string, char*)

/// Reply field to a scores fetch request.
#define GJ_SCORE_RESPONSE_TABLE(X_MACRO) \
	X_MACRO(score,            string, char*) \
	X_MACRO(sort,             string, char*) \
	X_MACRO(extra_data,       string, char*) \
	X_MACRO(user,             string, char*) \
	X_MACRO(user_id,          string, char*) \
	X_MACRO(guest,            string, char*) \
	X_MACRO(stored,           string, char*) \
	X_MACRO(stored_timestamp, string, char*)

/// Reply fields to a scores tables request.
#define GJ_SCORE_TABLE_RESPONSE_TABLE(X_MACRO) \
	X_MACRO(id,          string, char*) \
	X_MACRO(name,        string, char*) \
	X_MACRO(description, string, char*) \
	X_MACRO(primary,     bool_num, bool)

/// Reply fields to a scores get-rank request.
#define GJ_SCORE_GETRANK_RESPONSE_TABLE(X_MACRO) \
	X_MACRO(message, string, char*) \
	X_MACRO(rank,    string, char*)

/// Reply fields to a users fetch request
#define GJ_USER_RESPONSE_TABLE(X_MACRO) \
	X_MACRO(id,                       string, char*) \
	X_MACRO(type,                     string, char*) \
	X_MACRO(username,                 string, char*) \
	X_MACRO(avatar_url,               string, char*) \
	X_MACRO(signed_up,                string, char*) \
	X_MACRO(signed_up_timestamp,      string, char*) \
	X_MACRO(last_logged_in,           string, char*) \
	X_MACRO(last_logged_in_timestamp, string, char*) \
	X_MACRO(status,                   string, char*) \
	X_MACRO(developer_name,           string, char*) \
	X_MACRO(developer_website,        string, char*) \
	X_MACRO(developer_description,    string, char*)

/// Expands a response table as a structure with its fields
#define X_AS_STRUCT(field, decoder, type) \
	type field;

/// Holds the data of a single trophy.
struct gj_trophy {
	GJ_TROPHY_RESPONSE_TABLE(X_AS_STRUCT);
	bool secret;	///< If true, trophy is secret
};

/// Holds the date/time from server
struct gj_time {
	GJ_TIME_RESPONSE_TABLE(X_AS_STRUCT);
};

/// Holds data of a single score.
struct gj_score {
	GJ_SCORE_RESPONSE_TABLE(X_AS_STRUCT);
};

/// Holds data of a single score table.
struct gj_score_table {
	GJ_SCORE_TABLE_RESPONSE_TABLE(X_AS_STRUCT);
};

/// Holds user data
struct gj_user {
	GJ_USER_RESPONSE_TABLE(X_AS_STRUCT);
};

/************************************************************************//**
 * \brief Initialize the GameJolt API.
 *
 * This function sets the API endpoint, game credentials and user credentials.
 * Call this function before using any other function in the module.
 *
 * \param[in] endpoint    Endpoint for the Game API. Most likely you want to
 *            use "https://api.gamejolt.com/api/game/v1_2/" here.
 * \param[in] game_id     Game identifier. E.g. "123456"
 * \param[in] private_key Game private key. Keep it safe!
 * \param[in] username    Username of the player.
 * \param[in] user_token  Token corresponding to username.
 * \param[in] reply_buf   Pre-allocated buffer to use for data reception.
 * \param[in] buf_len     Length of reply_buf buffer.
 * \param[in] tout_frames Number of frames to wait for requests before
 *                        timing out.
 *
 * \return false on success, true on error.
 *
 * \warning reply_buf length determines the maximum length of the reply to a
 * command. If buffer length is small, API will not be able to receive
 * long responses such as trophy lists or long user lists.
 ****************************************************************************/
bool gj_init(const char *endpoint, const char *game_id, const char *private_key,
		const char *username, const char *user_token, char *reply_buf,
		uint16_t buf_len, uint16_t tout_frames);

/************************************************************************//**
 * \brief Return the last error code.
 *
 * If any other of the functions in the module return with error, call this
 * function immediately after the error, to get the error code.
 *
 * \return The error code. 0 (GJ_ERR_NONE) if no error occurred.
 ****************************************************************************/
enum gj_error gj_get_error(void);

/************************************************************************//**
 * \brief Fetch player trophies.
 *
 * \param[in] achieved    If true, only achieved trophies are get.
 * \param[in] trophy_id   If not NULL, a single trophy with specified id
 *            is retrieved.
 *
 * \return Raw trophy data on success, NULL on failure. Use gj_trophy_get_next()
 * to decode the raw information.
 ****************************************************************************/
char *gj_trophies_fetch(bool achieved, const char *trophy_id);

/************************************************************************//**
 * \brief Decode the trophy raw data for the next entry.
 *
 * On first call, set pos to the value returned by gj_trophies_fetch(). On
 * successive calls, set pos to the last non-NULL returned value of this
 * function.
 *
 * \param[inout] pos    Position of the trophy to extract. Note that input
 *               raw data is modified to add null terminations for fields
 * \param[out]   trophy Decoded trophy data.
 *
 * \return Position of the next trophy to decode (to be used on next call
 * to this function), or NULL if the current trophy could not be decoded.
 ****************************************************************************/
char *gj_trophy_get_next(char *pos, struct gj_trophy *trophy);

/************************************************************************//**
 * \brief Mark a trophy as achieved.
 *
 * \param[in] trophy_id   Identifier of the trophy to mark as achieved.
 *
 * \return true if error, false on success.
 ****************************************************************************/
bool gj_trophy_add_achieved(const char *trophy_id);

/************************************************************************//**
 * \brief Mark a trophy as not achieved.
 *
 * \param[in] trophy_id   Identifier of the trophy to mark as not achieved.
 *
 * \return true if error, false on success.
 ****************************************************************************/
bool gj_trophy_remove_achieved(const char *trophy_id);

/************************************************************************//**
 * \brief Get the string corresponding to a trophy difficulty.
 *
 * \param[in] difficulty Difficulty value to be translated to a string.
 *
 * \return The string corresponding to the specified difficulty. If the input
 * value of difficulty is out of range, "Unknown" string will be returned.
 ****************************************************************************/
const char *gj_trophy_difficulty_str(enum gj_trophy_difficulty difficulty);

/************************************************************************//**
 * \brief Get the date and time from server.
 *
 * \param[out] time        Date and time from the server.
 *
 * \return true if error, false on success.
 ****************************************************************************/
bool gj_time(struct gj_time *time);

/************************************************************************//**
 * \brief Fetch scores data.
 *
 * \param[in] limit       Number of scores to return (defaults to 10).
 * \param[in] table_id    Table id, or NULL for the main game table.
 * \param[in] guest       Set if you want to get score only from guest player.
 * \param[in] better_than Get only scores better than this sort value.
 * \param[in] worse_than  Get only scores worse than this sort value.
 * \param[in] only_user   Set to true if you want to get only the user scores.
 *
 * \return Raw scores data on success, or NULL on failure.
 * Use gj_score_get_next() to decode the raw score data.
 * \note All parameters are optional, use NULL if you do not want to set
 * them.
 ****************************************************************************/
char *gj_scores_fetch(const char *limit, const char *table_id,
		const char *guest, const char *better_than,
		const char *worse_than, bool only_user);

/************************************************************************//**
 * \brief Decode the score raw data for the next entry.
 *
 * On first call, set pos to the value returned by gj_scores_fetch(). On
 * successive calls, set pos to the last non-NULL returned value of this
 * function.
 *
 * \param[inout] pos   Position of the score to extract. Note that input
 *               raw data is modified to add null terminations for fields
 * \param[out]   score Decoded score data.
 *
 * \return Position of the next score to decode (to be used on next call
 * to this function), or NULL if the current score could not be decoded.
 ****************************************************************************/
char *gj_score_get_next(char *pos, struct gj_score *score);

/************************************************************************//**
 * \brief Fetch score tables.
 *
 * \return Raw score tables data on success, or NULL on failure.
 * Use gj_score_table_get_next() to decode the raw score table data.
 ****************************************************************************/
char *gj_scores_tables_fetch(void);

/************************************************************************//**
 * \brief Decode the score tables raw data for the next entry.
 *
 * On first call, set pos to the value returned by gj_scores_tables_fetch().
 * On successive calls, set pos to the last non-NULL returned value by this
 * function.
 *
 * \param[inout] pos         Position of the score table to extract. Note that
 *               input raw data is modified to add null terminations for fields
 * \param[out]   score_table Decoded score table data.
 *
 * \return Position of the next score to decode (to be used on next call
 * to this function), or NULL if the current score could not be decoded.
 ****************************************************************************/
char *gj_score_table_get_next(char *pos, struct gj_score_table *score_table);

/************************************************************************//**
 * \brief Get ranking corresponding to a sort parameter.
 *
 * \param[in] sort     Score sort value to get ranking position from table.
 * \param[in] table_id Score table id, or NULL for the main table.
 *
 * \return Ranking corresponding to sort parameter, or NULL on error.
 ****************************************************************************/
char *gj_scores_get_rank(const char *sort, const char *table_id);

/************************************************************************//**
 * \brief Add a score to a scoreboard.
 *
 * \param[in] score      Score in textual format (e.g. "500 torreznos")
 * \param[in] sort       Number used to sort the score (e.g. "500")
 * \param[in] table_id   Table id, or NULL for the main game table.
 * \param[in] guest      Name of the guest player, or NULL for user player.
 * \param[in] extra_data Extra data to save with score, NULL for none.
 *
 * \return true if error, false on success.
 ****************************************************************************/
bool gj_scores_add(const char *score, const char *sort, const char *table_id,
		const char *guest, const char *extra_data);

/************************************************************************//**
 * \brief Sets a key/value pair in the data store.
 *
 * \param[in] key        Key to set.
 * \param[in] data       Value to set.
 * \param[in] user_store When true, data is saved in user storage. Otherwise
 *                       it will be saved in the game global store.
 *
 * \return true if error, false on success.
 ****************************************************************************/
bool gj_data_store_set(const char *key, const char *data, bool user_store);

/************************************************************************//**
 * \brief Fetch data store keys.
 *
 * \param[in] pattern    Optional. If set, match returned keys with pattern.
 * \param[in] user_store When true, data is retrieved from user storage.
 *                       Otherwise it will be retrieved from the game global
 *                       store.
 *
 * \return Raw keys on success, or NULL on failure. Use
 * gj_data_store_key_next() to decode the raw key data.
 ****************************************************************************/
char *gj_data_store_keys_fetch(const char *pattern, bool user_store);

/************************************************************************//**
 * \brief Decode the key data from a gj_data_store_keys_fetch() call.
 *
 * On first call, set pos to the value returned by gj_data_store_keys_fetch().
 * On successive calls, set pos to the last non-NULL returned value by this
 * function.
 *
 * \param[inout] pos Position of the key to extract. Note that input raw data
 *                   is modified to add null terminations.
 * \param[out]   key Decoded key data.
 *
 * \return Position of the next key to decode (to be used on next call
 * to this function), or NULL if the current key could not be decoded.
 ****************************************************************************/
char *gj_data_store_key_next(char *pos, char **key);

/************************************************************************//**
 * \brief Retrieve data from the data store.
 *
 * \param[in] key        Key to use for data retrieval.
 * \param[in] user_store When true, data is retrieved from user storage.
 *                       Otherwise it will be retrieved from the game global
 *                       store.
 *
 * \return Data associated with requested key, or NULL on failure.
 ****************************************************************************/
char *data_store_fetch(const char *key, bool user_store);

/************************************************************************//**
 * \brief Update data in the data store.
 *
 * \param[in] key        Key to use for the data to be updated.
 * \param[in] operation  Operation to apply to the data to update.
 * \param[in] value      Value to use in the update operation.
 * \param[in] user_store When true, data is updated from user storage.
 *                       Otherwise it will be updated in the game global store.
 *
 * \return The updated data on success, or NULL on error.
 ****************************************************************************/
char *gj_data_store_update(const char *key,
		enum gj_data_store_update_operation operation,
		const char *value, bool user_store);

/************************************************************************//**
 * \brief Remove data from the data store.
 *
 * \param[in] key        Key to use for data removal.
 * \param[in] user_store When true, data is removed from user storage.
 *                       Otherwise it will be removed in the game global store.
 *
 * \return false on success, true on error.
 ****************************************************************************/
bool gj_data_store_remove(const char *key, bool user_store);

/************************************************************************//**
 * \brief Open a game session for the player.
 *
 * \return false on success, true on error.
 ****************************************************************************/
bool gj_sessions_open(void);

/************************************************************************//**
 * \brief Ping a session, and allow setting it as active or idle.
 *
 * \param[in] active If true, session is marked as active. Otherwise, session
 *                   is marked as idle.
 *
 * \return false on success, true on error.
 ****************************************************************************/
bool gj_sessions_ping(bool active);

/************************************************************************//**
 * \brief Checks if a user session is active in the game.
 *
 * \param[in] username   (Optional) username to check for open session.
 * \param[in] user_token (Optional) user token to check for open session.
 *
 * \return true if session is active. false is session is not active or other
 * error has occurred.
 *
 * \note If username or user_token is NULL, the session check is performed
 * against the user configured in gj_init() call.
 * \note To make sure a session is not active and no error has occurred, when
 * this function returns false, gj_get_error() must return GJ_ERR_NONE.
 ****************************************************************************/
bool gj_sessions_check(const char *username, const char *user_token);

/************************************************************************//**
 * \brief Close a previously opened session.
 *
 * \return false on success, true on error.
 ****************************************************************************/
bool gj_sessions_close(void);

/************************************************************************//**
 * \brief Get user data
 *
 * \param[in]  username (Optional) User name to get data from.
 * \param[in]  user_id  (Optional) user token to check for open session.
 *
 * \return Raw user data on success, NULL if error has occurred.
 *
 * \note You can pass multiple user ids by separating them with commas (',').
 * If you do it, be careful not to overflow the receive buffer!
 * \note Only one of the optional parameters (username, user_id) must be
 * specified. Set the other to NULL.
 ****************************************************************************/
char *gj_users_fetch(const char *username, const char *user_id);

/************************************************************************//**
 * \brief Decode the user raw data for the next entry.
 *
 * On first call, set pos to the value returned by gj_users_fetch(). On
 * successive calls, set pos to the last non-NULL returned value of this
 * function.
 *
 * \param[inout] pos  Position of the user to extract. Note that input raw
 *                    data is modified to add null terminations for fields
 * \param[out]   user Decoded user data
 *
 * \return Position of the next user to decode (to be used on next call
 * to this function), or NULL if the current user could not be decoded.
 ****************************************************************************/
char *gj_user_get_next(char *pos, struct gj_user *user);

/************************************************************************//**
 * \brief Check user credentials.
 *
 * \return true if credentials are correct, false if credentials do not match
 * or error has occurred.
 *
 * \note To make sure credentials are not valid and no error has occurred, when
 * this function returns false, gj_get_error() must return GJ_ERR_NONE.
 ****************************************************************************/
bool gj_users_auth(void);

/************************************************************************//**
 * \brief Get friends list.
 *
 * \return Raw user data on success, NULL if error has occurred. Use
 * gj_friend_get_next() to decode the raw data and get friend user_ids.
 ****************************************************************************/
char *gj_friends_fetch(void);

/************************************************************************//**
 * \brief Decode the friend raw data for the next entry.
 *
 * On first call, set pos to the value returned by gj_friends_fetch(). On
 * successive calls, set pos to the last non-NULL returned value of this
 * function.
 *
 * \param[inout] pos     Position of the friend to extract. Note that input raw
 *                       data is modified to add null terminations for fields
 * \param[out]   user_id Decoded friend user_id.
 *
 * \return Position of the next friend to decode (to be used on next call
 * to this function), or NULL if the current friend could not be decoded.
 ****************************************************************************/
char *gj_friend_get_next(char *pos, char **user_id);

/************************************************************************//**
 * \brief Generic GameJolt Game API request.
 *
 * Usually you do not need to use this function directly. Use more specific
 * API calls that do the hard work of filling parameters and decoding the
 * response.
 *
 * \param[in]  path         Array of paths for the request.
 * \param[in]  num_paths    Number of elements in path array.
 * \param[in]  key          Array of keys for key/value parameters.
 * \param[in]  value        Array of values for key/value parameters.
 * \param[in]  num_kv_pairs Number of elements in key and value arrays.
 * \param[out] out_len      Length of the received reply to request.
 *
 * \return The raw reply data to the request, or NULL on error.
 ****************************************************************************/
char *gj_request(const char **path, uint8_t num_paths, const char **key,
		const char **value, uint8_t num_kv_pairs, uint32_t *out_len);

#endif /* _GAMEJOLT_H_ */

/** \} */
````

## File: inc/ext/mw/jsmn.h
````c
/*
 * MIT License
 *
 * Copyright (c) 2010 Serge Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef JSMN_H
#define JSMN_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JSMN_STATIC
#define JSMN_API static
#else
#define JSMN_API extern
#endif

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
  JSMN_UNDEFINED = 0,
  JSMN_OBJECT = 1,
  JSMN_ARRAY = 2,
  JSMN_STRING = 3,
  JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
  /* Not enough tokens were provided */
  JSMN_ERROR_NOMEM = -1,
  /* Invalid character inside JSON string */
  JSMN_ERROR_INVAL = -2,
  /* The string is not a full JSON packet, more bytes expected */
  JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct {
  jsmntype_t type;
  int start;
  int end;
  int size;
#ifdef JSMN_PARENT_LINKS
  int parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string.
 */
typedef struct {
  unsigned int pos;     /* offset in the JSON string */
  unsigned int toknext; /* next token to allocate */
  int toksuper;         /* superior token node, e.g. parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
JSMN_API void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each
 * describing
 * a single JSON object.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens);

#ifndef JSMN_HEADER
/**
 * Allocates a fresh unused token from the token pool.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens,
                                   const size_t num_tokens) {
  jsmntok_t *tok;
  if (parser->toknext >= num_tokens) {
    return NULL;
  }
  tok = &tokens[parser->toknext++];
  tok->start = tok->end = -1;
  tok->size = 0;
#ifdef JSMN_PARENT_LINKS
  tok->parent = -1;
#endif
  return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, const jsmntype_t type,
                            const int start, const int end) {
  token->type = type;
  token->start = start;
  token->end = end;
  token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
                                const size_t len, jsmntok_t *tokens,
                                const size_t num_tokens) {
  jsmntok_t *token;
  int start;

  start = parser->pos;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    switch (js[parser->pos]) {
#ifndef JSMN_STRICT
    /* In strict mode primitive must be followed by "," or "}" or "]" */
    case ':':
#endif
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ']':
    case '}':
      goto found;
    default:
                   /* to quiet a warning from gcc*/
      break;
    }
    if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
#ifdef JSMN_STRICT
  /* In strict mode primitive must be followed by a comma/object/array */
  parser->pos = start;
  return JSMN_ERROR_PART;
#endif

found:
  if (tokens == NULL) {
    parser->pos--;
    return 0;
  }
  token = jsmn_alloc_token(parser, tokens, num_tokens);
  if (token == NULL) {
    parser->pos = start;
    return JSMN_ERROR_NOMEM;
  }
  jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
  token->parent = parser->toksuper;
#endif
  parser->pos--;
  return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
                             const size_t len, jsmntok_t *tokens,
                             const size_t num_tokens) {
  jsmntok_t *token;

  int start = parser->pos;

  parser->pos++;

  /* Skip starting quote */
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c = js[parser->pos];

    /* Quote: end of string */
    if (c == '\"') {
      if (tokens == NULL) {
        return 0;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
      }
      jsmn_fill_token(token, JSMN_STRING, start + 1, parser->pos);
#ifdef JSMN_PARENT_LINKS
      token->parent = parser->toksuper;
#endif
      return 0;
    }

    /* Backslash: Quoted symbol expected */
    if (c == '\\' && parser->pos + 1 < len) {
      int i;
      parser->pos++;
      switch (js[parser->pos]) {
      /* Allowed escaped symbols */
      case '\"':
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
      case 't':
        break;
      /* Allows escaped symbol \uXXXX */
      case 'u':
        parser->pos++;
        for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0';
             i++) {
          /* If it isn't a hex character we have an error */
          if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) ||   /* 0-9 */
                (js[parser->pos] >= 65 && js[parser->pos] <= 70) ||   /* A-F */
                (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
            parser->pos = start;
            return JSMN_ERROR_INVAL;
          }
          parser->pos++;
        }
        parser->pos--;
        break;
      /* Unexpected symbol */
      default:
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
    }
  }
  parser->pos = start;
  return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens) {
  int r;
  int i;
  jsmntok_t *token;
  int count = parser->toknext;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c;
    jsmntype_t type;

    c = js[parser->pos];
    switch (c) {
    case '{':
    case '[':
      count++;
      if (tokens == NULL) {
        break;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        return JSMN_ERROR_NOMEM;
      }
      if (parser->toksuper != -1) {
        jsmntok_t *t = &tokens[parser->toksuper];
#ifdef JSMN_STRICT
        /* In strict mode an object or array can't become a key */
        if (t->type == JSMN_OBJECT) {
          return JSMN_ERROR_INVAL;
        }
#endif
        t->size++;
#ifdef JSMN_PARENT_LINKS
        token->parent = parser->toksuper;
#endif
      }
      token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
      token->start = parser->pos;
      parser->toksuper = parser->toknext - 1;
      break;
    case '}':
    case ']':
      if (tokens == NULL) {
        break;
      }
      type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
      if (parser->toknext < 1) {
        return JSMN_ERROR_INVAL;
      }
      token = &tokens[parser->toknext - 1];
      for (;;) {
        if (token->start != -1 && token->end == -1) {
          if (token->type != type) {
            return JSMN_ERROR_INVAL;
          }
          token->end = parser->pos + 1;
          parser->toksuper = token->parent;
          break;
        }
        if (token->parent == -1) {
          if (token->type != type || parser->toksuper == -1) {
            return JSMN_ERROR_INVAL;
          }
          break;
        }
        token = &tokens[token->parent];
      }
#else
      for (i = parser->toknext - 1; i >= 0; i--) {
        token = &tokens[i];
        if (token->start != -1 && token->end == -1) {
          if (token->type != type) {
            return JSMN_ERROR_INVAL;
          }
          parser->toksuper = -1;
          token->end = parser->pos + 1;
          break;
        }
      }
      /* Error if unmatched closing bracket */
      if (i == -1) {
        return JSMN_ERROR_INVAL;
      }
      for (; i >= 0; i--) {
        token = &tokens[i];
        if (token->start != -1 && token->end == -1) {
          parser->toksuper = i;
          break;
        }
      }
#endif
      break;
    case '\"':
      r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->toksuper != -1 && tokens != NULL) {
        tokens[parser->toksuper].size++;
      }
      break;
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      break;
    case ':':
      parser->toksuper = parser->toknext - 1;
      break;
    case ',':
      if (tokens != NULL && parser->toksuper != -1 &&
          tokens[parser->toksuper].type != JSMN_ARRAY &&
          tokens[parser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
        parser->toksuper = tokens[parser->toksuper].parent;
#else
        for (i = parser->toknext - 1; i >= 0; i--) {
          if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
            if (tokens[i].start != -1 && tokens[i].end == -1) {
              parser->toksuper = i;
              break;
            }
          }
        }
#endif
      }
      break;
#ifdef JSMN_STRICT
    /* In strict mode primitives are: numbers and booleans */
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 't':
    case 'f':
    case 'n':
      /* And they must not be keys of the object */
      if (tokens != NULL && parser->toksuper != -1) {
        const jsmntok_t *t = &tokens[parser->toksuper];
        if (t->type == JSMN_OBJECT ||
            (t->type == JSMN_STRING && t->size != 0)) {
          return JSMN_ERROR_INVAL;
        }
      }
#else
    /* In non-strict mode every unquoted value is a primitive */
    default:
#endif
      r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
      if (r < 0) {
        return r;
      }
      count++;
      if (parser->toksuper != -1 && tokens != NULL) {
        tokens[parser->toksuper].size++;
      }
      break;

#ifdef JSMN_STRICT
    /* Unexpected char in strict mode */
    default:
      return JSMN_ERROR_INVAL;
#endif
    }
  }

  if (tokens != NULL) {
    for (i = parser->toknext - 1; i >= 0; i--) {
      /* Unmatched opened object or array */
      if (tokens[i].start != -1 && tokens[i].end == -1) {
        return JSMN_ERROR_PART;
      }
    }
  }

  return count;
}

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
JSMN_API void jsmn_init(jsmn_parser *parser) {
  parser->pos = 0;
  parser->toknext = 0;
  parser->toksuper = -1;
}

#endif /* JSMN_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* JSMN_H */
````

## File: inc/ext/mw/json.h
````c
/************************************************************************//**
 * \file
 *
 * \brief JSON implementation, based on jsmn by Serge Zaitsev.
 *
 * \defgroup json json
 * \{
 *
 * \brief JSON implementation, based on jsmn by Serge Zaitsev.
 *
 * This module just adds some helper functions to jsmn to ease navigating
 * the JSON structure and extracting values.
 *
 * \author Jesus Alonso (doragasu)
 * \date 2019
 *
 * \todo Add more functions to extract data.
 ****************************************************************************/

#ifndef _JSON_H_
#define _JSON_H_

#include "types.h"

/// Hides jsmn API definitions to avoid linking errors
#define JSMN_STATIC
#include "ext/mw/jsmn.h"

/// Macro to extract the item (as string) from a token. Requires the json_str
/// to have the null terminations added by json_null_terminate().
#define json_item(json_str, obj_tok, idx) (idx) < 0 ? NULL : \
	&((json_str)[(obj_tok)[idx].start])

/************************************************************************//**
 * \brief Add null-terminators to the input JSON string. Useful to further
 * extract token values as null-terminated strings.
 *
 * \param[inout] json_str The JSON string to add null terminators to.
 * \param[in]    obj_tok  Tokens extracted from json_str.
 * \param[in]    num_tok  Number of tokens in obj_tok.
 *
 * \return 0 on success, JSMN_ERROR_INVAL if string ends prematurely.
 *
 * \warning Resulting string can no longer be used as a valid json.
 ****************************************************************************/
int json_null_terminate(char *json_str, const jsmntok_t *obj_tok, int num_tok);

/************************************************************************//**
 * \brief Determines if pointed token corresponds to a key in a key:value
 * pair.
 *
 * \param[in] json_tok JSON token array.
 * \param[in] obj_idx  Index of the token to analyse.
 * \param[in] num_tok  Number of tokens in json_tok.
 *
 * \return true if token is a key token. false otherwise.
 ****************************************************************************/
bool json_is_key(const jsmntok_t *json_tok, int obj_idx, int num_tok);

/************************************************************************//**
 * \brief Return the index of the next item. This function only works
 * properly when specified token is of JSMN_OBJECT type.
 *
 * \param[in] obj_tok    JSON token array.
 * \param[in] obj_idx    Index of the token to analyse.
 * \param[in] parent_idx Index of the parent object or array token.
 * \param[in] num_tok    Number of tokens in obj_tok.
 *
 * \return The index of the next item on success, of JSMN_ERROR_PART if there
 * are no more items available in the parent object.
 ****************************************************************************/
int json_object_next(const jsmntok_t *obj_tok, int obj_idx, int parent_idx,
		int num_tok);

/************************************************************************//**
 * \brief Return the index of the next key in the current JSON level.
 *
 * \param[in] json_tok   JSON token array.
 * \param[in] obj_idx    Index of the token to analyse.
 * \param[in] parent_idx Index of the parent object or array token.
 * \param[in] num_tok    Number of tokens in obj_tok.
 *
 * \return The index of the next key item on success, of JSMN_ERROR_PART if
 * there are no more key items available in the parent object.
 ****************************************************************************/
int json_key_next(const jsmntok_t *json_tok, int obj_idx, int parent_idx,
		int num_tok);

/************************************************************************//**
 * \brief Search for a given key in the current JSON level.
 *
 * \param[in] key        key string to search.
 * \param[in] json_str   JSON string.
 * \param[in] json_tok   JSON token array.
 * \param[in] obj_idx    Index of the token to analyse.
 * \param[in] parent_idx Index of the parent object or array token.
 * \param[in] num_tok    Number of tokens in obj_tok.
 *
 * \return The index of the value token on success, or JSMN_ERROR_PART if
 * the specified key was not found in parent object.
 * \note The returned value on success is the index of the value token, it
 * is NOT the index of the found key token.
 ****************************************************************************/
int json_key_search(const char *key, const char *json_str,
		const jsmntok_t *json_tok, int obj_idx, int parent_idx,
		int num_tok);

/************************************************************************//**
 * \brief Obtains the boolean value corresponding to the indicated token.
 *
 * \param[in] json_str   JSON string.
 * \param[in] json_tok   JSON token array extracted from json_str.
 * \param[in] obj_idx    Index of the token to analyse.
 *
 * \return 1 if true, 0 if false, less than 0 if token is not a proper boolean.
 ****************************************************************************/
int json_bool_get(const char *json_str, const jsmntok_t *json_tok, int obj_idx);

#endif /*_JSON_H_*/

 /** \} */
````

## File: inc/ext/mw/lsd.h
````c
/************************************************************************//**
 * \brief  Local Symmetric Data-link. Implements an extremely simple
 *         protocol to link two full-duplex devices, multiplexing the
 *         data link.
 *
 * The multiplexing facility allows having up to LSD_MAX_CH simultaneous
 * channels on the serial link.
 *
 * The module has synchronous functions to send/receive data (easy to use, but
 * due to polling hang the console until transfer is complete) and their
 * asyncronous counterparts. The asynchronous functions return immediately,
 * but require calling frequently lsd_process() to actually send/receive data.
 * Once the asynchronous functions complete sending/receiving data, the
 * specified callback is run.
 *
 * \author Jesus Alonso (doragasu)
 * \author Juan Antonio (PaCHoN)
 * \date   2019~2025
 * \note   Unfortunately the Megadrive does have neither an interrupt pin nor
 *         DMA threshold pins in the cartridge slot, so polling is the only
 *         way. So you have
 *         Megadrive does not have an interrupt pin on the cart, implementing
 *         more efficient data transmission techniques will be tricky.
 * \warning The syncrhonous API is easier to use, but a lot less reliable:
 * * It polls, using all the CPU until the send/recv operation completes.
 * * A lsd_recv_sync() can freeze the machine if no frame is received. USE IT
 *   WITH CARE!
 *
 * \defgroup lsd lsd
 * \{
 ****************************************************************************/

/*
 * Frame format is:
 *
 * STX : CH-LENH : LENL : DATA : ETX
 *
 * - STX and ETX are the start/end of transmission characters (1 byte each).
 * - CH-LENH is the channel number (first 4 bits) and the 4 high bits of the
 *   data length.
 * - LENL is the low 8 bits of the data length.
 * - DATA is the payload, of the previously specified length.
 */
#ifndef _LSD_H_
#define _LSD_H_

#include "config.h"
#include "types.h"
#include "ext/mw/comm.h"
#include "ext/mw/mw-msg.h"

/// LSD frame overhead in bytes
#define LSD_OVERHEAD		4

/// Maximum number of available simultaneous channels
#define LSD_MAX_CH		4

/// Maximum data payload length
#define LSD_MAX_LEN		 4095

/// Number of buffer frames available
#define LSD_BUF_FRAMES		2

/// Return status codes for LSD functions
enum lsd_status {
	LSD_STAT_ERR_FRAMING = -5,		///< Frame format error
	LSD_STAT_ERR_INVALID_CH = -4,		///< Invalid channel
	LSD_STAT_ERR_FRAME_TOO_LONG = -3,	///< Frame is too long
	LSD_STAT_ERR_IN_PROGRESS = -2,		///< Operation in progress
	LSD_STAT_ERROR = -1,			///< General error
	LSD_STAT_COMPLETE = 0,			///< No error
	LSD_STAT_BUSY = 1			///< Doing requested operation
};

/// Callback for the asynchronous lsd_send() function.
typedef void (*lsd_send_cb)(enum lsd_status stat, void *ctx);
/// Callback for the asynchronous lsd_recv() function.
typedef void (*lsd_recv_cb)(enum lsd_status stat, uint8_t ch,
		char *data, uint16_t len, void *ctx);

/************************************************************************//**
 * \brief Module initialization.
 ****************************************************************************/
void lsd_init(void);

/************************************************************************//**
 * \brief Enables a channel to start reception and be able to send data.
 *
 * \param[in] ch Channel number.
 *
 * \return LSD_OK on success, LSD_ERROR otherwise.
 ****************************************************************************/
int lsd_ch_enable(uint8_t ch);

/************************************************************************//**
 * \brief Disables a channel to stop reception and prohibit sending data.
 *
 * \param[in] ch Channel number.
 *
 * \return LSD_OK on success, LSD_ERROR otherwise.
 ****************************************************************************/
int lsd_ch_disable(uint8_t ch);


/************************************************************************//**
 * \brief Asynchronously sends data through a previously enabled channel.
 *
 * \param[in] ch      Channel number to use.
 * \param[in] data    Buffer to send.
 * \param[in] len     Length of the buffer to send.
 * \param[in] ctx     Context for the send callback function.
 * \param[in] send_cb Callback to run when send completes or errors.
 *
 * \return Status of the send procedure. Usually LSD_STAT_BUSY is returned,
 * and the send procedure is then performed in background.
 * \note Calling this function while there is a send procedure in progress,
 * will cause the function call to fail with LSD_STAT_SEND_ERR_IN_PROGRESS.
 ****************************************************************************/
enum lsd_status lsd_send(uint8_t ch, const char *data, int16_t len,
		void *ctx, lsd_send_cb send_cb);

/************************************************************************//**
 * \brief Synchronously sends data through a previously enabled channel.
 *
 * \param[in] ch   Channel number to use.
 * \param[in] data Buffer to send.
 * \param[in] len  Length of the buffer to send.
 *
 * \return Status of the send procedure.
 * \warning This function polls until the procedure is complete (or errors).
 ****************************************************************************/
enum lsd_status lsd_send_sync(uint8_t ch, const char *data, int16_t len);

/************************************************************************//**
 * \brief Asyncrhonously Receives a frame using LSD protocol.
 *
 * \param[in] buf     Buffer for reception.
 * \param[in] len     Buffer length.
 * \param[in] ctx     Context for the receive callback function.
 * \param[in] recv_cb Callback to run when receive completes or errors.
 *
 * \return Status of the receive procedure.
 ****************************************************************************/
enum lsd_status lsd_recv(char *buf, int16_t len, void *ctx,
		lsd_recv_cb recv_cb);

/************************************************************************//**
 * \brief Syncrhonously Receives a frame using LSD protocol.
 *
 * \param[out]   buf Buffer for received data.
 * \param[inout] len On input: buffer length. On output: received frame length.
 * \param[out]   ch  Channel on which the data has been received.
 *
 * \warning This function polls until the reception is complete, or a reception
 * error occurs.
 * \warning If no frame is received when this function is called, the machine
 * will lock.
 ****************************************************************************/
enum lsd_status lsd_recv_sync(char *buf, uint16_t *len, uint8_t *ch);

/************************************************************************//**
 * \brief Processes sends/receives pending data.
 *
 * Call this function as much as possible when using the asynchronous
 * lsd_send() and lsd_receive() functions.
 ****************************************************************************/
void lsd_process(void);

/** \} */

#endif //_LSD_H_
````

## File: inc/ext/mw/megawifi.h
````c
/************************************************************************//**
 * \file
 *
 * \brief MegaWiFi API implementation.
 *
 * \defgroup megawifi megawifi
 * \{
 *
 * \brief MegaWiFi API implementation.
 *
 * API to communicate with the wifi module and the Internet. API calls are
 * documented and most of them are self explanatory. Mostly the only weird
 * thing about the API is UDP reuse mode. If you enable reuse mode (setting
 * the dst_addr and/or dst_port to NULL in the mw_udp_set() call), received
 * data will prepend the IP and port of the peer (using mw_reuse_payload data
 * structure), and data to be sent also requires the IP and port to be
 * prepended to the payload.
 *
 * \author Jesus Alonso (doragasu)
 * \author Juan Antonio (PaCHoN)
 * \date 2025
 * 
 * \note This module requires setting MODULE_MEGAWIFI to 1 in config.h and
 * rebuilding the library (if you had to change them).
 ****************************************************************************/

#ifndef _MEGAWIFI_H_
#define _MEGAWIFI_H_

#include "ext/mw/mw-msg.h"
#include "ext/mw/lsd.h"
#include "ext/mw/comm.h"

/// API version implemented, major number
#define MW_API_VERSION_MAJOR	1

/// API version implemented, minor number
#define MW_API_VERSION_MINOR	5

/// Timeout for standard commands in milliseconds
#define MW_COMMAND_TOUT_MS	2000
/// Timeout for TCP connections
#define MW_CONNECT_TOUT_MS	10000
/// Timeout for HTTP open command in milliseconds
#define MW_HTTP_OPEN_TOUT_MS	10000
/// Timeout for the AP scan command in milliseconds
#define MW_SCAN_TOUT_MS		10000
/// Timeout for the AP associate command in milliseconds
#define MW_ASSOC_TOUT_MS	20000
/// Time to sleep before waiting for assoc in milliseconds
#define MW_ASSOC_WAIT_SLEEP_MS	5000
/// Timeout for upgrade command in milliseconds
#define MW_UPGRADE_TOUT_MS	600000
/// Timeout for ping command in milliseconds
#define MW_PING_TOUT_MS	30000
/// Milliseconds between status polls while in wm_ap_assoc_wait()
#define MW_STAT_POLL_MS		250

/// Error codes for MegaWiFi API functions
enum mw_err {
	MW_ERR_NONE = 0,		///< No error (success)
	MW_ERR,				///< General error
	MW_ERR_NOT_READY,		///< Not ready to run command
	MW_ERR_BUFFER_TOO_SHORT,	///< Command buffer is too small
	MW_ERR_PARAM,			///< Input parameter out of range
	MW_ERR_SEND,			///< Error sending data
	MW_ERR_RECV			///< Error receiving data
};

/// Supported HTTP methods
enum mw_http_method {
    MW_HTTP_METHOD_GET = 0,    ///< HTTP GET Method
    MW_HTTP_METHOD_POST,       ///< HTTP POST Method
    MW_HTTP_METHOD_PUT,        ///< HTTP PUT Method
    MW_HTTP_METHOD_PATCH,      ///< HTTP PATCH Method
    MW_HTTP_METHOD_DELETE,     ///< HTTP DELETE Method
    MW_HTTP_METHOD_HEAD,       ///< HTTP HEAD Method
    MW_HTTP_METHOD_NOTIFY,     ///< HTTP NOTIFY Method
    MW_HTTP_METHOD_SUBSCRIBE,  ///< HTTP SUBSCRIBE Method
    MW_HTTP_METHOD_UNSUBSCRIBE,///< HTTP UNSUBSCRIBE
    MW_HTTP_METHOD_OPTIONS,    ///< HTTP OPTIONS
    MW_HTTP_METHOD_MAX,
};

/// Maximum SSID length (including '\0').
#define MW_SSID_MAXLEN		32
/// Maximum password length (including '\0').
#define MW_PASS_MAXLEN		64
/// Maximum length of an NTP pool URI (including '\0').
#define MW_NTP_POOL_MAXLEN	80
/// Number of AP configurations stored to nvflash.
#define MW_NUM_CFG_SLOTS	3
/// Number of DSN servers supported per AP configuration.
#define MW_NUM_DNS_SERVERS	2
/// Length of the FSM queue
#define MW_FSM_QUEUE_LEN	8
/// Maximum number of simultaneous TCP connections
#define MW_MAX_SOCK			3
/// Control channel used for LSD protocol
#define MW_CTRL_CH			0
/// Channel used for HTTP requests and cert sets
#define MW_HTTP_CH			LSD_MAX_CH - 1

/// Minimum command buffer length to be able to send all available commands
/// with minimum data payload. This length might not guarantee that commands
/// like mw_sntp_cfg_set() can be sent if payload length is big enough).
#define MW_CMD_MIN_BUFLEN	168

/// Access Point data.
struct mw_ap_data {
	enum mw_security auth;	///< Security type
	uint8_t channel;	///< WiFi channel.
	int8_t rssi;		///< Signal strength.
	uint8_t ssid_len;	///< Length of ssid field.
	char *ssid;		///< SSID string (not NULL terminated).
};

/// Interface type for the mw_bssid_get() function.
enum mw_if_type {
	MW_IF_STATION = 0,	///< Station interface
	MW_IF_SOFTAP,		///< Access Point interface
	MW_IF_MAX		///< Number of supported interface types
};

/************************************************************************//**
 * \brief Module initialization. Must be called once before using any
 *        other function. It also initializes de UART.
 *
 * \param[in] cmd_buf Pointer to the buffer used to send and receive commands.
 * \param[in] buf_len Length of cmdBuf in bytes.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
int16_t mw_init(uint16_t *cmd_buf, uint16_t buf_len);

/************************************************************************//**
 * \brief Processes sends/receives pending data.
 *
 * Call this function as much as possible to process incoming/outgoing data.
 *
 * \warning No data will be sent/received if this function is not frequently
 * invoked.
 ****************************************************************************/
static inline void mw_process(void)	{lsd_process();}

/************************************************************************//**
 * \brief Sets the callback function to be run when network data is received
 * while waiting for a command reply.
 *
 * \param[in] cmd_recv_cb Callback to be run when data is received while
 *                        waiting for a command reply.
 *
 * \warning If this callback is not set, data received while waiting for a
 * command reply will be silently discarded.
 ****************************************************************************/
void mw_cmd_data_cb_set(lsd_recv_cb cmd_recv_cb);

/************************************************************************//**
 * \brief Performs the startup sequence for the WiFi module, and tries
 * detecting it by requesting the version data.
 *
 * \param[out] major   Major version number.
 * \param[out] minor   Minor version number.
 * \param[out] variant String with firmware variant ("std" for standard).
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_detect(uint8_t *major, uint8_t *minor, char **variant);

/************************************************************************//**
 * \brief Obtain module version numbers and string
 *
 * \param[out] version Version numbers (major, minor, micro) in order.
 * \param[out] variant String with firmware variant ("std" for standard).
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_version_get(uint8_t version[3], char **variant);

/************************************************************************//**
 * \brief Gets the module BSSID (the MAC address) for the specified interface.
 *
 * \param[in] interface_type Type of the interface to obtain BSSID from.
 *
 * \return The requested BSSID (6 byte binary data), or NULL on error.
 ****************************************************************************/
uint8_t *mw_bssid_get(enum mw_if_type interface_type);

/************************************************************************//**
 * \brief Set default module configuration (AKA factory settings).
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note For this command to take effect, it must be followed by a module
 *       reset.
 ****************************************************************************/
enum mw_err mw_default_cfg_set(void);

/************************************************************************//**
 * \brief Set access point configuration (SSID and password).
 *
 * \param[in] slot Configuration slot to use.
 * \param[in] ssid String with the AP SSID to set.
 * \param[in] pass String with the AP SSID to set.
 * \param[in] phy_type Bitmask with the PHY type configuration.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note Strings must be NULL terminated. Maximum SSID length is 32 bytes,
 *       maximum pass length is 64 bytes.
 * \note After a successful invocation, call mw_cfg_save() for changes to
 * be persistent
 ****************************************************************************/
enum mw_err mw_ap_cfg_set(uint8_t slot, const char *ssid, const char *pass,
		 enum mw_phy_type phy_type);

/************************************************************************//**
 * \brief Gets access point configuration (SSID and password).
 *
 * \param[in]  slot Configuration slot to use.
 * \param[out] ssid String with the AP SSID got.
 * \param[out] pass String with the AP SSID got.
 * \param[out] phy_type Bitmask with the PHY type configuration.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \warning ssid is zero padded up to 32 bytes, and pass is zero padded up
 *          to 64 bytes. If ssid is 32 bytes, it will NOT be NULL terminated.
 *          Also if pass is 64 bytes, it will NOT be NULL terminated.
 ****************************************************************************/
enum mw_err mw_ap_cfg_get(uint8_t slot, char **ssid, char **pass,
		enum mw_phy_type *phy_type);

/************************************************************************//**
 * \brief Set IPv4 configuration.
 *
 * \param[in] slot Configuration slot to use.
 * \param[in] ip   Pointer to the mw_ip_cfg structure, with IP configuration.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note After a successful invocation, call mw_cfg_save() for changes to
 * be persistent
 ****************************************************************************/
enum mw_err mw_ip_cfg_set(uint8_t slot, const struct mw_ip_cfg *ip);

/************************************************************************//**
 * \brief Get IPv4 configuration.
 *
 * \param[in]  slot Configuration slot to use.
 * \param[out] ip   Double pointer to mw_ip_cfg structure, with IP conf.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_ip_cfg_get(uint8_t slot, struct mw_ip_cfg **ip);

/************************************************************************//**
 * \brief Set advanced WiFi configuration.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \warning This function is dangerous. Changing these parameters is rarely
 * needed, and setting incorrect values, may render the connection unstable
 * and/or crash the WiFi module. Invalid configurations can even cause the
 * module to crash in a bootloop, requiring a programmer to unbrick it.
 * Make sure you thoroughly test the values you allow users to set here.
 * \note If you want to change WiFi parameters, the recommendation is to get
 * the current configuration via mw_wifi_adv_cfg_get(), and from it change
 * only the required parameters.
 * \note These parameters will not take effect until saved to non-volatile
 * storage (with mw_cfg_save()) and issuing a module reboot.
 ****************************************************************************/
enum mw_err mw_wifi_adv_cfg_set(const struct mw_wifi_adv_cfg *wifi);

/************************************************************************//**
 * \brief Get advanced WiFi configuration.
 *
 * \return Pointer to the advanced WiFi configuration, or NULL on error.
 ****************************************************************************/
struct mw_wifi_adv_cfg *mw_wifi_adv_cfg_get(void);

/************************************************************************//**
 * \brief Saves changed configuration parameters to non-volatile memory.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_cfg_save(void);

/************************************************************************//**
 * \brief Get current IP configuration, of the joined AP.
 *
 * \param[out] ip Double pointer to mw_ip_cfg structure, with IP conf.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_ip_current(struct mw_ip_cfg **ip);

/************************************************************************//**
 * \brief Scan for access points.
 *
 * \param[in]  phy_type Bitmask with the PHY type configuration.
 * \param[out] ap_data Data of the found access points. Each entry has the
 *             format specified on the mw_ap_data structure.
 * \param[out] aps     Number of found access points.
 *
 * \return Length in bytes of the output data if operation completes
 *         successfully, or -1 if scan fails.
 ****************************************************************************/
int16_t mw_ap_scan(enum mw_phy_type phy_type, char **ap_data, uint8_t *aps);

/************************************************************************//**
 * \brief Parses received AP data and fills information of the AP at "pos".
 *        Useful to extract AP information from the data obtained by
 *        calling mw_ap_scan() function.
 *
 * \param[in]  ap_data  Access point data obtained from mw_ap_scan().
 * \param[in]  pos      Position at which to extract data.
 * \param[out] apd      Pointer to the extracted data from an AP.
 * \param[in]  data_len Lenght of apData.
 *
 * \return Position of the next AP entry in apData, 0 if no more APs
 *         available or MW_ERROR if ap data/pos combination is not valid.
 *
 * \note This functions executes locally, does not communicate with the
 *       WiFi module.
 ****************************************************************************/
int16_t mw_ap_fill_next(const char *ap_data, uint16_t pos,
		struct mw_ap_data *apd, uint16_t data_len);

/************************************************************************//**
 * \brief Tries associating to an AP. If successful, also configures IPv4.
 *
 * \param[in] slot Configuration slot to use.
 *
 * \return MW_ERR_NONE if AP join operation has been successfully started,
 ****************************************************************************/
enum mw_err mw_ap_assoc(uint8_t slot);

/************************************************************************//**
 * \brief Polls the module status until it reports device is associated to
 * AP or timeout occurs.
 *
 * \param[in] tout_frames Maximun number of frames to wait for association.
 *            Set to TSK_PEND_FOREVER for an infinite wait.
 *
 * \return MW_ERR_NONE if device is associated to AP. MW_ERR_NOT_READY if
 * the timeout has expired.
 ****************************************************************************/
enum mw_err mw_ap_assoc_wait(int16_t tout_frames);

/************************************************************************//**
 * \brief Sets default AP/IP configuration.
 *
 * \param[in] slot Configuration slot to use.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note After a successful invocation, call mw_cfg_save() for changes to
 * be persistent
 ****************************************************************************/
enum mw_err mw_def_ap_cfg(uint8_t slot);

/************************************************************************//**
 * \brief Dissasociates from a previously associated AP.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_ap_disassoc(void);

/************************************************************************//**
 * \brief Gets default AP/IP configuration slot.
 *
 * \return The default configuration slot, of -1 on error.
 ****************************************************************************/
int16_t mw_def_ap_cfg_get(void);

/************************************************************************//**
 * \brief Tries establishing a TCP connection with specified server.
 *
 * \param[in] ch       Channel used for the connection.
 * \param[in] dst_addr Address (IP or DNS entry) of the server.
 * \param[in] dst_port Port in which server is listening.
 * \param[in] src_port Port from which try establishing connection. Set to
 *                     0 or empty string for automatic port allocation.
 *
 * \return MW_ERR_NONE on success, other code if connection failed.
 ****************************************************************************/
enum mw_err mw_tcp_connect(uint8_t ch, const char *dst_addr,
		const char *dst_port, const char *src_port);

/************************************************************************//**
 * \brief Closes and disconnects a socket from specified channel.
 *
 * This function can be used to free the channel associated to both TCP and
 * UDP sockets.
 *
 * \param[in] ch Channel associated to the socket to disconnect.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_close(uint8_t ch);

/// Closes a TCP socket. This is an alias of mw_close().
#define mw_tcp_disconnect(ch)	mw_close(ch)

/************************************************************************//**
 * \brief Configures a UDP socket to send/receive data.
 *
 * \param[in] ch       Channel used for the connection.
 * \param[in] dst_addr Address (IP or DNS entry) to send data to.
 * \param[in] dst_port Port to send data to.
 * \param[in] src_port Local port to listen message on.
 *
 * \return MW_ERR_NONE on success, other code if connection failed.
 *
 * \note Setting to NULL dst_addr and/or dst_port, enables reuse mode.
 ****************************************************************************/
enum mw_err mw_udp_set(uint8_t ch, const char *dst_addr, const char *dst_port,
		const char *src_port);

/// Frees a UDP socket. This is an alias of mw_close().
#define mw_udp_unset(ch)	mw_close(ch)

/************************************************************************//**
 * \brief Binds a socket to a port, and listens to connections on the port.
 *        If a connection request is received, it will be automatically
 *        accepted.
 *
 * \param[in] ch   Channel associated to the socket bound t port.
 * \param[in] port Port number to which the socket will be bound.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_tcp_bind(uint8_t ch, uint16_t port);

/************************************************************************//**
 * \brief Polls a socket until it is ready to transfer data. Typical use of
 * this function is after a successful mw_tcp_bind().
 *
 * \param[in] ch          Channel associated to the socket to monitor.
 * \param[in] tout_frames Maximum number of frames to wait for connection.
 *            Set to 0 for an infinite wait.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_sock_conn_wait(uint8_t ch, int16_t tout_frames);

/************************************************************************//**
 * \brief Receive data, asyncrhonous interface.
 *
 * \param[in] buf     Reception buffer.
 * \param[in] len     Length of the receive buffer.
 * \param[in] ctx     Context pointer to pass to the reception callbak.
 * \param[in] recv_cb Callback to run when reception is complete or errors.
 *
 * \return Status of the receive procedure.
 ****************************************************************************/
static inline enum lsd_status mw_recv(char *buf, int16_t len, void *ctx,
		lsd_recv_cb recv_cb)
{
	return lsd_recv(buf, len, ctx, recv_cb);
}

/************************************************************************//**
 * \brief Receive data using an UDP socket in reuse mode.
 *
 * \param[in] data    Receive buffer including the remote address and the
 *                    data payload.
 * \param[in] len     Length of the receive buffer.
 * \param[in] ctx     Context pointer to pass to the reception callbak.
 * \param[in] recv_cb Callback to run when reception is complete or errors.
 *
 * \return Status of the receive procedure.
 ****************************************************************************/
static inline enum lsd_status mw_udp_reuse_recv(struct mw_reuse_payload *data,
		int16_t len, void *ctx, lsd_recv_cb recv_cb)
{
	return lsd_recv((char*)data, len, ctx, recv_cb);
}

/************************************************************************//**
 * \brief Send data using a UDP socket in reuse mode.
 *
 * \param[in] ch      Channel to use for the send operation.
 * \param[in] data    Send buffer including the remote address and the
 *                    data payload.
 * \param[in] len     Length of the receive buffer.
 * \param[in] ctx     Context pointer to pass to the reception callbak.
 * \param[in] send_cb Callback to run when sending completes or errors.
 *
 * \return Status of the receive procedure.
 ****************************************************************************/
static inline enum lsd_status mw_udp_reuse_send(uint8_t ch,
		const struct mw_reuse_payload *data, int16_t len, void *ctx,
		lsd_send_cb send_cb)
{
	return lsd_send(ch, (const char*)data, len, ctx, send_cb);
}

/************************************************************************//**
 * \brief Sends data through a socket, using a previously allocated channel.
 * Asynchronous interface.
 *
 * \param[in] ch      Channel used to send the data.
 * \param[in] data    Buffer to send.
 * \param[in] len     Length of the data to send.
 * \param[in] ctx     Context for the send callback function.
 * \param[in] send_cb Callback to run when send completes or errors.
 *
 * \return Status of the send procedure. Usually LSD_STAT_BUSY is returned,
 * and the send procedure is then performed in background.
 * \note Calling this function while there is a send procedure in progress,
 * will cause the function call to fail with LSD_STAT_SEND_ERR_IN_PROGRESS.
 * \warning For very short data frames, it is possible that the send callback
 * is run before this function returns. In this case, the function returns
 * LSD_STAT_COMPLETE.
 ****************************************************************************/
static inline enum lsd_status mw_send(uint8_t ch, const char *data, int16_t len,
		void *ctx, lsd_send_cb send_cb)
{
	return lsd_send(ch, data, len, ctx, send_cb);
}

/************************************************************************//**
 * \brief Receive data, syncrhonous interface.
 *
 * \param[out] ch         Channel on which data was received.
 * \param[out] buf        Reception buffer.
 * \param[inout] buf_len  On input, length of the buffer.
 *                        On output, received data length in bytes.
 * \param[in] tout_frames Reception timeout in frames. Set to TSK_PEND_FOREVER
 *                        for infinite wait (dangerous!).
 *
 * \return Status of the receive procedure.
 * \warning Do not use more than one syncrhonous call at once. You must wait
 * until a syncrhonous call ends to issue another one.
 ****************************************************************************/
enum mw_err mw_recv_sync(uint8_t *ch, char *buf, int16_t *buf_len,
		int16_t tout_frames);

/************************************************************************//**
 * \brief Sends data through a socket, using a previously allocated channel.
 * Synchronous interface.
 *
 * \param[in] ch          Channel used to send the data.
 * \param[in] data        Buffer to send.
 * \param[in] len         Length of the data to send.
 * \param[in] tout_frames Timeout for send operation in frames. Set to 0 for
 *                        infinite wait (dangerous!).
 *
 * \return Status of the send procedure. Usually LSD_STAT_BUSY is returned,
 * and the send procedure is then performed in background.
 * \warning Do not use more than one syncrhonous call at once. You must wait
 * until a syncrhonous call ends to issue another one.
 ****************************************************************************/
enum mw_err mw_send_sync(uint8_t ch, const char *data, uint16_t len,
		int16_t tout_frames);

/************************************************************************//**
 * \brief Get system status.
 *
 * \return Pointer to system status structure on success, or NULL on error.
 ****************************************************************************/
union mw_msg_sys_stat *mw_sys_stat_get(void);

/************************************************************************//**
 * \brief Get socket status.
 *
 * \param[in] ch Channel associated to the socket asked for status.
 *
 * \return Socket status data on success, or -1 on error.
 ****************************************************************************/
enum mw_sock_stat mw_sock_stat_get(uint8_t ch);

/************************************************************************//**
 * \brief Configure SNTP parameters and timezone.
 *
 * \param[in] tz_str Timezone string (e.g. "CET"). See tzset(3) for details.
 * \param[in] server Array of up to three NTP servers. If less than three
 *                   servers are desired, unused entries must be empty.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note After a successful invocation, call mw_cfg_save() for changes to
 * be persistent
 ****************************************************************************/
enum mw_err mw_sntp_cfg_set(const char *tz_str, const char *server[3]);

/************************************************************************//**
 * \brief Get SNTP parameters and timezone configuration.
 *
 * \param[out] tz_str Timezone string (e.g. "CET"). See tzset(3) for details.
 * \param[out] server Array of three NTP server pointers. If less than 3
 *                    servers are configured, unused ones will be NULL.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_sntp_cfg_get(char **tz_str, char *server[3]);

/************************************************************************//**
 * \brief Get date and time.
 *
 * \param[out] dt_bin Date and time in seconds since Epoch. If set to NULL,
 *                    this info is not filled (but return value will still
 *                    be properly set).
 *
 * \return A string with the date and time in textual format, e.g.: "Thu Mar
 *         3 12:26:51 2016", or NULL if error.
 ****************************************************************************/
char *mw_date_time_get(uint32_t dt_bin[2]);

/************************************************************************//**
 * \brief Get the identifiers of the flash chip in the WiFi module.
 *
 * \param[out] man_id ID of the flash chip manufacturer.
 * \param[out] dev_id Device IDs of the flash chip.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_flash_id_get(uint8_t *man_id, uint16_t *dev_id);

/************************************************************************//**
 * \brief Erase a 4 KiB Flash sector. Every byte of an erased sector will be
 *        read as 0xFF.
 *
 * \param[in] sect Sector number to erase.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_flash_sector_erase(uint16_t sect);

/************************************************************************//**
 * \brief Write data to specified flash address.
 *
 * \param[in] addr     Address to which data will be written.
 * \param[in] data     Data to be written to flash chip.
 * \param[in] data_len Length in bytes of data field.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_flash_write(uint32_t addr, uint8_t *data, uint16_t data_len);

/************************************************************************//**
 * \brief Read data from specified flash address.
 *
 * \param[in] addr     Address from which data will be read.
 * \param[in] data_len Number of bytes to read from addr.
 *
 * \return Pointer to read data on success, or NULL if command failed.
 ****************************************************************************/
uint8_t *mw_flash_read(uint32_t addr, uint16_t data_len);

/************************************************************************//**
 * \brief Set gamertag information for one slot.
 *
 * \param[in] slot     Slot to use (from 0 to 2).
 * \param[in] gamertag Gamertag information to set on specified slot.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note After a successful invocation, call mw_cfg_save() for changes to
 * be persistent
 ****************************************************************************/
enum mw_err mw_gamertag_set(uint8_t slot, const struct mw_gamertag *gamertag);

/************************************************************************//**
 * \brief Get gamertag information for one slot.
 *
 * \param[in] slot Slot to get gamertag from.
 *
 * \return Gamertag information on success, NULL on error.
 ****************************************************************************/
struct mw_gamertag *mw_gamertag_get(uint8_t slot);

/************************************************************************//**
 * \brief Write a message to the WiFi module log trace.
 *
 * \param[in] msg Message to write to the log trace.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_log(const char *msg);

/************************************************************************//**
 * \brief Set factory default configuration.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 * \note It is recommended to reboot the module after this command.
 ****************************************************************************/
enum mw_err mw_factory_settings(void);

/************************************************************************//**
 * \brief Powers off the WiFi module.
 *
 * The module will be put in deep sleep mode. To wake it up, the RESET pin
 * must be toggled.
 ****************************************************************************/
void mw_power_off(void);

/************************************************************************//**
 * \brief Sleep the specified amount of frames
 *
 * \param[in] frames Number of frames to sleep.
 ****************************************************************************/
void mw_sleep(int16_t frames);

/************************************************************************//**
 * \brief Set URL for HTTP requests.
 *
 * \param[in] url URL to set.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_http_url_set(const char *url);

/************************************************************************//**
 * \brief Set method for HTTP requests.
 *
 * \param[in] method Method to set.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_http_method_set(enum mw_http_method method);

/************************************************************************//**
 * \brief Add an HTTP header.
 *
 * \param[in] key   Header key.
 * \param[in] value Value to set for the key.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_http_header_add(const char *key, const char *value);

/************************************************************************//**
 * \brief Delete a previously added HTTP header.
 *
 * \param[in] key Key of the header to delete.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_http_header_del(const char *key);

/************************************************************************//**
 * \brief Open HTTP connection.
 *
 * This functions opens the HTTP connection, sends the HTTP headers, and
 * prepares the module to send the specified content_len if (if any) with
 * a successive mw_send() or mw_send_sync(), using MW_HTTP_CH channel.
 *
 * \param[in] content_len Length of the content to write in HTTP request,
 *            after a successfull call to this function.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_http_open(uint32_t content_len);

/************************************************************************//**
 * \brief Finish an opened HTTP request.
 *
 * After a successful call to mw_http_open(), and sending the content (if
 * any), call this function to receive the HTTP response headers, and obtain
 * the length of the body to receive with a further call to mw_recv() or
 * mw_recv_sync(), using MW_HTTP_CH.
 *
 * \param[out] content_len Length of the response content to receive after a
 *             successfull call to this function.
 * \param[in]  tout_frames Maximun number of frames to wait for reply.
 *
 * \return The HTTP status code if the request was completed, or an error
 * code (lower than 100) if the HTTP request did not complete.
 * \note Even if the HTTP request is completed, that does not mean there are
 * no errors, if the returned status code is 4xx or 5xx, there is a client
 * side or server side error.
 ****************************************************************************/
int16_t mw_http_finish(uint32_t *content_len, int16_t tout_frames);

/************************************************************************//**
 * \brief Query the X.509 hash of the installed PEM certificate.
 *
 * \return 0xFFFFFFFF if certificate is not installed or error occurs, or
 * the installed X.509 certificate hash on success.
 ****************************************************************************/
uint32_t mw_http_cert_query(void);

/************************************************************************//**
 * \brief Set the PEM certificate to use on HTTPS requests.
 *
 * The certificate is stored on the non volatile memory of the module, and
 * when present will be used in HTTPS requestes. This function can also be
 * used to delete a previously saved certificate using a NULL input value.
 *
 * \param[in] cert_hash X.509 hash of the certificate to set, ignored if
 *                      cert_len set to 0.
 * \param[in] cert      PEM certificate in plain text. Ignored if cert_len
 *                      set to 0.
 *                      previously stored certificate.
 * \param[in] cert_len  Certificate length in bytes. Set to 0 to delete a
 *                      previously stored certificate.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
enum mw_err mw_http_cert_set(uint32_t cert_hash, const char *cert,
		uint16_t cert_len);

/************************************************************************//**
 * \brief Clean-up an HTTP request, freeing associated resources.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 ****************************************************************************/
int16_t mw_http_cleanup(void);

/************************************************************************//**
 * \brief Get the default server used for MegaWiFi connections.
 *
 * \return The server URL string, or NULL on error.
 ****************************************************************************/
char *mw_def_server_get(void);

/************************************************************************//**
 * \brief Set the default server used for MegaWiFi connections.
 *
 * \param[in] server_url The server URL to set.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note After a successful invocation, call mw_cfg_save() for changes to
 * be persistent
 ****************************************************************************/
enum mw_err mw_def_server_set(const char *server_url);

/************************************************************************//**
 * \brief Get random numbers.
 *
 * \param[in] rnd_len Number of bytes of resulting random array.
 *
 * \return The buffer with the requested random numbers on success, or NULL
 * when error.
 ****************************************************************************/
uint8_t *mw_hrng_get(uint16_t rnd_len);

/************************************************************************//**
 * \brief Set endpoint for Game API.
 *
 * Example endpoint for GameJolt: "https://api.gamejolt.com/api/game/v1_2/".
 *
 * \param[in] endpoint Endpoint for the Game API to set.
 * \param[in] priv_key Private key used for request signatures.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note The endpoint set persists between successive mw_ga_request() calls.
 ****************************************************************************/
enum mw_err mw_ga_endpoint_set(const char *endpoint, const char *priv_key);

/************************************************************************//**
 * \brief Add parameters to the Game API request in key/value format.
 *
 * Example key:value pair for GameJolt: "game_id":"123456".
 *
 * \param[in] key       Array with the keys to add.
 * \param[in] value     Array with the values to add
 * \param[in] num_pairs Number of key/value pairs to add.
 *
 * \return MW_ERR_NONE on success, other code on failure.
 *
 * \note The key/value pairs set persist between successive mw_ga_request()
 * calls.
 * \note Call mw_ga_key_value_add(NULL, NULL, 0) to clear previously set
 * key/value pairs.
 * \note Key/value pairs must NOT be URL encoded. Encoding is handled
 * internally.
 ****************************************************************************/
enum mw_err mw_ga_key_value_add(const char **key, const char **value,
		uint16_t num_pairs);

/************************************************************************//**
 * \brief Perform a GameAPI request, with the previously set endpoint and
 * key/value pairs.
 *
 * The request can also have URL encoded parameters. that are added to the
 * previously set key/value pairs.
 *
 * Example request for GameJolt:
 * - method: MW_HTTP_METHOD_GET
 * - path: "trophies"
 * - key:value: "achieved":"true"
 *
 * \param[in]  method       HTTP method to use. Most likely MW_HTTP_METHOD_GET.
 * \param[in]  path         Additional paths to add to the request.
 * \param[in]  num_paths    Number of additional paths to add.
 * \param[in]  key          Additional paths to add to the request.
 * \param[in]  value        Additional paths to add to the request.
 * \param[in]  num_kv_pairs Number of key/value pairs.
 * \param[out] content_len   Content length of the API response.
 * \param[in]  tout_frames   Number of frames to wait before canceling the
 *             request due to a timeout error.
 *
 * \return HTTP status code on success (e.g. 200), or an error (lower than
 * 100) if the HTTP request could not be completed.
 * \note Even if the HTTP request is completed, that does not mean there are
 * no errors, if the returned status code is 4xx or 5xx, there is a client
 * side or server side error.
 * \note path, key and value parameters must not be URL encoded. Encoding is
 * handled internally.
 ****************************************************************************/
int16_t mw_ga_request(enum mw_http_method method, const char **path,
		uint8_t num_paths, const char **key, const char **value,
		uint8_t num_kv_pairs, uint32_t *content_len,
		int16_t tout_frames);

/************************************************************************//**
 * \brief List available upgrades WiFi module firmware.
 *
 * \param[in] page           Page of list
 * \param[in] page_size      Page size of list
 * \param[in] offset         Offset number
 * \param[out] listUpgrades  Pointer to list of char*
 * \param[out] len 			 Result length
 * \param[out] total         Total elements
 *
 * \return Status of the send procedure.
 ****************************************************************************/
enum mw_err mw_fw_list_upgrades(uint8_t page, uint8_t size, uint8_t offset, char **listUpgrades, uint8_t *len, uint8_t *total);

/************************************************************************//**
 * \brief Over-The-Air upgrade WiFi module firmware.
 *
 * \param[in] name Name of the firmware blob to upgrade.
 *                 E.g. "mw_rtos_std_v1.4.1"
 *
 * \return Status of the send procedure.
 ****************************************************************************/
enum mw_err mw_fw_upgrade(const char *name);

/************************************************************************//**
 * \brief Run ICMP requesto to domain.
 *
 * \param[in] domain Domain to ICMP request.
 *                 E.g. "www.example.com"
 * \param[in] retries retries to ICMP request
 *                 E.g. 5
 *
 * \return Status of the send procedure.
 ****************************************************************************/
struct mw_ping_response *mw_ping(const char* domain, u8 retries);

/****** THE FOLLOWING COMMANDS ARE LOWER LEVEL AND USUALLY NOT NEEDED ******/

/************************************************************************//**
 * \brief Send a command to the WiFi module.
 *
 * \param[in] cmd     Pointer to the filled mw_cmd command structure.
 * \param[in] ctx     Context for callback function.
 * \param[in] send_cb Callback for the send operation completion.
 *
 * \return Status of the send procedure.
 ****************************************************************************/
static inline enum lsd_status mw_cmd_send(mw_cmd *cmd, void *ctx,
		lsd_send_cb send_cb)
{
	// Send data on control channel (0).
	return lsd_send(MW_CTRL_CH, cmd->packet, cmd->data_len + 4,
			ctx, send_cb);
}

/************************************************************************//**
 * \brief Try obtaining a reply to a command.
 *
 * \param[in] rep     Buffer to hold the command reply.
 * \param[in] ctx     Context for the reception callback.
 * \param[in] recv_cb Callback for data reception completion.
 *
 * \return Status of the reception procedure.
 ****************************************************************************/
static inline enum lsd_status mw_cmd_recv(mw_cmd *rep, void *ctx,
		lsd_recv_cb recv_cb) {
	return lsd_recv(rep->packet, sizeof(mw_cmd), ctx, recv_cb);
}

#endif /*_MEGAWIFI_H_*/

/** \} */
````

## File: inc/ext/mw/mw-msg.h
````c
/************************************************************************//**
 * \file
 * \brief MegaWiFi command message definitions.
 *
 * \defgroup mw-msg mw-msg
 * \{
 *
 * \brief MegaWiFi command message definitions.
 *
 * Contains the definition of the command codes and the data structures
 * conforming the command message queries and responses. Usually this module
 * is not directly used by the application, but by megawifi internally.
 *
 * \author Jesus Alonso (doragasu)
 * \author Juan Antonio (PaCHoN)
 * \date 2015~2025
 ****************************************************************************/
#ifndef _MW_MSG_H_
#define _MW_MSG_H_

#include "types.h"
#include "sys.h"

/// Maximum buffer length (bytes)
#define MW_MSG_MAX_BUFLEN	512
#define MW_MS_TO_FRAMES(ms)	(((ms)*60/500 + 1)/2)

#define MW_BUFLEN 1500

/// Command header length (command code and data length fields).
#define MW_CMD_HEADLEN		(2 * sizeof(uint16_t))

/// Maximum data length contained inside command buffer.
#define MW_CMD_MAX_BUFLEN	(MW_MSG_MAX_BUFLEN - MW_CMD_HEADLEN)

/// Maximum SSID length (including '\0').
#define MW_SSID_MAXLEN		32
/// Maximum password length (including '\0').
#define MW_PASS_MAXLEN		64

/// Gamertag nickname maximum length
#define MW_GT_NICKNAME_MAX		32
/// Gamertag security maximum length
#define MW_GT_SECURITY_MAX		32
/// Gamertag tagline maximum length
#define MW_GT_TAGLINE_MAX		32
/// Gamertag avatar graphick width in pixels
#define MW_GT_AVATAR_WIDTH		32
/// Gamertag avatar graphick height in pixels
#define MW_GT_AVATAR_HEIGHT		48
/// Telegram token maximum length
#define MW_GT_TG_TOKEN_MAX		64

/// Supported commands.
enum PACKED mw_command {
	MW_CMD_OK		 =   0,	///< OK command reply
	MW_CMD_VERSION      	 =   1,	///< Get firmware version
	MW_CMD_ECHO		 =   2,	///< Echo data
	MW_CMD_AP_SCAN		 =   3,	///< Scan for access points
	MW_CMD_AP_CFG		 =   4,	///< Configure access point
	MW_CMD_AP_CFG_GET   	 =   5,	///< Get access point configuration
	MW_CMD_IP_CURRENT 	 =   6,	///< Get current IPv4 configuration
// Reserved
	MW_CMD_IP_CFG		 =   8,	///< Configure IPv4
	MW_CMD_IP_CFG_GET	 =   9,	///< Get IPv4 configuration
	MW_CMD_DEF_AP_CFG	 =  10,	///< Set default AP configuration
	MW_CMD_DEF_AP_CFG_GET	 =  11,	///< Get default AP configuration
	MW_CMD_AP_JOIN		 =  12,	///< Join access point
	MW_CMD_AP_LEAVE		 =  13,	///< Leave previously joined AP
	MW_CMD_TCP_CON		 =  14,	///< Connect TCP socket
	MW_CMD_TCP_BIND		 =  15,	///< Bind TCP socket to port
// Reserved
	MW_CMD_CLOSE		 =  17,	///< Disconnect and free TCP/UDP socket
	MW_CMD_UDP_SET		 =  18,	///< Configure UDP socket
// Reserved (for setting socket  options)
	MW_CMD_SOCK_STAT	 =  20,	///< Get socket status
	MW_CMD_PING		 =  21,	///< Ping host
	MW_CMD_SNTP_CFG		 =  22,	///< Configure SNTP service
	MW_CMD_SNTP_CFG_GET      =  23,  ///< Get SNTP configuration
	MW_CMD_DATETIME		 =  24,	///< Get date and time
	MW_CMD_DT_SET       	 =  25,	///< Set date and time
	MW_CMD_FLASH_WRITE	 =  26,	///< Write to WiFi module flash
	MW_CMD_FLASH_READ	 =  27,	///< Read from WiFi module flash
	MW_CMD_FLASH_ERASE	 =  28,	///< Erase sector from WiFi flash
	MW_CMD_FLASH_ID 	 =  29,	///< Get WiFi flash chip identifiers
	MW_CMD_SYS_STAT		 =  30,	///< Get system status
	MW_CMD_DEF_CFG_SET	 =  31,	///< Set default configuration
	MW_CMD_HRNG_GET		 =  32,	///< Gets random numbers
	MW_CMD_BSSID_GET	 =  33,	///< Gets the WiFi BSSID
	MW_CMD_GAMERTAG_SET	 =  34,	///< Configures a gamertag
	MW_CMD_GAMERTAG_GET	 =  35,	///< Gets a stored gamertag
	MW_CMD_LOG		 =  36,	///< Write a message to log trace
	MW_CMD_FACTORY_RESET	 =  37,	///< Set default configuratioSet default configuration
	MW_CMD_SLEEP		 =  38,	///< Set the module to sleep mode
	MW_CMD_HTTP_URL_SET	 =  39,	///< Set HTTP URL for request
	MW_CMD_HTTP_METHOD_SET	 =  40,	///< Set HTTP request method
	MW_CMD_HTTP_CERT_QUERY   =  41,	///< Query the X.509 hash of cert
	MW_CMD_HTTP_CERT_SET	 =  42,	///< Set HTTPS certificate
	MW_CMD_HTTP_HDR_ADD	 =  43,	///< Add HTTP request header
	MW_CMD_HTTP_HDR_DEL	 =  44,	///< Delete HTTP request header
	MW_CMD_HTTP_OPEN	 =  45,	///< Open HTTP request
	MW_CMD_HTTP_FINISH	 =  46,	///< Finish HTTP request
	MW_CMD_HTTP_CLEANUP	 =  47,	///< Clean request data
// Reserved
	MW_CMD_SERVER_URL_GET	 =  49,	///< Get the main server URL
	MW_CMD_SERVER_URL_SET	 =  50,	///< Set the main server URL
	MW_CMD_WIFI_ADV_GET	 =  51,	///< Get advanced WiFi parameters
	MW_CMD_WIFI_ADV_SET	 =  52,	///< Set advanced WiFi parameters
	MW_CMD_NV_CFG_SAVE	 =  53,	///< Save non-volatile config
	MW_CMD_UPGRADE_LIST	 =  54,	///< Get firmware upgrade versions
	MW_CMD_UPGRADE_PERFORM	 =  55,	///< Start firmware upgrade
	MW_CMD_GAME_ENDPOINT_SET =  56,	///< Set game API endpoint
	MW_CMD_GAME_KEYVAL_ADD	 =  57,	///< Add key/value appended to requests
	MW_CMD_GAME_REQUEST	 =  58,	///< Perform a game API request
	MW_CMD_ERROR		 = 255	///< Error command reply
};

/// Supported security protocols
enum PACKED mw_security {
	MW_SEC_OPEN = 0,	///< Open WiFi network
	MW_SEC_WEP,		///< WEP security
	MW_SEC_WPA_PSK,		///< WPA PSK security
	MW_SEC_WPA2_PSK,	///< WPA2 PSK security
	MW_SEC_WPA_WPA2_PSK,	///< WPA or WPA2 security
	MW_SEC_WPA2_ENTERPRISE,	///< WPA2 Enterprise security
	MW_SEC_WPA3_PSK,	///< WPA3 PSK security
	MW_SEC_WPA2_WPA3_PSK,	///< WPA2 or WPA3 security
	MW_SEC_WAPI_PSK,	///< WAPI PSK security
	MW_SEC_OWE,		///< OWE security
	MW_SEC_UNKNOWN,		///< Unknown security
	__MW_SEC_MAX
};

/// WiFi PHY configuration for the connection to the AP
enum PACKED mw_phy_type {
	MW_PHY_11B = 1,		///< Legacy, do not use unless necessary
	MW_PHY_11BG = 3,	///< No 802.11n compatibility
	MW_PHY_11BGN = 7	///< 802.11bgn compatible
};

/// IPv4 address
union ip_addr {
	uint32_t addr;		///< IP address in 32 bit form
	uint8_t byte[4];	///< IP address in 4-byte form
};

/// TCP/UDP address message
struct mw_msg_in_addr {
	char dst_port[6];	///< TCP destination port string
	char src_port[6];	///< TCP source port string
	uint8_t channel;	///< LSD channel used for communications
	/// Data payload
	char dst_addr[];
};

/// IP configuration parameters
struct mw_ip_cfg {
	union ip_addr addr;	///< Host IP address in binary format
	union ip_addr mask;	///< Subnet mask in binary IP format
	union ip_addr gateway;	///< Gateway IP address in binary format
	union ip_addr dns1;	///< DNS server 1 IP address in binary format
	union ip_addr dns2;	///< DNS server 2 IP address in binary format
};

/// \brief AP configuration message
/// \warning If ssid length is MW_SSID_MAXLEN, the string will not be NULL
///          terminated. Also if pass length equals MW_PASS_MAXLEN, pass
//           string will not be NULL terminated.
struct mw_msg_ap_cfg {
	uint8_t cfg_num;		///< Configuration number
	enum mw_phy_type phy_type;	///< PHY type bitmask
	char ssid[MW_SSID_MAXLEN];	///< SSID string
	char pass[MW_PASS_MAXLEN];	///< Password string
};

/// IP configuration message
struct mw_msg_ip_cfg {
	uint8_t cfg_slot;	///< Configuration slot
	uint8_t reserved[3];	///< Reserved (set to 0)
	struct mw_ip_cfg ip;	///< IPv4 configuration data
};

/// SNTP and timezone configuration
struct mw_msg_sntp_cfg {
	uint16_t up_delay;	///< Update delay in seconds (min: 15)
	int8_t tz;		///< Timezone (from -11 to 13)
	uint8_t dst;		///< Daylight savines (set to 1 to add 1 hour)
	/// Up to 3 NTP server URLs, separated by a NULL character. A double
	/// NULL marks the end of the server list.
	char servers[MW_CMD_MAX_BUFLEN - 4];
};

/// Date and time message
struct mw_msg_date_time {
	uint32_t dt_bin[2];	///< Number of seconds since Epoch (64-bit)
	/// Date and time in textual format
	char dt_str[MW_CMD_MAX_BUFLEN - 2 * sizeof(uint32_t)];
};

/// Flash memory address and data
struct mw_msg_flash_data {
	uint32_t addr;		///< Flash memory address
	/// Data associated to the address
	uint8_t data[MW_CMD_MAX_BUFLEN - sizeof(uint32_t)];
};

/// Flash memory block
struct mw_msg_flash_range {
	uint32_t addr;		///< Start address
	uint16_t len;		///< Length of the block
};

/// Bind message data
struct mw_msg_bind {
	uint32_t reserved;	///< Reserved, set to 0
	uint16_t port;		///< Port to bind to
	uint8_t  channel;	///< Channel used for the socket bound to port
};

/// Advanced WiFi configuration
struct mw_wifi_adv_cfg {
	uint8_t qos_enable;			///< WiFi QOS feature enable flag
	uint8_t ampdu_rx_enable;		///< WiFi AMPDU RX feature enable flag
	uint8_t rx_ba_win;			///< WiFi Block Ack RX window size
	uint8_t rx_ampdu_buf_num;		///< WiFi AMPDU RX buffer number
	uint32_t rx_ampdu_buf_len;		///< WiFi AMPDU RX buffer length
	uint32_t rx_max_single_pkt_len;		///< WiFi RX max single packet size
	uint32_t rx_buf_len;			///< WiFi RX buffer size
	uint8_t amsdu_rx_enable;		///< WiFi AMSDU RX feature enable flag
	uint8_t rx_buf_num;			///< WiFi RX buffer number
	uint8_t rx_pkt_num;			///< WiFi RX packet number
	uint8_t left_continuous_rx_buf_num;	///< WiFi Rx left continuous rx buffer number
	uint8_t tx_buf_num;			///< WiFi TX buffer number
	uint8_t reserved[3];			///< Unused, set to 0
};

/// Gamertag data
struct mw_gamertag {
	/// Unique gamertag id
	int id;
	/// User nickname
	char nickname[MW_GT_NICKNAME_MAX];
	/// User security string
	char security[MW_GT_SECURITY_MAX];
	/// User defined text tag
	char tagline[MW_GT_TAGLINE_MAX];
	/// Telegram token
	char tg_token[MW_GT_TG_TOKEN_MAX];
	/// Avatar image tiles
	uint8_t avatar_tiles[MW_GT_AVATAR_WIDTH * MW_GT_AVATAR_HEIGHT / 2];
	/// Avatar image palette
	uint8_t avatar_pal[32];
};

/// Gamertag set message data
struct mw_gamertag_set_msg {
	uint8_t slot;			///< Slot to store gamertag (0 to 2)
	uint8_t reserved[3];		///< Reserved, set to 0
	struct mw_gamertag gamertag;	///< Gamertag to set
};

/// MwState Possible states of the system state machine.
enum mw_state {
	MW_ST_INIT = 0,		///< Initialization state.
	MW_ST_IDLE,		///< Idle state, until connected to an AP.
	MW_ST_AP_JOIN,		///< Trying to join an access point.
	MW_ST_SCAN,		///< Scanning access points.
	MW_ST_READY,		///< Connected to The Internet.
	MW_ST_TRANSPARENT,	///< Transparent communication state.
	MW_ST_MAX		///< Limit number for state machine.
};

/// Socket status.
enum mw_sock_stat {
	MW_SOCK_NONE = 0,	///< Unused socket.
	MW_SOCK_TCP_LISTEN,	///< Socket bound and listening.
	MW_SOCK_TCP_EST,	///< TCP socket, connection established.
	MW_SOCK_UDP_READY	///< UDP socket ready for sending/receiving
};

/// System status
union mw_msg_sys_stat {
	uint32_t st_flags;		///< Accesses all the flags at once
	struct {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		enum mw_state sys_stat:8;	///< System status
		uint8_t online:1;	///< Module is connected to the Internet
		uint8_t cfg_ok:1;	///< Configuration OK
		uint8_t dt_ok:1;	///< Date and time synchronized at least once
		uint8_t cfg:2;		///< Default network configuration
		uint16_t reserved:3;	///< Reserved flags
		uint16_t ch_ev:16;	///< Channel flags with the pending event
#else
		uint16_t ch_ev:16;	///< Channel flags with the pending event
		uint16_t reserved:3;	///< Reserved flags
		uint8_t cfg:2;		///< Default network configuration
		uint8_t dt_ok:1;	///< Date and time synchronized at least once
		uint8_t cfg_ok:1;	///< Configuration OK
		uint8_t online:1;	///< Module is connected to the Internet
		enum mw_state sys_stat:8;	///< System status
#endif
	};
};

/// Flash chip identifiers
struct mw_flash_id {
	uint16_t device;	///< Device ID
	uint8_t manufacturer;	///< Manufacturer ID
};

/// Game API request
struct mw_ga_request {
	uint8_t method;		///< Request method
	uint8_t num_paths;	///< Number of paths
	uint8_t num_kv_pairs;	///< Number of key/value pairs
	char req[];		///< Request data
};

struct mw_ping_request{
	uint8_t retries;
	char domain[64];
};

struct mw_ping_response{
	uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
};

struct mw_upgrade_list_response{
	uint16_t total;
	uint16_t len;
    char *payload;
};

/// Command sent to system FSM
typedef union mw_cmd {
	char packet[MW_CMD_MAX_BUFLEN + 2 * sizeof(uint16_t)];	///< Packet raw data
	struct {
		uint16_t cmd;			///< Command code
		uint16_t data_len;		///< Data length
		// If datalen is nonzero, additional command data goes here until
		// filling datalen bytes.
		union {
			uint8_t ch;		///< Channel number for channel related requests
			/// RAW data in uint8_t format
			uint8_t data[MW_CMD_MAX_BUFLEN];
			/// RAW data in uint16_t format
			uint16_t w_data[MW_CMD_MAX_BUFLEN / sizeof(uint16_t)];
			/// RAW data in uint32_t format
			uint32_t dw_data[MW_CMD_MAX_BUFLEN / sizeof(uint32_t)];
			struct mw_msg_in_addr in_addr;		///< Internet address
			struct mw_msg_ap_cfg ap_cfg;		///< Access Point configuration
			struct mw_msg_ip_cfg ip_cfg;		///< IP configuration
			struct mw_msg_sntp_cfg sntp_cfg;	///< SNTP client configuration
			struct mw_msg_date_time date_time;	///< Date and time message
			struct mw_msg_flash_data fl_data;	///< Flash memory data
			struct mw_msg_flash_range fl_range;	///< Flash memory range
			struct mw_msg_bind bind;		///< Bind message
			struct mw_ping_request ping;    ///< Ping message
			struct mw_ping_response ping_response;    ///< Ping message
			struct mw_upgrade_list_response ug_list_response;    ///< Ping message
			union mw_msg_sys_stat sys_stat;		///< System status
			struct mw_gamertag_set_msg gamertag_set;///< Gamertag set
			struct mw_gamertag gamertag_get;	///< Gamertag get
			struct mw_wifi_adv_cfg wifi_adv_cfg;	///< Advanced WiFi configuration
			struct mw_flash_id flash_id;		///< Flash chip identifiers
			struct mw_ga_request ga_request;	///< Game API request
			uint16_t fl_sect;	///< Flash sector
			uint32_t fl_id;		///< Flash IDs
			uint16_t rnd_len;	///< Length of the random buffer to fill
		};
	};
} mw_cmd;

/// \brief Payload with remote IP and port.
///
/// Data sent/received using UDP sockets, uses this special format when
/// reuse flag has been set in the mw_udp_set() call. This allows the program
/// using the UDP socket, to filter incoming IPs, and to be able to properly
/// answer to incoming packets from several peers.
struct mw_reuse_payload {
	uint32_t remote_ip;	///< IP of the remote end
	uint16_t remote_port;	///< Port of the remote end
	/// Data payload
	char payload[MW_CMD_MAX_BUFLEN - 4 - 2];
};

#endif //_MW_MSG_H_

/** \} */
````

## File: inc/ext/mw/ssf_ed_pro.h
````c
/************************************************************************
 * \brief Simple SSF driver to Everdrive PRO.
 *
 * \author Juan Antonio Ruiz (PaCHoN)
 * \date   2024-2025
 * \defgroup SSF SSF
 * \brief
 *      https://github.com/krikzz/mega-ed-pub
*************************************** */
#ifndef _SSF_ED_PRO_H_
#define _SSF_ED_PRO_H_

#include "types.h"

#define REG_FIFO_DATA (*((volatile u16*)(0xA130D0)))
#define REG_FIFO_STAT (*((volatile u16*)(0xA130D2)))
#define REG_SYS_STAT (*((volatile u16*)(0xA130D4)))

#define FIFO_CPU_RXF 0x8000 // fifo flags. system cpu can read
#define FIFO_RXF_MSK 0x7FF
#define STAT_PRO_PRESENT 0x55A0
#define CMD_USB_WR 0x22

#define MW_EDPRO_BUFLEN	1436
#define MW_EDPRO_TXFIFO_LEN 512

bool ssf_ed_pro_is_present(void);
void ssf_ed_pro_write(u8 data);
bool ssf_ed_pro_read_ready(void);
bool ssf_ed_pro_write_ready(void);
u8 ssf_ed_pro_read(void);

/************************************************************************//**
 * \brief Initializes the driver on Everdrive PRO.
 ****************************************************************************/
void ssf_ed_pro_init(void);

#endif /*_SSF_ED_PRO_H_*/
````

## File: inc/ext/mw/ssf_ed_x7.h
````c
/************************************************************************
 * \brief Simple SSF driver to Everdrive X7.
 *
 * \author Juan Antonio Ruiz (PaCHoN)
 * \date   2024-2025
 * \defgroup SSF SSF
 * \brief
 *      USB IO
 *        0xA130E2 [........ DDDDDDDD] read/write
 *        D data bits
 * 
 *      IO status
 *        0xA130E4 [.C...... .....RWS] read only
 *        S SPI controller ready. Not used on WIFI.
 *        W USB fifo ready to write
 *        R USB fifo ready to read
 *        C SD card type. 0=SD, 1=SDHC. Not used on WIFI.
*************************************** */
#ifndef _SSF_ED_X7_H_
#define _SSF_SSF_ED_X7_H_

#include "types.h"

//#define SSF_CTRL_P 0x8000 //register accesss protection bit. should be set, otherwise register will ignore any attempts to write
//#define SSF_CTRL_X 0x4000 //32x mode
//#define SSF_CTRL_W 0x2000 //ROM memory write protection
//#define SSF_CTRL_L 0x1000 //led

//#define USB_RD_BUSY while ((REG_STE & STE_USB_RD_RDY) == 0)
//#define USB_WR_BUSY while ((REG_STE & STE_USB_WR_RDY) == 0)
/**************************************** */


/// SSF UART base address
#define X7_UART_BASE		0xA13000

/// Receiver holding register. Read only.
#define X7_UART_DATA	          (*((volatile u16*)(X7_UART_BASE +  226)))
#define X7_UART_STE	          (*((volatile u16*)(X7_UART_BASE +  228)))
#define X7_UART_REG_CFG          (*((volatile u16*)(X7_UART_BASE +  230)))
#define X7_UART_REG_SSF_CTRL     (*((volatile u16*)(X7_UART_BASE +  240)))

#define SSF_CTRL_P 0x8000 //register accesss protection bit. should be set, otherwise register will ignore any attempts to write
#define SSF_CTRL_X 0x4000 //32x mode
#define SSF_CTRL_W 0x2000 //ROM memory write protection
#define SSF_CTRL_L 0x1000 //led

#define X7_UART_SPR X7_UART_DATA

#define X7_UART_STE_WR_RDY 2//usb write ready bit
#define X7_UART_STE_RD_RDY 4//usb read ready bit

//spi chip select signal
#define CFG_SPI_SS 1
#define CFG_SPI_QRD 6
#define CFG_SPI_QWR 2

#define MW_EDX7_BUFLEN	1436
#define MW_EDX7_TXFIFO_LEN 1

bool ssf_ed_x7_is_present(void);
void ssf_ed_x7_write(u8 data);
bool ssf_ed_x7_read_ready(void);
bool ssf_ed_x7_write_ready(void);
u8 ssf_ed_x7_read(void);

/************************************************************************//**
 * \brief Initializes the driver on Everdrive X7.
 ****************************************************************************/
void ssf_ed_x7_init(void);

#endif /*_SSF_ED_X7_H_*/
````

## File: inc/ext/serial/buffer.h
````c
#include "genesis.h"

u16 buffer_init(u16 size);
u8 buffer_read(void);
void buffer_write(u8 data);
u8 buffer_canRead(void);
u16 buffer_available(void);
void buffer_free(void);
````

## File: inc/ext/serial/serial.h
````c
/************************************************************************
 * \brief Simple Serial Driver to comunicate by game port.
 *
 * \author Juan Antonio Ruiz (PaCHoN)
 * \date   2024-2025
 * \defgroup MW Serial
 * \brief
 *      ## Hardware
 * 
 *      ### Controller Port Pinout
 *      
 *      | Pin | Parallel Mode \*    | Serial Mode | I/O Reg Bit |
 *      | --- | ------------------- | ----------- | ----------- |
 *      | 1   | Up                  |             | UP (D0)     |
 *      | 2   | Down                |             | DOWN (D1)   |
 *      | 3   | Gnd / Left          |             | LEFT (D2)   |
 *      | 4   | Gnd / Right         |             | RIGHT (D3)  |
 *      | 5   | +5VDC               | +5VDC       |
 *      | 6   | Button A / Button B | Tx          | TL (D4)     |
 *      | 7   | Select              |             |
 *      | 8   | Gnd                 | Gnd         |
 *      | 9   | Start / Button C    | Rx          | TR (D5)     |
 *      
 *      \* Pins in parallel mode can have two interpretations, depending on if `Select` has been set low or high (L / H) from the console.
 *      
 *      ### Controller Plug
 *      
 *      Looking directly at plug (Female 9-pin Type D)
 *      
 *      ```
 *      -------------
 *      \ 5 4 3 2 1 /
 *       \ 9 8 7 6 /
 *        ---------
 *      ```
 *      
 *      ### FTDI USB TTL Serial Cable
 *      
 *      From the [datasheet](https://www.ftdichip.com/Support/Documents/DataSheets/Cables/DS_TTL-232RG_CABLES.pdf):
 *      
 *      | Colour | Name | Type            | Description                                                                                                                        |
 *      | ------ | ---- | --------------- | ---------------------------------------------------------------------------------------------------------------------------------- |
 *      | Black  | GND  | GND             | Device ground supply pin.                                                                                                          |
 *      | Brown  | CTS# | Input           | Clear to Send Control input / Handshake signal.                                                                                    |
 *      | Red    | VCC  | Output or input | Power Supply Output except for the TTL-232RG-VIPWE were this is an input and power is supplied by the application interface logic. |
 *      | Orange | TXD  | Output          | Transmit Asynchronous Data output.                                                                                                 |
 *      | Yellow | RXD  | Input           | Receive Asynchronous Data input.                                                                                                   |
 *      | Green  | RTS# | Output          | Request To Send Control Output / Handshake signal.                                                                                 |
 *      
 *      ### Mappings
 *      
 *      | FTDI Cable   | Mega Drive Port Pin |
 *      | ------------ | ------------------- |
 *      | Red (VCC)    | 5 (5v)              |
 *      | Orange (TXD) | 9 (Rx)              |
 *      | Yellow (RXD) | 6 (Tx)              |
 *      | Black (GND)  | 8 (Gnd)             |
 *      *************************************** */
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "genesis.h"
#include "types.h"

#define PORT2_CTRL 0xA1000B
#define PORT2_SCTRL 0xA10019
#define PORT2_TX 0xA10015
#define PORT2_RX 0xA10017

#define EXT_CTRL 0xA1000D
#define EXT_SCTRL 0xA1001F
#define EXT_TX 0xA1001B
#define EXT_RX 0xA1001D

#define SCTRL_SIN 0x20
#define SCTRL_SOUT 0x10
#define SCTRL_300_BPS 0xC0
#define SCTRL_1200_BPS 0x80
#define SCTRL_2400_BPS 0x40
#define SCTRL_4800_BPS 0x00

#define SCTRL_TFUL 0x1
#define SCTRL_RRDY 0x2
#define SCTRL_RERR 0x4
#define SCTRL_RINT 0x8

#define CTRL_PCS_OUT 0x7F

#define VDP_MODE_REG_3 0xB
#define VDP_IE2 0x08
#define INT_MASK_LEVEL_ENABLE_ALL 1

#define SERIAL_BUFLEN	1
#define SERIAL_TXFIFO_LEN 1

typedef enum IoPort {
    IoPort_Ext,
    IoPort_Ctrl2
} IoPort;

bool serial_is_present(void);
void serial_write(u8 data);
bool serial_read_ready(void);
bool serial_write_ready(void);
u8 serial_read(void);
void serial_init(void);

void serial_reset_fifos(void);
u16 serial_baud_rate(void);
u8 serial_get_sctrl(void);
void serial_set_sctrl(u8 value);

/**
 * \brief Set the serial mode (asynchronous or synchronous).
 * \param value The mode to set. Use size of buffer for asynchronous mode, or 0 for synchronous mode.
 * \return Number of bytes available to read in the receive buffer.
 */
u16 serial_set_mode(u16 value);

/**
 * \brief Get the current receive buffer length.
 * \param port The I/O port to query.
 * \return Number of bytes available to read in the receive buffer.
 */
void serial_set_port(IoPort port);

/**
 * \brief Get the current I/O port used for serial communication.
 * \return The I/O port currently in use for serial communication.
 */
IoPort serial_get_port(void);

#endif /*_SERIAL_H_*/
````

## File: inc/ext/stb/stb_sprintf.h
````c
// stb_sprintf - v1.06 - public domain snprintf() implementation
// originally by Jeff Roberts / RAD Game Tools, 2015/10/20
// http://github.com/nothings/stb
//
// allowed types:  sc uidBboXx p AaGgEef n
// lengths      :  h ll j z t I64 I32 I
//
// Contributors:
//    Fabian "ryg" Giesen (reformatting)
//
// Contributors (bugfixes):
//    github:d26435
//    github:trex78
//    github:account-login
//    Jari Komppa (SI suffixes)
//    Rohit Nirmal
//    Marcin Wojdyr
//    Leonard Ritter
//    Stefano Zanotti
//    Adam Allison
//
// LICENSE:
//
//   See end of file for license information.

#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE

/*
Single file sprintf replacement.

Originally written by Jeff Roberts at RAD Game Tools - 2015/10/20.
Hereby placed in public domain.

This is a full sprintf replacement that supports everything that
the C runtime sprintfs support, including float/double, 64-bit integers,
hex floats, field parameters (%*.*d stuff), length reads backs, etc.

Why would you need this if sprintf already exists?  Well, first off,
it's *much* faster (see below). It's also much smaller than the CRT
versions code-space-wise. We've also added some simple improvements
that are super handy (commas in thousands, callbacks at buffer full,
for example). Finally, the format strings for MSVC and GCC differ
for 64-bit integers (among other small things), so this lets you use
the same format strings in cross platform code.

It uses the standard single file trick of being both the header file
and the source itself. If you just include it normally, you just get
the header file function definitions. To get the code, you include
it from a C or C++ file and define STB_SPRINTF_IMPLEMENTATION first.

It only uses va_args macros from the C runtime to do it's work. It
does cast doubles to S64s and shifts and divides U64s, which does
drag in CRT code on most platforms.

It compiles to roughly 8K with float support, and 4K without.
As a comparison, when using MSVC static libs, calling sprintf drags
in 16K.

API:
====
int stbsp_sprintf( char * buf, char const * fmt, ... )
int stbsp_snprintf( char * buf, int count, char const * fmt, ... )
  Convert an arg list into a buffer.  stbsp_snprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintf( char * buf, char const * fmt, va_list va )
int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
  Convert a va_list arg list into a buffer.  stbsp_vsnprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintfcb( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va )
    typedef char * STBSP_SPRINTFCB( char const * buf, void * user, int len );
  Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
  Your callback can then copy the chars out, print them or whatever.
  This function is actually the workhorse for everything else.
  The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
    // you return the next buffer to use or 0 to stop converting

void stbsp_set_separators( char comma, char period )
  Set the comma and period characters to use.

FLOATS/DOUBLES:
===============
This code uses a internal float->ascii conversion method that uses
doubles with error correction (double-doubles, for ~105 bits of
precision).  This conversion is round-trip perfect - that is, an atof
of the values output here will give you the bit-exact double back.

One difference is that our insignificant digits will be different than
with MSVC or GCC (but they don't match each other either).  We also
don't attempt to find the minimum length matching float (pre-MSVC15
doesn't either).

If you don't need float or doubles at all, define STB_SPRINTF_NOFLOAT
and you'll save 4K of code space.

64-BIT INTS:
============
This library also supports 64-bit integers and you can use MSVC style or
GCC style indicators (%I64d or %lld).  It supports the C99 specifiers
for size_t and ptr_diff_t (%jd %zd) as well.

EXTRAS:
=======
Like some GCCs, for integers and floats, you can use a ' (single quote)
specifier and commas will be inserted on the thousands: "%'d" on 12345
would print 12,345.

For integers and floats, you can use a "$" specifier and the number
will be converted to float and then divided to get kilo, mega, giga or
tera and then printed, so "%$d" 1000 is "1.0 k", "%$.2d" 2536000 is
"2.53 M", etc. For byte values, use two $:s, like "%$$d" to turn
2536000 to "2.42 Mi". If you prefer JEDEC suffixes to SI ones, use three
$:s: "%$$$d" -> "2.42 M". To remove the space between the number and the
suffix, add "_" specifier: "%_$d" -> "2.53M".

In addition to octal and hexadecimal conversions, you can print
integers in binary: "%b" for 256 would print 100.

PERFORMANCE vs MSVC 2008 32-/64-bit (GCC is even slower than MSVC):
===================================================================
"%d" across all 32-bit ints (4.8x/4.0x faster than 32-/64-bit MSVC)
"%24d" across all 32-bit ints (4.5x/4.2x faster)
"%x" across all 32-bit ints (4.5x/3.8x faster)
"%08x" across all 32-bit ints (4.3x/3.8x faster)
"%f" across e-10 to e+10 floats (7.3x/6.0x faster)
"%e" across e-10 to e+10 floats (8.1x/6.0x faster)
"%g" across e-10 to e+10 floats (10.0x/7.1x faster)
"%f" for values near e-300 (7.9x/6.5x faster)
"%f" for values near e+300 (10.0x/9.1x faster)
"%e" for values near e-300 (10.1x/7.0x faster)
"%e" for values near e+300 (9.2x/6.0x faster)
"%.320f" for values near e-300 (12.6x/11.2x faster)
"%a" for random values (8.6x/4.3x faster)
"%I64d" for 64-bits with 32-bit values (4.8x/3.4x faster)
"%I64d" for 64-bits > 32-bit values (4.9x/5.5x faster)
"%s%s%s" for 64 char strings (7.1x/7.3x faster)
"...512 char string..." ( 35.0x/32.5x faster!)
*/

#if defined(__has_feature)
   #if __has_feature(address_sanitizer)
      #define STBI__ASAN __attribute__((no_sanitize("address")))
   #endif
#endif
#ifndef STBI__ASAN
#define STBI__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBI__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBI__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBI__ASAN
#endif
#endif

#ifndef STB_SPRINTF_NOSTD
#include <stdarg.h> // for va_list()
#include <stddef.h> // size_t, ptrdiff_t
#endif

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char *STBSP_SPRINTFCB(char *buf, void *user, int len);

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name // define this before including if you want to change the names
#endif

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...);

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);

#endif // STB_SPRINTF_H_INCLUDE

#ifdef STB_SPRINTF_IMPLEMENTATION

#ifndef STB_SPRINTF_NOSTD
#include <stdlib.h> // for va_arg()
#endif

#define stbsp__uint32 unsigned int
#define stbsp__int32 signed int

#ifdef _MSC_VER
#define stbsp__uint64 unsigned __int64
#define stbsp__int64 signed __int64
#else
#define stbsp__uint64 unsigned long long
#define stbsp__int64 signed long long
#endif
#define stbsp__uint16 unsigned short

#ifndef stbsp__uintptr
#if defined(__ppc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
#define stbsp__uintptr stbsp__uint64
#else
#define stbsp__uintptr stbsp__uint32
#endif
#endif

#ifndef STB_SPRINTF_MSVC_MODE // used for MSVC2013 and earlier (MSVC2015 matches GCC)
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define STB_SPRINTF_MSVC_MODE
#endif
#endif

#ifdef STB_SPRINTF_NOUNALIGNED // define this before inclusion to force stbsp_sprintf to always use aligned accesses
#define STBSP__UNALIGNED(code)
#else
#define STBSP__UNALIGNED(code) code
#endif

#ifndef STB_SPRINTF_NOFLOAT
// internal float utility functions
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits);
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value);
#define STBSP__SPECIAL 0x7000
#endif

static char stbsp__period = '.';
static char stbsp__comma = ',';
static struct
{
   short temp; // force next field to be 2-byte aligned
   char pair[201];
} stbsp__digitpair =
{
  0,
   "00010203040506070809101112131415161718192021222324"
   "25262728293031323334353637383940414243444546474849"
   "50515253545556575859606162636465666768697071727374"
   "75767778798081828384858687888990919293949596979899"
};

STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char pcomma, char pperiod)
{
   stbsp__period = pperiod;
   stbsp__comma = pcomma;
}

#define STBSP__LEFTJUST 1
#define STBSP__LEADINGPLUS 2
#define STBSP__LEADINGSPACE 4
#define STBSP__LEADING_0X 8
#define STBSP__LEADINGZERO 16
#define STBSP__INTMAX 32
#define STBSP__TRIPLET_COMMA 64
#define STBSP__NEGATIVE 128
#define STBSP__METRIC_SUFFIX 256
#define STBSP__HALFWIDTH 512
#define STBSP__METRIC_NOSPACE 1024
#define STBSP__METRIC_1024 2048
#define STBSP__METRIC_JEDEC 4096

static void stbsp__lead_sign(stbsp__uint32 fl, char *sign)
{
   sign[0] = 0;
   if (fl & STBSP__NEGATIVE) {
      sign[0] = 1;
      sign[1] = '-';
   } else if (fl & STBSP__LEADINGSPACE) {
      sign[0] = 1;
      sign[1] = ' ';
   } else if (fl & STBSP__LEADINGPLUS) {
      sign[0] = 1;
      sign[1] = '+';
   }
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va)
{
   static char hex[] = "0123456789abcdefxp";
   static char hexu[] = "0123456789ABCDEFXP";
   char *bf;
   char const *f;
   int tlen = 0;

   bf = buf;
   f = fmt;
   for (;;) {
      stbsp__int32 fw, pr, tz;
      stbsp__uint32 fl;

      // macros for the callback buffer stuff
      #define stbsp__chk_cb_bufL(bytes)                        \
         {                                                     \
            int len = (int)(bf - buf);                         \
            if ((len + (bytes)) >= STB_SPRINTF_MIN) {          \
               tlen += len;                                    \
               if (0 == (bf = buf = callback(buf, user, len))) \
                  goto done;                                   \
            }                                                  \
         }
      #define stbsp__chk_cb_buf(bytes)    \
         {                                \
            if (callback) {               \
               stbsp__chk_cb_bufL(bytes); \
            }                             \
         }
      #define stbsp__flush_cb()                      \
         {                                           \
            stbsp__chk_cb_bufL(STB_SPRINTF_MIN - 1); \
         } // flush if there is even one byte in the buffer
      #define stbsp__cb_buf_clamp(cl, v)                \
         cl = v;                                        \
         if (callback) {                                \
            int lg = STB_SPRINTF_MIN - (int)(bf - buf); \
            if (cl > lg)                                \
               cl = lg;                                 \
         }

      // fast copy everything up to the next % (or end of string)
      for (;;) {
         while (((stbsp__uintptr)f) & 3) {
         schk1:
            if (f[0] == '%')
               goto scandd;
         schk2:
            if (f[0] == 0)
               goto endfmt;
            stbsp__chk_cb_buf(1);
            *bf++ = f[0];
            ++f;
         }
         for (;;) {
            // Check if the next 4 bytes contain %(0x25) or end of string.
            // Using the 'hasless' trick:
            // https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
            stbsp__uint32 v, c;
            v = *(stbsp__uint32 *)f;
            c = (~v) & 0x80808080;
            if (((v ^ 0x25252525) - 0x01010101) & c)
               goto schk1;
            if ((v - 0x01010101) & c)
               goto schk2;
            if (callback)
               if ((STB_SPRINTF_MIN - (int)(bf - buf)) < 4)
                  goto schk1;
            #ifdef STB_SPRINTF_NOUNALIGNED
                if(((stbsp__uintptr)bf) & 3) {
                    bf[0] = f[0];
                    bf[1] = f[1];
                    bf[2] = f[2];
                    bf[3] = f[3];
                } else
            #endif
            {
                *(stbsp__uint32 *)bf = v;
            }
            bf += 4;
            f += 4;
         }
      }
   scandd:

      ++f;

      // ok, we have a percent, read the modifiers first
      fw = 0;
      pr = -1;
      fl = 0;
      tz = 0;

      // flags
      for (;;) {
         switch (f[0]) {
         // if we have left justify
         case '-':
            fl |= STBSP__LEFTJUST;
            ++f;
            continue;
         // if we have leading plus
         case '+':
            fl |= STBSP__LEADINGPLUS;
            ++f;
            continue;
         // if we have leading space
         case ' ':
            fl |= STBSP__LEADINGSPACE;
            ++f;
            continue;
         // if we have leading 0x
         case '#':
            fl |= STBSP__LEADING_0X;
            ++f;
            continue;
         // if we have thousand commas
         case '\'':
            fl |= STBSP__TRIPLET_COMMA;
            ++f;
            continue;
         // if we have kilo marker (none->kilo->kibi->jedec)
         case '$':
            if (fl & STBSP__METRIC_SUFFIX) {
               if (fl & STBSP__METRIC_1024) {
                  fl |= STBSP__METRIC_JEDEC;
               } else {
                  fl |= STBSP__METRIC_1024;
               }
            } else {
               fl |= STBSP__METRIC_SUFFIX;
            }
            ++f;
            continue;
         // if we don't want space between metric suffix and number
         case '_':
            fl |= STBSP__METRIC_NOSPACE;
            ++f;
            continue;
         // if we have leading zero
         case '0':
            fl |= STBSP__LEADINGZERO;
            ++f;
            goto flags_done;
         default: goto flags_done;
         }
      }
   flags_done:

      // get the field width
      if (f[0] == '*') {
         fw = va_arg(va, stbsp__uint32);
         ++f;
      } else {
         while ((f[0] >= '0') && (f[0] <= '9')) {
            fw = fw * 10 + f[0] - '0';
            f++;
         }
      }
      // get the precision
      if (f[0] == '.') {
         ++f;
         if (f[0] == '*') {
            pr = va_arg(va, stbsp__uint32);
            ++f;
         } else {
            pr = 0;
            while ((f[0] >= '0') && (f[0] <= '9')) {
               pr = pr * 10 + f[0] - '0';
               f++;
            }
         }
      }

      // handle integer size overrides
      switch (f[0]) {
      // are we halfwidth?
      case 'h':
         fl |= STBSP__HALFWIDTH;
         ++f;
         break;
      // are we 64-bit (unix style)
      case 'l':
         fl |= ((sizeof(long) == 8) ? STBSP__INTMAX : 0);
         ++f;
         if (f[0] == 'l') {
            fl |= STBSP__INTMAX;
            ++f;
         }
         break;
      // are we 64-bit on intmax? (c99)
      case 'j':
         fl |= (sizeof(size_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      // are we 64-bit on size_t or ptrdiff_t? (c99)
      case 'z':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      case 't':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      // are we 64-bit (msft style)
      case 'I':
         if ((f[1] == '6') && (f[2] == '4')) {
            fl |= STBSP__INTMAX;
            f += 3;
         } else if ((f[1] == '3') && (f[2] == '2')) {
            f += 3;
         } else {
            fl |= ((sizeof(void *) == 8) ? STBSP__INTMAX : 0);
            ++f;
         }
         break;
      default: break;
      }

      // handle each replacement
      switch (f[0]) {
         #define STBSP__NUMSZ 512 // big enough for e308 (with commas) or e-307
         char num[STBSP__NUMSZ];
         char lead[8];
         char tail[8];
         char *s;
         char const *h;
         stbsp__uint32 l, n, cs;
         stbsp__uint64 n64;
#ifndef STB_SPRINTF_NOFLOAT
         double fv;
#endif
         stbsp__int32 dp;
         char const *sn;

      case 's':
         // get the string
         s = va_arg(va, char *);
         if (s == 0)
            s = (char *)"null";
         // get the length
         sn = s;
         for (;;) {
            if ((((stbsp__uintptr)sn) & 3) == 0)
               break;
         lchk:
            if (sn[0] == 0)
               goto ld;
            ++sn;
         }
         n = 0xffffffff;
         if (pr >= 0) {
            n = (stbsp__uint32)(sn - s);
            if (n >= (stbsp__uint32)pr)
               goto ld;
            n = ((stbsp__uint32)(pr - n)) >> 2;
         }
         while (n) {
            stbsp__uint32 v = *(stbsp__uint32 *)sn;
            if ((v - 0x01010101) & (~v) & 0x80808080UL)
               goto lchk;
            sn += 4;
            --n;
         }
         goto lchk;
      ld:

         l = (stbsp__uint32)(sn - s);
         // clamp to precision
         if (l > (stbsp__uint32)pr)
            l = pr;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         // copy the string in
         goto scopy;

      case 'c': // char
         // get the character
         s = num + STBSP__NUMSZ - 1;
         *s = (char)va_arg(va, int);
         l = 1;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;

      case 'n': // weird write-bytes specifier
      {
         int *d = va_arg(va, int *);
         *d = tlen + (int)(bf - buf);
      } break;

#ifdef STB_SPRINTF_NOFLOAT
      case 'A':              // float
      case 'a':              // hex float
      case 'G':              // float
      case 'g':              // float
      case 'E':              // float
      case 'e':              // float
      case 'f':              // float
         va_arg(va, double); // eat it
         s = (char *)"No float";
         l = 8;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
#else
      case 'A': // hex float
      case 'a': // hex float
         h = (f[0] == 'A') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_parts((stbsp__int64 *)&n64, &dp, fv))
            fl |= STBSP__NEGATIVE;

         s = num + 64;

         stbsp__lead_sign(fl, lead);

         if (dp == -1023)
            dp = (n64) ? -1022 : 0;
         else
            n64 |= (((stbsp__uint64)1) << 52);
         n64 <<= (64 - 56);
         if (pr < 15)
            n64 += ((((stbsp__uint64)8) << 56) >> (pr * 4));
// add leading chars

#ifdef STB_SPRINTF_MSVC_MODE
         *s++ = '0';
         *s++ = 'x';
#else
         lead[1 + lead[0]] = '0';
         lead[2 + lead[0]] = 'x';
         lead[0] += 2;
#endif
         *s++ = h[(n64 >> 60) & 15];
         n64 <<= 4;
         if (pr)
            *s++ = stbsp__period;
         sn = s;

         // print the bits
         n = pr;
         if (n > 13)
            n = 13;
         if (pr > (stbsp__int32)n)
            tz = pr - n;
         pr = 0;
         while (n--) {
            *s++ = h[(n64 >> 60) & 15];
            n64 <<= 4;
         }

         // print the expo
         tail[1] = h[17];
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
         n = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }

         dp = (int)(s - sn);
         l = (int)(s - (num + 64));
         s = num + 64;
         cs = 1 + (3 << 24);
         goto scopy;

      case 'G': // float
      case 'g': // float
         h = (f[0] == 'G') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6;
         else if (pr == 0)
            pr = 1; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000))
            fl |= STBSP__NEGATIVE;

         // clamp the precision and delete extra zeros after clamp
         n = pr;
         if (l > (stbsp__uint32)pr)
            l = pr;
         while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
            --pr;
            --l;
         }

         // should we use %e
         if ((dp <= -4) || (dp > (stbsp__int32)n)) {
            if (pr > (stbsp__int32)l)
               pr = l - 1;
            else if (pr)
               --pr; // when using %e, there is one digit before the decimal
            goto doexpfromg;
         }
         // this is the insane action to get the pr to match %g semantics for %f
         if (dp > 0) {
            pr = (dp < (stbsp__int32)l) ? l - dp : 0;
         } else {
            pr = -dp + ((pr > (stbsp__int32)l) ? (stbsp__int32) l : pr);
         }
         goto dofloatfromg;

      case 'E': // float
      case 'e': // float
         h = (f[0] == 'E') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000))
            fl |= STBSP__NEGATIVE;
      doexpfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;
         // handle leading chars
         *s++ = sn[0];

         if (pr)
            *s++ = stbsp__period;

         // handle after decimal
         if ((l - 1) > (stbsp__uint32)pr)
            l = pr + 1;
         for (n = 1; n < l; n++)
            *s++ = sn[n];
         // trailing zeros
         tz = pr - (l - 1);
         pr = 0;
         // dump expo
         tail[1] = h[0xe];
         dp -= 1;
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
#ifdef STB_SPRINTF_MSVC_MODE
         n = 5;
#else
         n = (dp >= 100) ? 5 : 4;
#endif
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }
         cs = 1 + (3 << 24); // how many tens
         goto flt_lead;

      case 'f': // float
         fv = va_arg(va, double);
      doafloat:
         // do kilos
         if (fl & STBSP__METRIC_SUFFIX) {
            double divisor;
            divisor = 1000.0f;
            if (fl & STBSP__METRIC_1024)
               divisor = 1024.0;
            while (fl < 0x4000000) {
               if ((fv < divisor) && (fv > -divisor))
                  break;
               fv /= divisor;
               fl += 0x1000000;
            }
         }
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr))
            fl |= STBSP__NEGATIVE;
      dofloatfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;

         // handle the three decimal varieties
         if (dp <= 0) {
            stbsp__int32 i;
            // handle 0.000*000xxxx
            *s++ = '0';
            if (pr)
               *s++ = stbsp__period;
            n = -dp;
            if ((stbsp__int32)n > pr)
               n = pr;
            i = n;
            while (i) {
               if ((((stbsp__uintptr)s) & 3) == 0)
                  break;
               *s++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)s = 0x30303030;
               s += 4;
               i -= 4;
            }
            while (i) {
               *s++ = '0';
               --i;
            }
            if ((stbsp__int32)(l + n) > pr)
               l = pr - n;
            i = l;
            while (i) {
               *s++ = *sn++;
               --i;
            }
            tz = pr - (n + l);
            cs = 1 + (3 << 24); // how many tens did we write (for commas below)
         } else {
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((600 - (stbsp__uint32)dp) % 3) : 0;
            if ((stbsp__uint32)dp >= l) {
               // handle xxxx000*000.0
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= l)
                        break;
                  }
               }
               if (n < (stbsp__uint32)dp) {
                  n = dp - n;
                  if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                     while (n) {
                        if ((((stbsp__uintptr)s) & 3) == 0)
                           break;
                        *s++ = '0';
                        --n;
                     }
                     while (n >= 4) {
                        *(stbsp__uint32 *)s = 0x30303030;
                        s += 4;
                        n -= 4;
                     }
                  }
                  while (n) {
                     if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                        cs = 0;
                        *s++ = stbsp__comma;
                     } else {
                        *s++ = '0';
                        --n;
                     }
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr) {
                  *s++ = stbsp__period;
                  tz = pr;
               }
            } else {
               // handle xxxxx.xxxx000*000
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= (stbsp__uint32)dp)
                        break;
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr)
                  *s++ = stbsp__period;
               if ((l - dp) > (stbsp__uint32)pr)
                  l = pr + dp;
               while (n < l) {
                  *s++ = sn[n];
                  ++n;
               }
               tz = pr - (l - dp);
            }
         }
         pr = 0;

         // handle k,m,g,t
         if (fl & STBSP__METRIC_SUFFIX) {
            char idx;
            idx = 1;
            if (fl & STBSP__METRIC_NOSPACE)
               idx = 0;
            tail[0] = idx;
            tail[1] = ' ';
            {
               if (fl >> 24) { // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                  if (fl & STBSP__METRIC_1024)
                     tail[idx + 1] = "_KMGT"[fl >> 24];
                  else
                     tail[idx + 1] = "_kMGT"[fl >> 24];
                  idx++;
                  // If printing kibits and not in jedec, add the 'i'.
                  if (fl & STBSP__METRIC_1024 && !(fl & STBSP__METRIC_JEDEC)) {
                     tail[idx + 1] = 'i';
                     idx++;
                  }
                  tail[0] = idx;
               }
            }
         };

      flt_lead:
         // get the length that we copied
         l = (stbsp__uint32)(s - (num + 64));
         s = num + 64;
         goto scopy;
#endif

      case 'B': // upper binary
      case 'b': // lower binary
         h = (f[0] == 'B') ? hexu : hex;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[0xb];
         }
         l = (8 << 4) | (1 << 8);
         goto radixnum;

      case 'o': // octal
         h = hexu;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 1;
            lead[1] = '0';
         }
         l = (3 << 4) | (3 << 8);
         goto radixnum;

      case 'p': // pointer
         fl |= (sizeof(void *) == 8) ? STBSP__INTMAX : 0;
         pr = sizeof(void *) * 2;
         fl &= ~STBSP__LEADINGZERO; // 'p' only prints the pointer with zeros
                                    // fall through - to X

      case 'X': // upper hex
      case 'x': // lower hex
         h = (f[0] == 'X') ? hexu : hex;
         l = (4 << 4) | (4 << 8);
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[16];
         }
      radixnum:
         // get the number
         if (fl & STBSP__INTMAX)
            n64 = va_arg(va, stbsp__uint64);
         else
            n64 = va_arg(va, stbsp__uint32);

         s = num + STBSP__NUMSZ;
         dp = 0;
         // clear tail, and clear leading if value is zero
         tail[0] = 0;
         if (n64 == 0) {
            lead[0] = 0;
            if (pr == 0) {
               l = 0;
               cs = (((l >> 4) & 15)) << 24;
               goto scopy;
            }
         }
         // convert to string
         for (;;) {
            *--s = h[n64 & ((1 << (l >> 8)) - 1)];
            n64 >>= (l >> 8);
            if (!((n64) || ((stbsp__int32)((num + STBSP__NUMSZ) - s) < pr)))
               break;
            if (fl & STBSP__TRIPLET_COMMA) {
               ++l;
               if ((l & 15) == ((l >> 4) & 15)) {
                  l &= ~15;
                  *--s = stbsp__comma;
               }
            }
         };
         // get the tens and the comma pos
         cs = (stbsp__uint32)((num + STBSP__NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         // copy it
         goto scopy;

      case 'u': // unsigned
      case 'i':
      case 'd': // integer
         // get the integer and abs it
         if (fl & STBSP__INTMAX) {
            stbsp__int64 i64 = va_arg(va, stbsp__int64);
            n64 = (stbsp__uint64)i64;
            if ((f[0] != 'u') && (i64 < 0)) {
               n64 = (stbsp__uint64)-i64;
               fl |= STBSP__NEGATIVE;
            }
         } else {
            stbsp__int32 i = va_arg(va, stbsp__int32);
            n64 = (stbsp__uint32)i;
            if ((f[0] != 'u') && (i < 0)) {
               n64 = (stbsp__uint32)-i;
               fl |= STBSP__NEGATIVE;
            }
         }

#ifndef STB_SPRINTF_NOFLOAT
         if (fl & STBSP__METRIC_SUFFIX) {
            if (n64 < 1024)
               pr = 0;
            else if (pr == -1)
               pr = 1;
            fv = (double)(stbsp__int64)n64;
            goto doafloat;
         }
#endif

         // convert to string
         s = num + STBSP__NUMSZ;
         l = 0;

         for (;;) {
            // do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
            char *o = s - 8;
            if (n64 >= 100000000) {
               n = (stbsp__uint32)(n64 % 100000000);
               n64 /= 100000000;
            } else {
               n = (stbsp__uint32)n64;
               n64 = 0;
            }
            if ((fl & STBSP__TRIPLET_COMMA) == 0) {
               do {
                  s -= 2;
                  *(stbsp__uint16 *)s = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
                  n /= 100;
               } while (n);
            }
            while (n) {
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = (char)(n % 10) + '0';
                  n /= 10;
               }
            }
            if (n64 == 0) {
               if ((s[0] == '0') && (s != (num + STBSP__NUMSZ)))
                  ++s;
               break;
            }
            while (s != o)
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = '0';
               }
         }

         tail[0] = 0;
         stbsp__lead_sign(fl, lead);

         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         if (l == 0) {
            *--s = '0';
            l = 1;
         }
         cs = l + (3 << 24);
         if (pr < 0)
            pr = 0;

      scopy:
         // get fw=leading/trailing space, pr=leading zeros
         if (pr < (stbsp__int32)l)
            pr = l;
         n = pr + lead[0] + tail[0] + tz;
         if (fw < (stbsp__int32)n)
            fw = n;
         fw -= n;
         pr -= l;

         // handle right justify and leading zeros
         if ((fl & STBSP__LEFTJUST) == 0) {
            if (fl & STBSP__LEADINGZERO) // if leading zeros, everything is in pr
            {
               pr = (fw > pr) ? fw : pr;
               fw = 0;
            } else {
               fl &= ~STBSP__TRIPLET_COMMA; // if no leading zeros, then no commas
            }
         }

         // copy the spaces and/or zeros
         if (fw + pr) {
            stbsp__int32 i;
            stbsp__uint32 c;

            // copy leading spaces (or when doing %8.4d stuff)
            if ((fl & STBSP__LEFTJUST) == 0)
               while (fw > 0) {
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i) {
                     *bf++ = ' ';
                     --i;
                  }
                  stbsp__chk_cb_buf(1);
               }

            // copy leader
            sn = lead + 1;
            while (lead[0]) {
               stbsp__cb_buf_clamp(i, lead[0]);
               lead[0] -= (char)i;
               while (i) {
                  *bf++ = *sn++;
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }

            // copy leading zeros
            c = cs >> 24;
            cs &= 0xffffff;
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((stbsp__uint32)(c - ((pr + cs) % (c + 1)))) : 0;
            while (pr > 0) {
               stbsp__cb_buf_clamp(i, pr);
               pr -= i;
               if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = '0';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x30303030;
                     bf += 4;
                     i -= 4;
                  }
               }
               while (i) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (cs++ == c)) {
                     cs = 0;
                     *bf++ = stbsp__comma;
                  } else
                     *bf++ = '0';
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }
         }

         // copy leader if there is still one
         sn = lead + 1;
         while (lead[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, lead[0]);
            lead[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy the string
         n = l;
         while (n) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, n);
            n -= i;
            STBSP__UNALIGNED(while (i >= 4) {
               *(stbsp__uint32 *)bf = *(stbsp__uint32 *)s;
               bf += 4;
               s += 4;
               i -= 4;
            })
            while (i) {
               *bf++ = *s++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy trailing zeros
         while (tz) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tz);
            tz -= i;
            while (i) {
               if ((((stbsp__uintptr)bf) & 3) == 0)
                  break;
               *bf++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)bf = 0x30303030;
               bf += 4;
               i -= 4;
            }
            while (i) {
               *bf++ = '0';
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy tail if there is one
         sn = tail + 1;
         while (tail[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tail[0]);
            tail[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // handle the left justify
         if (fl & STBSP__LEFTJUST)
            if (fw > 0) {
               while (fw) {
                  stbsp__int32 i;
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i--)
                     *bf++ = ' ';
                  stbsp__chk_cb_buf(1);
               }
            }
         break;

      default: // unknown, just copy code
         s = num + STBSP__NUMSZ - 1;
         *s = f[0];
         l = 1;
         fw = fl = 0;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
      }
      ++f;
   }
endfmt:

   if (!callback)
      *bf = 0;
   else
      stbsp__flush_cb();

done:
   return tlen + (int)(bf - buf);
}

// cleanup
#undef STBSP__LEFTJUST
#undef STBSP__LEADINGPLUS
#undef STBSP__LEADINGSPACE
#undef STBSP__LEADING_0X
#undef STBSP__LEADINGZERO
#undef STBSP__INTMAX
#undef STBSP__TRIPLET_COMMA
#undef STBSP__NEGATIVE
#undef STBSP__METRIC_SUFFIX
#undef STBSP__NUMSZ
#undef stbsp__chk_cb_bufL
#undef stbsp__chk_cb_buf
#undef stbsp__flush_cb
#undef stbsp__cb_buf_clamp

// ============================================================================
//   wrapper functions

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);
   result = STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
   va_end(va);
   return result;
}

typedef struct stbsp__context {
   char *buf;
   int count;
   char tmp[STB_SPRINTF_MIN];
} stbsp__context;

static char *stbsp__clamp_callback(char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         char *s, *d, *se;
         d = c->buf;
         s = buf;
         se = buf + len;
         do {
            *d++ = *s++;
         } while (s < se);
      }
      c->buf += len;
      c->count -= len;
   }

   if (c->count <= 0)
      return 0;
   return (c->count >= STB_SPRINTF_MIN) ? c->buf : c->tmp; // go direct into buffer if you can
}

static char * stbsp__count_clamp_callback( char * buf, void * user, int len )
{
   stbsp__context * c = (stbsp__context*)user;

   c->count += len;
   return c->tmp; // go direct into buffer if you can
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;
   int l;

   if ( (count == 0) && !buf )
   {
      c.count = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
      l = c.count;
   }
   else
   {
      if ( count == 0 )
         return 0;

      c.buf = buf;
      c.count = count;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );

      // zero-terminate
      l = (int)( c.buf - buf );
      if ( l >= count ) // should never be greater, only equal (or less) than count
         l = count - 1;
      buf[l] = 0;
   }

   return l;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);

   result = STB_SPRINTF_DECORATE(vsnprintf)(buf, count, fmt, va);
   va_end(va);

   return result;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va)
{
   return STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
}

// =======================================================================
//   low level float utility functions

#ifndef STB_SPRINTF_NOFLOAT

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP__COPYFP(dest, src)                   \
   {                                               \
      int cn;                                      \
      for (cn = 0; cn < 8; cn++)                   \
         ((char *)&dest)[cn] = ((char *)&src)[cn]; \
   }

// get float info
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value)
{
   double d;
   stbsp__int64 b = 0;

   // load value and round at the frac_digits
   d = value;

   STBSP__COPYFP(b, d);

   *bits = b & ((((stbsp__uint64)1) << 52) - 1);
   *expo = (stbsp__int32)(((b >> 52) & 2047) - 1023);

   return (stbsp__int32)((stbsp__uint64) b >> 63);
}

static double const stbsp__bot[23] = {
   1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
   1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
};
static double const stbsp__negbot[22] = {
   1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
   1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
};
static double const stbsp__negboterr[22] = {
   -5.551115123125783e-018,  -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
   4.5251888174113739e-024,  -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028,  2.0113352370744385e-029,
   -3.0373745563400371e-030, 1.1806906454401013e-032,  -7.7705399876661076e-032, 2.0902213275965398e-033,  -7.1542424054621921e-034, -7.1542424054621926e-035,
   2.4754073164739869e-036,  5.4846728545790429e-037,  9.2462547772103625e-038,  -4.8596774326570872e-039
};
static double const stbsp__top[13] = {
   1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
};
static double const stbsp__negtop[13] = {
   1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
};
static double const stbsp__toperr[13] = {
   8388608,
   6.8601809640529717e+028,
   -7.253143638152921e+052,
   -4.3377296974619174e+075,
   -1.5559416129466825e+098,
   -3.2841562489204913e+121,
   -3.7745893248228135e+144,
   -1.7356668416969134e+167,
   -3.8893577551088374e+190,
   -9.9566444326005119e+213,
   6.3641293062232429e+236,
   -5.2069140800249813e+259,
   -5.2504760255204387e+282
};
static double const stbsp__negtoperr[13] = {
   3.9565301985100693e-040,  -2.299904345391321e-063,  3.6506201437945798e-086,  1.1875228833981544e-109,
   -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178,  -5.7778912386589953e-201,
   7.4997100559334532e-224,  -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
   8.0970921678014997e-317
};

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000,
   100000000000,
   1000000000000,
   10000000000000,
   100000000000000,
   1000000000000000,
   10000000000000000,
   100000000000000000,
   1000000000000000000,
   10000000000000000000U
};
#define stbsp__tento19th ((stbsp__uint64)1000000000000000000)
#else
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000ULL,
   100000000000ULL,
   1000000000000ULL,
   10000000000000ULL,
   100000000000000ULL,
   1000000000000000ULL,
   10000000000000000ULL,
   100000000000000000ULL,
   1000000000000000000ULL,
   10000000000000000000ULL
};
#define stbsp__tento19th (1000000000000000000ULL)
#endif

#define stbsp__ddmulthi(oh, ol, xh, yh)                            \
   {                                                               \
      double ahi = 0, alo, bhi = 0, blo;                           \
      stbsp__int64 bt;                                             \
      oh = xh * yh;                                                \
      STBSP__COPYFP(bt, xh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(ahi, bt);                                      \
      alo = xh - ahi;                                              \
      STBSP__COPYFP(bt, yh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(bhi, bt);                                      \
      blo = yh - bhi;                                              \
      ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
   }

#define stbsp__ddtoS64(ob, xh, xl)          \
   {                                        \
      double ahi = 0, alo, vh, t;           \
      ob = (stbsp__int64)ph;                \
      vh = (double)ob;                      \
      ahi = (xh - vh);                      \
      t = (ahi - xh);                       \
      alo = (xh - (ahi - t)) - (vh + t);    \
      ob += (stbsp__int64)(ahi + alo + xl); \
   }

#define stbsp__ddrenorm(oh, ol) \
   {                            \
      double s;                 \
      s = oh + ol;              \
      ol = ol - (s - oh);       \
      oh = s;                   \
   }

#define stbsp__ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);

#define stbsp__ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);

static void stbsp__raise_to_power10(double *ohi, double *olo, double d, stbsp__int32 power) // power can be -323 to +350
{
   double ph, pl;
   if ((power >= 0) && (power <= 22)) {
      stbsp__ddmulthi(ph, pl, d, stbsp__bot[power]);
   } else {
      stbsp__int32 e, et, eb;
      double p2h, p2l;

      e = power;
      if (power < 0)
         e = -e;
      et = (e * 0x2c9) >> 14; /* %23 */
      if (et > 13)
         et = 13;
      eb = e - (et * 23);

      ph = d;
      pl = 0.0;
      if (power < 0) {
         if (eb) {
            --eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__negbot[eb]);
            stbsp__ddmultlos(ph, pl, d, stbsp__negboterr[eb]);
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__negtop[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__negtop[et], stbsp__negtoperr[et]);
            ph = p2h;
            pl = p2l;
         }
      } else {
         if (eb) {
            e = eb;
            if (eb > 22)
               eb = 22;
            e -= eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__bot[eb]);
            if (e) {
               stbsp__ddrenorm(ph, pl);
               stbsp__ddmulthi(p2h, p2l, ph, stbsp__bot[e]);
               stbsp__ddmultlos(p2h, p2l, stbsp__bot[e], pl);
               ph = p2h;
               pl = p2l;
            }
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__top[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__top[et], stbsp__toperr[et]);
            ph = p2h;
            pl = p2l;
         }
      }
   }
   stbsp__ddrenorm(ph, pl);
   *ohi = ph;
   *olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits)
{
   double d;
   stbsp__int64 bits = 0;
   stbsp__int32 expo, e, ng, tens;

   d = value;
   STBSP__COPYFP(bits, d);
   expo = (stbsp__int32)((bits >> 52) & 2047);
   ng = (stbsp__int32)((stbsp__uint64) bits >> 63);
   if (ng)
      d = -d;

   if (expo == 2047) // is nan or inf?
   {
      *start = (bits & ((((stbsp__uint64)1) << 52) - 1)) ? "NaN" : "Inf";
      *decimal_pos = STBSP__SPECIAL;
      *len = 3;
      return ng;
   }

   if (expo == 0) // is zero or denormal
   {
      if ((bits << 1) == 0) // do zero
      {
         *decimal_pos = 1;
         *start = out;
         out[0] = '0';
         *len = 1;
         return ng;
      }
      // find the right expo for denormals
      {
         stbsp__int64 v = ((stbsp__uint64)1) << 51;
         while ((bits & v) == 0) {
            --expo;
            v >>= 1;
         }
      }
   }

   // find the decimal exponent as well as the decimal bits of the value
   {
      double ph, pl;

      // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
      tens = expo - 1023;
      tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

      // move the significant bits into position and stick them into an int
      stbsp__raise_to_power10(&ph, &pl, d, 18 - tens);

      // get full as much precision from double-double as possible
      stbsp__ddtoS64(bits, ph, pl);

      // check if we undershot
      if (((stbsp__uint64)bits) >= stbsp__tento19th)
         ++tens;
   }

   // now do the rounding in integer land
   frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
   if ((frac_digits < 24)) {
      stbsp__uint32 dg = 1;
      if ((stbsp__uint64)bits >= stbsp__powten[9])
         dg = 10;
      while ((stbsp__uint64)bits >= stbsp__powten[dg]) {
         ++dg;
         if (dg == 20)
            goto noround;
      }
      if (frac_digits < dg) {
         stbsp__uint64 r;
         // add 0.5 at the right position and round
         e = dg - frac_digits;
         if ((stbsp__uint32)e >= 24)
            goto noround;
         r = stbsp__powten[e];
         bits = bits + (r / 2);
         if ((stbsp__uint64)bits >= stbsp__powten[dg])
            ++tens;
         bits /= r;
      }
   noround:;
   }

   // kill long trailing runs of zeros
   if (bits) {
      stbsp__uint32 n;
      for (;;) {
         if (bits <= 0xffffffff)
            break;
         if (bits % 1000)
            goto donez;
         bits /= 1000;
      }
      n = (stbsp__uint32)bits;
      while ((n % 1000) == 0)
         n /= 1000;
      bits = n;
   donez:;
   }

   // convert to string
   out += 64;
   e = 0;
   for (;;) {
      stbsp__uint32 n;
      char *o = out - 8;
      // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
      if (bits >= 100000000) {
         n = (stbsp__uint32)(bits % 100000000);
         bits /= 100000000;
      } else {
         n = (stbsp__uint32)bits;
         bits = 0;
      }
      while (n) {
         out -= 2;
         *(stbsp__uint16 *)out = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
         n /= 100;
         e += 2;
      }
      if (bits == 0) {
         if ((e) && (out[0] == '0')) {
            ++out;
            --e;
         }
         break;
      }
      while (out != o) {
         *--out = '0';
         ++e;
      }
   }

   *decimal_pos = tens;
   *start = out;
   *len = e;
   return ng;
}

#undef stbsp__ddmulthi
#undef stbsp__ddrenorm
#undef stbsp__ddmultlo
#undef stbsp__ddmultlos
#undef STBSP__SPECIAL
#undef STBSP__COPYFP

#endif // STB_SPRINTF_NOFLOAT

// clean up
#undef stbsp__uint16
#undef stbsp__uint32
#undef stbsp__int32
#undef stbsp__uint64
#undef stbsp__int64
#undef STBSP__UNALIGNED

#endif // STB_SPRINTF_IMPLEMENTATION

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
````

## File: inc/ext/console.h
````c
/**
 *  \file   console.h
 *  \brief  TTY text console
 *  \author Andreas Dietrich
 *  \date   09/2022
 *
 *  This unit provides a simple TTY text console. Characters are written as a
 *  stream, where lines are automatically wrapped if the horizontal border of
 *  the screen is reached. When at the bottom of the screen window, the console
 *  content is moved up by one text line and a blank row is inserted.
 *
 *  Per default, the console occupies a standad screen of 40x28 tiles. All text
 *  attributes, such as font, palette, plane etc., are taken from SGDK text
 *  settings. Screen updates are done using DMA transfer mode (which can be
 *  changed with CON_setTransferMethod()).
 *
 *  One of the use cases are assert messages. To this end, the Genesis state can
 *  be automatically reset before text is displayed (see assert macro below).
 */

// *****************************************************************************
//
//  Includes
//
// *****************************************************************************

#include "config.h"

#if (MODULE_CONSOLE != 0)

#include "types.h"
#include "string.h"
#include "maths.h"
#include "dma.h"

#pragma once

// *****************************************************************************
//
//  Types
//
// *****************************************************************************

/// Callback prototype for a vsprintf() function using va_list
typedef int (*vsprintf_t)(char *buf, const char *fmt, va_list args);
/// Callback prototype for a vsnprintf() function using va_list
typedef int (*vsnprintf_t)(char *buf, int count, const char *fmt, va_list args);

// *****************************************************************************
//
//  Macros
//
// *****************************************************************************

/// Helper macro to stringify numbers
#define str(s) xstr(s)
/// Helper macro
#define xstr(s) #s

/**
 *  \brief
 *      Assert a condition.
 *
 *  \param condition
 *      Expression that must evaluate to TRUE, otherwise an error error message
 *      in printed to the console. This will also reset the system to the SGDK
 *      default state.
 *
 *  Can be disabled by defining NDEBUG or using SGDK release build (LIB_DEBUG=0).
 */

#if !defined(NDEBUG) && (LIB_DEBUG != 0)
#define con_assert(condition)                                                          \
    if ( !(condition) )                                                                \
    {                                                                                  \
        CON_reset();                                                                   \
        CON_systemResetOnNextWrite();                                                  \
        CON_write(__FILE__":"str(__LINE__)": Assertion \'"str(condition)"\' failed."); \
        while (TRUE);                                                                  \
    }
#else
#define con_assert(condition)
#endif

/**
 *  \brief
 *      Assert a condition.
 *
 * Uppercase version. Same as assert().
 */

#define CON_ASSERT(condition) con_assert(condition)

// *****************************************************************************
//
//  Function Declarations
//
// *****************************************************************************

// -----------------------------------------------------------------------------
// printf functions
// -----------------------------------------------------------------------------

/**
 *  \brief
 *      Standard C library sprintf function.
 *
 *  \param buf
 *      Pointer to the buffer where the resulting C-string is stored.
 *  \param fmt
 *      C-string that contains the text to be written. It can optionally
 *      contain embedded format specifiers that are replaced by the values
 *      specified in subsequent additional arguments and formatted as requested.
 *      (See https://en.wikipedia.org/wiki/Printf_format_string)
 *  \param ... (additional arguments)
 *      The function may expect a sequence of additional arguments, each
 *      containing a value to be used to replace a format specifier in the
 *      format string.
 *  \return
 *      The total number of characters written is returned. This count does not
 *      include the additional null-character automatically appended at the end
 *      of the string.
 *
 *  Composes a string that would result from using a standard C library printf
 *  function. In case fmt includes format specifiers (beginning with %), the
 *  additional arguments are formatted and inserted in the resulting string
 *  replacing their respective specifiers.
 *
 *  This function is a wrapper for a vsprintf type function. Per default SGDK's
 *  built-in vsprintf() is used. CON_setVsprintf() can be used to register
 *  another implementation, e.g., stbsp_vsprintf().
 */

int CON_sprintf(char* buf, const char *fmt, ...)  __attribute__ ((format (printf, 2, 3)));

/**
 *  \brief
 *      Standard C library snprintf function.
 *
 *  \param buf
 *      Pointer to the buffer where the resulting C-string is stored.
 *  \param count
 *      Maximum number of bytes to be used in the buffer. The generated string
 *      has a length of at most count-1, leaving space for the additional
 *      terminating null-character.
 *  \param fmt
 *      C-string that contains the text to be written. It can optionally
 *      contain embedded format specifiers that are replaced by the values
 *      specified in subsequent additional arguments and formatted as requested.
 *      (See https://en.wikipedia.org/wiki/Printf_format_string)
 *  \param ... (additional arguments)
 *      The function may expect a sequence of additional arguments, each
 *      containing a value to be used to replace a format specifier in the
 *      format string.
 *  \return
 *      The total number of characters written is returned. This count does not
 *      include the additional null-character automatically appended at the end
 *      of the string.
 *
 *  Composes a string that would result from using a standard C library printf
 *  function. In case fmt includes format specifiers (beginning with %), the
 *  additional arguments are formatted and inserted in the resulting string
 *  replacing their respective specifiers.
 *
 *  This function is a wrapper for a vsnprintf type function. Per default no
 *  function is registered. CON_setVsnprintf() can be used to register an
 *  implementation, e.g., stbsp_vsnprintf().
 */

int CON_snprintf(char* buf, int count, const char *fmt, ...)  __attribute__ ((format (printf, 3, 4)));

/**
 *  \brief
 *      Register a standard vsprintf function as callback for CON_sprintf()
 *
 *  \param vsprintf_func
 *      Function pointer to a standard C library vsprintf function
 *
 *  Registers a callback function that will be wrapped by CON_sprintf(). The
 *  funcion supplied needs to be a vsprintf function that takes va_list as
 *  arguments, defined as follows:
 *
 *  typedef int (*vsprintf_t)(char *buf, const char *fmt, va_list args);
 *
 *  Possible implementations are SGDK's vsprintf() or alternatively
 *  stbsp_vsprintf() (http://github.com/nothings/stb).
 */

void CON_setVsprintf(vsprintf_t vsprintf_func);

/**
 *  \brief
 *      Register a standard vsnprintf function as callback for CON_snprintf()
 *
 *  \param vsnprintf_func
 *      Function pointer to a standard C library vsnprintf function
 *
 *  Registers a callback function that will be wrapped by CON_snprintf(). The
 *  funcion supplied needs to be a vsnprintf function that takes va_list as
 *  arguments, defined as follows:
 *
 *  typedef int (*vsnprintf_t)(char *buf, int count, const char *fmt, va_list args);
 *
 *  A possible implementation is stbsp_vsnprintf()
 *  (http://github.com/nothings/stb).
 */

void CON_setVsnprintf(vsnprintf_t vsnprintf_func);

// -----------------------------------------------------------------------------
// Console setup
// -----------------------------------------------------------------------------

/**
 *  \brief
 *      Set the size of the console window.
 *
 *  \param left
 *      Position of the leftmost window column in tiles. Default is 0.
 *  \param top
 *      Position of the topmost window row in tiles. Default is 0.
 *  \param width
 *      Width of the console window in tiles. Default is 40.
 *  \param height
 *      Height of the console window in tiles. Default is 28.
 *
 *  If either width or height are 0 then VDP_getScreenWidth() and
 *  VDP_getScreenHeight() will be used to determine suitable default values.
 */

void CON_setConsoleSize(u16 left, u16 top, u16 width, u16 height);

/**
 *  \brief
 *      Set the size of the character line buffer.
 *
 *  \param size
 *      Buffer size in bytes. This is the number of characters the buffer can
 *      hold including a terminating null-character.
 *
 *  Sets the size of the character buffer that is used internally to compose
 *  the formatted output string which is printed on the console.
 *
 *  Upon calling CON_setLineBufferSize(), the previous line buffer memory is
 *  immedieately freed. The new buffer will be allocated once the next call to
 *  CON_write() occurs.
 *
 *  Per default, the buffer size is set to 160 characters. Note that if only a
 *  vsprintf function is registered (see CON_setVsprintf()), the user is
 *  responsible for making sure a call to CON_write() does not exceed the line
 *  buffer. In case a vsnprintf function is registered (see CON_setVsnprintf()),
 *  it will automtically check the buffer size and prevent overflows.
 */

void CON_setLineBufferSize(u16 size);

/**
 *  \brief
 *      Set the transfer method used to upload the console tile buffer to VDP
 *      RAM.
 *
 *  \param tm
 *      Transfer method. <br>
 *      Accepted values are: <br>
 *      - CPU <br>
 *      - DMA <br>
 *      - DMA_QUEUE <br>
 *      - DMA_QUEUE_COPY
 *
 *  This sets the transfer method used with VDP_setTileMapDataRect(). The
 *  default value is DMA, which will cause an immediate upload. Note that when
 *  DMA_QUEUE or DMA_QUEUE_COPY is used, the user is responsible for triggering
 *  DMA upload, e.g., by calling SYS_doVBlankProcess().
 */

void CON_setTransferMethod(TransferMethod tm);

/**
 *  \brief
 *      Reset the console.
 *
 *  This function is a shortcut for:
 *
 *  CON_setConsoleSize(0, 0, 40, 28); <br>
 *  CON_setLineBufferSize(160); <br>
 *  CON_setTransferMethod(DMA);
 */

void CON_reset();

/**
 *  \brief
 *      Reset the system to the SGDK default state when CON_write() is called
 *      next time.
 *
 *  To reset the system the following SGDK functions will be called:
 *
 *  - SYS_disableInts() <br>
 *  - YM2612_reset() <br>
 *  - PSG_init() <br>
 *  - Z80_init() <br>
 *  - VDP_init()
 */

void CON_systemResetOnNextWrite();

// -----------------------------------------------------------------------------
// Console write functions
// -----------------------------------------------------------------------------

/**
 *  \brief
 *      Clear the console window.
 *
 *  This clears the console window by filling it with space characters. It is
 *  assumed that the first tile of the font is a space character. DMA settings
 *  apply here as well.
 */

void CON_clear();

/**
 *  \brief
 *      Set a new cursor position.
 *
 *  \param x
 *      New cursor x position.
 *  \param y
 *      New cursor y position.
 *
 *  This function specifies a new column and row for the cursor. This is the
 *  position where the next character will appear when CON_write() is processed.
 *  A position of (0,0) relates to the tile in the top/left corner.
 */

void CON_setCursorPosition(u16 x, u16 y);

/**
 *  \brief
 *      Return the current cursor position.
 *
 *  \return
 *      A 2D vector containing the current (x,y) position of the cursor, where
 *      (0,0) means the top/left corner of the console window.
 */

V2u16 CON_getCursorPosition();

/**
 *  \brief
 *      Write a C-string to the console window.
 *
 *  \param fmt
 *      C-string that contains the text to be written. It can optionally
 *      contain embedded format specifiers that are replaced by the values
 *      specified in subsequent additional arguments and formatted as requested.
 *      (See https://en.wikipedia.org/wiki/Printf_format_string)
 *  \param ... (additional arguments)
 *      The function may expect a sequence of additional arguments, each
 *      containing a value to be used to replace a format specifier in the
 *      format string.
 *  \return
 *      The total number of characters written is returned. This count does not
 *      include the additional null-character automatically appended at the end
 *      of the string.
 *
 *  This function writes a C-string into the console window where lines are
 *  automatically wrapped if the horizontal border of the screen is reached.
 *  When at the bottom of the screen window, the console content is moved up by
 *  one text line and a blank row is inserted.
 *
 *  Internally it uses either vsprintf or vsnprintf functions that have been
 *  registered with CON_setVsprintf() or CON_setVsnprintf(), respectively.
 *
 *  While processing the string, this function evaluates and executes escape
 *  control sequences. The following control characters are supported:
 *
 *  - \\b : backspace <br>
 *  - \\n : new line (line feed) <br>
 *  - \\r : carriage return <br>
 *  - \\t : horizontal tab <br>
 *  - \\v : vertical tab
 */

int CON_write(const char *fmt, ...)  __attribute__ ((format (printf, 1, 2)));

#endif // MODULE_CONSOLE
````

## File: inc/ext/everdrive.h
````c
/**
 *  \file everdrive.h
 *  \brief Everdrive support
 *  \author Krikzz
 *  \date XX/20XX
 *
 * This unit provides support for everdrive flash cart
 */

#ifndef _EVERDRIVE
#define _EVERDRIVE

#if (MODULE_EVERDRIVE != 0)

//config register bits
#define _SS 0
#define _FULL_SPEED 1
#define _SPI16 2
#define _GAME_MODE 3
#define _SMS_MODE 4
#define _HARD_RESET 5
#define _RAM_MODE_1 6
#define _RAM_ON 7
#define _VBL_CATCH 8
#define _MEGAKEY_ON 9
#define _MEGAKEY_REGION_1 10
#define _SSF_MODE_ON 11
#define _RAM_FS 12
#define _CART 13

//state register bits
#define _SPI_READY 0
#define _RY 1
#define _SMS_KEY 2
#define _SD_CART 3

//everdrive hardware registers
#define SPI_PORT *((volatile u16*) (0xA13000))
#define CFG_PORT *((volatile u16*) (0xA13002))
#define VBL_PORT *((volatile u16*) (0xA13004))
#define SRAM_BANK_PORT *((volatile u16*) (0xA13006))
#define VER_PORT *((volatile u16*) (0xA13008))
#define ROM_MAP_PORT *((volatile u16*) (0xA1300a))


#define CFGC(bit)(cfg &= ~(1 << bit), CFG_PORT = cfg)
#define CFGS(bit)(cfg |= (1 << bit), CFG_PORT = cfg)

#define IS_RY (CFG_PORT & (1 << _RY))
#define IS_SPI_READY (CFG_PORT & (1 << _SPI_READY))
#define IS_SMS_KEY_PRESSED (CFG_PORT & (1 << _SMS_KEY))
#define IS_SD_SLOT_EMPTY (CFG_PORT & (1 << _SD_CART))

#define SPI_HI_SPEED_ON CFGS(_FULL_SPEED)
#define SPI_HI_SPEED_OFF CFGC(_FULL_SPEED)

#define SPI16_ON CFGS(_SPI16);
#define SPI16_OFF CFGC(_SPI16);

#define SS_ON CFGC(_SS)
#define SS_OFF CFGS(_SS)

#define CART_ON CFGC(_CART)
#define CART_OFF CFGS(_CART)

#define RAM_ON CFGS(_RAM_ON);
#define RAM_OFF CFGC(_RAM_ON);

#define VBL_CATCH_ON CFGS(_VBL_CATCH);
#define VBL_CATCH_OFF CFGC(_VBL_CATCH);

#define SPI_BUSY while(!IS_SPI_READY)
#define EPR_BUSY while(!IS_RY)



extern u16 cfg;


//SD/MMC card initialization. should be run just one times, aer this cart will be ready for work
//will return 0 success
u8 evd_mmcInit();


//read block (512b) from SD/MMC card. mmc_addr should be multiple to 512
//will return 0 success
u8 evd_mmcRdBlock(u32 mmc_addr, u8 *stor);


//write block (512b) to SD/MMC card. mmc_addr should be multiple to 512
//will return 0 success
u8 evd_mmcWrBlock(u32 mmc_addr, u8 *data_ptr);


//erase flash memry sector(64kb). rom_addr should be multiple to 64k.
//code of this function should be placed in ram because rom memory inaccessible while erase process
//WARNING! this function may damage cart bios if sectors in range 0 - 0x40000 will be erased
void evd_eprEraseBlock(u32 rom_addr);


//write data to flash memory. len should be multiple to 4.
//each byte of flah memory should be erased before writeing by evd_eprEraseBlock
//code of this function should be placed in ram because rom memory inaccessible while writeing process
//WARNING! this function may damage cart bios if memory will be writen in area 0 - 0x40000
void evd_eprProgBlock(u16 *data, u32 rom_addr, u32 len);


//everdrive initialization.
//def_rom_bank = 0 if app placed in 0-0x400000 area, 1 if in 0x400000-0x800000 arae
//_is_ram_app = 0 if app assembled for work in rom, 1 if app assembleed for work in ram
void evd_init(u16 def_rom_bank, u8 _is_ram_app);


#endif // MODULE_EVERDRIVE

#endif
````

## File: inc/ext/fat16.h
````c
/**
 *  \file fat16.h
 *  \brief FAT16 file system support
 *  \author Krikzz
 *  \date XX/20XX
 *
 * This unit provides basic FAT16 file system support through everdrive flash cart hardware
 */

#ifndef _FAT16_H
#define _FAT16_H


#if (MODULE_FAT16 != 0)

#define FAT16_DIR_SIZE 32
#define FAT16_TYPE_FILE  0x20
#define FAT16_TYPE_DIR  0x10


typedef struct {
    u8 pointer[3]; //0
    u8 oem_name[8]; //3
    u16 byte_per_sector; //11
    u8 sector_per_cluster; //13
    u16 reserved_sectors; //14
    u8 fat_copys; //16
    u16 root_size; //17
    u16 small_partition_sectros; //19
    u8 drive_type; //21
    u16 sectors_per_fat; //22
    u16 sectors_per_track; //24
    u16 heads; //26
    u32 first_reserved_sectors; //28
    u32 total_partition_sectors; //32
    u16 drive_number; //36
    u8 extendet_boot_signature; //38
    u32 serial_number; //39
    u8 drive_name[11]; //43
    u8 fat_name[8]; //54
} Fat16PBR;


typedef struct {
    u8 name[14];
    u8 long_name[38];
    u8 flags;
    u16 entry;
    u32 size;
    u32 rec_addr;
} Fat16Record;

typedef struct {
    Fat16Record records[FAT16_DIR_SIZE];
    u16 entry;
    volatile u16 size;
} Fat16Dir;

typedef struct {
    Fat16Record *record;
    u8 sectror_buff[512];
    u16 cluster;
    u32 pos;
    u32 addr_buff;
    u8 sector;
} Fat16File;


extern Fat16PBR fat16_pbr;


u8 fat16Init();
u8 fat16OpenDir(u16 entry, Fat16Dir *dir);
u8 fat16OpenFile(Fat16Record *rec, Fat16File *file);
u8 fat16ReadNextSector(Fat16File *file);
u8 fat16WriteNextSector(Fat16File *file);
u8 fat16DeleteRecord(Fat16Record *rec);
u8 fat16CreateRecord(Fat16Record *rec, Fat16Dir *dir);
u8 fat16SkipSectors(Fat16File *file, u16 num);

#endif  /* MODULE_FAT16 */


#endif  /* _FAT16_H */
````

## File: inc/ext/link_cable.h
````c
/**
 * Link Cable Protocol (LCP) - Protocol for data exchange via SEGA Link Cable
 * between two Sega Mega Drive / Sega Genesis consoles connected through the second controller port
 * for SGDK
 *
 * BlodTor 2025
 *
 * *************************************************************************************************
 * *************************************************************************************************
 * Master console function call sequence:
 *
 * 1) Initialize Link Cable Protocol.
 * LCP_init();
 *
 * 2) Prepare data for transmission from master console via Link Cable Protocol.
 * LCP_objectToPacketForSend(transferObject, objectType, objectTypeSizes);
 * LCP_objectToPacketForSend(transferObject, objectType, objectTypeSizes);
 * ...
 * LCP_objectToPacketForSend(transferObject, objectType, objectTypeSizes);
 *
 * 3) Trigger external interrupt on slave console and send/receive data via Link Cable Protocol
 * LCP_masterCycle();
 *
 * 4) Extract received objects from packet transmitted by slave console
 * objectType = LCP_getNextObjectFromReceivePacket(transferObject, objectSizes);
 * objectType = LCP_getNextObjectFromReceivePacket(transferObject, objectSizes);
 * ...
 * objectType = LCP_getNextObjectFromReceivePacket(transferObject, objectSizes);
 * until objectType becomes 0
 *
 * 5) Close Link Cable Protocol if no further interaction with slave console is required.
 * LCP_close();
 *
 * *************************************************************************************************
 * *************************************************************************************************
 * Slave console function call sequence:
 *
 * 1) Initialize Link Cable Protocol
 * LCP_init();
 *
 * 2) Open port to enable slave console to handle external interrupts triggered by
 *    master console. Master console can only call LCP_slaveCycle() - the external
 *    interrupt handler on slave console - after this method is called.
 * LCP_open();
 *
 * 3) Prepare data for transmission from slave console via Link Cable Protocol.
 *    Transmission occurs in LCP_slaveCycle() after receiving data from master console
 *    in this method. LCP_slaveCycle() is called asynchronously by suspending game code
 *    execution on slave console when external interrupt from LCP_masterCycle() method
 *    of master console occurs.
 * LCP_objectToPacketForSend(transferObject, objectType, objectTypeSizes);
 * LCP_objectToPacketForSend(transferObject, objectType, objectTypeSizes);
 * ...
 * LCP_objectToPacketForSend(transferObject, objectType, objectTypeSizes);
 *
 * 4) Extract received objects from packet transmitted by master console
 * objectType = LCP_getNextObjectFromReceivePacket(transferObject, objectSizes);
 * objectType = LCP_getNextObjectFromReceivePacket(transferObject, objectSizes);
 * ...
 * objectType = LCP_getNextObjectFromReceivePacket(transferObject, objectSizes);
 * until objectType becomes 0
 *
 * 5) Close Link Cable Protocol if no further interaction with master console is required.
 *    Slave console will stop responding to external interrupts from master console!
 * LCP_close();
 *
 * *************************************************************************************************
 * *************************************************************************************************
 * IMPORTANT USAGE NOTES:
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * Object types (objectType) must be defined in YOUR game's main code!
 * See constant declaration examples in 'Super Turbo MEGA Pac-Man 2.1' game code
 * starting with 'OBJECT_TYPE_' prefix
 *
 * Object sizes (objectSizes) in bytes must be defined in YOUR game's main code!
 * The protocol transfers 2 bytes of information per cycle, so there's no point in setting
 * object lengths not multiples of 2 bytes! See constant declaration examples in
 * 'Super Turbo MEGA Pac-Man 2.1' game code ending with '_OBJECT_LENGTH' postfix,
 * as well as the LINK_TYPES_LENGHT array definition example which is passed as objectSizes
 * parameter to LCP_objectToPacketForSend() and LCP_getNextObjectFromReceivePacket() functions
 *
 * Master/Slave console determination implementation must be in YOUR game's code!
 * See initControllerPort2() function example in 'Super Turbo MEGA Pac-Man 2.1' game code
 * which determines which of the two consoles becomes master and which becomes slave
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * For debugging, use the function that returns transmission errors via SEGA Link Cable:
 * LCP_getError()
 *
 */
#ifndef _LINK_CABLE_H_
#define _LINK_CABLE_H_

#include "config.h"

#if (MODULE_LINK_CABLE != 0)

#include "types.h"

/**
 * Constants
 */

// Input/output direction for SEGA MD console port 2 (set individually for each bit)
// Determines the data line operation mode (input or output) and controls the interrupt request mask from external devices.
// Essentially, this is memory address 0xA1000B for data transfer control
// via SEGA Link Cable (CTRL port register CTRL 2 in SEGA documentation)
// Bit 7 (INT) - Interrupt on SYN line signal. 0 = disabled; 1 = enabled.
// Bit 6 (PC6) - Data line "SYN" mode. 0 = input; 1 = output.
// Bits 0-5 (PC0-PC5) - Data line mode. 0 = input; 1 = output.
#define LCP_Ctrl   				(*(vu8*) 0xa1000b)

// State of port 2 on SEGA MD console
// Essentially, this is memory address 0xA10005 for direct data transfer
// via SEGA Link Cable (DATA register of CTRL port 2 in SEGA documentation)
//
// Lower 4 bits (0-3) are used for data transfer.
//
// Bit addressing features between consoles:
// Bit 5 in the current console corresponds to bit 6 in the other console
// for the same memory address 0xA10005. If we change bit 5 on our console,
// bit 6 will change in the other console's LCP_Data variable, and vice versa -
// if bit 5 is changed on the other console, bit 6 will change on our console.
//
// Bits 0-3 at address 0xA10005 will have the same value in both consoles,
// i.e., if we change these bits on one console, they will also change on the other.
// Which console is currently responsible for this is determined by the LCP_Ctrl state -
// i.e., which data line is in output mode (sending data),
// and which is in input mode (receiving data).
#define LCP_Data   				(*(vu8*) 0xa10005)

// Bit 5 is set to 1, others to 0. Primarily used to set bit 5 to 1
// in LCP_Data and LCP_Ctrl to signal the other console that it can
// read or send data via Link Cable.
#define LCP_BIT5				0b00100000

// Bit 6 is set to 1, others to 0. Mainly used to determine
// whether bit 6 is set in LCP_Data, signaling that
// data can be read from the other console or sent to the other console via Link Cable.
#define LCP_BIT6				0b01000000

// Bits 7 and 5 are set to 1, other bits are 0. Used to set these bits in LCP_Ctrl, which indicates
// readiness for data exchange through console port 2 upon external interrupt EX-INT (External Interrupt
// in SEGA documentation), i.e., reading data from the port (data reception) in the external interrupt handler LCP_slaveCycle().
//
// LCP_Ctrl bit values:
// - Interrupt on SYN line signal enabled, bit 7 = 1
// - Data line PC5 in output mode (transmission), bit 5 = 1
// - Data lines PC6, PC4, PC3, PC2, PC1, PC0 in input mode (reception). Corresponding bits = 0
//
// At the "hardware" level for my code this means:
// - Enable pins 0-3 of controller port 2 for data reception
// - Pin 5 signals that data has been received by our console (on the other console this will be bit 6 in LCP_Data)
// - Pin 6 signals that data has been sent by the other console (the other console should change bit 5 in its LCP_Data)
#define	LCP_BIT7_AND_BIT5 		0b10100000;

// Bits 5, 3, 2, 1, 0 are set to 1, others are 0. Used to set these bits in LCP_Ctrl, which means
// starting the data write cycle to port 2 (data transmission) by triggering an external interrupt on the slave console.
//
// I.e., on the master console, the LCP_masterCycle() method should be called from the main game code, which will set
// the bits in LCP_Ctrl to the state specified above, and on the slave console we will previously trigger an external
// interrupt that will halt the main game code execution and call the LCP_slave() method. This will occur when on the
// master console the 5th bit in LCP_Data is reset to 0 in the LCP_startSendCycle() method.
//
// LCP_Ctrl bit values:
// - Interrupt on SYN line signal disabled, bit 7 = 0
// - Data lines PC5, PC3, PC2, PC1, PC0 in output mode (transmission), bits 5, 3, 2, 1, 0 = 1
// - Data lines PC6, PC4 in input mode (reception). Corresponding bits = 0
//
// At the "hardware" level for my code this means:
// - Enable pins 0-3 of controller port 2 for data transmission
// - Pin 5 signals that data can be read by the other console (on the other console this will be bit 6 in LCP_Data)
// - Pin 6 signals that data has been read by the other console (the other console should change bit 5 in its LCP_Data)
#define LCP_BIT_5_3_2_1_0 		0b00101111;

// Mask 00001111. Mainly used to reset the high 4 bits in a byte to 0
#define LCP_LO_BITS 			0b00001111

// Mask 11110000. Mainly used to reset the low 4 bits in a byte to 0
#define LCP_HI_BITS  			0b11110000

// Timeout - used for waiting for data from the other console (waiting for reset or set of bit 6 in LCP_Data)
#define LCP_TIME_OUT 			0x40

// Maximum packet size for data transmission via Link Cable
#define LCP_PACKET_SIZE			2048


/**
 * Library function declarations
 */


/**
 * Opening port 2 of the console for data exchange via Link Cable
 * upon external interrupt - External Interrupt (EX-INT)
 * interrupt mask level 2 for Motorola 68000 processor
 * setting up a callback function called when EX-INT interrupt occurs
 */
void LCP_open();


/**
 * Initialization of console port 2 for data exchange via Link Cable
 * for direct data writing to the port and subsequent data reading from the port.
 * EX-INT interrupts are disabled for this console.
 * Reset of global variables.
 */
void LCP_init();


/**
 * Closing the console port for data exchange via Link Cable
 * disabling the external interrupt handler - External Interrupt (EX-INT)
 * interrupt mask level 2 for Motorola 68000 processor
 */
void LCP_close();


/**
 * Function for copying the transferObject as a byte array
 * into the LCP_sendPacket for transmission via Link Cable.
 *
 * Packet structure:
 * - First 2 bytes contain the type of transmitted object (cannot be 0)
 * - Followed by object data (2 bytes each) according to objectTypeSizes table
 *
 * Multiple objects can be transmitted at once, as long as they fit in the buffer.
 * It is assumed that the function can be called multiple times in sequence until
 * the errors described below occur.
 *
 * Error codes (error code = LCP_error & 0x000F):
 * 0xC - transmitted object size is negative or zero
 * 0xD - transmitted object size exceeds remaining packet space
 * 0xF - packet data start index is greater than data end index
 *
 * Parameters:
 * transferObject - object to send as byte array via Link Cable
 * objectType - type of object to send (number corresponding to index in object sizes array)
 * objectTypeSizes - array of object sizes (defined in YOUR main game code)
 */
void LCP_objectToPacketForSend(u8 *transferObject, u16 objectType , u16 *objectTypeSizes);


/**
 * Function extracts the next object as a byte array transferObject from the received LCP_ReceivePacket
 * via Link Cable and returns the type of this object or 0 if the object could not be extracted from LCP_ReceivePacket.
 *
 * Since multiple objects can be transmitted at once, it is assumed that this function can be called
 * multiple times in sequence until 0 is returned, meaning - all objects have been extracted from the transmitted packet.
 *
 * Packet structure:
 * - First 2 bytes of LCP_ReceivePacket contain the type of transmitted object (cannot be 0)
 * - Followed by object data (2 bytes each) according to objectSizes table
 * - Multiple objects can be transmitted at once, as long as they fit in the LCP_ReceivePacket array
 *
 * Error codes (error code = LCP_error & 0xF000):
 * 0x1000 - no object in received packet, all objects already extracted or nothing was transmitted
 * 0x2000 - size of object to extract from read packet buffer is negative or zero
 * 0x3000 - size of object to extract exceeds remaining data in received packet
 * 0x4000 - size of object to extract exceeds remaining size of LCP_ReceivePacket array
 *
 * Parameters:
 * transferObject - next received object as byte array via Link Cable
 * objectSizes - array of object sizes (defined in YOUR main game code)
 *
 * Returns:
 * type of received object or 0 if object could not be extracted from received packet
 */
u16 LCP_getNextObjectFromReceivePacket(u8 *transferObject, u16 *objectSizes);


/**
 * Data transmission and reception cycle via Link Cable for SEGA master console
 * At the very beginning, disable all interrupt processing and reset LCP_timeOut and LCP_error to 0.
 * Both in case of error and successful function completion (i.e. always), call LCP_masterCycleEnd() at the end
 * to set console port 2 to correct state and enable all interrupt processing.
 *
 * Error codes (error code = LCP_error & 0x00F0):
 * 0x10 - error calling LCP_startSendCycle() - start of data transmission from master console
 * 0x20 - error calling LCP_send(size) - transmission of 2 bytes (packet size) from master console
 * 0x30 - error calling LCP_send(data) - transmission of 2 bytes (packet data) from master console
 * 0x40 - error calling LCP_send(checksum) - transmission of 2 bytes (checksum) from master console
 * 0x50 - error calling LCP_endSendCycle() - end of main packet transmission cycle from master to slave console
 * 0x60 - error calling LCP_startReceiveCycle() - start of data reception cycle from slave to master console
 * 0x70 - error calling checksum = LCP_Receive() - reception of 2 bytes (packet checksum) from slave console
 * 0x80 - checksum calculated by master console != checksum received from slave console
 * 0x90 - error calling size = LCP_Receive() - reception of 2 bytes (packet size) from slave console
 * 0xA0 - error calling data = LCP_Receive() - reception of 2 bytes (packet data) from slave console
 * 0xB0 - error calling checksum = LCP_Receive() - main reception cycle of 2 bytes (checksum) from slave console
 * 0xC0 - checksum calculated by master console != checksum received from slave console
 * 0xD0 - error calling LCP_endReceiveCycle() - end of data reception cycle from slave console
 * 0xE0 - error calling LCP_startSendCycle() - start of checksum transmission from master to slave console
 * 0xF0 - error calling LCP_send(checksum) - transmission of checksum from master to slave console
 */
void LCP_masterCycle();


/**
 * Error in data transmission via SEGA Link Cable
 *
 * If 0x0000, no errors occurred!                                        Value ranges     in binary format
 * Lower 4 bits of low byte  - data reception errors via Link Cable    (0x0001 - 0x000F)  000000000000xxxx
 * Upper 4 bits of low byte  - data transmission errors via Link Cable (0x0010 - 0x00F0)  00000000xxxx0000
 * Lower 4 bits of high byte - packet reception or transmission errors (0x0100 - 0x0F00)  0000xxxx00000000
 * Upper 4 bits of high byte - object extraction errors from packet    (0x1000 - 0xF000)  xxxx000000000000
 *
 * return LCP_error
 */
u16 LCP_getError();

#endif // MODULE_LINK_CABLE

#endif // _LINK_CABLE_H
````

## File: inc/snd/pcm/snd_dpcm2.h
````c
/**
 *  \file SND_DPCM2.h
 *  \brief Z80_DRIVER_DPCM2 sound driver API
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides playback method for the <b>Z80_DRIVER_DPCM2</b> sound driver.<br>
 * Sound driver description:<br>
 * 2 channels 4 bits DPCM sample driver, it can mix up to 2 DCPM samples at a fixed 22050 Hz Khz rate.<br>
 * Address and size of samples have to be 256 bytes boundary.<br>
 *<br>
 * Note that SGDK sound drivers may not fit your needs so it's important to know<br>
 * that some alternatives sound drivers exist (see SGDK readme.md for more info)
 */

#ifndef _SND_DPCM2_H_
#define _SND_DPCM2_H_

#define SND_isPlaying_2ADPCM    _Pragma("GCC error \"This method is deprecated, use SND_DPCM2_isPlaying instead.\"")
#define SND_startPlay_2ADPCM    _Pragma("GCC error \"This method is deprecated, use SND_DPCM2_startPlay instead.\"")
#define SND_stopPlay_2ADPCM     _Pragma("GCC error \"This method is deprecated, use SND_DPCM2_stopPlay instead.\"")

/**
 *  \brief
 *      Load the Z80_DRIVER_DPCM2 sound driver.
 */
void SND_DPCM2_loadDriver(const bool waitReady);

/**
 *  \brief
 *      Return play status of specified channel (2 channels ADPCM player driver).
 *
 *  \param channel_mask
 *      Channel(s) we want to retrieve play state.<br>
 *      SOUND_PCM_CH1_MSK    = channel 1<br>
 *      SOUND_PCM_CH2_MSK    = channel 2<br>
 *      <br>
 *      You can combine mask to retrieve state of severals channels at once:<br>
 *      <code>isPlaying_2ADPCM(SOUND_PCM_CH1_MSK | SOUND_PCM_CH2_MSK)</code><br>
 *      will actually return play state for channel 1 and channel 2.
 *
 *  \return
 *      Return TRUE if specified channel(s) is(are) playing.
 */
bool SND_DPCM2_isPlaying(const u16 channel_mask);
/**
 *  \brief
 *      Start playing a sample on specified channel (2 channels ADPCM player driver).<br>
 *      If a sample was currently playing on this channel then it's stopped and the new sample is played instead.
 *
 *  \param sample
 *      Sample address, should be 128 bytes boundary aligned<br>
 *      SGDK automatically align resource as needed
 *  \param len
 *      Size of sample in bytes, should be a multiple of 128<br>
 *      SGDK automatically adjust resource size as needed
 *  \param channel
 *      Channel where we want to play sample, accepted values are:<br>
 *      SOUND_PCM_CH_AUTO  = auto selection from current channel usage<br>
 *      SOUND_PCM_CH1      = channel 1<br>
 *      SOUND_PCM_CH2      = channel 2<br>
 *  \param loop
 *      Loop flag.<br>
 *      If TRUE then the sample will be played in loop (else sample is played only once).
 */
void SND_DPCM2_startPlay(const u8 *sample, const u32 len, const SoundPCMChannel channel, const bool loop);
/**
 *  \brief
 *      Stop playing the specified channel (2 channels ADPCM player driver).<br>
 *      No effect if no sample was currently playing on this channel.
 *
 *  \param channel
 *      Channel we want to stop, accepted values are:<br>
 *      SOUND_PCM_CH1    = channel 1<br>
 *      SOUND_PCM_CH2    = channel 2<br>
 */
void SND_DPCM2_stopPlay(const SoundPCMChannel channel);

#endif // _SND_DPCM2_H_
````

## File: inc/snd/pcm/snd_pcm.h
````c
/**
 *  \file snd_pcm.h
 *  \brief Z80_DRIVER_PCM sound driver API
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides playback method for the <b>Z80_DRIVER_PCM</b> sound driver.<br>
 *<br>
 * Sound driver description:<br>
 * This sound driver allow Single PCM channel 8 bits signed sample driver.<br>
 * It can play a sample (8 bit signed) from 8 Khz up to 32 Khz rate.<br>
 *<br>
 * Note that SGDK sound drivers may not fit your needs so it's important to know<br>
 * that some alternatives sound drivers exist (see SGDK readme.md for more info)
 */

#ifndef _SND_PCM_H_
#define _SND_PCM_H_

/**
 *  \brief
 *      PCM sample rate values
 */
typedef enum
{
    SOUND_PCM_RATE_32000,   /** Best quality but take lot of rom space */
    SOUND_PCM_RATE_22050,
    SOUND_PCM_RATE_16000,
    SOUND_PCM_RATE_13400,
    SOUND_PCM_RATE_11025,
    SOUND_PCM_RATE_8000     /** Worst quality but take less rom place */
} SoundPcmSampleRate;


#define SOUND_RATE_32000    _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_RATE_32000 instead.\"")
#define SOUND_RATE_22050    _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_RATE_22050 instead.\"")
#define SOUND_RATE_16000    _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_RATE_16000 instead.\"")
#define SOUND_RATE_13400    _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_RATE_13400 instead.\"")
#define SOUND_RATE_11025    _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_RATE_11025 instead.\"")
#define SOUND_RATE_8000     _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_RATE_8000 instead.\"")


#define SND_isPlaying_PCM   _Pragma("GCC error \"This method is deprecated, use SND_PCM_isPlaying instead.\"")
#define SND_startPlay_PCM   _Pragma("GCC error \"This method is deprecated, use SND_PCM_startPlay instead.\"")
#define SND_stopPlay_PCM    _Pragma("GCC error \"This method is deprecated, use SND_PCM_stopPlay instead.\"")

/**
 *  \brief
 *      Load the Z80_DRIVER_PCM sound driver.
 */
void SND_PCM_loadDriver(const bool waitReady);

/**
 *  \brief
 *      Return play status (Single channel PCM player driver).
 *
 *  \return
 *      Return non zero if PCM player is currently playing a sample
 */
bool SND_PCM_isPlaying(void);
/**
 *  \brief
 *      Start playing a sample (Single channel PCM player driver).<br>
 *      If a sample was currently playing then it's stopped and the new sample is played instead.
 *
 *  \param sample
 *      Sample address, should be 256 bytes boundary aligned<br>
 *      SGDK automatically align resource as needed
 *  \param len
 *      Size of sample in bytes, should be a multiple of 256<br>
 *      SGDK automatically adjust resource size as needed
 *  \param rate
 *      Playback rate :<br>
 *      SOUND_PCM_RATE_32000 = 32 Khz (best quality but take lot of rom space)<br>
 *      SOUND_PCM_RATE_22050 = 22 Khz<br>
 *      SOUND_PCM_RATE_16000 = 16 Khz<br>
 *      SOUND_PCM_RATE_13400 = 13.4 Khz<br>
 *      SOUND_PCM_RATE_11025 = 11 Khz<br>
 *      SOUND_PCM_RATE_8000  = 8 Khz (worst quality but take less rom place)<br>
 *  \param pan
 *      Panning :<br>
 *      SOUND_PAN_NONE   = mute<br>
 *      SOUND_PAN_LEFT   = play on left speaker<br>
 *      SOUND_PAN_RIGHT  = play on right speaker<br>
 *      SOUND_PAN_CENTER = play on both speaker<br>
 *  \param loop
 *      Loop flag.<br>
 *      If TRUE then the sample will be played in loop (else sample is played only once).
 */
void SND_PCM_startPlay(const u8 *sample, const u32 len, const SoundPcmSampleRate rate, const SoundPanning pan, const bool loop);
/**
 *  \brief
 *      Stop playing (Single channel PCM player driver).<br>
 *      No effect if no sample was currently playing.
 */
void SND_PCM_stopPlay(void);

#endif // _SND_PCM_H_
````

## File: inc/snd/pcm/snd_pcm4.h
````c
/**
 *  \file snd_pcm4.h
 *  \brief Z80_DRIVER_PCM4 sound driver API
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides playback method for the <b>Z80_DRIVER_PCM4</b> sound driver.<br>
 * Sound driver description:<br>
 * 4 channels 8 bits signed sample driver with volume support.<br>
 * It can mix up to 4 samples (8 bit signed) at a fixed 16 Khz rate.<br>
 * with volume support (16 levels du to memory limitation).<br>
 * Address and size of samples have to be 256 bytes boundary.<br>
 * The driver does support "cutoff" when mixing so you can use true 8 bits samples :)<br>
 *<br>
 * Note that SGDK sound drivers may not fit your needs so it's important to know<br>
 * that some alternatives sound drivers exist (see SGDK readme.md for more info)
 */

#ifndef _SND_PCM4_H_
#define _SND_PCM4_H_

#define SND_isPlaying_4PCM_ENV      _Pragma("GCC error \"This method is deprecated, use SND_PCM4_isPlaying instead.\"")
#define SND_startPlay_4PCM_ENV      _Pragma("GCC error \"This method is deprecated, use SND_PCM4_startPlay instead.\"")
#define SND_stopPlay_4PCM_ENV       _Pragma("GCC error \"This method is deprecated, use SND_PCM4_stopPlay instead.\"")
#define SND_getVolume_4PCM_ENV      _Pragma("GCC error \"This method is deprecated, use SND_PCM4_getVolume instead.\"")
#define SND_setVolume_4PCM_ENV      _Pragma("GCC error \"This method is deprecated, use SND_PCM4_setVolume instead.\"")

#define SND_isPlaying_4PCM          _Pragma("GCC error \"This method is deprecated, use SND_PCM4_isPlaying instead.\"")
#define SND_startPlay_4PCM          _Pragma("GCC error \"This method is deprecated, use SND_PCM4_startPlay instead.\"")
#define SND_stopPlay_4PCM           _Pragma("GCC error \"This method is deprecated, use SND_PCM4_stopPlay instead.\"")
#define SND_getVolume_4PCM          _Pragma("GCC error \"This method is deprecated, use SND_PCM4_getVolume instead.\"")
#define SND_setVolume_4PCM          _Pragma("GCC error \"This method is deprecated, use SND_PCM4_setVolume instead.\"")

/**
 *  \brief
 *      Load the Z80_DRIVER_PCM4 sound driver.
 */
void SND_PCM4_loadDriver(const bool waitReady);

/**
 *  \brief
 *      Return play status of specified channel (4 channels PCM player driver).
 *
 *  \param channel_mask
 *      Channel(s) we want to retrieve play state.<br>
 *      SOUND_PCM_CH1_MSK    = channel 1<br>
 *      SOUND_PCM_CH2_MSK    = channel 2<br>
 *      SOUND_PCM_CH3_MSK    = channel 3<br>
 *      SOUND_PCM_CH4_MSK    = channel 4<br>
 *      <br>
 *      You can combine mask to retrieve state of severals channels at once:<br>
 *      <code>SND_PCM4_isPlaying(SOUND_PCM_CH1_MSK | SOUND_PCM_CH2_MSK)</code><br>
 *      will actually return play state for channel 1 and channel 2.
 *
 *  \return
 *      Return TRUE if specified channel(s) is(are) playing.
 */
bool SND_PCM4_isPlaying(const u16 channel_mask);
/**
 *  \brief
 *      Start playing a sample on specified channel (4 channels PCM player driver).<br>
 *      If a sample was currently playing on this channel then it's stopped and the new sample is played instead.
 *
 *  \param sample
 *      Sample address, should be 256 bytes boundary aligned<br>
 *      SGDK automatically align resource as needed
 *  \param len
 *      Size of sample in bytes, should be a multiple of 256<br>
 *      SGDK automatically adjust resource size as needed
 *  \param channel
 *      Channel where we want to play sample, accepted values are:<br>
 *      SOUND_PCM_CH_AUTO  = auto selection from current channel usage<br>
 *      SOUND_PCM_CH1      = channel 1<br>
 *      SOUND_PCM_CH2      = channel 2<br>
 *      SOUND_PCM_CH3      = channel 3<br>
 *      SOUND_PCM_CH4      = channel 4<br>
 *  \param loop
 *      Loop flag.<br>
 *      If TRUE then the sample will be played in loop (else sample is played only once).
 */
void SND_PCM4_startPlay(const u8 *sample, const u32 len, const SoundPCMChannel channel, const bool loop);
/**
 *  \brief
 *      Stop playing the specified channel (4 channels PCM player driver).<br>
 *      No effect if no sample was currently playing on this channel.
 *
 *  \param channel
 *      Channel we want to stop, accepted values are:<br>
 *      SOUND_PCM_CH1    = channel 1<br>
 *      SOUND_PCM_CH2    = channel 2<br>
 *      SOUND_PCM_CH3    = channel 3<br>
 *      SOUND_PCM_CH4    = channel 4<br>
 */
void SND_PCM4_stopPlay(const SoundPCMChannel channel);
/**
 *  \brief
 *      Change envelop / volume of specified channel (4 channels PCM player driver).
 *
 *  \param channel
 *      Channel we want to set envelop, accepted values are:<br>
 *      SOUND_PCM_CH1    = channel 1<br>
 *      SOUND_PCM_CH2    = channel 2<br>
 *      SOUND_PCM_CH3    = channel 3<br>
 *      SOUND_PCM_CH4    = channel 4<br>
 *  \param volume
 *      Volume to set: 16 possible level from 0 (minimum) to 15 (maximum).
 */
void SND_PCM4_setVolume(const SoundPCMChannel channel, const u8 volume);
/**
 *  \brief
 *      Return envelop / volume level of specified channel (4 channels PCM player driver).
 *
 *  \param channel
 *      Channel we want to retrieve envelop level, accepted values are:<br>
 *      SOUND_PCM_CH1    = channel 1<br>
 *      SOUND_PCM_CH2    = channel 2<br>
 *      SOUND_PCM_CH3    = channel 3<br>
 *      SOUND_PCM_CH4    = channel 4<br>
 *  \return
 *      Envelop of specified channel.<br>
 *      The returned value is comprised between 0 (quiet) to 15 (loud).
 */
u8   SND_PCM4_getVolume(const SoundPCMChannel channel);

#endif // _SND_PCM4_H_
````

## File: inc/snd/pcm/tab_vol.h
````c
#ifndef _TAB_VOL_H_
#define _TAB_VOL_H_

extern const u8 tab_vol[0x1000];

#endif // _TAB_VOL_H_
````

## File: inc/snd/smp_null_dpcm.h
````c
#ifndef _SMP_NULL_DPCM_H_
#define _SMP_NULL_DPCM_H_

extern const u8 smp_null_dpcm[0x80];

#endif // _SMP_NULL_DPCM_H_
````

## File: inc/snd/smp_null.h
````c
#ifndef _SMP_NULL_H_
#define _SMP_NULL_H_

extern const u8 smp_null[0x100];

#endif // _SMP_NULL_H_
````

## File: inc/snd/sound.h
````c
/**
 *  \file sound.h
 *  \brief General / shared sound driver definitions
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides basic definitions for sound drivers.<br>
 */

#ifndef _SOUND_H_
#define _SOUND_H_

/**
 *  \brief
 *      Sound PCM channel enum
 */
typedef enum
{
    SOUND_PCM_CH_AUTO = -1,       // auto-select
    SOUND_PCM_CH1 = 0,            // channel 1
    SOUND_PCM_CH2,                // channel 2
    SOUND_PCM_CH3,                // channel 3
    SOUND_PCM_CH4,                // channel 4
} SoundPCMChannel;

/**
 *  \brief
 *      Sound panning enum
 */
typedef enum
{
    SOUND_PAN_NONE = 0x00,        // mute
    SOUND_PAN_RIGHT = 0x40,       // right speaker only
    SOUND_PAN_LEFT = 0x80,        // left speaker only
    SOUND_PAN_CENTER = 0xC0       // center (both speakers)
} SoundPanning;

/**
 *  \brief
 *      PCM channel 1 (first channel) selection mask.
 */
#define SOUND_PCM_CH1_MSK   (1 << SOUND_PCM_CH1)
/**
 *  \brief
 *      PCM channel 2 selection mask.
 */
#define SOUND_PCM_CH2_MSK   (1 << SOUND_PCM_CH2)
/**
 *  \brief
 *      PCM channel 3 selection mask.
 */
#define SOUND_PCM_CH3_MSK   (1 << SOUND_PCM_CH3)
/**
 *  \brief
 *      PCM channel 4 selection mask.
 */
#define SOUND_PCM_CH4_MSK   (1 << SOUND_PCM_CH4)


#define SOUND_PCM_AUTO      _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH_AUTO instead.\"")


/**
 *  \brief
 *      Load the dummy Z80 driver.
 */
void SND_NULL_loadDriver(void);


#endif // _SOUND_H_
````

## File: inc/snd/xgm.h
````c
/**
 *  \file xgm.h
 *  \brief XGM sound driver
 *  \author Stephane Dallongeville
 *  \date 08/2015
 *
 * This unit provides methods to use the XGM (eXtended Genesis Music) sound driver.<br>
 * This driver takes VGM (or XGM) file as input to play music.<br>
 * It supports 4 PCM channels at a fixed 14 Khz and allows to play SFX through PCM with 16 level of priority.<br>
 * The driver is designed to avoid DMA contention when possible (depending CPU load).
 */

#ifndef _XGM_H_
#define _XGM_H_


/**
 * \deprecated use XGM_isPlaying() instead
 */
#define SND_isPlaying_XGM()         _Pragma("GCC error \"This method is deprecated, use XGM_isPlaying() instead.\"")
/**
 * \deprecated use XGM_startPlay(..) instead
 */
#define SND_startPlay_XGM(song)     _Pragma("GCC error \"This method is deprecated, use XGM_startPlay(..) instead.\"")
/**
 * \deprecated use XGM_stopPlay(..) instead
 */
#define SND_stopPlay_XGM()          _Pragma("GCC error \"This method is deprecated, use XGM_stopPlay() instead.\"")
/**
 * \deprecated use XGM_pausePlay(..) instead
 */
#define SND_pausePlay_XGM()         _Pragma("GCC error \"This method is deprecated, use XGM_pausePlay() instead.\"")
/**
 * \deprecated use XGM_resumePlay(..) instead
 */
#define SND_resumePlay_XGM()        _Pragma("GCC error \"This method is deprecated, use XGM_resumePlay() instead.\"")

/**
 * \deprecated use XGM_isPlayingPCM(..) instead
 */
#define SND_isPlayingPCM_XGM(channel_mask)  _Pragma("GCC error \"This method is deprecated, use XGM_isPlayingPCM(..) instead.\"")
/**
 * \deprecated use XGM_setPCM(..) instead
 */
#define SND_setPCM_XGM(id, sample, len)     _Pragma("GCC error \"This method is deprecated, use XGM_setPCM(..) instead.\"")
/**
 * \deprecated use XGM_setPCMFast(..) instead
 */
#define SND_setPCMFast_XGM(id, sample, len) _Pragma("GCC error \"This method is deprecated, use XGM_setPCMFast(..) instead.\"")
/**
 * \deprecated use XGM_startPlayPCM(..) instead
 */
#define SND_startPlayPCM_XGM(id, prio, ch)  _Pragma("GCC error \"This method is deprecated, use XGM_startPlayPCM(..) instead.\"")
/**
 * \deprecated use XGM_stopPlayPCM(..) instead
 */
#define SND_stopPlayPCM_XGM(channel)        _Pragma("GCC error \"This method is deprecated, use XGM_stopPlayPCM(..) instead.\"")

/**
 * \deprecated use XGM_getMusicTempo(..) instead
 */
#define SND_getMusicTempo_XGM()         _Pragma("GCC error \"This method is deprecated, use XGM_getMusicTempo() instead.\"")
/**
 * \deprecated use XGM_setMusicTempo(..) instead
 */
#define SND_setMusicTempo_XGM(value)    _Pragma("GCC error \"This method is deprecated, use XGM_setMusicTempo(..) instead.\"")

/**
 * \deprecated use XGM_getManualSync(..) instead
 */
#define SND_getManualSync_XGM()         _Pragma("GCC error \"This method is deprecated, use XGM_getManualSync() instead.\"")
/**
 * \deprecated use XGM_setManualSync(..) instead
 */
#define SND_setManualSync_XGM()         _Pragma("GCC error \"This method is deprecated, use XGM_setManualSync() instead.\"")
/**
 * \deprecated use XGM_nextFrame(..) instead
 */
#define SND_nextFrame_XGM()             _Pragma("GCC error \"This method is deprecated, use XGM_nextFrame() instead.\"")
/**
 * \deprecated use XGM_nextXFrame(..) instead
 */
#define SND_nextXFrame_XGM(num)         _Pragma("GCC error \"This method is deprecated, use XGM_nextXFrame(..) instead.\"")

/**
 * \deprecated use XGM_setLoopNumber(..) instead
 */
#define SND_setLoopNumber_XGM(value)    _Pragma("GCC error \"This method is deprecated, use XGM_setLoopNumber(..) instead.\"")

/**
 * \deprecated use XGM_set68KBUSProtection(..) instead
 */
#define SND_set68KBUSProtection_XGM(value)  _Pragma("GCC error \"This method is deprecated, use XGM_set68KBUSProtection(..) instead.\"")
/**
 * \deprecated use XGM_getForceDelayDMA(..) instead
 */
#define SND_getForceDelayDMA_XGM()          _Pragma("GCC error \"This method is deprecated, use XGM_getForceDelayDMA() instead.\"")
/**
 * \deprecated use XGM_setForceDelayDMA(..) instead
 */
#define SND_setForceDelayDMA_XGM(value)     _Pragma("GCC error \"This method is deprecated, use XGM_setForceDelayDMA(..) instead.\"")
/**
 * \deprecated use XGM_getCPULoad(..) instead
 */
#define SND_getCPULoad_XGM()                _Pragma("GCC error \"This method is deprecated, use XGM_getCPULoad() instead.\"")


/**
 *  \brief
 *      Load the XGM sound driver.
 */
void XGM_loadDriver(const bool waitReady);

/**
 * \brief
 *      Returns play music state (XGM music player driver).
 */
bool XGM_isPlaying(void);
/**
 *  \brief
 *      Start playing the specified XGM track (XGM music player driver).
 *
 *  \param song
 *      XGM track address.
 *
 *  \see XGM_stopPlay
 *  \see XGM_pausePlay
 *  \see XGM_nextFrame
 *  \see XGM_startPlay_FAR
 */
void XGM_startPlay(const u8 *song);
/**
 *  \brief
 *      Same as #XGM_startPlay(..) except it supports music accessible through bank switch
 *
 *  \param song
 *      XGM track address.
 *  \param size
 *      XGM track size (in byte)
 *
 *  \see XGM_startPlay
 *  \see XGM_stopPlay
 *  \see XGM_pausePlay
 *  \see XGM_nextFrame
 */
void XGM_startPlay_FAR(const u8 *song, u32 size);

/**
 *  \brief
 *      Stop playing music (XGM music player driver).
 *
 *  \see XGM_pausePlay
 */
void XGM_stopPlay(void);
/**
 * \brief
 *      Pause playing music, music can be resumed by calling #XGM_resumePlay (XGM music player driver).<br>
 *      Note that due to the nature of the music chip (FM synthesis), resume play operation will never be perfect
 *      and some notes will miss until next key-on event occurs.
 *
 *  \see XGM_resumePlay
 *  \see XGM_stopPlay
 */
void XGM_pausePlay(void);
/**
 * \brief
 *      Resume playing music after pausing with #XGM_pausePlay (XGM music player driver).<br>
 *      Note that due to the nature of the music chip (FM synthesis), resume play operation will never be perfect
 *      and some notes will miss until next key-on event occurs.
 *
 *  \see XGM_pausePlay
 *  \see XGM_nextFrame
 */
void XGM_resumePlay(void);

/**
 *  \brief
 *      Return play status of specified PCM channel (XGM music player driver).
 *
 *  \param channel_mask
 *      Channel(s) we want to retrieve play state.<br>
 *      SOUND_PCM_CH1_MSK    = channel 1<br>
 *      SOUND_PCM_CH2_MSK    = channel 2<br>
 *      SOUND_PCM_CH3_MSK    = channel 3<br>
 *      SOUND_PCM_CH4_MSK    = channel 4<br>
 *      <br>
 *      You can combine mask to retrieve state of severals channels at once:<br>
 *      <code>XGM_isPlayingPCM(SOUND_PCM_CH1_MSK | SOUND_PCM_CH2_MSK)</code><br>
 *      will actually return play state for channel 1 and channel 2.
 *
 *  \return
 *      Return the channel mask of current playing channel(s).<br>
 *      For instance it returns (SOUND_PCM_CH1_MSK | SOUND_PCM_CH3_MSK) if channels 1 and 3 are currently playing.
 */
u8 XGM_isPlayingPCM(const u16 channel_mask);
/**
 *  \brief
 *      Declare a new PCM sample (maximum = 255) for the XGM music player driver.<br>
 *      Sample id < 64 are reserved for music while others are used for SFX
 *      so if you want to declare a new SFX PCM sample use an id >= 64
 *
 *  \param id
 *      Sample id:<br>
 *      value 0 is not allowed<br>
 *      values from 1 to 63 are used for music
 *      values from 64 to 255 are used for SFX
 *  \param sample
 *      Sample address, should be 256 bytes boundary aligned<br>
 *      SGDK automatically align sample resource as needed
 *  \param len
 *      Size of sample in bytes, should be a multiple of 256<br>
 *      SGDK automatically adjust resource size as needed
 */
void XGM_setPCM(const u8 id, const u8 *sample, const u32 len);
/**
 *  \brief
 *      Same as #XGM_setPCM but fast version.<br>
 *      This method assume that XGM driver is loaded and that 68000 has access to Z80 bus
 *
 *  \param id
 *      Sample id:<br>
 *      value 0 is not allowed<br>
 *      values from 1 to 63 are used for music
 *      values from 64 to 255 are used for SFX
 *  \param sample
 *      Sample address, should be 256 bytes boundary aligned<br>
 *      SGDK automatically align sample resource as needed
 *  \param len
 *      Size of sample in bytes, should be a multiple of 256<br>
 *      SGDK automatically adjust resource size as needed
 */
void XGM_setPCMFast(const u8 id, const u8 *sample, const u32 len);
/**
 *  \brief
 *      Same as #XGM_setPCM(..) allowing PCM access through bank switch
 */
void XGM_setPCM_FAR(const u8 id, const u8 *sample, const u32 len);
/**
 *  \brief
 *      Same as #XGM_setPCMFast(..) allowing PCM access through bank switch
 */
void XGM_setPCMFast_FAR(const u8 id, const u8 *sample, const u32 len);
/**
 *  \brief
 *      Play a PCM sample on specified channel (XGM music player driver).<br>
 *      If a sample was currently playing on this channel then priority of the newer sample should be are compared then it's stopped and the new sample is played instead.<br>
 *      Note that music may use the first PCM channel so it's better to use channel 2 to 4 for SFX.
 *
 *  \param id
 *      Sample id (use #XGM_setPCM(..) method first to set id)
 *  \param priority
 *      Value should go from 0 to 15 where 0 is lowest priority and 15 the highest one.<br>
 *      If the channel was already playing the priority is used to determine if the new SFX should replace the current one (new priority >= old priority).
 *  \param channel
 *      Channel where we want to play sample, accepted values are:<br>
 *      SOUND_PCM_CH1    = channel 1 (usually used by music)<br>
 *      SOUND_PCM_CH2    = channel 2<br>
 *      SOUND_PCM_CH3    = channel 3<br>
 *      SOUND_PCM_CH4    = channel 4<br>
 */
void XGM_startPlayPCM(const u8 id, const u8 priority, const SoundPCMChannel channel);
/**
 *  \brief
 *      Stop play PCM on specified channel (XGM music player driver).<br>
 *      No effect if no sample was currently playing on this channel.
 *
 *  \param channel
 *      Channel we want to stop, accepted values are:<br>
 *      SOUND_PCM_CH1    = channel 1<br>
 *      SOUND_PCM_CH2    = channel 2<br>
 *      SOUND_PCM_CH3    = channel 3<br>
 *      SOUND_PCM_CH4    = channel 4<br>
 */
void XGM_stopPlayPCM(const SoundPCMChannel channel);

/**
 *  \brief
 *      Return the elapsed play time since the last #XGM_startPlay(..) call.<br>
 *      The returned value is in music frame which can be 50/60 per second depending the base music play rate (NTSC/PAL).
 *
 *  \see XGM_startPlay(..)
 *  \see XGM_setMusicTempo()
 */
u32 XGM_getElapsed(void);
/**
 *  \brief
 *      Get the current music tempo (in tick per second).<br>
 *      Default value is 60 or 50 depending the system is NTSC or PAL.<br>
 *      This method is meaningful only if you use the automatic music sync mode (see XGM_setManualSync() method)
 *      which is the default mode.<br>
 *      Note that using specific tempo (not 60 or 50) will affect performance of DMA contention and external command parsing
 *      so it's recommended to stand with default one.
 *
 *  \see XGM_setManualSync(..)
 *  \see XGM_setMusicTempo(..)
 */
u16 XGM_getMusicTempo(void);
/**
 *  \brief
 *      Set the music tempo (in tick per second).<br>
 *      Default value is 60 or 50 depending the system is NTSC or PAL.
 *      This method is meaningful only if you use the automatic music sync mode (see XGM_setManualSync() method)
 *      which is the default mode.<br>
 *      Note that using specific tempo (not 60 or 50) can completely distord FM instruments sound and affect
 *      performance of DMA contention and external command parsing so it's recommended to stand with default one.
 *
 *  \see XGM_setManualSync(..)
 *  \see XGM_getMusicTempo(void)
 */
void XGM_setMusicTempo(const u16 value);

/**
 *  \brief
 *      Returns manual sync mode state of XGM driver (by default auto sync is used).
 *
 *  \see XGM_setManualSync(void)
 */
u16 XGM_getManualSync(void);
/**
 *  \brief
 *      Set manual sync mode of XGM driver (by default auto sync is used).
 *
 *  \param value TRUE or FALSE
 *  \see XGM_getManualSync(void)
 *  \see XGM_nextFrame(void)
 */
void XGM_setManualSync(const bool value);
/**
 *  \brief
 *      Notify the Z80 a new frame just happened (XGM music player driver).
 *
 *  Sound synchronization was initially 100% done by Z80 itself using the V-Interrupt but
 *  if the Z80 is stopped right at V-Int time (bus request from 68000 or DMA stall) then
 *  the V-Int can be missed by the Z80 and music timing affected.<br>
 *  To fix that issue and also to offer more flexibility the music timing should now be handled by the 68k.<br>
 *  By default this method is called automatically by SGDK at V-Int time but you can decide to handle sync
 *  manually (see XGM_setManualSync(..) method).<br>
 *  When you are in manual sync you normally should call this method once per frame (in the V-Int callback for instance)
 *  but you are free to play with it to increase or decrease music tempo.<br>
 *  Note that it's better to call this method a bit before (3/4 scanlines should be fine) doing DMA operation for best
 *  main bus contention protection (see #XGM_set68KBUSProtection() and #XGM_setForceDelayDMA() methods).
 *
 * \see XGM_setManualSync(..)
 * \see XGM_nextXFrame(..)
 * \see XGM_set68KBUSProtection(..)
 * \see XGM_setForceDelayDMA(..)
 */
#define XGM_nextFrame()  XGM_nextXFrame(1)
/**
 *  \brief
 *      Same as XGM_nextFrame() except you can specify the numer of frame.
 *
 * \see XGM_nextFrame(..)
 */
void XGM_nextXFrame(const u16 num);

/**
 *  \brief
 *      Set the loop number for music with loop command.<br>
 *      Default value is -1 for pseudo unfinite (255) loops plays.
 *      A value of 0 means single play without any loop, 1 = single play + 1 loop...
 */
void XGM_setLoopNumber(const s8 value);

/**
 *  \brief
 *      Same as Z80_setBusProtection(..) (generic version of the method)
 *
 *  \see XGM_setForceDelayDMA(..)
 */
void XGM_set68KBUSProtection(const bool value);
/**
 *  \brief
 *      Same as Z80_getForceDelayDMA() (generic version of the method)
 *
 *  \see XGM_setForceDelayDMA()
 */
bool XGM_getForceDelayDMA(void);
/**
 *  \brief
 *      Same as Z80_setForceDelayDMA(..) (generic version of the method)
 */
void XGM_setForceDelayDMA(const bool value);

/**
 *  \brief
 *      Returns an estimation of the Z80 CPU load (XGM driver).<br>
 *      The low 16 bits returns the estimated Z80 CPU load where the high 16 bits returns the part
 *      spent waiting in the DMA contention (see #XGM_set68KBUSProtection method).<br>
 *      The method computes CPU load mean over 32 frames and so it's important to call it at
 *      each frame (on VInt for instance) to get meaningful value.<br>
 *      Note that it returns CPU load only for the XGM music parsing part as PCM channel mixing is always ON.<br>
 *      Idle usage is 40% on NTSC and 30% on PAL, 100% usage usually mean overrun and may result in music slowdown
 *      and incorrect PCM operations.
 */
u32 XGM_getCPULoad(void);


#endif // _XGM_H_
````

## File: inc/snd/xgm2.h
````c
/**
 *  \file xgm2.h
 *  \brief XGM2 sound driver
 *  \author Stephane Dallongeville
 *  \date 04/2023
 *
 * This unit provides methods to use the XGM2 (eXtended Genesis Music) sound driver.<br>
 * It takes VGM (or XGM2) file as input to play music.<br>
 * It supports 3 PCM channels at either 13.3 Khz or 6.65 Khz and envelop control for both FM and PSG.<br>
 * It allows to play SFX through PCM with 16 level of priority.<br>
 * The driver supports renforced protection against DMA contention.
 */

#ifndef _XGM2_H_
#define _XGM2_H_


/**
 *  \brief
 *      Load the XGM2 sound driver.
 */
void XGM2_loadDriver(bool waitReady);

/**
 * \brief
 *      Returns play music state.
 */
bool XGM2_isPlaying(void);

/**
 *  \brief
 *      Load the specified XGM2 music blob (prepare for play, useful for multi tracks XGM2 music)
 *
 *  \param song
 *      XGM2 music blob address
 *
 *  \see XGM2_playTrack
 *  \see XGM2_stop
 *  \see XGM2_load_FAR
 */
void XGM2_load(const u8 *song);
/**
 *  \brief
 *      Same as #XGM2_load(..) except it supports access through bank switch
 *
 *  \param song
 *      XGM2 music blob address
 *  \param len
 *      XGM2 music blob size (in byte)
 *
 *  \see XGM2_playTrack
 *  \see XGM2_stop
 *  \see XGM2_load
 */
void XGM2_load_FAR(const u8 *song, const u32 len);
/**
 *  \brief
 *      Start playing the specified track (need to call XGM2_load(..) first)
 *
 *  \param track
 *      track index (for multi track XGM2 music blob)
 *
 *  \see XGM2_load
 *  \see XGM2_load_FAR
 *  \see XGM2_stop
 */
void XGM2_playTrack(const u16 track);
/**
 *  \brief
 *      Start playing the specified XGM2 music blob (fast play for single track XGM2 music)
 *
 *  \param song
 *      XGM2 track address.
 *
 *  \see XGM2_stop
 *  \see XGM2_play_FAR
 *  \see XGM2_load
 */
void XGM2_play(const u8* song);
/**
 *  \brief
 *      Same as #XGM2_play(..) except it supports music accessible through bank switch
 *
 *  \param song
 *      XGM2 track address.
 *  \param len
 *      XGM2 track size (in byte)
 *
 *  \see XGM2_stop
 *  \see XGM2_play
 *  \see XGM2_load_FAR
 */
void XGM2_play_FAR(const u8* song, const u32 len);

/**
 *  \brief
 *      Stop playing music (cannot be resumed).<br>
 *
 *  \see XGM2_pause
 *  \see XGM2_resume
 *  \see XGM2_play
 *  \see XGM2_playTrack
 */
void XGM2_stop(void);
/**
 * \brief
 *      Pause playing music, music can be resumed by calling #XGM2_resume().<br>
 *      Note that due to the nature of the music chip (FM synthesis), resume play operation will never be perfect
 *      and some notes will miss until next key-on event occurs.
 *
 *  \see XGM2_resume
 *  \see XGM2_stop
 */
void XGM2_pause(void);
/**
 * \brief
 *      Resume playing music after pausing with XGM2_stop().<br>
 *      Note that due to the nature of the music chip (FM synthesis), resume play operation will never be perfect
 *      and some notes will miss until next key-on event occurs.
 *
 *  \see XGM2_pause
 *  \see XGM2_stop
 */
void XGM2_resume(void);

/**
 *  \brief
 *      Return play status of specified PCM channel
 *
 *  \param channel_mask
 *      Channel(s) we want to retrieve play state.<br>
 *      SOUND_PCM_CH1_MSK    = channel 1<br>
 *      SOUND_PCM_CH2_MSK    = channel 2<br>
 *      SOUND_PCM_CH3_MSK    = channel 3<br>
 *      <br>
 *      You can combine mask to retrieve state of severals channels at once:<br>
 *      <code>isPlayingPCM(SOUND_PCM_CH1_MSK | SOUND_PCM_CH2_MSK)</code><br>
 *      will actually return play state for channel 1 and channel 2.
 *
 *  \return
 *      Return the channel mask of current playing channel(s).<br>
 *      For instance it returns (SOUND_PCM_CH1_MSK | SOUND_PCM_CH3_MSK) if channels 1 and 3 are currently playing.
 *
 *  \see XGM2_playPCM
 *  \see XGM2_stopPCM
 */
u8 XGM2_isPlayingPCM(const u16 channel_mask);
/**
 *  \brief
 *      Play a PCM sample on specified channel (XGM2 music player driver).<br>
 *      The method use a default priority value of 6 which is below the minimum music PCM priority (7)
 *
 *  \param sample
 *      Sample address, should be 256 bytes boundary aligned<br>
 *      (SGDK automatically align WAV resource as needed)
 *  \param len
 *      Size of sample in bytes, should be a multiple of 256<br>
 *      (SGDK automatically adjust WAV resource size as needed)
 *  \param channel
 *      Channel to use to play sample.<br>
 *      SOUND_PCM_CH_AUTO = auto selection from current channel usage<br>
 *      SOUND_PCM_CH1     = channel 1 (usually used by music)<br>
 *      SOUND_PCM_CH2     = channel 2<br>
 *      SOUND_PCM_CH3     = channel 3<br>
 *  \return FALSE if there is no channel available to play the PCM, TRUE otherwise
 *
 *  \see XGM2_playPCMEx
 *  \see XGM2_stopPCM
 *  \see XGM2_isPlayingPCM
 */
bool XGM2_playPCM(const u8 *sample, const u32 len, const SoundPCMChannel channel);
/**
 *  \brief
 *      Play a PCM sample on specified channel (XGM2 music player driver).<br>
 *      If a sample was currently playing on this channel then priority of the newer sample should be are compared then it's stopped and the new sample is played instead.<br>
 *      Note that music may use the first PCM channel so it's better to use channel 2 to 4 for SFX.
 *
 *  \param sample
 *      Sample address, should be 256 bytes boundary aligned<br>
 *      (SGDK automatically align WAV resource as needed)
 *  \param len
 *      Size of sample in bytes, should be a multiple of 256<br>
 *      (SGDK automatically adjust WAV resource size as needed)
 *  \param channel
 *      Channel to use to play sample.<br>
 *      SOUND_PCM_CH_AUTO = auto selection from current channel usage<br>
 *      SOUND_PCM_CH1     = channel 1 (usually used by music)<br>
 *      SOUND_PCM_CH2     = channel 2<br>
 *      SOUND_PCM_CH3     = channel 3<br>
 *  \param priority
 *      Value should go from 0 to 15 where 0 is lowest priority and 15 the highest one (music PCM priority can either be set to 7 or 15).<br>
 *      If the channel was already playing the priority is used to determine if the new SFX should replace the current one (new priority >= old priority).
 *  \param halfRate
 *      Set to TRUE to play the sample at half rate (6.65 Khz) instead of default 13.3 Khz
 *  \param loop
 *      Set to TRUE to enable looping sample play
 *  \return FALSE if there is no channel available to play the PCM, TRUE otherwise
 *
 *  \see XGM2_playPCM
 *  \see XGM2_stopPCM
 *  \see XGM2_isPlayingPCM
 */
bool XGM2_playPCMEx(const u8 *sample, const u32 len, const SoundPCMChannel channel, const u8 priority, const bool halfRate, const bool loop);
/**
 *  \brief
 *      Stop play PCM on specified channel (XGM2 music player driver).<br>
 *      No effect if no sample was currently playing on this channel.
 *
 *  \param channel
 *      Channel we want to stop.<br>
 *      SOUND_PCM_CH1 = channel 1<br>
 *      SOUND_PCM_CH2 = channel 2<br>
 *      SOUND_PCM_CH3 = channel 3<br>
 *
 *  \see XGM2_playPCM
 *  \see XGM2_isPlayingPCM
 */
void XGM2_stopPCM(const SoundPCMChannel channel);

/**
 *  \return
 *      TRUE if currently processing a volume fade effect, FALSE otherwise.
 */
bool XGM2_isProcessingFade(void);
/**
 *  \brief
 *      Process music volume "fade-in" effect, must be called right after a "start play" or "resume" command.<br>
 *      Gradually increase FM and PSG volume starting from 0 up to the current volume levels.
 *
 *  \param numFrame
 *      Duration of music fade-in effect in number of frame.
 */
void XGM2_fadeIn(const u16 numFrame);
/**
 *  \brief
 *      Process music volume "fade-out" effect.<br>
 *      Gradually decrease FM and PSG volume starting from current levels down to 0.
 *
 *  \param numFrame
 *      Duration of music fade-out effect in number of frame.
 */
void XGM2_fadeOut(const u16 numFrame);
/**
 *  \brief
 *      Process music volume "fade-out" effect and stop.<br>
 *      Gradually decrease FM and PSG volume starting from current levels down to 0, then issue a 'stop play' command.
 *
 *  \param numFrame
 *      Duration of music fade-out effect in number of frame.
 */
void XGM2_fadeOutAndStop(const u16 numFrame);
/**
 *  \brief
 *      Process music volume "fade-out" effect and pause.<br>
 *      Gradually decrease FM and PSG volume starting from current levels down to 0, then issue a 'stop play' command.
 *
 *  \param numFrame
 *      Duration of music fade-out effect in number of frame.
 */
void XGM2_fadeOutAndPause(const u16 numFrame);
/**
 *  \brief
 *      Process a specific music volume "fade" effect.<br>
 *      Gradually change FM and PSG volume starting from current levels to the specified ones.
 *
 *  \param fmVolume
 *      FM volume to reach at the end of fade effect.
 *  \param psgVolume
 *      PSG volume to reach at the end of fade effect.
 *  \param numFrame
 *      Duration of music fade effect in number of frame.
 */
void XGM2_fadeTo(const u16 fmVolume, const u16 psgVolume, const u16 numFrame);

/**
 *  \brief
 *      Set the loop number for music with loop command.<br>
 *      Default value is -1 for pseudo unfinite (255) loops plays.
 *      A value of 0 means single play without any loop, 1 = single play + 1 loop...
 */
void XGM2_setLoopNumber(const s8 value);

/**
 *  \brief
 *      Return the elapsed play time since the last #XGM2_play(..) call.<br>
 *      The returned value is in music frame which can be 50/60 per second depending the base music play rate (NTSC/PAL).
 *
 *  \see XGM2_play(..)
 *  \see XGM2_playTrack(..)
 *  \see XGM2_setMusicTempo(..)
 */
u32 XGM2_getElapsed(void);

/**
 *  \brief
 *      Get the current music tempo (in tick per second).<br>
 *      Default value is 60 or 50 depending the system is NTSC or PAL.<br>
 *      This method is meaningful only if you use the automatic music sync mode (see XGM2_setManualSync() method)
 *      which is the default mode.<br>
 *      Note that using specific tempo (not 60 or 50) will affect performance of DMA contention and external command parsing
 *      so it's recommended to stand with default one.
 *
 *  \see XGM2_setMusicTempo
 */
u16 XGM2_getMusicTempo(void);
/**
 *  \brief
 *      Set the music tempo (in tick per second).<br>
 *      Default value is 60 or 50 depending the system is NTSC or PAL.
 *      This method is meaningful only if you use the automatic music sync mode (see XGM2_setManualSync() method)
 *      which is the default mode.<br>
 *      Note that using specific tempo (not 60 or 50) can completely distord FM instruments sound and affect
 *      performance of DMA contention and external command parsing so it's recommended to stand with default one.
 *
 *  \see XGM2_getMusicTempo
 */
void XGM2_setMusicTempo(const u16 value);

/**
 *  \brief
 *      Set the volume level for the FM music part
 *
 *  \param value
 *      volume level (0 to 100)
 *
 *  \see XGM2_setPSGVolume
 */
void XGM2_setFMVolume(const u16 value);
/**
 *  \brief
 *      Set the <i>volume</i> level for the PSG music part
 *
 *  \param value
 *      volume level (0 to 100)
 *
 *  \see XGM2_setFMVolume
 */
void XGM2_setPSGVolume(const u16 value);

/**
 * \brief
 *      Returns TRUE if specified xgm2 use PAL timing.
 */
bool XGM2_isPAL(const u8 *xgm2);

/**
 *  \brief
 *      Returns an estimation of the Z80 CPU load (XGM2 driver).
 *
 *  \param mean
 *      if set to TRUE then return a mean load computed on the last 8 frames otherwise return instant last frame load
 */
u16 XGM2_getCPULoad(const bool mean);
/**
 *  \brief
 *      Returns an estimation of the Z80 CPU time spent in waiting for DMA completion (see #Z80_setBusProtection(bool) method).
*
 *  \param mean
 *      if set to TRUE then return a mean wait computed on the last 8 frames otherwise return instant last frame DMA wait
 */
u16 XGM2_getDMAWaitTime(const bool mean);

/**
 *  \brief
 *      Returns the internal frame counter (v-int process number).<br>
 *      Debug function to verify the driver is working optimally.
 */
u16 XGM2_getDebugFrameCounter(void);
/**
 *  \brief
 *      Returns the real PCM playback rate (XGM2 driver).<br>
 *      Debug function to verify the driver is working optimally.
 */
u16 XGM2_getDebugPCMRate(void);
/**
 *  \brief
 *      Returns the number of missed frames since last startPlay command.<br>
 *      Debug function to verify the driver is working optimally.
 */
u8 XGM2_getDebugMissedFrames(void);
/**
 *  \brief
 *      Returns the ending process time in number of sample for the specified v-int process (v-int process number).<br>
 *      Debug function to verify the driver is working optimally.
 */
u8 XGM2_getDebugProcessDuration(const u16 ind);


#endif // _XGM2_H_
````

## File: inc/asm.h
````c
/**
 *  \file asm.h
 *  \brief Assembly helper
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides methods to help dealing with inline assembly or header inclusion GAS.
 */

#ifndef _ASM_H_
#define _ASM_H_


#define VAR2REG_B(var, reg)       asm volatile ("move.b %0, %/"reg"" :: "r" (var) : ""reg"");
#define VAR2REG_W(var, reg)       asm volatile ("move.w %0, %/"reg"" :: "r" (var) : ""reg"");
#define VAR2REG_L(var, reg)       asm volatile ("move.l %0, %/"reg"" :: "r" (var) : ""reg"");

#define REG2VAR_B(reg, var)       asm volatile ("move.b %/"reg", %0" : "=r" (var));
#define REG2VAR_W(reg, var)       asm volatile ("move.w %/"reg", %0" : "=r" (var));
#define REG2VAR_L(reg, var)       asm volatile ("move.l %/"reg", %0" : "=r" (var));

// enumeration helper for GAS
#if defined(__ASSEMBLY__) || defined(__ASSEMBLER__)

  .set last_enum_value, 0
  .macro enum_val name
  .equiv \name, last_enum_value
  .set last_enum_value, last_enum_value + 1
  .endm

    #define ENUM_BEGIN  .set last_enum_value, 0
    #define ENUM_BEGIN_EX(tmpName)  .set last_enum_value, 0
    #define ENUM_BEGIN_SHORT  .set last_enum_value, 0
    #define ENUM_BEGIN_SHORT_EX(tmpName)  .set last_enum_value, 0
    #define ENUM_VAL(name) enum_val name
    #define ENUM_VALASSIGN(name, value)            \
      .set last_enum_value, value                 ;\
      enum_val name
    #define ENUM_END(enum_name)
#else
    #define ENUM_BEGIN typedef enum {
    #define ENUM_BEGIN_EX(tmpName) typedef enum tmpName {
    #define ENUM_BEGIN_SHORT typedef enum __attribute__ ((__packed__)) {
    #define ENUM_BEGIN_SHORT_EX(tmpName) typedef enum __attribute__ ((__packed__)) tmpName {
    #define ENUM_VAL(name) name,
    #define ENUM_VALASSIGN(name, value) name = value,
    #define ENUM_END(enum_name) } enum_name;
#endif


#endif // _ASM_H_
````

## File: inc/bmp.h
````c
/**
 *  \file bmp.h
 *  \brief Software bitmap engine
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides methods to simulate bitmap mode on SEGA genesis.<br>
 *<br>
 * The software bitmap engine permit to simulate a 256x160 pixels bitmap screen.<br>
 * Bitmap engine requires a large amount of memory (~41KB) which is dynamically allocated at BMP_init(..) time and released when BMP_end() is called.<br>
 * Bitmap engine uses a double buffer so you can draw to the write buffer while the read buffer is being sent to video memory.<br>
 * These buffers are transferred to VRAM during blank area, by default on NTSC system the blanking period is really short so it takes approximately 10 frames to blit an entire buffer.<br>
 * To improve transfer performance the blank area is extended to fit bitmap resolution:<br>
 * scanline 0-31 = blank<br>
 * scanline 32-191 = active<br>
 * scanline 192-262/312 = blank<br>
 * <br>
 * With extended blank bitmap buffer can be transferred to VRAM 20 times per second in NTSC<br>
 * and 25 time per second in PAL.
 */

#include "maths.h"
#include "vdp.h"
#include "pal.h"

#ifndef _BMP_H_
#define _BMP_H_


#define BMP_PLANE_WIDTH_SFT         planeWidthSft
#define BMP_PLANE_HEIGHT_SFT        planeHeightSft
/**
 *  \brief
 *          Bitmap plane width (in tile)
 */
#define BMP_PLANE_WIDTH             64
/**
 *  \brief
 *          Bitmap plane height (in tile)
 */
#define BMP_PLANE_HEIGHT            64

#define BMP_TILE_WIDTH_SFT          5
/**
 *  \brief
 *          Bitmap width (in tile): 32
 */
#define BMP_TILE_WIDTH              (1 << BMP_TILE_WIDTH_SFT)
#define BMP_TILE_WIDTH_MASK         (BMP_TILE_WIDTH - 1)
/**
 *  \brief
 *          Bitmap height (in tile): 20
 */
#define BMP_TILE_HEIGHT             20

#define BMP_TILE_XOFFSET            (((screenWidth >> 3) - BMP_TILE_WIDTH) / 2)
#define BMP_TILE_YOFFSET            (((screenHeight >> 3) - BMP_TILE_HEIGHT) / 2)

#define BMP_XPIXPERTILE_SFT         3
#define BMP_YPIXPERTILE_SFT         3
/**
 *  \brief
 *          Number of X pixel per tile: 8 pixels per tile.
 */
#define BMP_XPIXPERTILE             (1 << BMP_XPIXPERTILE_SFT)
/**
 *  \brief
 *          Number of y pixel per tile: 8 pixels per tile.
 */
#define BMP_YPIXPERTILE             (1 << BMP_YPIXPERTILE_SFT)
#define BMP_XPIXPERTILE_MASK        (BMP_XPIXPERTILE - 1)
#define BMP_YPIXPERTILE_MASK        (BMP_YPIXPERTILE - 1)

#define BMP_WIDTH_SFT               (BMP_TILE_WIDTH_SFT + BMP_XPIXPERTILE_SFT)
/**
 *  \brief
 *          Bitmap width (in pixel): 256
 */
#define BMP_WIDTH                   (1 << BMP_WIDTH_SFT)
/**
 *  \brief
 *          Bitmap height (in pixel): 160
 */
#define BMP_HEIGHT                  (BMP_TILE_HEIGHT * BMP_YPIXPERTILE)
#define BMP_WIDTH_MASK              (BMP_WIDTH - 1)

#define BMP_PITCH_SFT               (BMP_TILE_WIDTH_SFT + 2)
/**
 *  \brief
 *          Bitmap scanline pitch (number of byte per scanline): 128
 */
#define BMP_PITCH                   (1 << BMP_PITCH_SFT)
#define BMP_PITCH_MASK              (BMP_PITCH - 1)

/**
 *  \brief
 *          Get width of genesis bitmap 16 object.
 */
#define BMP_GENBMP16_WIDTH(genbmp16)    ((genbmp16)[0])
/**
 *  \brief
 *          Get height of genesis bitmap 16 object.
 */
#define BMP_GENBMP16_HEIGHT(genbmp16)   ((genbmp16)[1])
/**
 *  \brief
 *          Return pointer to palette of genesis bitmap 16 object.
 */
#define BMP_GENBMP16_PALETTE(genbmp16)  (&((genbmp16)[2]))
/**
 *  \brief
 *          Return pointer to image data of genesis bitmap 16 object.
 */
#define BMP_GENBMP16_IMAGE(genbmp16)    (&((genbmp16)[18]))

/**
 *  \deprecated Use BMP_getPixelFast(..) instead (inlining make macro useless)
 */
#define BMP_GETPIXEL(x, y)          _Pragma("GCC error \"This method is deprecated, use BMP_getPixelFast(..) instead.\"")
/**
 *  \deprecated Use BMP_setPixelFast(..) instead (inlining make macro useless)
 */
#define BMP_SETPIXEL(x, y, col)     _Pragma("GCC error \"This method is deprecated, use BMP_setPixelFast(..) instead.\"")


#define BMP_BASE_TILE_INDEX         TILE_USER_INDEX

#define BMP_FB0_TILE_INDEX          BMP_BASE_TILE_INDEX
#define BMP_FB1_TILE_INDEX          (BMP_BASE_TILE_INDEX + (BMP_TILE_WIDTH * BMP_TILE_HEIGHT))

#define BMP_FB0_END_TILE_INDEX      (BMP_FB0_TILE_INDEX + (BMP_TILE_WIDTH * BMP_TILE_HEIGHT))
#define BMP_FB1_END_TILE_INDEX      (BMP_FB1_TILE_INDEX + (BMP_TILE_WIDTH * BMP_TILE_HEIGHT))
#define BMP_END_TILE_INDEX          BMP_FB1_END_TILE_INDEX

#define BMP_BASE_ADDR               (BMP_BASE_TILE_INDEX * 32)
#define BMP_FB0_ADDR                (BMP_FB0_TILE_INDEX * 32)
#define BMP_FB1_ADDR                (BMP_FB1_TILE_INDEX * 32)
#define BMP_END_ADDR                (BMP_END_TILE_INDEX * 32)


/**
 *  \brief
 *      Genesis 4bpp Bitmap structure definition.<br>
 *      Use the unpackBitmap() method to unpack if compression is enabled.
 *
 *  \param compression
 *      compression type for image data, accepted values:<br>
 *      <b>COMPRESSION_NONE</b><br>
 *      <b>COMPRESSION_APLIB</b><br>
 *      <b>COMPRESSION_LZ4W</b><br>
 *  \param w
 *      Width in pixel.
 *  \param h
 *      Height in pixel.
 *  \param palette
 *      Palette data.
 *  \param image
 *      Image data, array size = (w * h / 2) - can be FAR pointer (see mapper.h unit for explaination)
 */
typedef struct
{
    u16 compression;
    u16 w;
    u16 h;
    const Palette *palette;
    const u8 *image;
} Bitmap;

/**
 *  \brief
 *          Pixel definition.
 *
 *  \param pt
 *          Coordinates.
 *  \param col
 *          Color (should be 8 bits filled: 0x0000, 0x0011, .. for plain color).
 *          we use u16 for alignment optimization
 */
typedef struct
{
    Vect2D_s16 pt;
    u16 col;
} Pixel;

/**
 *  \brief
 *          Line definition.
 *
 *  \param pt1
 *          Start point.
 *  \param pt2
 *          End point.
 *  \param col
 *          Color (should be 8 bits filled: 0x0000, 0x0011, .. for plain color).
 *          we use u16 for alignment optimization
 */
typedef struct
{
    Vect2D_s16 pt1;
    Vect2D_s16 pt2;
    u16 col;
} Line;

/**
 *  \brief
 *          Triangle definition.
 *
 *  \param pt1
 *          Start point.
 *  \param pt2
 *          Second point.
 *  \param pt3
 *          End point.
 *  \param col
 *          Color (should be 8 bits filled: 0x00, 0x11, .. for plain color).
 *          we use u16 for alignment optimization
 */
typedef struct
{
    Vect2D_s16 pt1;
    Vect2D_s16 pt2;
    Vect2D_s16 pt3;
    u16 col;
} Triangle;


/**
 *      Current bitmap read buffer.
 */
extern u8 *bmp_buffer_read;
/**
 *      Current bitmap write buffer.
 */
extern u8 *bmp_buffer_write;


/**
 *  \brief
 *      Initialize the software bitmap engine.
 *
 *  \param double_buffer
 *      Enabled VRAM double buffer.<br>
 *      VRAM Double buffer permit to avoid image tearing because of partial screen refresh.<br>
 *      It requires almost all VRAM tiles space (~41 KB) so enable it only if you don't need other plane or sprites.
 *  \param plane
 *      Plane to use to display the bitmap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param palette
 *      Palette index to use to render the bitmap plane.<br>
 *      Set it to 0 if unsure.
 *  \param priority
 *      Set the priority of bitmap plane.<br>
 *      0 = low priority (default).<br>
 *      1 = high priority.
 *
 * Requires ~41 KB of memory which is dynamically allocated.
 */
void BMP_init(u16 double_buffer, VDPPlane plane, u16 palette, u16 priority);
/**
 *  \brief
 *      End the software bitmap engine.
 *
 * Release memory used by software bitmap engine (~41 KB).
 */
void BMP_end(void);
/**
 *  \brief
 *      Reset the software bitmap engine.
 *
 * Rebuild tilemap for bitmap engine and clear buffers.
 */
void BMP_reset(void);
/**
 *  \brief
 *      Enable back buffer preservation.
 *
 * The bitmap engine is always using double buffering in maim memory so you can continue to write your bitmap
 * while the previous bitmap is being transferred to video memory.<br>
 * The problem with double buffer is that your content is not preserved on a frame basis as you have 2 differents buffers,
 * by enabling "buffer copy" you can preserve your bitmap but this has an important CPU cost as we need to copy bitmap buffer
 * at each flip operation.<br>
 * By default buffer copy is disabled for obvious performance reason.
 */
void BMP_setBufferCopy(u16 value);
/**
 *  \brief
 *      Flip bitmap buffer to screen.
 *
 * Blit the current bitmap back buffer to the screen then flip buffers
 * so back buffer becomes front buffer and vice versa.<br>
 * If you use async flag the Bitmap buffer will be sent to video memory asynchronously
 * during blank period and the function return immediatly.<br>
 * If a flip is already in process then flip request is marked as pending
 * and will be processed as soon the current one complete.<br>
 * You can use #BMP_waitWhileFlipRequestPending() method to ensure no more flip request are pending
 * before writing to bitmap buffer.<br>
 * If a flip request is already pending the function wait until no more request are pending.
 *
 *  \param async
 *      Asynchronous flip operation flag.
 *  \return
 *   Only meaningful for async operation:<br>
 *   0 if the flip request has be marked as pending as another flip is already in process.<br>
 *   1 if the flip request has be initiated.
 *
 *  \see #BMP_hasFlipRequestPending()
 *  \see #BMP_waitWhileFlipRequestPending()
 */
u16 BMP_flip(u16 async);

/**
 *  \brief
 *      Clear bitmap buffer.
 */
void BMP_clear(void);

/**
 *  \brief
 *      Get write pointer for specified pixel.
 *
 *  \param x
 *      X pixel coordinate.
 *  \param y
 *      Y pixel coordinate.
 *
 * As coordinates are expressed for 4bpp pixel BMP_getWritePointer(0,0)
 * and BMP_getWritePointer(1,0) actually returns the same address.
 */
u8*  BMP_getWritePointer(u16 x, u16 y);
/**
 *  \brief
 *      Get read pointer for specified pixel.
 *
 *  \param x
 *      X pixel coordinate.
 *  \param y
 *      Y pixel coordinate.
 *
 * As coordinates are expressed for 4bpp pixel BMP_getReadPointer(0,0)
 * and BMP_getReadPointer(1,0) actually returns the same address.
 */
u8*  BMP_getReadPointer(u16 x, u16 y);


/**
 *  \brief
 *      Return true (!= 0) if a flip request is pending.
 */
u16  BMP_hasFlipRequestPending(void);
/**
 *  \brief
 *      Wait until no more flip request is pending.
 */
void BMP_waitWhileFlipRequestPending(void);
/**
 *  \brief
 *      Return true (!= 0) if a flip operation is in progress.
 */
u16  BMP_hasFlipInProgess(void);
/**
 *  \brief
 *      Wait until the asynchronous flip operation is completed.<br>
 *      Blit operation is the bitmap buffer transfer to VRAM.<br>
 */
void BMP_waitFlipComplete(void);

/**
 *  \brief
 *      Draw text in bitmap mode.
 *
 *  \param str
 *      String to display.
 *  \param x
 *      X coordinate (in tile).
 *  \param y
 *      y coordinate (in tile).
 */
void BMP_drawText(const char *str, u16 x, u16 y);
/**
 *  \brief
 *      Clear text in bitmap mode.
 *
 *  \param x
 *      X coordinate (in tile).
 *  \param y
 *      y coordinate (in tile).
 *  \param w
 *      Number of characters to clear.
 */
void BMP_clearText(u16 x, u16 y, u16 w);
/**
 *  \brief
 *      Clear a line of text in bitmap mode.
 *
 *  \param y
 *      y coordinate (in tile).
 */
void BMP_clearTextLine(u16 y);

/**
 *  \brief
 *      Show the frame rate in bitmap mode.
 *
 *  \param float_display
 *      If this value is true (!= 0) the frame rate is displayed as float (else it's integer).
 *  \param x
 *      X coordinate (in tile).
 *  \param y
 *      y coordinate (in tile).
 */
void BMP_showFPS(u16 float_display, u16 x, u16 y);

/**
 *  \brief
 *      Get pixel at specified position (safe version with bounds verification)
 *
 *  \param x
 *      X coordinate.
 *  \param y
 *      Y coordinate.
 *
 *  \see #BMP_getPixelFast
 *  \see #BMP_setPixel
 */
u8   BMP_getPixel(u16 x, u16 y);
/**
 *  \brief
 *      Get pixel at specified position (fast version without bounds verification)
 *
 *  \param x
 *      X coordinate.
 *  \param y
 *      Y coordinate.
 *
 *  \see #BMP_getPixel
 *  \see #BMP_setPixelFast
 */
u8   BMP_getPixelFast(u16 x, u16 y);
/**
 *  \brief
 *      Set pixel at specified position (safe version with bounds verification)
 *
 *  \param x
 *      X pixel coordinate.
 *  \param y
 *      Y pixel coordinate.
 *  \param col
 *      pixel color (should be 8 bits filled: 0x00, 0x11, .. for plain color)
 *
 *  \see #BMP_setPixelFast
 *  \see #BMP_getPixel
 */
void BMP_setPixel(u16 x, u16 y, u8 col);
/**
 *  \brief
 *      Set pixel at specified position (fast version without bounds verification)
 *
 *  \param x
 *      X pixel coordinate.
 *  \param y
 *      Y pixel coordinate.
 *  \param col
 *      pixel color (should be 8 bits filled: 0x00, 0x11, .. for plain color)
 *
 *  \see #BMP_setPixel
 *  \see #BMP_getPixelFast
 */
void BMP_setPixelFast(u16 x, u16 y, u8 col);
/**
 *  \brief
 *      Set pixels from Vect2D array (safe version with bounds verification)
 *
 *  \param crd
 *      Vect2D_u16 Coordinates buffer.
 *  \param col
 *      pixels color (should be 8 bits filled: 0x00, 0x11, .. for plain color)
 *  \param num
 *      number of pixel to draw (length of coordinates buffer).
 *
 */
 void BMP_setPixels_V2D(const Vect2D_u16 *crd, u8 col, u16 num);
/**
 *  \brief
 *      Set pixels from Vect2D array (fast version without bounds verification)
 *
 *  \param crd
 *      Vect2D_u16 Coordinates buffer.
 *  \param col
 *      pixels color (should be 8 bits filled: 0x00, 0x11, .. for plain color)
 *  \param num
 *      number of pixel to draw (length of coordinates buffer).
 *
 */
 void BMP_setPixelsFast_V2D(const Vect2D_u16 *crd, u8 col, u16 num);
/**
 *  \brief
 *      Set pixels from Pixel array (safe version with bounds verification)
 *
 *  \param pixels
 *      Pixels buffer.
 *  \param num
 *      number of pixel to draw (length of pixels buffer).
 */
 void BMP_setPixels(const Pixel *pixels, u16 num);
/**
 *  \brief
 *      Set pixels from Pixel array (fast version without bounds verification)
 *
 *  \param pixels
 *      Pixels buffer.
 *  \param num
 *      number of pixel to draw (length of pixels buffer).
 */
 void BMP_setPixelsFast(const Pixel *pixels, u16 num);

/**
 *  \brief
 *      Clip the specified line so it fit in bitmap screen (0, 0, BMP_WIDTH, BMP_HEIGHT)
 *
 *  \param l
 *      Line to clip.
 *  \return
 *      FALSE (0) is the line is entirely outside bitmap screen (no clip is done).<br>
 *      TRUE if at least one pixel is on screen (line is clipped if needed).
 */
u16   BMP_clipLine(Line *l);
/**
 *  \brief
 *      Draw a line (no bounds verification).
 *      You can use #BMP_clipLine(..) first to clip the line to view range if needed.
 *
 *  \param l
 *      Line to draw.
 */
void BMP_drawLine(Line *l);
/**
 *  \brief
 *      Determine if the specified polygon is culled.<br>
 *      The polygon points should be defined in clockwise order.<br>
 *      The method returns 1 if the polygon is back faced and should be eliminated for 3D rendering.
 *
 *  \param pts
 *      Polygon points buffer.
 *  \param num
 *      number of point (length of points buffer).
 *  \return 1 if polygon is culled (should not be draw) and 0 otherwise.<br>
 */
u16 BMP_isPolygonCulled(const Vect2D_s16 *pts, u16 num);
/**
 *  \brief
 *      Draw a polygon.<br>
 *      The polygon points should be defined in clockwise order.<br>
 *      Use the BMP_isPolygonCulled(..) method to test if polygon should be draw or not.
 *
 *  \param pts
 *      Polygon points buffer.
 *  \param num
 *      number of point (length of points buffer).
 *  \param col
 *      fill color.
 *  \return 0 if polygon was not drawn (outside screen or whatever).
 */
u16 BMP_drawPolygon(const Vect2D_s16 *pts, u16 num, u8 col);

/**
 *  \brief
 *      Draw the specified 4BPP bitmap data.
 *
 *  \param data
 *      4BPP image data buffer.
 *  \param x
 *      X coordinate (should be an even value for byte alignment).
 *  \param y
 *      y coordinate.
 *  \param w
 *      width (should be an even value for byte alignment).
 *  \param h
 *      height.
 *  \param pitch
 *      bitmap pitch (number of bytes per bitmap scanline).
 *
 * X coordinate as Width are aligned to even value for performance reason.<br>
 * So BMP_drawBitmapData(data,0,0,w,h,w) will produce same result as BMP_drawBitmapData(data,1,0,w,h,w)
 */
void BMP_drawBitmapData(const u8 *data, u16 x, u16 y, u16 w, u16 h, u32 pitch);
/**
 *  \brief
 *      Draw a Genesis Bitmap.<br>
 *
 *      A Genesis bitmap is automatically created from .bmp or .png file via the rescomp tool.<br>
 *      The resulting file contains bitmap size info and 16 colors palette.<br>
 *
 *  \param bitmap
 *      Genesis Bitmap.<br>
 *      The Bitmap is unpacked "on the fly" if needed (require some memory).
 *  \param x
 *      X coordinate (should be an even value).
 *  \param y
 *      y coordinate.
 *  \param loadpal
 *      Load the bitmap palette information.
 *  \return
 *      FALSE if there is not enough memory to unpack the specified Bitmap (only if compression was enabled).
 *
 * X coordinate is aligned to even value for performance reason.<br>
 * So BMP_drawBitmap(bitmap,0,0,TRUE) will produce same result as BMP_drawBitmap(bitmap,1,0,TRUE)
 */
bool BMP_drawBitmap(const Bitmap *bitmap, u16 x, u16 y, bool loadpal);
/**
 *  \brief
 *      Load and draw a Genesis Bitmap with specified dimension.<br>
 *
 *      A Genesis bitmap is automatically created from .bmp or .png file via the rescomp tool.<br>
 *      The resulting file contains bitmap size info and 16 colors palette.<br>
 *      This method will scale and draw the bitmap with the specified dimension.
 *
 *  \param bitmap
 *      Genesis bitmap.<br>
 *      The Bitmap is unpacked "on the fly" if needed (require some memory).
 *  \param x
 *      X coordinate (should be an even value).
 *  \param y
 *      y coordinate.
 *  \param w
 *      final width.
 *  \param h
 *      final height.
 *  \param loadpal
 *      Load the bitmap palette information
 *  \return
 *      FALSE if there is not enough memory to unpack the specified Bitmap (only if compression was enabled).
 *
 * X coordinate as width are aligned to even values for performance reason.<br>
 * So BMP_drawBitmapScaled(bitmap,0,0,w,h,pal) will produce same result as BMP_drawBitmapScaled(bitmap,1,0,w,h,pal)
 */
bool BMP_drawBitmapScaled(const Bitmap *bitmap, u16 x, u16 y, u16 w, u16 h, bool loadpal);

/**
 *  \deprecated
 *      Use BMP_drawBitmapData(..) instead.
 */
#define BMP_loadBitmapData(data, x, y, w, h, pitch)             _Pragma("GCC error \"This method is deprecated, use BMP_drawBitmapData(..) instead.\"")
/**
 *  \deprecated
 *      Use BMP_drawBitmap(..) instead.
 */
#define BMP_loadBitmap(bitmap, x, y, loadpal)                   _Pragma("GCC error \"This method is deprecated, use BMP_drawBitmap(..) instead.\"")
/**
 *  \deprecated
 *      Use BMP_drawBitmapEx(..) instead.
 */
#define BMP_loadAndScaleBitmap(bitmap, x, y, w, h, loadpal)     _Pragma("GCC error \"This method is deprecated, use BMP_drawBitmapScaled(..) instead.\"")

/**
 *  \deprecated
 *      Uses bitmap->palette instead.
 */
#define BMP_getBitmapPalette(bitmap, pal)                       _Pragma("GCC error \"This method is deprecated, use bitmap->palette instead.\"")

/**
 *  \brief
 *      Scale the specified source bitmap image to specified dimension.<br>
 *
 *      Take a source bitmap with its specified dimension and scale it in the<br>
 *      destination buffer with specified dimension.<br>
 *
 *  \param src_buf
 *      source bitmap buffer.
 *  \param src_wb
 *      source width in byte.
 *  \param src_h
 *      source height.
 *  \param src_pitch
 *      source pitch (number of bytes per scanline).
 *  \param dst_buf
 *      destination bitmap buffer.
 *  \param dst_wb
 *      destination width in byte.
 *  \param dst_h
 *      destination height.
 *  \param dst_pitch
 *      destination pitch (number of bytes per scanline).
 */
void BMP_scale(const u8 *src_buf, u16 src_wb, u16 src_h, u16 src_pitch, u8 *dst_buf, u16 dst_wb, u16 dst_h, u16 dst_pitch);


#endif // _BMP_H_
````

## File: inc/config.h
````c
/**
 *  \file config.h
 *  \brief Basic SGDK library configuration file
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit is used to define some specific compilation option of the library.
 */

#ifndef _CONFIG_
#define _CONFIG_


/**
 *  \brief
 *      Log disable
 */
#define LOG_LEVEL_DISABLE       0
/**
 *  \brief
 *      Log for error only
 */
#define LOG_LEVEL_ERROR         1
/**
 *  \brief
 *      Log for error and warning
 */
#define LOG_LEVEL_WARNING       2
/**
 *  \brief
 *      Log for error, warning and info (as memory allocation)
 */
#define LOG_LEVEL_INFO          3

/**
 *  \brief
 *      Define library log level (for debug build)
 */
#define LIB_LOG_LEVEL           LOG_LEVEL_ERROR

/**
 *  \brief
 *      Set it to 1 to enable KDebug logging (Gens KMod) to log some errors (as memory allocation).
 */
#if (DEBUG != 0)
    #define LIB_DEBUG           1
#else
    #define LIB_DEBUG           0
    #undef LIB_LOG_LEVEL
    #define LIB_LOG_LEVEL       LOG_LEVEL_DISABLE
#endif

/**
 *  \brief
 *      Set it to 1 if you want to force Z80 halt during DMA operation (default).<br>
 *      Some Megadrive models need it to prevent some possible DMA transfer corruption or even 68000 memory or Z80 invalid data fetch in very rare case.<br>
 *      This actually happen when Z80 access the main BUS exactly at same time you trigger a DMA operation.<br>
 *      If you are 100% sure that you are actually avoiding that case you may try to disable the flag (at your own risk though).
 */
#define HALT_Z80_ON_DMA         1

/**
 *  \brief
 *      Set it to 1 if you want to force Z80 halt during IO port (controller) accesses.<br>
 *      Some Megadrive models (as some MD2) need it to prevent some possible (but very rare) Z80 corruption bugs
 *      (may happen when Z80 access the main BUS during IO port access from 68K).
 */
#define HALT_Z80_ON_IO          1

/**
 *  \brief
 *      Set it to 1 if you want to completely disable DMA transfers (for testing purpose) and replace them with (slower) software CPU copy.
 */
#define DMA_DISABLED            0

/**
 *  \brief
 *      Set it to 1 if you want to use the old sprite engine instead of the new one.<br>
 *      The old sprite engine allow to access or change the internal VDP sprite indexes, this can be useful in some situations (as sprite multiplexing).<br>
 *      The new sprite engine provide better VDP sprite usage in general allowing to display a bit more sprites in practice.
 */
#define LEGACY_SPRITE_ENGINE    0

/**
 * \brief
 *      Set it to 1 to use the original SGDK's error handling screen and vectors.<br>
 *      Otherwise error handler from the MD Debugger project is used, which supports source code symbols with "debug" build proifle and backtrace.<br>
 *
 *      The new error handler, however, completely takes over exception vectors and doesn't allow to set your own callbacks for errors (e.g. addressErrorCB).<br>
 *      If you rely on callbacks to override error handling behavior, consider using legacy system instead.
 */
#define LEGACY_ERROR_HANDLER    0

/**
 * \brief
 *      Set it to 1 to keep the original SGDK's font tileset location in VRAM (end of the tileset space).
 *      Set it to 0 to move the font tileset at the beginning of the VRAM (start of tileset space), which allows to use the full tileset space
 *      for your game assets and avoid some VRAM fragmentation issues.
 *
 *      @see TILE_FONT_INDEX definition
 */
#define LEGACY_FONT_LOCATION    1

/**
 *  \brief
 *      Set it to 1 to enable automatic bank switch using official SEGA mapper for ROM > 4MB.
 *
 *      When automatic bank switch is enabled all internal BIN data structures declared in .far_rodata section
 *      will be accessed using BANK_getFarData(..) method (mapper.c). That may impact performance quite a bit
 *      it's why it's disabled by default if you don't require bank switch.
 */
#define ENABLE_BANK_SWITCH      0

/**
 *  \brief
 *      Set it to 1 if you want to use newlib with SGDK.<br>
 *      That will disable any standard methods from SGDK to replace them by newlib library implementation.
 */
#define ENABLE_NEWLIB           0

/**
 *  \brief
 *      Set it to 1 if you want to have the kit intro logo
 */
#define ENABLE_LOGO             0

#if (ENABLE_LOGO != 0)

/**
 *  \brief
 *      Set it to 1 if you want zoom intro logo effect instead of classic fading
 */
#define ZOOMING_LOGO            0

#endif // ENABLE_LOGO


/**
 *  \brief
 *      Set it to 1 if you want to use EVERDRIVE programming methods (written by Krikzz).
 */
#define MODULE_EVERDRIVE        0

/**
 *  \brief
 *      Set it to 1 if you want to use FAT16 methods for Everdrive cart (written by Krikzz).<br>
 *      This cost a bit more than 1 KB of RAM.
 */
#define MODULE_FAT16            0

// FAT16 need EVERDRIVE
#if ((MODULE_EVERDRIVE == 0) && (MODULE_FAT16 != 0))
#error "Cannot enable FAT16 module without EVERDRIVE module"
#endif

/**
 *  \brief
 *      Set it to 1 if you want to use PORT_2 or PORT_EXT as Serial port COMM.
 */
#define MODULE_SERIAL            0

/**
 *  \brief
 *      Set it to 1 if you want to enable MegaWiFi functions and support code (written by Jesus Alonso - doragasu)
 */
#define MODULE_MEGAWIFI         0
#if MODULE_MEGAWIFI

#define MEGAWIFI_IMPLEMENTATION_CROSS    0x01    // Cross (Serial)
#define MEGAWIFI_IMPLEMENTATION_MW_CART  0x02    // MegaWiFi Cart: Defined to use MegaWiFi Cart distributions
#define MEGAWIFI_IMPLEMENTATION_ED       0x04    // EverDrive: Defined to use EverDrive distributions (testing purposes)
#define MEGAWIFI_IMPLEMENTATION       (MEGAWIFI_IMPLEMENTATION_CROSS | MEGAWIFI_IMPLEMENTATION_ED) // Set the implementation to use
// Caution USING BOTH MW_CART AND EVERDRIVE IMPLEMENTATIONS MAY CAUSE ISSUES AS THEY BOTH USE SAME COMM VTABLE STRUCTURE
// MAKE SURE TO TEST PROPERLY IF YOU ENABLE BOTH IMPLEMENTATIONS

// Check that if using cross implementation, serial module is enabled
// Serial module is required for cross implementation
#if ((MODULE_SERIAL == 0) && (MEGAWIFI_IMPLEMENTATION & MEGAWIFI_IMPLEMENTATION_CROSS))
#error "Cannot enable MegaWiFi cross implementation without SERIAL module"
#endif
// Check that if using EverDrive implementation, EverDrive module is enabled
// Switching banks is required for EverDrive implementation
#if ((ENABLE_BANK_SWITCH == 0) && (MEGAWIFI_IMPLEMENTATION & MEGAWIFI_IMPLEMENTATION_ED))
#error "Cannot enable MegaWiFi module without BANK SWITCH"
#endif

#endif // MODULE_MEGAWIFI
/**
 *  \brief
 *      Set it to 1 if you want to enable Flash Save functions (written by Jesus Alonso - doragasu).<br>
 *      There is no reason to disable it as it doesn't consume extra memory
 */
#define MODULE_FLASHSAVE        1

/**
 *  \brief
 *      Set it to 1 if you want to enable the TTY text console module (written by Andreas Dietrich).<br>
 *      It consume about 28 bytes of memory when enabled.
 */
#define MODULE_CONSOLE          1

/**
 *  \brief
 *      Set it to 1 if you want to enable Link Cable Protocol functions (written by BlodTor)
 */
#define MODULE_LINK_CABLE       0

#endif // _CONFIG_
````

## File: inc/dma.h
````c
/**
 *  \file dma.h
 *  \brief DMA support
 *  \author Stephane Dallongeville
 *  \date 04/2015
 *
 * This unit provides methods to use the hardware DMA capabilities.
 */

#include "config.h"
#include "types.h"

#ifndef _DMA_H_
#define _DMA_H_


/**
 *  \brief
 *      VRAM destination for DMA operation.
 */
#define DMA_VRAM    0
/**
 *  \brief
 *      CRAM destination for DMA operation.
 */
#define DMA_CRAM    1
/**
 *  \brief
 *      VSRAM destination for DMA operation.
 */
#define DMA_VSRAM   2

#define DMA_QUEUE_SIZE_DEFAULT          80
#define DMA_QUEUE_SIZE_MIN              32

#define DMA_TRANSFER_CAPACITY_NTSC      7200
#define DMA_TRANSFER_CAPACITY_PAL_LOW   8000
#define DMA_TRANSFER_CAPACITY_PAL_MAX   15000

// better to allocate a bit more than DMA capacity
#define DMA_BUFFER_SIZE_NTSC            8192
#define DMA_BUFFER_SIZE_PAL_LOW         8192
#define DMA_BUFFER_SIZE_PAL_MAX         (14 * 1024)
#define DMA_BUFFER_SIZE_MIN             (2 * 1024)


/**
 *  \brief
 *      VRAM transfer method
 */
typedef enum
{
    CPU = 0,            /**< Transfer through the CPU immediately (slower.. useful for testing purpose mainly) */
    DMA = 1,            /**< Transfer through DMA immediately, using DMA is faster but can lock Z80 execution */
    DMA_QUEUE = 2,      /**< Put in the DMA queue so it will be transferred at next VBlank. Using DMA is faster but can lock Z80 execution */
    DMA_QUEUE_COPY = 3  /**< Copy the buffer and put in the DMA queue so it will be transferred at next VBlank. Using DMA is faster but can lock Z80 execution */
} TransferMethod;


/**
 *  \brief
 *      DMA transfer definition (used for DMA queue)
 */
typedef struct
{
    u16 regLenL;        // (newLen & 0xFF) | 0x9300;
    u16 regLenH;        // ((newLen >> 8) & 0xFF) | 0x9400;
    u32 regAddrMStep;   // (((addr << 7) & 0xFF0000) | 0x96008F00) + step;
    u32 regAddrHAddrL;  // ((addr >> 1) & 0x7F00FF) | 0x97009500;
    u32 regCtrlWrite;   // VDP_DMA_VRAMCOPY_ADDR(to)
} DMAOpInfo;


/**
 *  \brief
 *      DMA queue structure
 */
extern DMAOpInfo *dmaQueues;
/**
 *  \brief
 *      DMA queue structure
 */
extern u16* dmaDataBuffer;

/**
 *  \brief
 *      Initialize the DMA queue sub system with default settings.
 *
 *      SGDK automatically call this method on hard reset so you don't need to call it again unless
 *      you want to change the default parameters in which casse you should use #DMA_initEx(..)
 *
 * \see #DMA_initEx(..)
 */
void DMA_init(void);
/**
 *  \brief
 *      Initialize the DMA queue sub system.
 *
 *  \param size
 *      The queue size (0 = default size = 64, min size = 20).
 *  \param capacity
 *      The maximum allowed size (in bytes) to transfer per #DMA_flushQueue() call (0 = default = no limit).<br>
 *      Depending the current selected strategy, furthers transfers can be ignored (by default all transfers are done whatever is the limit).
 *      See the #DMA_setIgnoreOverCapacity(..) method to change the strategy to adopt when capacity limit is reached.
 *  \param bufferSize
 *      Size of the buffer (in bytes) to store temporary data that will be transferred through the DMA queue (0 = default size = 8192 on NTSC and 14336 on PAL).<br>
 *      The buffer should be big enough to contains all temporary that you need to store before next #DMA_flushQueue() call.
 *
 *      SGDK automatically call this method on hard reset so you don't need to call it again unless
 *      you want to change the default parameters.
 *
 * \see #DMA_setIgnoreOverCapacity(..)
 */
void DMA_initEx(u16 size, u16 capacity, u16 bufferSize);

/**
 *  \brief
 *      Returns TRUE if the DMA_flushQueue() method is automatically called at VBlank to process DMA operations
 *      pending in the queue.
 *
 *  \see DMA_setAutoFlush()
 *  \see DMA_flushQueue()
 */
bool DMA_getAutoFlush(void);
/**
 *  \brief
 *      If set to TRUE (default) then the DMA_flushQueue() method is automatically called at VBlank
 *      to process DMA operations pending in the queue otherwise you have to call the DMA_flushQueue()
 *      method by yourself.
 *
 *  \see DMA_flushQueue()
 */
void DMA_setAutoFlush(bool value);
/**
 *  \brief
 *      Returns the maximum allowed number of pending transfer in the queue (allocated queue size).
 *
 *  \see DMA_setMaxQueueSize()
 */
u16 DMA_getMaxQueueSize(void);
/**
 *  \brief
 *      Sets the maximum allowed number of pending transfer in the queue (allocated queue size).<br>
 *      <b>WARNING:</b> changing the queue size will clear the DMA queue.
 *
 *  \param value
 *      The queue size (minimum allowed size = 20)
 *
 *  \see DMA_getMaxQueueSize()
 *  \see DMA_setMaxQueueSizeToDefault()
 */
void DMA_setMaxQueueSize(u16 value);
/**
 *  \brief
 *      Sets the maximum allowed number of pending transfer in the queue (allocated queue size) to default value (64).<br>
 *      <b>WARNING:</b> changing the queue size will clear the DMA queue.
 *
 *  \see DMA_setMaxQueueSize(..)
 */
void DMA_setMaxQueueSizeToDefault(void);
/**
 *  \brief
 *      Returns the maximum allowed size (in bytes) to transfer per #DMA_flushQueue() call.<br>
 *      A value of 0 means there is no DMA limit.
 *
 *  \see DMA_setMaxTransferSize(..)
 */
u16 DMA_getMaxTransferSize(void);
/**
 *  \brief
 *      Sets the maximum amount of data (in bytes) to transfer per #DMA_flushQueue() call.<br>
 *      VBlank period allows to transfer up to 7.2 KB on NTSC system and 15 KB on PAL system.<br>
 *      By default there is no size limit (0)
 *
 *  \param value
 *      The maximum amount of data (in bytes) to transfer during DMA_flushQueue() operation.<br>
 *      Use <b>0</b> for no limit.
 *
 *  \see DMA_flushQueue(void)
 */
void DMA_setMaxTransferSize(u16 value);
/**
 *  \brief
 *      Sets the maximum amount of data (in bytes) to default value (7.2 KB on NTSC system and 15 KB on PAL system).
 *
 *  \see DMA_setMaxTransferSize()
 */
void DMA_setMaxTransferSizeToDefault(void);
/**
 *  \brief
 *      Returns the size (in bytes) of the temporary data buffer which can be used to store data
 *      that will be transferred through the DMA queue.
 *
 *  \see DMA_setBufferSize(void)
 */
u16 DMA_getBufferSize(void);
/**
 *  \brief
 *      Sets the size (in bytes) of the temporary data buffer which can be used to store data
 *      that will be transferred through the DMA queue.<br>
 *      <b>WARNING:</b> changing the buffer size will clear the DMA queue.
 *
 *  \param value
 *      The size of the temporary data buffer (in bytes).<br>
 *      Minimum allowed buffer size if 2048 (internals methods require a minimal buffer size)
 *
 *  \see DMA_getBufferSize(void)
 *  \see DMA_setBufferSizeToDefault(void)
 */
void DMA_setBufferSize(u16 value);
/**
 *  \brief
 *      Sets the size (in bytes) of the temporary data buffer to default value (8 KB on NTSC system and 14 KB on PAL system).
 *
 *  \see DMA_setBufferSize(void)
 */
void DMA_setBufferSizeToDefault(void);
/**
 *  \brief
 *      Return TRUE means that we ignore future DMA operation when we reach the maximum capacity (see #DMA_setIgnoreOverCapacity(..) method).
 *
 *  \see DMA_setIgnoreOverCapacity(void)
 */
bool DMA_getIgnoreOverCapacity(void);
/**
 *  \brief
 *      Set the "over capacity" DMA queue strategy (default is FALSE).
 *
 *      When set to <i>TRUE</i> any DMA operation queued after we reached the maximum defined transfer capacity
 *      with #DMA_setMaxTransferSize(..) are ignored.<br>
 *      When set to <i>FALSE</i> all DMA operations are done even when we are over the maximum capacity (which can lead to important slowdown).
 *
 *  \see DMA_setMaxTransferSize()
 */
void DMA_setIgnoreOverCapacity(bool value);

/**
 *  \brief
 *      Clears the DMA queue (all queued operations are lost).<br>
 *  \see DMA_flushQueue(void)
 */
void DMA_clearQueue(void);
/**
 *  \brief
 *      Transfer the content of the DMA queue to the VDP:<br>
 *      Each pending DMA operation is sent to the VDP and processed as quickly as possible.<br>
 *      This method returns when all DMA operations present in the queue has been transferred (or when maximum capacity has been reached).<br>
 *      Note that this method is automatically called at VBlank time and you shouldn't call yourself except if
 *      you want to process it before vblank (if you manually extend blank period with h-int for instance) in which case
 *      you can disable the auto flush feature (see DMA_setAutoFlush(...))
 *
 *  \see DMA_queue(...)
 *  \see DMA_setAutoFlush(...)
 */
void DMA_flushQueue(void);

/**
 *  \brief
 *      Returns the number of transfer currently pending in the DMA queue.
 */
u16 DMA_getQueueSize(void);
/**
 *  \brief
 *      Returns the size (in byte) of data to be transferred currently present in the DMA queue.<br>
 *      NTSC frame allows about 7.6 KB of data to be transferred during VBlank (in H40) while
 *      PAL frame allows about 17 KB (in H40).
 */
u16 DMA_getQueueTransferSize(void);

/**
 *  \brief
 *      Tool method allowing to allocate memory from the DMA temporary buffer if you need a very temporary buffer.<br>
 *      Don't forget to release memory using #DMA_releaseTemp(..).<br>
 *      <b>WARNING:</b> it's very important to disable interrupts while using the temporary DMA buffer as DMA buffer can be flushed on interrupt.
 *
 *  \param len
 *      Number of word to allocate.
 *  \return
 *      The source buffer pointer if allocation succeeded,.<br>
 *      Returns NULL if the buffer is full (or not big enough).
 *  \see DMA_releaseTemp(..)
 */
void* DMA_allocateTemp(u16 len);
/**
 *  \brief
 *      Tool method allowing to release memory previously allocated using #DMA_allocateTemp(..).<br>
 *      <b>WARNING:</b> it's very important to disable interrupts while using the temporary DMA buffer as DMA buffer can be flushed on interrupt.
 *
 *  \param len
 *      Number of word to release.
 *  \see DMA_allocateTemp(..)
 */
void DMA_releaseTemp(u16 len);

/**
 *  \brief
 *      General method to transfer data to VDP memory.
 *
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *  \param location
 *      Destination location.<br>
 *      Accepted values:<br>
 *      - DMA_VRAM (for VRAM transfer).<br>
 *      - DMA_CRAM (for CRAM transfer).<br>
 *      - DMA_VSRAM (for VSRAM transfer).<br>
 *  \param from
 *      Source buffer.
 *  \param to
 *      VRAM/CRAM/VSRAM destination address.
 *  \param len
 *      Number of word to allocate and transfer.
 *  \param step
 *      destination (VRAM/VSRAM/CRAM) address increment step after each write (0 to 255).<br>
 *      By default you should set it to 2 for normal copy operation but you can use different value
 *      for specific operation.<br>
 *  \return
 *      TRUE if the operation succeeded, FALSE otherwise (buffer or queue full).
 *  \see DMA_queueDMA(..)
 */
bool DMA_transfer(TransferMethod tm, u8 location, void* from, u16 to, u16 len, u16 step);

/**
 *  \brief
 *      Return TRUE if we have enough DMA capacity to transfer the given data block len (see #DMA_setMaxTransferSize(..))
 *
 *  \param location
 *      Destination location.<br>
 *      Accepted values:<br>
 *      - DMA_VRAM (for VRAM transfer).<br>
 *      - DMA_CRAM (for CRAM transfer).<br>
 *      - DMA_VSRAM (for VSRAM transfer).<br>
 *  \param len
 *      Number of word we want to transfer.
 *  \return
 *      TRUE if we have enough capacity, FALSE otherwise
 *  \see DMA_getMaxTransferSize(..)
 *  \see DMA_queueDma(..)
 */
bool DMA_canQueue(u8 location, u16 len);
/**
 *  \brief
 *      Allocate temporary memory and queues the DMA transfer operation in the DMA queue.<br>
 *      The idea of the DMA queue is to burst all DMA operations during VBLank to maximize bandwidth usage.<br>
 *      <b>IMPORTANT:</b> You need to fill the returned data buffer before DMA occurs so it's a good practise to disable interrupts before
 *      calling this method and re-enable them *after* you filled the buffer to avoid DMA queue flush operation happening in between.
 *
 *  \param location
 *      Destination location.<br>
 *      Accepted values:<br>
 *      - DMA_VRAM (for VRAM transfer).<br>
 *      - DMA_CRAM (for CRAM transfer).<br>
 *      - DMA_VSRAM (for VSRAM transfer).<br>
 *  \param to
 *      VRAM/CRAM/VSRAM destination address.
 *  \param len
 *      Number of word to allocate and transfer.
 *  \param step
 *      destination (VRAM/VSRAM/CRAM) address increment step after each write (0 to 255).<br>
 *      By default you should set it to 2 for normal copy operation but you can use different value
 *      for specific operation.<br>
 *  \return
 *      The source buffer pointer that will be used for the DMA transfer so you can fill its content.<br>
 *      Returns NULL if the buffer is full or if the DMA queue operation failed (queue is full).
 *  \see DMA_queueDMA(..)
 *  \see DMA_copyAndQueueDma(..)
 */
void* DMA_allocateAndQueueDma(u8 location, u16 to, u16 len, u16 step);
/**
 *  \brief
 *      Same as #DMA_queueDma(..) method except that it first copies the data to transfer through DMA queue into a temporary buffer.<br>
 *      This is useful when you know that source data may be modified before DMA acutally occurs and want to avoid that.
 *
 *  \param location
 *      Destination location.<br>
 *      Accepted values:<br>
 *      - DMA_VRAM (for VRAM transfer).<br>
 *      - DMA_CRAM (for CRAM transfer).<br>
 *      - DMA_VSRAM (for VSRAM transfer).<br>
 *  \param from
 *      Source buffer.
 *  \param to
 *      VRAM/CRAM/VSRAM destination address.
 *  \param len
 *      Number of word to transfer.
 *  \param step
 *      destination (VRAM/VSRAM/CRAM) address increment step after each write (0 to 255).<br>
 *      By default you should set it to 2 for normal copy operation but you can use different value
 *      for specific operation.<br>
 *  \return
 *      FALSE if the operation failed (queue is full)
 *  \see DMA_doDma(..)
 *  \see DMA_queueDma(..)
 */
bool DMA_copyAndQueueDma(u8 location, void* from, u16 to, u16 len, u16 step);
/**
 *  \brief
 *      Queues the specified DMA transfer operation in the DMA queue.<br>
 *      The idea of the DMA queue is to burst all DMA operations during VBLank to maximize bandwidth usage.<br>
 *
 *  \param location
 *      Destination location.<br>
 *      Accepted values:<br>
 *      - DMA_VRAM (for VRAM transfer).<br>
 *      - DMA_CRAM (for CRAM transfer).<br>
 *      - DMA_VSRAM (for VSRAM transfer).<br>
 *  \param from
 *      Source buffer address.
 *  \param to
 *      VRAM/CRAM/VSRAM destination address.
 *  \param len
 *      Number of word to transfer.
 *  \param step
 *      destination (VRAM/VSRAM/CRAM) address increment step after each write (0 to 255).<br>
 *      By default you should set it to 2 for normal copy operation but you can use different value
 *      for specific operation.<br>
 *  \return
 *      FALSE if the operation failed (queue is full)
 *  \see DMA_doDma(..)
 */
bool DMA_queueDma(u8 location, void* from, u16 to, u16 len, u16 step);
/**
 *  \brief
 *      Same as #DMA_queueDma(..) method except if doesn't check for 128 KB bank crossing on source
 *  \see #DMA_queueDma(..)
 */
bool DMA_queueDmaFast(u8 location, void* from, u16 to, u16 len, u16 step);

/**
 *  \brief
 *      Do DMA transfer operation immediately
 *
 *  \param location
 *      Destination location.<br>
 *      Accepted values:<br>
 *      - DMA_VRAM (for VRAM transfer).<br>
 *      - DMA_CRAM (for CRAM transfer).<br>
 *      - DMA_VSRAM (for VSRAM transfer).<br>
 *  \param from
 *      Source buffer address.
 *  \param to
 *      VRAM/CRAM/VSRAM destination address.
 *  \param len
 *      Number of word to transfer.
 *  \param step
 *      destination (VRAM/VSRAM/CRAM) address increment step after each write (-1 to keep current step).<br>
 *      By default you should set it to 2 for normal copy operation but you can use different value
 *      for specific operation.
 *  \see DMA_queue(..)
 */
void DMA_doDma(u8 location, void* from, u16 to, u16 len, s16 step);
/**
 *  \brief
 *      Same as #DMA_doDma(..) method except if doesn't check for 128 KB bank crossing on source
 *  \see #DMA_doDma(..)
 */
void DMA_doDmaFast(u8 location, void* from, u16 to, u16 len, s16 step);

/**
 *  \brief
 *      Do software (CPU) copy to VDP memory (mainly for testing purpose as it's slower than using DMA)
 *
 *  \param location
 *      Destination location.<br>
 *      Accepted values:<br>
 *      - DMA_VRAM (for VRAM transfer).<br>
 *      - DMA_CRAM (for CRAM transfer).<br>
 *      - DMA_VSRAM (for VSRAM transfer).<br>
 *  \param from
 *      Source buffer.
 *  \param to
 *      VRAM/CRAM/VSRAM destination address.
 *  \param len
 *      Number of word to transfer.
 *  \param step
 *      destination (VRAM/VSRAM/CRAM) address increment step after each write (-1 to keep current step).<br>
 *      By default you should set it to 2 for normal copy operation but you can use different value
 *      for specific operation.
 */
void DMA_doCPUCopy(u8 location, void* from, u16 to, u16 len, s16 step);
/**
 *  \brief
 *      Do software (CPU) copy to VDP memory (mainly for testing purpose as it's slower than using DMA)
 *
 *  \param cmd
 *      VDP packed control command (contains operation and destination address).
 *  \param from
 *      Source buffer.
 *  \param len
 *      Number of word to transfer.
 *  \param step
 *      destination (VRAM/VSRAM/CRAM) address increment step after each write (-1 to keep current step).<br>
 *      By default you should set it to 2 for normal copy operation but you can use different value
 *      for specific operation.
 */
void DMA_doCPUCopyDirect(u32 cmd, void* from, u16 len, s16 step);

/**
 *  \brief
 *      Do VRAM DMA fill operation.
 *
 *  \param to
 *      VRAM destination address.
 *  \param len
 *      Number of byte to fill (minimum is 2 for even addr destination and 3 for odd addr destination).<br>
 *      A value of 0 mean 0x10000.
 *  \param value
 *      Fill value (byte).
 *  \param step
 *      VRAM address increment step after each write (-1 to keep current step).<br>
 *      should be 1 for a classic fill operation but you can use different value for specific operation.
 */
void DMA_doVRamFill(u16 to, u16 len, u8 value, s16 step);
/**
 *  \brief
 *      Do VRAM DMA copy operation.

 *  \param from
 *      VRAM Source address.
 *  \param to
 *      VRAM destination address.
 *  \param len
 *      Number of byte to copy.
 *  \param step
 *      VRAM address increment step after each write (-1 to keep current step).<br>
 *      should be 1 for a classic copy operation but you can use different value for specific operation.
 */
void DMA_doVRamCopy(u16 from, u16 to, u16 len, s16 step);

/**
 *  \brief
 *      Wait current DMA fill/copy operation to complete (same as #VDP_waitDMACompletion())
 */
void DMA_waitCompletion(void);


#endif // _DMA_H_
````

## File: inc/genesis.h
````c
#ifndef _GENESIS_H_
#define _GENESIS_H_

#define SGDK_VERSION    2.12

#include "types.h"

#define SGDK            TRUE

#include "config.h"
#include "asm.h"

#include "sys.h"
#include "sram.h"
#include "mapper.h"
#include "memory.h"
#include "tools.h"

#include "pool.h"
#include "object.h"

#include "string.h"

#include "tab_cnv.h"

#include "maths.h"
#include "maths3D.h"

#include "vdp.h"
#include "vdp_bg.h"
#include "vdp_spr.h"
#include "vdp_tile.h"
#include "vdp_pal.h"
#include "pal.h"
#include "vram.h"
#include "dma.h"
#include "map.h"
#include "bmp.h"
#include "sprite_eng.h"
#include "sprite_eng_legacy.h"

#include "z80_ctrl.h"
#include "ym2612.h"
#include "psg.h"

#include "snd/sound.h"
#include "snd/xgm.h"
#include "snd/xgm2.h"
#include "snd/smp_null.h"
#include "snd/smp_null_dpcm.h"
#include "snd/pcm/snd_pcm.h"
#include "snd/pcm/snd_dpcm2.h"
#include "snd/pcm/snd_pcm4.h"

#include "joy.h"
#include "timer.h"

#include "task.h"

// modules
#if (MODULE_EVERDRIVE != 0)
#include "ext/everdrive.h"
#endif

#if (MODULE_FAT16 != 0)
#include "ext/fat16.h"
#endif

#if (MODULE_MEGAWIFI != 0)
#include "ext/mw/megawifi.h"
#endif

#if (MODULE_FLASHSAVE != 0)
#include "ext/flash-save/flash.h"
#include "ext/flash-save/saveman.h"
#endif

#if (MODULE_CONSOLE != 0)
#include "ext/console.h"
#endif

#if (MODULE_LINK_CABLE != 0)
#include "ext/link_cable.h"
#endif

// preserve compatibility with old resources name
#define logo_lib sgdk_logo
#define font_lib font_default
#define font_pal_default palette_grey
#define font_pal_lib font_pal_default

#endif // _GENESIS_H_
````

## File: inc/joy.h
````c
/**
 *  \file joy.h
 *  \brief General controller support.
 *  \author Chilly Willy
 *  \author Stephane Dallongeville
 *  \date 05/2012
 *
 * This unit provides methods to read controller state.<br>
 *<br>
 * Here is the list of supported controller device:<br>
 * - 3 buttons joypad<br>
 * - 6 buttons joypad<br>
 * - Sega Mouse<br>
 * - Sega Team Player adapter<br>
 * - EA 4-Way Play<br>
 * - the Menacer<br>
 * - the Justifier<br>
 * - Sega Master System pad<br>
 * - Sega Trackball<br>
 * - Sega Phaser<br>
 *
 * Although Sega Master System pad, trackball, and Phaser are supported, they aren't automatically detected on JOY_init().<br>
 * Another caveat is that although the Menacer and Justifier are automatically recognized and supported, the support is not enabled
 * until the programmer tells it to because of the extra overhead that lightguns add. This way, SGDK defaults to a lower overhead state for controllers.
 * Games that can use lightguns can check and enable the lightgun as part of the init process, while normal games
 * can ignore that and simply count on the default init state for controllers.<br>
 * Note that mice are enabled by default by JOY_init() and will return emulated pad values for reading the port as if it were a controller.<br>
 * This allows using a mouse as a pad for SGDK games. However, mice use a little more overhead than pads, so a game that doesn't use mice
 * as mice and wants every last scrap of speed should check if mice are connected and turn them off to get just that extra little bit of speed.<br>
 * If you have an existing SGDK based game that needs every last bit of speed and doesn't check for mice, unplug any mice for extra speed. Mice are only enabled if detected.
 */

#ifndef _JOY_H_
#define _JOY_H_


#define PORT_1          0x0000
#define PORT_2          0x0001


#define JOY_1           0x0000
#define JOY_2           0x0001
#define JOY_3           0x0002
#define JOY_4           0x0003
#define JOY_5           0x0004
#define JOY_6           0x0005
#define JOY_7           0x0006
#define JOY_8           0x0007
#define JOY_NUM         0x0008
#define JOY_ALL         0xFFFF


#define BUTTON_UP       0x0001
#define BUTTON_DOWN     0x0002
#define BUTTON_LEFT     0x0004
#define BUTTON_RIGHT    0x0008
#define BUTTON_A        0x0040
#define BUTTON_B        0x0010
#define BUTTON_C        0x0020
#define BUTTON_START    0x0080
#define BUTTON_X        0x0400
#define BUTTON_Y        0x0200
#define BUTTON_Z        0x0100
#define BUTTON_MODE     0x0800

#define BUTTON_LMB      0x0040
#define BUTTON_MMB      0x0010
#define BUTTON_RMB      0x0020

#define BUTTON_DIR      0x000F
#define BUTTON_BTN      0x0FF0
#define BUTTON_ALL      0x0FFF

#define JOY_TYPE_PAD3           0x00
#define JOY_TYPE_PAD6           0x01
#define JOY_TYPE_MOUSE          0x02
#define JOY_TYPE_TRACKBALL      0x03
#define JOY_TYPE_MENACER        0x04
#define JOY_TYPE_JUSTIFIER      0x05
#define JOY_TYPE_PHASER         0x06
#define JOY_TYPE_UNKNOWN        0x0F

#define PORT_TYPE_MENACER       0x00
#define PORT_TYPE_JUSTIFIER     0x01
#define PORT_TYPE_MOUSE         0x03
#define PORT_TYPE_TEAMPLAYER    0x07
#define PORT_TYPE_PAD           0x0D
#define PORT_TYPE_UNKNOWN       0x0F
#define PORT_TYPE_EA4WAYPLAY    0x10

#define JOY_SUPPORT_OFF             0x00
#define JOY_SUPPORT_3BTN            0x01
#define JOY_SUPPORT_6BTN            0x02
#define JOY_SUPPORT_MOUSE           0x03
#define JOY_SUPPORT_TRACKBALL       0x04
#define JOY_SUPPORT_TEAMPLAYER      0x05
#define JOY_SUPPORT_EA4WAYPLAY      0x06
#define JOY_SUPPORT_MENACER         0x07
#define JOY_SUPPORT_JUSTIFIER_BLUE  0x08
#define JOY_SUPPORT_JUSTIFIER_BOTH  0x09
#define JOY_SUPPORT_PHASER          0x0A
#define JOY_SUPPORT_ANALOGJOY       0x0B
#define JOY_SUPPORT_KEYBOARD        0x0C


/**
 *  \brief
 *      Joypad event callback.
 *
 *  \param joy
 *      Joypad which generated the event.<br>
 *      <b>JOY_1</b>    = joypad 1<br>
 *      <b>JOY_2</b>    = joypad 2<br>
 *      <b>...  </b>    = ...<br>
 *      <b>JOY_8</b>    = joypad 8 (only possible with 2 teamplayers connected)<br>
 *      <b>JOY_ALL</b>  = joypad 1 | joypad 2 | ... | joypad 8<br>
 *  \param change
 *      Changed state (button for which state changed).<br>
 *      <b>BUTTON_UP</b>    = UP button<br>
 *      <b>BUTTON_DOWN</b>  = DOWN button<br>
 *      <b>BUTTON_LEFT</b>  = LEFT button<br>
 *      <b>BUTTON_RIGHT</b> = RIGHT button<br>
 *      <b>BUTTON_A</b>     = A button<br>
 *      <b>BUTTON_B</b>     = B button<br>
 *      <b>BUTTON_C</b>     = C button<br>
 *      <b>BUTTON_START</b> = START button<br>
 *      <b>BUTTON_X</b>     = X button<br>
 *      <b>BUTTON_Y</b>     = Y button<br>
 *      <b>BUTTON_Z</b>     = Z button<br>
 *      <b>BUTTON_MODE</b>  = MODE button<br>
 *      <b>BUTTON_LMB</b>   = Alias for A button for mouse<br>
 *      <b>BUTTON_MMB</b>   = Alias for B button for mouse<br>
 *      <b>BUTTON_RMC</b>   = Alias for C button for mouse<br>
 *  \param state
 *      Current joypad state.<br>
 *<br>
 *      Ex: Test if button START on joypad 1 just get pressed:<br>
 *      joy = JOY_1; changed = BUTTON_START; state = BUTTON_START | (previous state)
 */
typedef void JoyEventCallback(u16 joy, u16 changed, u16 state);


/**
 *  \brief
 *      Initialize the controller sub system.<br>
 *
 *      Software and hardware controller port initialization (reset and devices detection).<br>
 *      Automatically called at SGDK initialization, no need to call it manually.
 */
void JOY_init(void);

/**
 *  \brief
 *      Reset the controller sub system.<br>
 *
 *      It will reset the controller port state and perform device detectionSoftware and hardware controller port initialization.<br>
 *      Automatically called at SGDK initialization, no need to call it manually.
 */
void JOY_reset(void);

/**
 *  \brief
 *      Returns the current set callback function for controller state change.<br>
 *<br>
 *      SGDK provides an event system to detect state change on controller.<br>
 *      It internally updates controllers state at each V Blank period and fire an event if a state change is detected.<br>
 *
 *  \return
 *      The current set callback method on joy state change.<br>
 */
JoyEventCallback* JOY_getEventHandler();
/**
 *  \brief
 *      Set the callback function for controller state changed.<br>
 *<br>
 *      SGDK provides an event system to detect state change on controller.<br>
 *      It internally updates controllers state at each V Blank period and fire an event if a state change is detected.<br>
 *
 *  \param CB
 *      Callback to call when controller(s) state changed.<br>
 *      The function prototype should reply to _joyEventCallback type :<br>
 *      void function(u16 joy, u16 changed, u16 state);<br>
 *<br>
 *      <b>Ex 1</b> : if player 1 just pressed START button you receive :<br>
 *      joy = JOY_1, changed = BUTTON_START, state = BUTTON_START<br>
 *      <b>Ex 2</b> : if player 2 just released the A button you receive :<br>
 *      joy = JOY_2, changed = BUTTON_A, state = 0<br>
 */
void JOY_setEventHandler(JoyEventCallback *CB);
/**
 *  \brief
 *      Set peripheral support for the specified port.<br>
 *<br>
 *      By default ports are configured to only enable support for joypads, unless<br>
 *      a pad is not detected. In that case, a multitap or mouse is enabled if<br>
 *      present.<br>
 *<br>
 *  \param port
 *      Port we want to set support.<br>
 *      <b>PORT_1</b>   = port 1<br>
 *      <b>PORT_2</b>   = port 2<br>
 *  \param support
 *      Peripheral support.<br>
 *      <b>JOY_SUPPORT_OFF</b>            = No peripheral support<br>
 *      <b>JOY_SUPPORT_3BTN</b>           = 3 button joypad<br>
 *      <b>JOY_SUPPORT_6BTN</b>           = 6 button joypad<br>
 *      <b>JOY_SUPPORT_TRACKBALL</b>      = Sega Sports Pad (SMS trackball)<br>
 *      <b>JOY_SUPPORT_MOUSE</b>          = Sega MegaMouse<br>
 *      <b>JOY_SUPPORT_TEAMPLAYER</b>     = Sega TeamPlayer<br>
 *      <b>JOY_SUPPORT_EA4WAYPLAY</b>     = EA 4-Way Play<br>
 *      <b>JOY_SUPPORT_MENACER</b>        = Sega Menacer<br>
 *      <b>JOY_SUPPORT_JUSTIFIER_BLUE</b> = Konami Justifier (blue gun only)<br>
 *      <b>JOY_SUPPORT_JUSTIFIER_BOTH</b> = Konami Justifier (both guns)<br>
 *      <b>JOY_SUPPORT_ANALOGJOY</b>      = Sega analog joypad (not yet supported)<br>
 *      <b>JOY_SUPPORT_KEYBOARD</b>       = Sega keyboard (not yet supported)<br>
 *<br>
 *      Ex: enable support for MegaMouse on second port<br>
 *      JOY_setSupport(PORT_2, JOY_SUPPORT_MOUSE);<br>
 *<br>
 */
void JOY_setSupport(u16 port, u16 support);

/**
 *  \brief
 *      Get peripheral type for the specified port.<br>
 *<br>
 *      The peripheral type for each port is automatically detected during #JOY_init() / #JOY_reset() call.<br>
 *      This function returns that type to help decide how the port support should be set.<br>
 *      Types greater than 15 are not derived via Sega's controller ID method.<br>
 *<br>
 *  \param port
 *      Port we want to get the peripheral type.<br>
 *      <b>PORT_1</b>   = port 1<br>
 *      <b>PORT_2</b>   = port 2<br>
 *  \return type
 *      Peripheral type.<br>
 *      <b>PORT_TYPE_MENACER</b>        = Sega Menacer<br>
 *      <b>PORT_TYPE_JUSTIFIER</b>      = Konami Justifier<br>
 *      <b>PORT_TYPE_MOUSE</b>          = Sega MegaMouse<br>
 *      <b>PORT_TYPE_TEAMPLAYER</b>     = Sega TeamPlayer<br>
 *      <b>PORT_TYPE_PAD</b>            = Sega joypad<br>
 *      <b>PORT_TYPE_UNKNOWN</b>        = unidentified or no peripheral<br>
 *      <b>PORT_TYPE_EA4WAYPLAY</b>     = EA 4-Way Play<br>
 *<br>
 *      Ex: get peripheral type in port 1<br>
 *      type = JOY_getPortType(PORT_1);<br>
 *<br>
 */
u8 JOY_getPortType(u16 port);

/**
 *  \brief
 *      Get joypad peripheral type connected to the specified joypad port.<br>
 *      The joypad peripheral type for each port is automatically detected during #JOY_init() or #JOY_reset() call.<br>
 *      Prefer this method over JOY_getPortType(..) when you need to get information<br>
 *      about peripheral connected to multi joypad adapter (as the Sega TeamPlayer).
 *
 *  \param joy
 *      Joypad port we query type.<br>
 *      <b>JOY_1</b>    = joypad 1<br>
 *      <b>JOY_2</b>    = joypad 2<br>
 *      <b>...  </b>    = ...<br>
 *      <b>JOY_8</b>    = joypad 8 (only possible with 2 TeamPlayer connected)<br>
 *  \return joypad peripheral type.<br>
 *      <b>JOY_TYPE_PAD3</b>        = 3 buttons joypad<br>
 *      <b>JOY_TYPE_PAD6</b>        = 6 buttons joypad<br>
 *      <b>JOY_TYPE_MOUSE</b>       = Sega Mouse<br>
 *      <b>JOY_TYPE_TRACKBALL</b>   = Sega trackball<br>
 *      <b>JOY_TYPE_MENACER</b>     = Sega Menacer gun<br>
 *      <b>JOY_TYPE_JUSTIFIER</b>   = Sega Justifier gun<br>
 *      <b>JOY_TYPE_UNKNOWN</b>     = Unknow adaptater or not connected<br>
 */
u8 JOY_getJoypadType(u16 joy);

/**
 *  \brief
 *      Get joypad state.
 *
 *  \param joy
 *      Joypad we query state.<br>
 *      <b>JOY_1</b>    = joypad 1<br>
 *      <b>JOY_2</b>    = joypad 2<br>
 *      <b>...  </b>    = ...<br>
 *      <b>JOY_8</b>    = joypad 8 (only possible with 2 teamplayers connected)<br>
 *      <b>JOY_ALL</b>  = joypad 1 | joypad 2 | ... | joypad 8<br>
 *  \return joypad state.<br>
 *      <b>BUTTON_UP</b>    = UP button<br>
 *      <b>BUTTON_DOWN</b>  = DOWN button<br>
 *      <b>BUTTON_LEFT</b>  = LEFT button<br>
 *      <b>BUTTON_RIGHT</b> = RIGHT button<br>
 *      <b>BUTTON_A</b>     = A button<br>
 *      <b>BUTTON_B</b>     = B button<br>
 *      <b>BUTTON_C</b>     = C button<br>
 *      <b>BUTTON_START</b> = START button<br>
 *      <b>BUTTON_X</b>     = X button<br>
 *      <b>BUTTON_Y</b>     = Y button<br>
 *      <b>BUTTON_Z</b>     = Z button<br>
 *      <b>BUTTON_MODE</b>  = MODE button<br>
 *<br>
 *      <b>BUTTON_LMB</b>   = Alias for A button for mouse<br>
 *      <b>BUTTON_MMB</b>   = Alias for B button for mouse<br>
 *      <b>BUTTON_RMC</b>   = Alias for C button for mouse<br>
 *<br>
 *      Ex : Test if button START or A is pressed on joypad 1 :<br>
 *      if (JOY_readJoypad(JOY_1) & (BUTTON_START | BUTTON_A))<br>
 *<br>
 */
u16  JOY_readJoypad(u16 joy);

/**
 *  \brief
 *      Get joypad X axis.
 *
 *  \param joy
 *      Joypad we query state.<br>
 *      <b>JOY_1</b>    = joypad 1<br>
 *      <b>JOY_2</b>    = joypad 2<br>
 *      <b>...  </b>    = ...<br>
 *      <b>JOY_8</b>    = joypad 8 (only possible with 2 teamplayers connected)<br>
 *  \return joypad X axis.<br>
 *      A mouse returns signed axis data. The change in this value indicates movement -<br>
 *      to the right for positive changes, or left for negative changes.<br>
 *<br>
 *      A light gun returns the unsigned screen X coordinate. This is not calibrated;<br>
 *      Calibration is left to the game to handle. The value is -1 if the gun is not<br>
 *      pointed at the screen, or the screen is too dim to detect.<br>
 *<br>
 *      Ex : Get X axis of pad 2 :<br>
 *      countX = JOY_readJoypadX(JOY_2);<br>
 *<br>
 */
s16  JOY_readJoypadX(u16 joy);

/**
 *  \brief
 *      Write joypad X axis.
 *
 *  \param joy
 *      Joypad we query state.<br>
 *      <b>JOY_1</b>    = joypad 1<br>
 *      <b>JOY_2</b>    = joypad 2<br>
 *      <b>...  </b>    = ...<br>
 *      <b>JOY_8</b>    = joypad 8 (only possible with 2 teamplayers connected)<br>
 *  \param pos
 *      Desired X position for joypad.<br>
 */
s16  JOY_writeJoypadX(u16 joy, u16 pos);

/**
 *  \brief
 *      Get joypad Y axis.
 *
 *  \param joy
 *      Joypad we query state.<br>
 *      <b>JOY_1</b>    = joypad 1<br>
 *      <b>JOY_2</b>    = joypad 2<br>
 *      <b>...  </b>    = ...<br>
 *      <b>JOY_8</b>    = joypad 8 (only possible with 2 teamplayers connected)<br>
 *  \return joypad Y axis.<br>
 *      A mouse returns signed axis data. The change in this value indicates movement -<br>
 *      upwards for positive changes, or downwards for negative changes.<br>
 *<br>
 *      A light gun returns the unsigned screen Y coordinate. This is not calibrated;<br>
 *      Calibration is left to the game to handle. The value is -1 if the gun is not<br>
 *      pointed at the screen, or the screen is too dim to detect.<br>
 *<br>
 *      Ex : Get Y axis of pad 2 :<br>
 *      countY = JOY_readJoypadY(JOY_2);<br>
 *<br>
 */
s16  JOY_readJoypadY(u16 joy);

/**
 *  \brief
 *      Write joypad Y axis.
 *
 *  \param joy
 *      Joypad we query state.<br>
 *      <b>JOY_1</b>    = joypad 1<br>
 *      <b>JOY_2</b>    = joypad 2<br>
 *      <b>...  </b>    = ...<br>
 *      <b>JOY_8</b>    = joypad 8 (only possible with 2 teamplayers connected)<br>
 *  \param pos
 *      Desired Y position for joypad.<br>
 */
s16  JOY_writeJoypadY(u16 joy, u16 pos);

/**
 *  \brief
 *      Wait until a button is pressed on any connected controller.
 */
void JOY_waitPressBtn(void);
/**
 *  \brief
 *      Wait the specified amount of time or until a button is pressed on any connected controller.
 */
u16 JOY_waitPressBtnTime(u16 ms);
/**
 *  \brief
 *      Wait for specified button to be pressed on specified joypad.
 *
 *  \param joy
 *      Joypad we want to check state (see JOY_readJoypad()).<br>
 *      You can also use JOY_ALL to check on any connected controller.
 *  \param btn
 *      button(s) we want to check state.<br>
 *      <b>BUTTON_UP</b>    = UP button<br>
 *      <b>BUTTON_DOWN</b>  = DOWN button<br>
 *      <b>BUTTON_LEFT</b>  = LEFT button<br>
 *      <b>BUTTON_RIGHT</b> = RIGHT button<br>
 *      <b>BUTTON_A</b>     = A button<br>
 *      <b>BUTTON_B</b>     = B button<br>
 *      <b>BUTTON_C</b>     = C button<br>
 *      <b>BUTTON_START</b> = START button<br>
 *      <b>BUTTON_X</b>     = X button<br>
 *      <b>BUTTON_Y</b>     = Y button<br>
 *      <b>BUTTON_Z</b>     = Z button<br>
 *      <b>BUTTON_MODE</b>  = MODE button<br>
 *      <b>BUTTON_DIR</b>   = Any of the direction buttons (UP, DOWN, LEFT or RIGHT)<br>
 *      <b>BUTTON_BTN</b>   = Any of the non direction buttons (A, B, C, START, X, Y, Z, MODE)<br>
 *      <b>BUTTON_ALL</b>   = Any of all buttons<br>
 *<br>
 *      <b>BUTTON_LMB</b>   = Alias for A button for mouse<br>
 *      <b>BUTTON_MMB</b>   = Alias for B button for mouse<br>
 *      <b>BUTTON_RMC</b>   = Alias for C button for mouse<br>
 *  \return
 *      The button actually pressed or FALSE if none of specified button has be pressed in the given time.
 *<br>
 *      Ex: if we want to wait any of direction buttons or button A is pressed on joypad 1 :<br>
 *      pressed = JOY_waitJoypad(JOY_1, BUTTON_DIR | BUTTON_A);<br>
 */
u16 JOY_waitPress(u16 joy, u16 btn);
/**
 *  \brief
 *      Wait for specified button(s) to be pressed on specified joypad.
 *
 *  \param joy
 *      Joypad we want to check state (see JOY_readJoypad()).<br>
 *      You can also use JOY_ALL to check on any connected controller.
 *  \param btn
 *      button(s) we want to check state.<br>
 *      <b>BUTTON_UP</b>    = UP button<br>
 *      <b>BUTTON_DOWN</b>  = DOWN button<br>
 *      <b>BUTTON_LEFT</b>  = LEFT button<br>
 *      <b>BUTTON_RIGHT</b> = RIGHT button<br>
 *      <b>BUTTON_A</b>     = A button<br>
 *      <b>BUTTON_B</b>     = B button<br>
 *      <b>BUTTON_C</b>     = C button<br>
 *      <b>BUTTON_START</b> = START button<br>
 *      <b>BUTTON_X</b>     = X button<br>
 *      <b>BUTTON_Y</b>     = Y button<br>
 *      <b>BUTTON_Z</b>     = Z button<br>
 *      <b>BUTTON_MODE</b>  = MODE button<br>
 *      <b>BUTTON_DIR</b>   = Any of the direction buttons (UP, DOWN, LEFT or RIGHT)<br>
 *      <b>BUTTON_BTN</b>   = Any of the non direction buttons (A, B, C, START, X, Y, Z, MODE)<br>
 *      <b>BUTTON_ALL</b>   = Any of all buttons<br>
 *<br>
 *      <b>BUTTON_LMB</b>   = Alias for A button for mouse<br>
 *      <b>BUTTON_MMB</b>   = Alias for B button for mouse<br>
 *      <b>BUTTON_RMC</b>   = Alias for C button for mouse<br>
 *  \param ms
 *      maximum time in ms to wait for the button press action (0 means wait infinitely).<br>
 *  \return
 *      The button actually pressed or FALSE if none of specified button has be pressed in the given time.
 *<br>
 *      Ex: if we want to wait a maximum of 5 secondes for any of direction buttons<br>
 *      or button A to be pressed on joypad 1:<br>
 *      pressed = JOY_waitJoypad(JOY_1, BUTTON_DIR | BUTTON_A, 5000);<br>
 */
u16 JOY_waitPressTime(u16 joy, u16 btn, u16 ms);

/**
 *  \brief
 *      Manual update joypad state.<br>
 *<br>
 *      By default the library update joypad state on V interrupt process.<br>
 *      Calling this method will force to update joypad state now.<br>
 */
void JOY_update(void);


#endif // _JOY_H_
````

## File: inc/kdebug.h
````c
/**
 *  \file kdebug.h
 *  \brief Gens KMod debug methods
 *  \author Kaneda
 *  \date XX/20XX
 *
 * This unit provides Gens KMod debug methods support.
 */

#ifndef _KDEBUG_H_
#define _KDEBUG_H_


extern void KDebug_Halt(void);
extern void KDebug_Alert(const char *str);
extern void KDebug_AlertNumber(u32 nVal);
extern void KDebug_StartTimer(void);
extern void KDebug_StopTimer(void);


#endif
````

## File: inc/map.h
````c
/**
 *  \file map.h
 *  \brief MAP (large background map) management unit
 *  \author Stephane Dallongeville
 *  \date 11/2020
 *
 * This unit provides methods to manipulate / scroll large background MAP:<br>
 * - Create / release MAP object<br>
 * - MAP decoding functions<br>
 * - Large MAP scrolling engine<br>
 * <br>
 * Using MAP resource you can encode large image as a #MapDefinition structure which will be
 * used to handle large background scrolling. The #MapDefinition structure is optimized to encode
 * large map efficiently (space wise), here's the encoding format:<br>
 * - background map is encoded as a grid of 128x128 pixels blocks<br>
 * - duplicated 128x128 blocks are optimized (keep only 1 reference)<br>
 * - each 128x128 blocks is encoded internally as a 8x8 grid of metatile<br>
 * - a metatile is a 16x16 pixels block (2x2 tiles block)<br>
 * - each duplicated / flipped metatile are optimized.<br>
 * <br>
 * Knowing that you can draw your level background to optimize its space usage by trying to optimize<br>
 * the number of unique 128x128 pixels block.
 *
 */

#ifndef _MAP_H_
#define _MAP_H_


#include "vdp.h"
#include "vdp_tile.h"
#include "pal.h"

// forward
typedef struct Map Map;

/**
 *  \brief
 *      Map data update type
 */
typedef enum
{
    ROW_UPDATE,        /** tilemap row update **/
    COLUMN_UPDATE      /** tilemap column update **/
} MapUpdateType;

/**
 *  \brief
 *      Map data patch callback.<br>
 *      It's used to modify/patch map data (for destructible blocks for instance) before sending it to VRAM.
 *
 *  \param map
 *      source Map structure containing map information.
 *  \param buf
 *      buffer containing the tilemap data to patch
 *  \param x
 *      tile X start update position
 *  \param y
 *      tile Y start update position
 *  \param updateType
 *      map data update type:<br>
 *      - ROW_UPDATE (tilemap row update)<br>
 *      - COLUMN_UPDATE (tilemap column update)<br>
 *  \param size
 *      size of the buffer (tilemap width or height depending we are on a row or column update type)
 */
typedef void MapDataPatchCallback(Map *map, u16 *buf, u16 x, u16 y, MapUpdateType updateType, u16 size);


/**
 *  \brief
 *      MapDefinition structure which contains data for large level background.<br>
 *      It's optimized to encode large map using 128x128 block chunk (block chunk are organized in metatile).
 *
 *  \param w
 *      map width in block (128x128 pixels block).
 *  \param h
 *      map height in block (128x128 pixels block).
 *  \param hp
 *      map height in block (128x128 pixels block) removing duplicated rows
 *  \param compression
 *      b0-b3=compression type for metaTiles<br>
 *      b4-b7=compression for blocks data<br>
 *      b8-b11=compression for blockIndexes data<br>
 *      Accepted values:<br>
 *        <b>COMPRESSION_NONE</b><br>
 *        <b>COMPRESSION_APLIB</b><br>
 *        <b>COMPRESSION_LZ4W</b><br>
 *  \param numMetaTile
 *      number of MetaTile
 *  \param numBlock
 *      number of Block (128x128 pixels chunk)
 *  \param metaTiles
 *      metatiles definition, each metatile is encoded as 2x2 tiles block:<br>
 *      - b15: priority<br>
 *      - b14-b13: palette<br>
 *      - b12: vflip<br>
 *      - b11: hflip<br>
 *      - b10-b0: tile index (from tileset)
 *  \param blocks
 *      blocks definition, each block is encoded as 8x8 metatiles:<br>
 *      if numMetaTile <= 256 --> 8 bit index for metaTile<br>
 *      else --> 16 bit index for metaTile
 *  \param blockIndexes
 *      block index array (referencing blocks) for the w * hp sized map<br>
 *      if numBlock <= 256 --> 8 bit index for block
 *      else --> 16 bit index for block
 *  \param blockRowOffsets
 *      block row offsets used internally for fast retrieval of block data (index = blockIndexes[blockRowOffsets[y] + x])
 */
typedef struct
{
    u16 w;
    u16 h;
    u16 hp;
    u16 compression;
    u16 numMetaTile;
    u16 numBlock;
    u16 *metaTiles;
    void* blocks;
    void* blockIndexes;
    u16* blockRowOffsets;
} MapDefinition;

/**
 *  \brief
 *      Map structure containing information for large background/plane update based on #MapDefinition
 *
 *  \param w
 *      map width in block (128x128 pixels block)
 *  \param h
 *      map height in block (128x128 pixels block)
 *  \param metaTiles
 *      internal - unpacked data of MapDefinition.metaTiles
 *  \param blocks
 *      internal - unpacked data of MapDefinition.blocks
 *  \param blockIndexes
 *      internal - unpacked data of MapDefinition.blockIndexes
 *  \param blockRowOffsets
 *      internal - direct access of MapDefinition.blockRowOffsets
 *  \param plane
 *      VDP plane where MAP is draw
 *  \param baseTile
 *      Base tile attributes used to provide base tile index offset and base palette index (see TILE_ATTR_FULL() macro)
 *  \param posX
 *      current view position X set using #MAP_scrollTo(..) method
 *  \param posY
 *      current view position Y set using #MAP_scrollTo(..) method
 *  \param wMask
 *      internal
 *  \param hMask
 *      internal
 *  \param planeWidthMask
 *      internal
 *  \param planeHeightMask
 *      internal
 *  \param lastXT
 *      internal
 *  \param lastYT
 *      internal
 *  \param hScrollTable
 *      internal
 *  \param vScrollTable
 *      internal
 *  \param prepareMapDataColumnCB
 *      internal
 *  \param prepareMapDataRowCB
 *      internal
 *  \param patchMapDataColumnCB
 *      internal
 *  \param patchMapDataRowCB
 *      internal
 *  \param getMetaTileCB
 *      internal
 *  \param getMetaTilemapRectCB
 *      internal
 */
typedef struct Map
{
    u16 w;
    u16 h;
    u16* metaTiles;
    void* blocks;
    void* blockIndexes;
    u16* blockRowOffsets;
    VDPPlane plane;
    u16 baseTile;
    u32 posX;
    u32 posY;
    u16 wMask;
    u16 hMask;
    u16 planeWidth;
    u16 planeHeight;
    u16 planeWidthMaskAdj;
    u16 planeHeightMaskAdj;
    u16 planeWidthSftAdj;
    u16 firstUpdate;
    u16 lastXT;
    u16 lastYT;
    u16 hScrollTable[240];
    u16 vScrollTable[20];
    void (*prepareMapDataColumnCB)(Map *map, u16 *bufCol1, u16 *bufCol2, u16 xm, u16 ym, u16 height);
    void (*prepareMapDataRowCB)(Map *map, u16 *bufRow1, u16 *bufRow2, u16 xm, u16 ym, u16 width);
    MapDataPatchCallback* mapDataPatchCB;
    u16  (*getMetaTileCB)(Map *map, u16 x, u16 y);
    void (*getMetaTilemapRectCB)(Map *map, u16 x, u16 y, u16 w, u16 h, u16* dest);
} Map;


/**
 *  \brief
 *      Create and return a Map structure required to use all MAP_xxx functions
 *      from a given MapDefinition.<br>
 *      When you're done with the map you shall use MAP_release(map) to release it.
 *
 *  \param mapDef
 *      MapDefinition structure containing background/plane data.
 *  \param plane
 *      Plane where we want to draw the Map (for #MAP_scrollTo(..) method).<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      If you want to use the map for collision or special behavior (using the MAP_getTile(..) method) then you can just let this parameter to 0.
 *  \param baseTile
 *      Used to provide base tile index and base palette index (see TILE_ATTR_FULL() macro).<br>
 *      Note that you can also use it to force HIGH priority but in that case your map should only contains LOW priority tiles
 *      otherwise the HIGH priority tiles will be set in LOW priority instead (mutually exclusive).
 *  \return initialized Map structure or <i>NULL</i> if there is not enough memory to allocate data for given MapDefinition.
 */
Map* MAP_create(const MapDefinition* mapDef, VDPPlane plane, u16 baseTile);

/**
 *  \brief
 *      Release the map and its resources (same as MEM_free(map))
 *
 *  \param map
 *      the Map structure to release
 */
void MAP_release(Map* map);

/**
 *  \brief
 *      Scroll map to specified position.<br>
 *      The fonction takes care of updating the VDP tilemap which will be transfered by DMA queue then
 *      VDP background scrolling is automatically set on VBlank (into the SYS_doVBlankProcess() tasks).<br>
 *      WARNING: first MAP_scrollTo(..) call will do a full plane update, for a 64x32 sized plane this represents 4KB of data.<br>
 *      That means you can't initialize 2 MAPs in the same frame (limited to 7.2 KB of data per frame) so take care of calling
 *      SYS_doVBlankProcess() in between.
 *
 *  \param map
 *      source Map structure containing map information.
 *  \param x
 *      view position X we want to scroll on
 *  \param y
 *      view position Y we want to scroll on
 *
 *  \see #MAP_create(..)
 */
void MAP_scrollTo(Map* map, u32 x, u32 y);
/**
 *  \brief
 *      Exactly as #MAP_scrollTo(..) except we can force complete map drawing
 *
 *  \param map
 *      source Map structure containing map information.
 *  \param x
 *      view position X we want to scroll on
 *  \param y
 *      view position Y we want to scroll on
 *  \param forceRedraw
 *      Set to <i>TRUE</i> to force a complete map redraw (take more time)
 *
 *  \see #MAP_scrollTo(..)
 */
void MAP_scrollToEx(Map* map, u32 x, u32 y, bool forceRedraw);

/**
 *  \brief
 *      Returns metatile index / number at given position (a metatile is a block of 2x2 tiles = 16x16 pixels)
 *
 *  \param map
 *      source Map structure containing map information.
 *  \param x
 *      metatile X position (16x16 pixels block)
 *  \param y
 *      metatile Y position (16x16 pixels block)
 *
 *  \return
 *      metatile index
 *
 *  \see #MAP_create(..)
 *  \see #MAP_getTile(..)
 *  \see #MAP_getMetaTilemapRect(..)
 */
u16 MAP_getMetaTile(Map* map, u16 x, u16 y);
/**
 *  \brief
 *      Returns given tile attribute (note than map->baseTile isn't added to the result)
 *
 *  \param map
 *      source Map structure containing map information.
 *  \param x
 *      tile X position
 *  \param y
 *      tile Y position
 *
 *  \return
 *      tile attribute:<br>
 *      - b15: priority<br>
 *      - b14-b13: palette<br>
 *      - b12: vflip<br>
 *      - b11: hflip<br>
 *      - b10-b0: tile index
 *
 *  \see #MAP_create(..)
 *  \see #MAP_getMetaTile(..)
 *  \see #MAP_getTilemapRect(..)
 */
u16 MAP_getTile(Map* map, u16 x, u16 y);
/**
 *  \brief
 *      Returns metatiles index for the specified region (a metatile is a block of 2x2 tiles = 16x16 pixels)
 *
 *  \param map
 *      source Map structure containing map information.
 *  \param x
 *      Region X start position (in metatile - 16x16 pixels block).
 *  \param y
 *      Region Y start position (in metatile - 16x16 pixels block).
 *  \param w
 *      Region Width (in metatile).
 *  \param h
 *      Region Heigh (in metatile).
 *  \param dest
 *      destination pointer receiving metatiles attribute data
 *
 *  \return
 *      metatiles index
 *
 *  \see #MAP_create(..)
 *  \see #MAP_getTilemapRect(..)
 */
void MAP_getMetaTilemapRect(Map* map, u16 x, u16 y, u16 w, u16 h, u16* dest);
/**
 *  \brief
 *      Returns tiles attribute data for the specified region (map->baseTile is used as base tiles attribute, see #MAP_create(..))
 *
 *  \param map
 *      source Map structure containing map information.
 *  \param x
 *      Region X start position <b>(in metatile)</b>
 *  \param y
 *      Region Y start position <b>(in metatile)</b>
 *  \param w
 *      Region Width <b>(in metatile)</b>
 *  \param h
 *      Region Heigh <b>(in metatile)</b>
 *  \param column
 *      if set to TRUE then tilemap data is stored by column order [Y,X] instead of row order [X,Y].
 *  \param dest
 *      destination pointer receiving tiles attribute data
 *
 *  \return
 *      tiles attribute:<br>
 *      - b15: priority<br>
 *      - b14-b13: palette<br>
 *      - b12: vflip<br>
 *      - b11: hflip<br>
 *      - b10-b0: tile index
 *
 *  \see #MAP_create(..)
 *  \see #MAP_getTile(..)
 *  \see #MAP_getMetaTilemapRect(..)
 */
void MAP_getTilemapRect(Map* map, u16 x, u16 y, u16 w, u16 h, bool column, u16* dest);

/**
 *  \brief
 *      Set the callback function to patch tilemap data.<br>
 *      Note that you need to set
 *<br>
 *      The method will be called when a new tilemap row / column is ready to be send to the VDP.<br>
 *      You can use this callback to modify the tilemap data before sending it to VRAM.<br>
 *      It can be useful, for instance, to implement destructibles blocks.
 *
 *  \param map
 *      source Map structure we want to set the patch data callback for.
 *  \param CB
 *      Callback to use to patch the new tilemap data (set to NULL by default = no callback).<br>
 *      See declaration of #MapDataPatchCallback to get information about the callback parameters.
 */
void MAP_setDataPatchCallback(Map* map, MapDataPatchCallback *CB);

/**
 *  \brief
 *      Override the system (VDP) plane size for this map (should be called after MAP_create(..))<br>
 *      Useful if you have VDP plane size set to 64x64 but you want to use 64x32 for a plane so you can use spare VRAM for something else.
 *
 *  \param map
 *      source Map structure we want to override VDP tilemap size for.
 *  \param w
 *      tilemap width (32, 64 or 128)
 *  \param h
 *      tilemap height (32, 64 or 128)
 */
void MAP_overridePlaneSize(Map* map, u16 w, u16 h);


#endif // _MAP_H_
````

## File: inc/mapper.h
````c
/**
 *  \file mapper.h
 *  \brief Mapper / bank switch methods.
 *  \author Stephane Dallongeville
 *  \date 01/2020
 *
 * This unit provides tools to deal with ROM larger than 4MB.<br>
 * It allows to do classic bank switching using the official SEGA mapper but also provide methods to get easy access to "far" data.<br>
 * Note that you can use the ENABLE_BANK_SWITCH flag in config.h file to enable automatic bank switch on binary data access.<br>
 *<br>
 * SEGA official mapper description (taken from Segaretro.org):<br>
 * The bankswitching mechanism is very simple. It views the addressable 4 mega-bytes of ROM as 8 512KB regions.<br>
 * The first area, 0x000000-0x07FFFF is fixed and cannot be remapped because that is where the vector table resides.<br>
 * The banking registers on the cartridge work by allocating the 512KB chunk to a given part of the addressable 4MB ROM space.<br>
 * Below are the registers and what range they correspond to. The value written to a register will cause the specified 512KB page to be mapped to that region.<br>
 * A page is specified with 6 bits (bits 7 and 6 are always 0) thus allowing a possible 64 pages = 32 MB (SSFII only has 10, though.)<br>
 *<br>
 *       0xA130F3:       0x080000 - 0x0FFFFF<br>
 *       0xA130F5:       0x100000 - 0x17FFFF<br>
 *       0xA130F7:       0x180000 - 0x1FFFFF<br>
 *       0xA130F9:       0x200000 - 0x27FFFF<br>
 *       0xA130FB:       0x280000 - 0x2FFFFF<br>
 *       0xA130FD:       0x300000 - 0x37FFFF<br>
 *       0xA130FF:       0x380000 - 0x3FFFFF<br>
 *<br>
 * The registers are accessed through byte writes.<br>
 * Examples:<br>
 *       If 0x01 is written to register 0xA130FF, 0x080000-0x0FFFFF is visible at 0x380000-0x3FFFFF.<br>
 *       If 0x08 is written to register 0xA130F9, the first 512KB of the normally invisible upper 1MB of ROM is now visible at 0x200000-0x27FFFF.<br>
 *<br>
 * The registers simply represent address ranges in the 4MB ROM area and you can page in data to these ranges by specifying the bank #
 */

#ifndef _MAPPER_H_
#define _MAPPER_H_


#include "config.h"
#include "types.h"


#define MAPPER_BASE     0xA130F1

#define BANK_SIZE       0x80000
#define BANK_IN_MASK    (BANK_SIZE - 1)
#define BANK_OUT_MASK   (0xFFFFFFFF ^ BANK_IN_MASK)

/**
 *  \brief
 *      Give access to specified 'far' data through SEGA official bank switch mechanism if needed.
 *
 *  \see #ENABLE_BANK_SWITCH flag in config.h file
 *  \see #SYS_getFarData(..)
 *  \see #SYS_getFarDataSafe(..)
 */
#if (ENABLE_BANK_SWITCH != 0)
    #define FAR(data) SYS_getFarData((void*) (data))
    #define FAR_SAFE(data, size) SYS_getFarDataSafe((void*) (data), size)
#else
    #define FAR(data) data
    #define FAR_SAFE(data, size) data
#endif


/**
 *  \brief
 *      Reset all banks to their initial / default value (automatically called at reset)
 */
void SYS_resetBanks();

/**
 *  \brief
 *      Returns the current bank of specified region index.
 *
 *  \param regionIndex the 512KB region index we want to get. Accepted values: 1-7 as region 0 (0x000000-0x07FFFF) is fixed.
 *  \return the effective 512KB data bank index mapped on this region (0 to 63)
 */
u16 SYS_getBank(u16 regionIndex);
/**
 *  \brief
 *      Set the current bank of specified region index.
 *
 *  \param regionIndex the 512KB region index we want to set. Accepted values: 1-7 as region 0 (0x000000-0x07FFFF) is fixed.
 *  \param bankIndex the effective 512KB data bank index mapped on this region. Accepted values: 0-63
 */
void SYS_setBank(u16 regionIndex, u16 bankIndex);

/**
 *  \brief
 *      Make the given binary data ressource accessible and return a pointer to it.
 *
 *  \param data data we want to access.
 *
 * This method will use bank switching to make the specified data accessible and return a valid pointer to it.<br>
 * <b>WARNING:</b> this method use the 0x00300000-0x003FFFFF range (2 regions) to make the requested data accessible using bank switching mechanism.<br>
 * If data bank is already accessible it re-uses the region otherwise it will change bank of one of the region so be careful of that if you want to access data
 * from different data bank at same time
 *
 *  \see SYS_getFarDataEx
 *  \see SYS_getFarDataSafe
 */
void* SYS_getFarData(void* data);
/**
 *  \brief
 *      Make the given binary data ressource accessible and return a pointer to it.
 *
 *  \param data far data we want to access.
 *  \param high if set to TRUE then we use the high remappable bank for the FAR acces otherwise we use the low one
 *
 * This method will use bank switching to make the specified data accessible and return a valid pointer to it.<br>
 * It will use the 0x00300000-0x0037FFFF or 0x00380000-0x003FFFFF region depending the value of <i>high</i> parameter.<br>
 *
 *  \see SYS_getFarData
 *  \see SYS_getFarDataSafe
 */
void* SYS_getFarDataEx(void* data, bool high);
/**
 *  \brief
 *      Returns TRUE if given binary data is crossing 2 512 KB banks
 *
 *  \param data far data pointer
 *  \param size size (in byte) of the far data block
 *
 * This method return TRUE is the given binary data block is crossing 2 512 KB banks
 *
 *  \see SYS_getFarDataSafe
 */
bool SYS_isCrossingBank(void* data, u32 size);
/**
 *  \brief
 *      Make the given binary data ressource accessible and return a pointer to it (safe version with possible bank crossing)
 *
 *  \param data far data we want to access.
 *  \param size size (in byte) of the far data block we want to access.<br>
 *     Note that size should be > 0, if you don't the size then use SYS_getFarData(..) method instead.
 *
 * This method will use bank switching to make the specified data accessible and return a valid pointer to it.<br>
 * <b>WARNING:</b> this method use the 0x00300000-0x003FFFFF range (2 regions) to make the requested data accessible using bank switching mechanism.<br>
 * If data bank is already accessible it re-uses the region otherwise it will change bank of one of the region so be careful of that if you want to access data
 * from different data bank at same time :p<br>
 * The method checks if the data is crossing banks in which case it will set the 2 switchable/remappable regions to make the data fully accessible.
 *
 *  \see SYS_getFarDataSafeEx
 *  \see SYS_getFarData
 */
void* SYS_getFarDataSafe(void* data, u32 size);
/**
 *  \brief
 *      Make the given binary data ressource accessible and return a pointer to it (safe version with possible bank crossing)
 *
 *  \param data address of far data we want to access.
 *  \param size size (in byte) of the far data block we want to access.<br>
 *     Note that size should be > 0, if you don't the size then use SYS_getFarData(..) method instead.
 *  \param high if set to TRUE then we use the high remappable bank for the FAR acces otherwise we use the low one
 *
 * This method will use bank switching to make the specified data accessible and return a valid pointer to it.<br>
 * It will use the 0x00300000-0x0037FFFF or 0x00380000-0x003FFFFF region depending the value of <i>high</i> parameter.<br>
 * The method checks if the data is crossing banks in which case it will set the 2 switchable/remappable regions to make the data fully accessible.
 *
 *  \see SYS_getFarDataSafe
 */
void* SYS_getFarDataSafeEx(void* data, u32 size, bool high);

/**
 *  \brief
 *      Retrieve the value of the next used region for FAR access with bank switch if data is not already accessible.
 *
 * This method should really be used in very specific case to know which region (6 or 7) will be used for next FAR access through bank switch but more
 * importantly eventually to save its state and restore it with #SYS_setNextFarAccessRegion(..) later if you need to temporary change banks.
 *
 *  \see SYS_setNextFarAccessRegion
 */
bool SYS_getNextFarAccessRegion();
/**
 *  \brief
 *      Set the value of the next used region for FAR access with bank switch if data is not already accessible.
 *
 * This method should really be used in very specific case to set which region (6 or 7) will be used for next FAR access through bank switch.<br>
 * It can also be used to restore internal state saved it through #SYS_getNextFarAccessRegion()
 *
 *  \see SYS_getNextFarAccessRegion
 *  \see SYS_getFarDataEx
 */
void SYS_setNextFarAccessRegion(bool high);

#endif // _MAPPER_H_
````

## File: inc/maths.h
````c
/**
 *  \file maths.h
 *  \brief Mathematical methods.
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides basic maths methods.<br>
 * You can find a tutorial about how use maths with SGDK <a href="https://github.com/Stephane-D/SGDK/wiki/Tuto-Maths">here</a>.<br>
 */

#ifndef _MATHS_H_
#define _MATHS_H_



// 1° step is enough for FIX16
extern const fix16 trigtab_f16[90 + 1];
// 0.25° step is ok for fix32
extern const fix32 trigtab_f32[(90 * 4) + 1];

extern const fix16 log2tab_f16[0x10000];
extern const fix16 log10tab_f16[0x10000];
extern const fix16 sqrttab_f16[0x10000];


#ifndef PI
/**
 *  \brief
 *      PI number (3,1415..)
 */
#define PI                          3.14159265358979323846
#endif


#define FIX16_INT_BITS              10
#define FIX16_FRAC_BITS             (16 - FIX16_INT_BITS)
#define FIX16_INT_MASK              (((1 << FIX16_INT_BITS) - 1) << FIX16_FRAC_BITS)
#define FIX16_FRAC_MASK             ((1 << FIX16_FRAC_BITS) - 1)

#define FIX32_INT_BITS              22
#define FIX32_FRAC_BITS             (32 - FIX32_INT_BITS)
#define FIX32_INT_MASK              (((1 << FIX32_INT_BITS) - 1) << FIX32_FRAC_BITS)
#define FIX32_FRAC_MASK             ((1 << FIX32_FRAC_BITS) - 1)

#define FASTFIX16_INT_BITS          8
#define FASTFIX16_FRAC_BITS         (16 - FASTFIX16_INT_BITS)
#define FASTFIX16_INT_MASK          (((1 << FASTFIX16_INT_BITS) - 1) << FASTFIX16_FRAC_BITS)
#define FASTFIX16_FRAC_MASK         ((1 << FASTFIX16_FRAC_BITS) - 1)

#define FASTFIX32_INT_BITS          16
#define FASTFIX32_FRAC_BITS         (32 - FASTFIX32_INT_BITS)
#define FASTFIX32_INT_MASK          (((1 << FASTFIX32_INT_BITS) - 1) << FASTFIX32_FRAC_BITS)
#define FASTFIX32_FRAC_MASK         ((1 << FASTFIX32_FRAC_BITS) - 1)

/**
 *  \brief
 *      Convert specified value to fix16
 *
 *  Ex:<br>
 *      f16 v = FIX16(-27.12);
 */
#define FIX16(value)                ((fix16) ((value) * (1 << FIX16_FRAC_BITS)))
/**
 *  \brief
 *      Convert specified value to fix32.
 *
 *  Ex:<br>
 *      f32 v = FIX32(34.567);
 */
#define FIX32(value)                ((fix32) ((value) * (1 << FIX32_FRAC_BITS)))

/**
 *  \brief
 *      Convert specified value to "fast" fix16
 *
 *  Ex:<br>
 *      ff16 v = FASTFIX16(-27.12);
 */
#define FASTFIX16(value)            ((fastfix16) ((value) * (1 << FASTFIX16_FRAC_BITS)))
/**
 *  \brief
 *      Convert specified value to "fast" fix32.
 *
 *  Ex:<br>
 *      ff32 v = FASTFIX32(34.567);
 */
#define FASTFIX32(value)            ((fastfix32) ((value) * (1 << FASTFIX32_FRAC_BITS)))

/**
 *  \brief
 *      Convert specified value to fix16 (short version)
 *  \see FIX16
 */
#define F16(value)                  FIX16(value)
/**
 *  \brief
 *      Convert specified value to fix32 (short version)
 *  \see FIX32
 */
#define F32(value)                  FIX32(value)
/**
 *  \brief
 *      Convert specified value to "fast" fix16 (short version)
 *  \see FASTFIX16
 */
#define FF16(value)                 FASTFIX16(value)
/**
 *  \brief
 *      Convert specified value to "fast" fix32 (short version)
 *  \see FASTFIX32
 */
#define FF32(value)                 FASTFIX32(value)


#define F16_PI                      ((fix16) F16(PI))
#define F16_RAD_TO_DEG              F16(57.29577951308232)

#define F32_PI                      ((fix32) F32(PI))


// 2D base structures

/**
 *  \brief
 *      2D Vector structure - u16 type.
 */
typedef struct
{
    u16 x;
    u16 y;
} Vect2D_u16;

/**
 *  \brief
 *      2D Vector structure - s16 type.
 */
typedef struct
{
    s16 x;
    s16 y;
} Vect2D_s16;

/**
 *  \brief
 *      2D Vector structure - u32 type.
 */
typedef struct
{
    u32 x;
    u32 y;
} Vect2D_u32;

/**
 *  \brief
 *      2D Vector structure - s32 type.
 */
typedef struct
{
    s32 x;
    s32 y;
} Vect2D_s32;

/**
 *  \brief
 *      2D Vector structure - f16 (fix16) type.
 */
typedef struct
{
    fix16 x;
    fix16 y;
} Vect2D_f16;

/**
 *  \brief
 *      2D Vector structure - f32 (fix32) type.
 */
typedef struct
{
    fix32 x;
    fix32 y;
} Vect2D_f32;

/**
 *  \brief
 *      2D Vector structure - ff16 (fastfix16) type.
 */
typedef struct
{
    fastfix16 x;
    fastfix16 y;
} Vect2D_ff16;

/**
 *  \brief
 *      2D Vector structure - ff32 (fastfix32) type.
 */
typedef struct
{
    fastfix32 x;
    fastfix32 y;
} Vect2D_ff32;

/**
 *  \brief
 *      2x2 Matrice structure - f16 (fix16) type.<br>
 *      Internally uses 2 2D vectors.
 */
typedef struct
{
    Vect2D_f16 a;
    Vect2D_f16 b;
} Mat2D_f16;

/**
 *  \brief
 *      2x2 Matrice structure - f32 (fix32) type.<br>
 *      Internally uses 2 2D vectors.
 */
typedef struct
{
    Vect2D_f32 a;
    Vect2D_f32 b;
} Mat2D_f32;

/**
 *  \brief
 *      2x2 Matrice structure - ff16 (fastfix16) type.<br>
 *      Internally uses 2 2D vectors.
 */
typedef struct
{
    Vect2D_ff16 a;
    Vect2D_ff16 b;
} Mat2D_ff16;

/**
 *  \brief
 *      2x2 Matrice structure - ff32 (fastfix32) type.<br>
 *      Internally uses 2 2D vectors.
 */
typedef struct
{
    Vect2D_ff32 a;
    Vect2D_ff32 b;
} Mat2D_ff32;

// short alias

/**
 *  \brief alias for Vect2D_u16
 */
typedef Vect2D_u16 V2u16;
/**
 *  \brief alias for Vect2D_s16
 */
typedef Vect2D_s16 V2s16;
/**
 *  \brief alias for Vect2D_u32
 */
typedef Vect2D_u32 V2u32;
/**
 *  \brief alias for Vect2D_s32
 */
typedef Vect2D_s32 V2s32;
/**
 *  \brief alias for Vect2D_f16
 */
typedef Vect2D_f16 V2f16;
/**
 *  \brief alias for Vect2D_f32
 */
typedef Vect2D_f32 V2f32;
/**
 *  \brief alias for Vect2D_ff16
 */
typedef Vect2D_ff16 V2ff16;
/**
 *  \brief alias for Vect2D_ff32
 */
typedef Vect2D_ff32 V2ff32;
/**
 *  \brief alias for Mat2D_f16
 */
typedef Mat2D_f16 M2f16;
/**
 *  \brief alias for Mat2D_f32
 */
typedef Mat2D_f32 M2f32;
/**
 *  \brief alias for Mat2D_ff16
 */
typedef Mat2D_ff16 M2ff16;
/**
 *  \brief alias for Mat2D_ff32
 */
typedef Mat2D_ff32 M2ff32;

///////////////////////////////////////////////
//  Basic math functions
///////////////////////////////////////////////

/**
 *  \brief
 *      Returns the lowest value between X an Y.
 */
#define min(X, Y)       (((X) < (Y))?(X):(Y))

/**
 *  \brief
 *      Returns the highest value between X an Y.
 */
#define max(X, Y)       (((X) > (Y))?(X):(Y))

/**
 *  \brief
 *      Returns L if X is less than L, H if X is greater than H or X if in between L and H.
 */
#define clamp(X, L, H)  (min(max((X), (L)), (H)))

#if (ENABLE_NEWLIB == 0)
/**
 *  \brief
 *      Returns the absolute value of X.
 */
#define abs(X)          (((X) < 0)?-(X):(X))
#endif  // ENABLE_NEWLIB


/**
 *  \brief
 *      ROL instruction for byte (8 bit) value
 *
 *  \param value
 *      value to apply bit rotation
 *  \param number
 *      number of bit rotation
 */
u8  rol8(u8 value, u16 number);
/**
 *  \brief
 *      ROL instruction for short (16 bit) value
 *
 *  \param value
 *      value to apply bit rotation
 *  \param number
 *      number of bit rotation
 */
u16 rol16(u16 value, u16 number);
/**
 *  \brief
 *      ROL instruction for long (32 bit) value
 *
 *  \param value
 *      value to apply bit rotation
 *  \param number
 *      number of bit rotation
 */
u32 rol32(u32 value, u16 number);
/**
 *  \brief
 *      ROR instruction for byte (8 bit) value
 *
 *  \param value
 *      value to apply bit rotation
 *  \param number
 *      number of bit rotation
 */
u8  ror8(u8 value, u16 number);
/**
 *  \brief
 *      ROR instruction for short (16 bit) value
 *
 *  \param value
 *      value to apply bit rotation
 *  \param number
 *      number of bit rotation
 */
u16 ror16(u16 value, u16 number);
/**
 *  \brief
 *      ROR instruction for long (32 bit) value
 *
 *  \param value
 *      value to apply bit rotation
 *  \param number
 *      number of bit rotation
 */
u32 ror32(u32 value, u16 number);

/**
 *  \brief
 *      16x16=32 unsigned multiplication. Force GCC to use proper 68000 <i>mulu</i> instruction.
 *
 *  \param op1
 *      first operand
 *  \param op2
 *      second operand
 *  \return 32 bit (unsigned) result of multiply
 */
u32 mulu(u16 op1, u16 op2);
/**
 *  \brief
 *      16x16=32 signed multiplication. Force GCC to use proper 68000 <i>muls</i> instruction.
 *
 *  \param op1
 *      first operand
 *  \param op2
 *      second operand
 *  \return 32 bit (signed) result of multiply
 */
s32 muls(s16 op1, s16 op2);
/**
 *  \brief
 *      Direct divu instruction (unsigned 32/16=16:16) access using inline assembly
 *      to process op1/op2 operation.
 *
 *  \param op1
 *      first operand - dividende (32 bit)
 *  \param op2
 *      second operand - divisor (16 bit)
 *  \return 16 bit (unsigned) result of the division
 */
u16 divu(u32 op1, u16 op2);
/**
 *  \brief
 *      Direct divs instruction (signed 32/16=16:16) access using inline assembly
 *      to process op1/op2 operation.
 *
 *  \param op1
 *      first operand (32 bit)
 *  \param op2
 *      second operand (16 bit)
 *  \return 16 bit (signed) result of the division
 */
s16 divs(s32 op1, s16 op2);
/**
 *  \brief
 *      Direct divu instruction (unsigned 32/16=16:16) access using inline assembly
 *
 *  \param op1
 *      first operand (32 bit)
 *  \param op2
 *      second operand (16 bit)
 *  \return 16 bit (unsigned) modulo result of the division
 */
u16 modu(u32 op1, u16 op2);
/**
 *  \brief
 *      Direct divs instruction (signed 32/16=16:16) access using inline assembly
 *
 *  \param op1
 *      first operand (32 bit)
 *  \param op2
 *      second operand (16 bit)
 *  \return 16 bit (signed) modulo result of the division
 */
s16 mods(s32 op1, s16 op2);

/**
 *  \brief
 *      Direct divu instruction (unsigned 32/16=16:16) access using inline assembly
 *      to process op1/op2 operation and op1%op2 at same time.
 *
 *  \param op1
 *      first operand - dividende (32 bit)
 *  \param op2
 *      second operand - divisor (16 bit)
 *  \return 16 bit (unsigned) result of the division in low 16 bit (0-15) and
 *      16 bit (unsigned) result of the modulo operation in high 16 bit (16-31)
 */
u32 divmodu(u32 op1, u16 op2);
/**
 *  \brief
 *      Direct divs instruction (signed 32/16=16:16) access using inline assembly
 *      to process op1/op2 operation and op1%op2 at same time.
 *
 *  \param op1
 *      first operand - dividende (32 bit)
 *  \param op2
 *      second operand - divisor (16 bit)
 *  \return 16 bit (signed) result of the division in low 16 bit (0-15) and
 *      16 bit (signed) result of the modulo operation in high 16 bit (16-31)
 */
s32 divmods(s32 op1, s16 op2);

/**
 *  \brief
 *      Convert u16 to BCD.
 *
 *  \param value
 *      u16 value to convert.
 */
u32 u16ToBCD(u16 value);
/**
 *  \brief
 *      Convert u32 to BCD.
 *
 *  \param value
 *      u32 value to convert.
 */
u32 u32ToBCD(u32 value);

/**
 *  \brief
 *      Return next pow2 value which is greater than specified 32 bits unsigned value.
 *      Ex:<br>
 *      getNextPow2(700) = 1024<br>
 *      getNextPow2(18) = 32<br>
 */
u32 getNextPow2(u32 value);
/**
 *  \brief
 *      Return integer log2 of specified 32 bits unsigned value.
 *      Ex:<br>
 *      getLog2Int(1024) = 10<br>
 *      getLog2Int(12345) = 13<br>
 *
 *  \param value
 *      value to return log2 of
 */
u16 getLog2(u32 value);

/**
 *  \brief
 *      Return euclidean distance approximation for specified vector.<br>
 *      The returned distance is not 100% perfect but calculation is fast.
 *
 *  \param dx
 *      delta X.
 *  \param dy
 *      delta Y.
 */
u32 getApproximatedDistance(s32 dx, s32 dy);
/**
 *  \brief
 *      Return euclidean distance approximation for specified vector.<br>
 *      The returned distance is not 100% perfect but calculation is fast.
 *
 *  \param v
 *      2D vector.
 */
u32 getApproximatedDistanceV(V2s32* v);


#define distance_approx(dx, dy)     _Pragma("GCC error \"This method is deprecated, use getApproximatedDistance(..) instead.\"")
#define getApproximatedLog2(value)  _Pragma("GCC error \"This method is deprecated, use FF32_getLog2Fast(..) instead.\"")
#define getLog2Int(value)           _Pragma("GCC error \"This method is deprecated, use getLog2(..) instead.\"")


///////////////////////////////////////////////
//  Fix16 Fixed point math functions
///////////////////////////////////////////////

/**
 *  \brief
 *      Convert fix16 to integer.
 */
s16 F16_toInt(fix16 value);
/**
 *  \brief
 *      Convert specified fix16 value to fix32.
 */
fix32 F16_toFix32(fix16 value);
/**
 *  \brief
 *      Convert specified fix16 value to fastfix16.
 */
fastfix16 F16_toFastFix16(fix16 value);
/**
 *  \brief
 *      Convert specified fix16 value to fastfix32.
 */
fastfix32 F16_toFastFix32(fix16 value);
/**
 *  \brief
 *      Return fractional part of the specified value (fix16).
 */
fix16 F16_frac(fix16 value);
/**
 *  \brief
 *      Return integer part of the specified value (fix16).
 */
fix16 F16_int(fix16 value);
/**
 *  \brief
 *      Return the absolute value of the specified value (fix16).
 */
fix16 F16_abs(fix16 x);
/**
 *  \brief
 *      Round the specified value to nearest integer (fix16).
 */
fix16 F16_round(fix16 value);
/**
 *  \brief
 *      Round and convert the specified fix16 to integer.
 */
s16 F16_toRoundedInt(fix16 value);

/**
 *  \brief
 *      Compute and return the result of the multiplication of val1 and val2 (fix16).
 */
fix16 F16_mul(fix16 val1, fix16 val2);
/**
 *  \brief
 *      Compute and return the result of the division of val1 by val2 (fix16).
 */
fix16 F16_div(fix16 val1, fix16 val2);
/**
 *  \brief
 *      Compute and return the result of the average of val1 by val2 (fix16).
 */
fix16 F16_avg(fix16 val1, fix16 val2);

/**
 *  \brief
 *      Compute and return the result of the Log2 of specified value (fix16).
 */
fix16 F16_log2(fix16 value);
/**
 *  \brief
 *      Compute and return the result of the Log10 of specified value (fix16).
 */
fix16 F16_log10(fix16 value);
/**
 *  \brief
 *      Compute and return the result of the root square of specified value (fix16).
 */
fix16 F16_sqrt(fix16 value);

/**
 *  \brief
 *      Return a normalized form of the input angle degree:<br>
 *      Output value is guaranteed to be in [FIX16(0)..FIX16(360)[ range.
 */
fix16 F16_normalizeAngle(fix16 angle);
/**
 *  \brief
 *      Compute sinus of specified angle (in degree) and return it (fix16).
 */
fix16 F16_sin(fix16 angle);
/**
 *  \brief
 *      Compute cosinus of specified angle (in degree) and return it (fix16).
 */
fix16 F16_cos(fix16 angle);
/**
 *  \brief
 *      Compute the tangent of specified angle (in degree) and return it (fix16).
 */
fix16 F16_tan(fix16 angle);
/**
 *  \brief
 *      Compute the arctangent of specified value and return it in degree (fix16).
 */
fix16 F16_atan(fix16 x);
/**
 *  \brief
 *      Compute the arctangent of y/x. i.e: return the angle (in degree) for the (0,0)-(x,y) vector.
 */
fix16 F16_atan2(fix16 y, fix16 x);

/**
 *  \deprecated Use F16_sin(..) instead
 *  \brief
 *      Compute sinus of specified value and return it as fix16.<br>
 *      The input value is an integer defined as [0..1024] range corresponding to radian [0..2PI] range.
 */
fix16 sinFix16(u16 value);
/**
 *  \deprecated Use F16_cos(..) instead
 *  \brief
 *      Compute cosinus of specified value and return it as fix16.<br>
 *      The input value is an integer defined as [0..1024] range corresponding to radian [0..2PI] range.
 */
fix16 cosFix16(u16 value);

/**
 *  \brief
 *      Convert degree to radian (fix16)
 */
fix16 F16_degreeToRadian(fix16 degree);
/**
 *  \brief
 *      Convert radian to degree (fix16)
 */
fix16 F16_radianToDegree(fix16 radian);
/**
 *  \brief
 *      Compute and return the angle in degree between the 2 points defined by (x1,y1) and (x2,y2).
 */
fix16 F16_getAngle(fix16 x1, fix16 y1, fix16 x2, fix16 y2);
/**
 *  \brief
 *      Compute the new position of the point defined by (x1, y1) by moving it by the given 'distance' in the 'angle' direction
 *      and store the result in (x2, y2).
 *
 *  \param x2
 *      new X position
 *  \param y2
 *      new Y position
 *  \param x1
 *      current X position
 *  \param y1
 *      current Y position
 *  \param ang
 *      angle in degree
 *  \param dist
 *      distance to move
 */
void F16_computePosition(fix16 *x2, fix16 *y2, fix16 x1, fix16 y1, fix16 ang, fix16 dist);
/**
 *  \brief
 *      Compute the new position of the point defined by (x1, y1) by moving it by the given 'distance' in the 'angle' direction
 *      and store the result in (x2, y2).
 *
 *  \param x2
 *      new X position
 *  \param y2
 *      new Y position
 *  \param x1
 *      current X position
 *  \param y1
 *      current Y position
 *  \param ang
 *      angle in degree
 *  \param dist
 *      distance to move
 *  \param cosMul
 *      cosine multiplier factor (default = FIX16(1))
 *  \param sinMul
 *      sine multiplier factor (default = FIX16(1))
 */
void F16_computePositionEx(fix16 *x2, fix16 *y2, fix16 x1, fix16 y1, fix16 ang, fix16 dist, fix16 cosMul, fix16 sinMul);


// Deprecated functions
#define intToFix16(a)           _Pragma("GCC error \"This method is deprecated, use FIX16(..) instead.\"")
#define fix16ToInt(a)           _Pragma("GCC error \"This method is deprecated, use F16_toInt(..) instead.\"")
#define fix16ToFix32(a)         _Pragma("GCC error \"This method is deprecated, use F16_toFix32(..) instead.\"")
#define fix16Frac(a)            _Pragma("GCC error \"This method is deprecated, use F16_frac(..) instead.\"")
#define fix16Int(a)             _Pragma("GCC error \"This method is deprecated, use F16_int(..) instead.\"")
#define fix16Round(a)           _Pragma("GCC error \"This method is deprecated, use F16_round(..) instead.\"")
#define fix16ToRoundedInt(a)    _Pragma("GCC error \"This method is deprecated, use F16_toRoundedInt(..) instead.\"")
#define fix16Add(a, b)          _Pragma("GCC error \"This method is deprecated, simply use '+' operator to add fix16 values together.\"")
#define fix16Sub(a, b)          _Pragma("GCC error \"This method is deprecated, simply use '-' operator to subtract fix16 values.\"")
#define fix16Neg(a)             _Pragma("GCC error \"This method is deprecated, simply use '0 - value' to get the negative fix16 value.\"")
#define fix16Mul(a, b)          _Pragma("GCC error \"This method is deprecated, use F16_mul(..) instead.\"")
#define fix16Div(a, b)          _Pragma("GCC error \"This method is deprecated, use F16_div(..) instead.\"")
#define fix16Avg(a, b)          _Pragma("GCC error \"This method is deprecated, use F16_avg(..) instead.\"")
#define fix16Log2(a)            _Pragma("GCC error \"This method is deprecated, use F16_log2(..) instead.\"")
#define fix16Log10(a)           _Pragma("GCC error \"This method is deprecated, use F16_log10(..) instead.\"")
#define fix16Sqrt(a)            _Pragma("GCC error \"This method is deprecated, use F16_sqrt(..) instead.\"")


///////////////////////////////////////////////
//  Fix32 Fixed point math functions
///////////////////////////////////////////////

/**
 *  \brief
 *      Convert fix32 to integer.
 */
s32 F32_toInt(fix32 value);
/**
 *  \brief
 *      Convert specified fix32 value to fix16.
 */
fix16 F32_toFix16(fix32 value);
/**
 *  \brief
 *      Convert specified fix32 value to fastfix16.
 */
fastfix16 F32_toFastFix16(fix32 value);
/**
 *  \brief
 *      Convert specified fix32 value to fastfix32.
 */
fastfix32 F32_toFastFix32(fix32 value);
/**
 *  \brief
 *      Return fractional part of the specified value (fix32).
 */
fix32 F32_frac(fix32 value);
/**
 *  \brief
 *      Return integer part of the specified value (fix32).
 */
fix32 F32_int(fix32 value);
/**
 *  \brief
 *      Round the specified value to nearest integer (fix32).
 */
fix32 F32_round(fix32 value);
/**
 *  \brief
 *      Round and convert the specified fix32 value to integer.
 */
s32 F32_toRoundedInt(fix32 value);

/**
 *  \brief
 *      Compute and return the result of the multiplication of val1 and val2 (fix32).<br>
 *      WARNING: result can easily overflow so its recommended to stick with fix16 type for mul and div operations.
 */
fix32 F32_mul(fix32 val1, fix32 val2);
/**
 *  \brief
 *      Compute and return the result of the division of val1 by val2 (fix32).<br>
 *      WARNING: result can easily overflow so its recommended to stick with fix16 type for mul and div operations.
 */
fix32 F32_div(fix32 val1, fix32 val2);
/**
 *  \brief
 *      Compute and return the result of the average of val1 by val2 (fix32).
 */
fix32 F32_avg(fix32 val1, fix32 val2);

/**
 *  \brief
 *      Compute sinus of specified angle (in degree) and return it as fix32.
 */
fix32 F32_sin(fix16 angle);
/**
 *  \brief
 *      Compute cosinus of specified angle (in degree) and return it as fix32.
 */
fix32 F32_cos(fix16 angle);

/**
 *  \deprecated Use F32_sin(..) instead
 *  \brief
 *      Compute sinus of specified value and return it as fix32.<br>
 *      The input value is an integer defined as [0..1024] range corresponding to radian [0..2PI] range.
 */
fix32 sinFix32(u16 value);
/**
 *  \deprecated Use F32_cos(..) instead
 *  \brief
 *      Compute cosinus of specified value and return it as fix32.<br>
 *      The input value is an integer defined as [0..1024] range corresponding to radian [0..2PI] range.
 */
fix32 cosFix32(u16 value);


// Deprecated functions
#define intToFix32(a)           _Pragma("GCC error \"This method is deprecated, use FIX32(..) instead.\"")
#define fix32ToInt(a)           _Pragma("GCC error \"This method is deprecated, use F32_toInt(..) instead.\"")
#define fix32ToFix16(a)         _Pragma("GCC error \"This method is deprecated, use F32_toFix16(..) instead.\"")
#define fix32Frac(a)            _Pragma("GCC error \"This method is deprecated, use F32_frac(..) instead.\"")
#define fix32Int(a)             _Pragma("GCC error \"This method is deprecated, use F32_int(..) instead.\"")
#define fix32Round(a)           _Pragma("GCC error \"This method is deprecated, use F32_round(..) instead.\"")
#define fix32ToRoundedInt(a)    _Pragma("GCC error \"This method is deprecated, use F32_toRoundedInt(..) instead.\"")
#define fix32Add(a, b)          _Pragma("GCC error \"This method is deprecated, simply use '+' operator to add fix32 values together.\"")
#define fix32Sub(a, b)          _Pragma("GCC error \"This method is deprecated, simply use '-' operator to subtract fix32 values.\"")
#define fix32Neg(a)             _Pragma("GCC error \"This method is deprecated, simply use '0 - value' to get the negative fix32 value.\"")
#define fix32Mul(a, b)          _Pragma("GCC error \"This method is deprecated, use F32_mul(..) instead.\"")
#define fix32Div(a, b)          _Pragma("GCC error \"This method is deprecated, use F32_div(..) instead.\"")
#define fix32Avg(a, b)          _Pragma("GCC error \"This method is deprecated, use F32_avg(..) instead.\"")


///////////////////////////////////////////////
//  Fast Fix16 Fixed point math functions
///////////////////////////////////////////////

/**
 *  \brief
 *      Convert fastfix16 to integer.
 */
s16 FF16_toInt(fastfix16 value);
/**
 *  \brief
 *      Convert fastfix16 to fix16.
 */
fix16 FF16_toFix16(fastfix16 value);
/**
 *  \brief
 *      Convert fastfix16 to fix32.
 */
fix32 FF16_toFix32(fastfix16 value);
/**
 *  \brief
 *      Convert fastfix16 to fastfix32.
 */
fastfix32 FF16_toFastFix32(fastfix16 value);
/**
 *  \brief
 *      Return fractional part of the specified value (fastfix16).
 */
fastfix16 FF16_frac(fastfix16 value);
/**
 *  \brief
 *      Return integer part of the specified value (fastfix16).
 */
fastfix16 FF16_int(fastfix16 value);
/**
 *  \brief
 *      Round the specified value to nearest integer (fastfix16).
 */
fastfix16 FF16_round(fastfix16 value);
/**
 *  \brief
 *      Round and convert the specified fastfix16 value to integer (fastfix16).
 */
s16 FF16_toRoundedInt(fastfix16 value);

/**
 *  \brief
 *      Compute and return the result of the multiplication of val1 and val2 (fastfix16).
 */
fastfix16 FF16_mul(fastfix16 val1, fastfix16 val2);
/**
 *  \brief
 *      Compute and return the result of the division of val1 by val2 (fastfix16).
 */
fastfix16 FF16_div(fastfix16 val1, fastfix16 val2);


// Deprecated functions
#define intToFastFix16(a)           _Pragma("GCC error \"This method is deprecated, use FASTFIX16(..) instead.\"")
#define fastFix16ToInt(a)           _Pragma("GCC error \"This method is deprecated, use FF16_toInt(..) instead.\"")
#define fastFix16Frac(a)            _Pragma("GCC error \"This method is deprecated, use FF16_frac(..) instead.\"")
#define fastFix16Int(a)             _Pragma("GCC error \"This method is deprecated, use FF16_int(..) instead.\"")
#define fastFix16Round(a)           _Pragma("GCC error \"This method is deprecated, use FF16_round(..) instead.\"")
#define fastFix16ToRoundedInt(a)    _Pragma("GCC error \"This method is deprecated, use FF16_toRoundedInt(..) instead.\"")
#define fastFix16Mul(a, b)          _Pragma("GCC error \"This method is deprecated, use FF16_mul(..) instead.\"")
#define fastFix16Div(a, b)          _Pragma("GCC error \"This method is deprecated, use FF16_div(..) instead.\"")


///////////////////////////////////////////////
//  Fast Fix32 Fixed point math functions
///////////////////////////////////////////////

/**
 *  \brief
 *      Convert integer to fastfix32.
 */
fastfix32 FF32_fromInt(s16 value);
/**
 *  \brief
 *      Convert fastfix32 to integer.
 */
s16 FF32_toInt(fastfix32 value);
/**
 *  \brief
 *      Convert fastfix32 to fix16.
 */
fix16 FF32_toFix16(fastfix32 value);
/**
 *  \brief
 *      Convert fastfix32 to fix32.
 */
fix32 FF32_toFix32(fastfix32 value);
/**
 *  \brief
 *      Convert fastfix32 to fastfix16.
 */
fastfix16 FF32_toFastFix16(fastfix32 value);
/**
 *  \brief
 *      Return fractional part of the specified value (fastfix32).
 */
fastfix32 FF32_frac(fastfix32 value);
/**
 *  \brief
 *      Return integer part of the specified value (fastfix32).
 */
fastfix32 FF32_int(fastfix32 value);
/**
 *  \brief
 *      Round the specified value to nearest integer (fastfix32).
 */
fastfix32 FF32_round(fastfix32 value);
/**
 *  \brief
 *      Round and convert the specified fastfix32 value to integer.
 */
s32 FF32_toRoundedInt(fastfix32 value);

/**
 *  \brief
 *      Compute and return the result of the multiplication of val1 and val2 (fastfix32).<br>
 *      WARNING: result can easily overflow so its recommended to stick with fix16 type for mul and div operations.
 */
fastfix32 FF32_mul(fastfix32 val1, fastfix32 val2);
/**
 *  \brief
 *      Compute and return the result of the division of val1 by val2 (fastfix32).<br>
 *      WARNING: result can easily overflow so its recommended to stick with fix16 type for mul and div operations.
 */
fastfix32 FF32_div(fastfix32 val1, fastfix32 val2);

/**
 *  \brief
 *      Return a not accurate but fast log2 *approximation* of the specified value (fastfixed32)
 *      Ex:<br>
 *      getLog2(FASTFIX32(1)) = 0<br>
 *      getLog2(12345 << 16) = ~9.5 (real value = ~13.6)<br>
 *
 *  \param value
 *      fastfixed32 value to return log2 of
 */
fastfix32 FF32_getLog2Fast(fastfix32 value);


// Deprecated functions
#define intToFastFix32(a)           _Pragma("GCC error \"This method is deprecated, use FASTFIX32(..) instead.\"")
#define fastFix32ToInt(a)           _Pragma("GCC error \"This method is deprecated, use FF32_toInt(..) instead.\"")
#define fastFix32Frac(a)            _Pragma("GCC error \"This method is deprecated, use FF32_frac(..) instead.\"")
#define fastFix32Int(a)             _Pragma("GCC error \"This method is deprecated, use FF32_int(..) instead.\"")
#define fastFix32Round(a)           _Pragma("GCC error \"This method is deprecated, use FF32_round(..) instead.\"")
#define fastFix32ToRoundedInt(a)    _Pragma("GCC error \"This method is deprecated, use FF32_toRoundedInt(..) instead.\"")
#define fastFix32Mul(a, b)          _Pragma("GCC error \"This method is deprecated, use FF32_mul(..) instead.\"")
#define fastFix32Div(a, b)          _Pragma("GCC error \"This method is deprecated, use FF32_div(..) instead.\"")


///////////////////////////////////////////////
//  Vector2D base math functions
///////////////////////////////////////////////

/**
 *  \brief
 *      Compute and return the angle in degree (fix16) between the 2 points pt1 and pt2.
 */
fix16 V2D_F16_getAngle(V2f16* pt1, V2f16* pt2);

/**
 *  \brief
 *      Compute the new position of the point 'pt' by moving it by the given 'distance' in the 'angle' direction.
 *
 *  \param pt
 *      point to move
 *  \param ang
 *      angle in degree
 *  \param dist
 *      distance to move
 */
void V2D_F16_computePosition(V2f16* pt, fix16 ang, fix16 dist);
/**
 *  \brief
 *      Compute the new position of the point 'pt' by moving it by the given 'distance' in the 'angle' direction.
 *
 *  \param pt
 *      point to move
 *  \param ang
 *      angle in degree
 *  \param dist
 *      distance to move
 *  \param cosMul
 *      cosine multiplier factor (default = FIX16(1))
 *  \param sinMul
 *      sine multiplier factor (default = FIX16(1))
 */
void V2D_F16_computePositionEx(V2f16* pt, fix16 ang, fix16 dist, fix16 cosMul, fix16 sinMUl);

/**
 *  \brief
 *      Return euclidean distance approximation for specified vector.<br>
 *      The returned distance is not 100% perfect but calculation is fast.
 *
 *  \param v
 *      2D vector.
 */
u32 V2D_S32_getApproximatedDistance(V2s32* v);


// Deprecated functions
#define getApproximatedDistanceV(dx, dy)     _Pragma("GCC error \"This method is deprecated, use V2D_S32_getApproximatedDistance(..) instead.\"")

#endif // _MATHS_H_
````

## File: inc/maths3D.h
````c
/**
 *  \file maths3D.h
 *  \brief 3D math engine.
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides 3D transformation methods :<br>
 * - translation X, Y, Z<br>
 * - rotation X, Y, Z<br>
 * - one directionnal light<br>
 * - 2D projection<br>
 *<br>
 * Can transform (including 2D projection) about ~10000 vertices / seconde.
 */

#ifndef _MATHS3D_H_
#define _MATHS3D_H_


// 3D base structutes

/**
 *  \brief
 *      3D Vector structure - u16 type.
 */
typedef struct
{
    u16 x;
    u16 y;
    u16 z;
} Vect3D_u16;

/**
 *  \brief
 *      3D Vector structure - s16 type.
 */
typedef struct
{
    s16 x;
    s16 y;
    s16 z;
} Vect3D_s16;

/**
 *  \brief
 *      3D Vector structure - u32 type.
 */
typedef struct
{
    u32 x;
    u32 y;
    u32 z;
} Vect3D_u32;

/**
 *  \brief
 *      3D Vector structure - s32 type.
 */
typedef struct
{
    s32 x;
    s32 y;
    s32 z;
} Vect3D_s32;

/**
 *  \brief
 *      3D Vector structure - f16 (fix16) type.
 */
typedef struct
{
    fix16 x;
    fix16 y;
    fix16 z;
} Vect3D_f16;

/**
 *  \brief
 *      3D Vector structure - f32 (fix32) type.
 */
typedef struct
{
    fix32 x;
    fix32 y;
    fix32 z;
} Vect3D_f32;

/**
 *  \brief
 *      3x3 Matrice structure - f16 (fix16) type.<br>
 *      Internally uses 3 3D vectors.
 */
typedef struct
{
    Vect3D_f16 a;
    Vect3D_f16 b;
    Vect3D_f16 c;
} Mat3D_f16;

/**
 *  \brief
 *      3x3 Matrice structure - f32 (fix32) type.<br>
 *      Internally uses 3 3D vectors.
 */
typedef struct
{
    Vect3D_f32 a;
    Vect3D_f32 b;
    Vect3D_f32 c;
} Mat3D_f32;


// 4D base structures

/**
 *  \brief
 *      4D Vector structure - f16 (fix16) type.
 */
typedef struct
{
    fix16 x;
    fix16 y;
    fix16 z;
    fix16 w;
} Vect4D_f16;

/**
 *  \brief
 *      4D Vector structure - f32 (fix32) type.
 */
typedef struct
{
    fix32 x;
    fix32 y;
    fix32 z;
    fix32 w;
} Vect4D_f32;

/**
 *  \brief
 *      4x4 Matrice structure - f16 (fix16) type.<br>
 *      Internally uses 4 4D vectors.
 */
typedef struct
{
    Vect4D_f16 a;
    Vect4D_f16 b;
    Vect4D_f16 c;
    Vect4D_f16 d;
} Mat4D_f16;

/**
 *  \brief
 *      4x4 Matrice structure - f32 (fix32) type.<br>
 *      Internally uses 4 4D vectors.
 */
typedef struct
{
    Vect4D_f32 a;
    Vect4D_f32 b;
    Vect4D_f32 c;
    Vect4D_f32 d;
} Mat4D_f32;


// short alias

/**
 *  \brief alias for Vect3D_u16
 */
typedef Vect3D_u16 V3u16;
/**
 *  \brief alias for Vect3D_s16
 */
typedef Vect3D_s16 V3s16;
/**
 *  \brief alias for Vect3D_u32
 */
typedef Vect3D_u32 V3u32;
/**
 *  \brief alias for Vect3D_s32
 */
typedef Vect3D_s32 V3s32;
/**
 *  \brief alias for Vect3D_f16
 */
typedef Vect3D_f16 V3f16;
/**
 *  \brief alias for Vect3D_f32
 */
typedef Vect3D_f32 V3f32;

/**
 *  \brief alias for Vect4D_f16
 */
typedef Vect4D_f16 V4f16;
/**
 *  \brief alias for Vect4D_f32
 */
typedef Vect4D_f32 V4f32;

/**
 *  \brief alias for Mat3D_f16
 */
typedef Mat3D_f16 M3f16;
/**
 *  \brief alias for Mat3D_f32
 */
typedef Mat3D_f32 M3f32;
/**
 *  \brief alias for Mat4D_f16
 */
typedef Mat4D_f16 M4f16;
/**
 *  \brief alias for Mat4D_f32
 */
typedef Mat4D_f32 M4f32;


// advanced 3D structures

/**
 *  \brief
 *      Structure hosting settings / context for the 3D transform engine.
 */
typedef struct
{
    V2u16 viewport;
    fix16 camDist;
    V3f16 light;
    u16 lightEnabled;
} Context3D;

/**
 *  \brief
 *      3D translation informations object - f16 (fix16) type.
 */
typedef V3f16 Translation3D;

/**
 *  \brief
 *      3D rotation informations object (angles are stored in degree) - f16 (fix16) type.
 */
typedef V3f16 Rotation3D;

/**
 *  \brief
 *      3D transformation object - f16 (fix16) type.<br>
 *      This object define the global 3D transformation informations and associated cached data.<br>
 *      If rotation information is modified the rebuildMat flag should be set to 1.<br>
 *      Rotation and translation objects are reference so don't forget to set them.
 */
typedef struct
{
    u16 rebuildMat;
    Translation3D* translation;
    Rotation3D* rotation;
    M3f16 mat;
    M3f16 matInv;
    V3f16 cameraInv;
    V3f16 lightInv;
} Transformation3D;

/**
 *  \brief
 *      Reset math 3D engine (reset matrices and transformation parameters mainly).
 */
void M3D_reset(void);

/**
 *  \brief
 *      Enable or disable light transformation calculation.
 */
void M3D_setLightEnabled(u16 enabled);
/**
 *  \brief
 *      Get light transformation calculation enabled flag.
 */
u16  M3D_getLightEnabled(void);

/**
 *  \brief
 *      Set viewport dimension.
 *
 *  \param w
 *      Viewport width (use BMP_WIDTH if you use 3D with software bitmap engine)
 *  \param h
 *      Viewport height (use BMP_HEIGHT if you use 3D with software bitmap engine)
 */
void M3D_setViewport(u16 w, u16 h);
/**
 *  \brief
 *      Set camera scene distance.
 *
 *  \param value
 *      Distance between the camera and the scene.
 */
void M3D_setCamDistance(fix16 value);
/**
 *  \brief
 *      Set light direction vector.
 */
void M3D_setLightXYZ(fix16 x, fix16 y, fix16 z);
/**
 *  \brief
 *      Set light direction vector.
 */
void M3D_setLight(V3f16* value);

/**
 *  \brief
 *      Reset the specified Transformation3D object.
 */
void M3D_resetTransform(Transformation3D* t);

/**
 *  \brief
 *      Set translation and rotation objects to the specified transformation object.
 */
void M3D_setTransform(Transformation3D* tr, Translation3D* t, Rotation3D *r);

/**
 *  \brief
 *      Set translation parameters to the specified transformation object.
 */
void M3D_setTranslation(Transformation3D* t, fix16 x, fix16 y, fix16 z);
/**
 *  \brief
 *      Set rotation parameters to the specified #Transformation3D object.<br>
 *      x, y, z angle values are given in degree (fix16)
 */
void M3D_setRotation(Transformation3D* t, fix16 x, fix16 y, fix16 z);

/**
 *  \brief
 *      Combine the specified right and left #Transformation3D objects and store result in <code>result</code>.
 *      result cannot be the same transformation object as left or right.
 *
 *  \param left
 *      Left #Transformation3D object.
 *  \param right
 *      Right #Transformation3D object.
 *  \param result
 *      Result #Transformation3D object.
 */
void M3D_combineTransform(Transformation3D* left, Transformation3D* right, Transformation3D* result);
/**
 *  \brief
 *      Combine the specified left #Translation3D and right #Transformation3D and store result in <code>result</code>.<br>
 *      right and result transformation object can be the same.
 *
 *  \param left
 *      Left #Transformation3D object.
 *  \param right
 *      Right #Transformation3D object.
 *  \param result
 *      Result #Transformation3D object.
 */
void M3D_combineTranslationLeft(Translation3D* left, Transformation3D* right, Transformation3D* result);
/**
 *  \brief
 *      Combine the specified left #Transformation3D with right #Translation3D and store result in <code>result</code>.<br>
 *      left and result transformation object can be the same.
 *
 *  \param left
 *      Left #Transformation3D object.
 *  \param right
 *      Right #Translation3D object.
 *  \param result
 *      Result #Transformation3D object.
 */
void M3D_combineTranslationRight(Transformation3D* left, Translation3D* right, Transformation3D* result);

/**
 *  \brief
 *      Build the transformation matrix of the specified #Transformation3D object.<br>
 *      This also rebuild cached informations as inverse transformation matrix, inverse camera view...
 *
 *  \param t
 *      #Transformation3D object.
 */
void M3D_buildMat3D(Transformation3D* t);
/**
 *  \brief
 *      Build the transformation matrix of the specified transformation object.
 *      Only rebuild the transformation matrix (faster), cached infos as inverse matrix are not rebuild.
 *
 *  \param t
 *      Transformation object.
 */
void M3D_buildMat3DOnly(Transformation3D* t);
/**
 *  \brief
 *      Only rebuild the cached infos as inverse matrix, inverse camera view...
 *
 *  \param t
 *      Transformation object.
 */
void M3D_buildMat3DExtras(Transformation3D* t);

/**
 *  \brief
 *      Process 3D translation only to specified 3D vertices buffer.
 *
 *  \param t
 *      Transformation object containing translation parameter.
 *  \param vertices
 *      3D vertices buffer to translate.
 *  \param numv
 *      Number of vertices to translate.
 */
void M3D_translate(Transformation3D* t, V3f16* vertices, u16 numv);
/**
 *  \brief
 *      Process 3D rotation only to specified 3D vertices buffer.
 *
 *  \param t
 *      Transformation object containing rotation parameter.
 *  \param src
 *      Source 3D vertices buffer.
 *  \param dest
 *      Destination 3D vertices buffer.
 *  \param numv
 *      Number of vertices to rotate.
 */
void M3D_rotate(Transformation3D* t, const V3f16* src, V3f16* dest, u16 numv);
/**
 *  \brief
 *      Process 3D inverse rotation only to specified 3D vertex.
 *
 *  \param t
 *      Transformation object containing rotation parameter.
 *  \param src
 *      Source 3D vertex.
 *  \param dest
 *      Destination 3D vertex.
 */
void M3D_rotateInv(Transformation3D* t, const V3f16* src, V3f16* dest);
/**
 *  \brief
 *      Process 3D transform (rotation and translation) to specified 3D vertices buffer.
 *
 *  \param t
 *      Transformation object containing rotation and translation parameters.
 *  \param src
 *      Source 3D vertices buffer.
 *  \param dest
 *      Destination 3D vertices buffer.
 *  \param numv
 *      Number of vertices to transform.
 */
void M3D_transform(Transformation3D* t, const V3f16* src, V3f16* dest, u16 numv);

/**
 *  \brief
 *      Process 2D projection to specified 3D vertices buffer (fix16 version).
 *
 *  \param src
 *      Source 3D vertices buffer.
 *  \param dest
 *      Destination 2D vertices buffer - fix16 format
 *  \param numv
 *      Number of vertices to project.
 */
void M3D_project_f16(const V3f16* src, V2f16* dest, u16 numv);
/**
 *  \brief
 *      Process 2D projection to specified 3D vertices buffer (s16 version).
 *
 *  \param src
 *      Source 3D vertices buffer.
 *  \param dest
 *      Destination 2D vertices buffer - s16 format
 *  \param numv
 *      Number of vertices to project.
 */
void M3D_project_s16(const V3f16* src, V2s16* dest, u16 numv);


#endif // _MATHS3D_H_
````

## File: inc/memory_base.h
````c
/**
 *  \file memory_base.h
 *  \brief Memory base definition
 *  \author Stephane Dallongeville
 *  \date 06/2022
 *
 * This unit provides basic memory address definitions.<br>
 * We keep this H unit separated so it could eventually be included / used in assembly file if needed.
 */

#ifndef _MEMORY_BASE_H_
#define _MEMORY_BASE_H_

/**
 *  \brief
 *      Define start of ROM region
 */
#define ROM      0x00000000
/**
 *  \brief
 *      Define start of RAM region
 */
#define RAM      0xE0FF0000

/**
 *  \brief
 *      Define memory allocated for stack (default = 0xA00)
 */
#define STACK_SIZE      0x0A00
/**
 *  \brief
 *      Define the memory high address limit for dynamic allocation
 */
#define MEMORY_HIGH     (0xE1000000 - STACK_SIZE)


#endif // _MEMORY_BASE_H_
````

## File: inc/memory.h
````c
/**
 *  \file memory.h
 *  \brief Memory handling methods
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides memory copy/set operation and dynamic memory allocation.<br>
 *<br>
 * <b>Memory organization :</b><br>
 *<br>
 * Memory is composed of bloc, the first 2 bytes of a bloc define its size and its state:<br>
 * b15-b1 = size in number of word (2 bytes)<br>
 * b0 = used state (1=used, 0=free)
 *<br>
 * To reach the next bloc you just need to do:<br>
 * <code>next_bloc_address = bloc_addres + bloc_size</code>
 * The end of memory is defined with a 0 sized bloc.<br>
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_

// asics memory address definitions
#include "memory_base.h"

/**
 *  \brief
 *      Get u32 from u8 array (BigEndian order).
 */
#define GET_DWORDFROMPBYTE(src)     ((src[0] << 24) | (src[1] << 16) | (src[2] << 8) | (src[3] << 0))
/**
 *  \brief
 *      Get u32 from u8 array (LittleEndian order).
 */
#define GET_DWORDFROMPBYTE_LI(src)  ((src[0] << 0) | (src[1] << 8) | (src[2] << 16) | (src[3] << 24))
/**
 *  \brief
 *      Get u16 from u8 array (BigEndian order).
 */
#define GET_WORDFROMPBYTE(src)      ((src[0] << 8) | (src[1] << 0))
/**
 *  \brief
 *      Get u16 from u8 array (LittleEndian order).
 */
#define GET_WORDFROMPBYTE_LI(src)   ((src[0] << 0) | (src[1] << 8))
/**
 *  \brief
 *      Get u32 from u16 array (BigEndian order).
 */
#define GET_DWORDFROMPWORD(src)     ((src[0] << 16) | (src[1] << 0))
/**
 *  \brief
 *      Get u32 from u16 array (LittleEndian order).
 */
#define GET_DWORDFROMPWORD_LI(src)  ((src[0] << 0) | (src[1] << 16))


/**
 *  \brief
 *      Exchange value of specified u8 variables.
 */
#define SWAP_u8(x, y)       \
{                           \
    u8 swp;                 \
                            \
    swp = x;                \
    x = y;                  \
    y = swp;                \
}

/**
 *  \brief
 *      Exchange value of specified s8 variables.
 */
#define SWAP_s8(x, y)       \
{                           \
    s8 swp;                 \
                            \
    swp = x;                \
    x = y;                  \
    y = swp;                \
}

/**
 *  \brief
 *      Exchange value of specified u16 variables.
 */
#define SWAP_u16(x, y)      \
{                           \
    u16 swp;                \
                            \
    swp = x;                \
    x = y;                  \
    y = swp;                \
}

/**
 *  \brief
 *      Exchange value of specified s16 variables.
 */
#define SWAP_s16(x, y)      \
{                           \
    s16 swp;                \
                            \
    swp = x;                \
    x = y;                  \
    y = swp;                \
}

/**
 *  \brief
 *      Exchange value of specified u32 variables.
 */
#define SWAP_u32(x, y)      \
{                           \
    u32 swp;                \
                            \
    swp = x;                \
    x = y;                  \
    y = swp;                \
}

/**
 *  \brief
 *      Exchange value of specified s32 variables.
 */
#define SWAP_s32(x, y)      \
{                           \
    s32 swp;                \
                            \
    swp = x;                \
    x = y;                  \
    y = swp;                \
}


#if (ENABLE_NEWLIB == 0)
// enable standard libc compatibility
#define malloc(x)       MEM_alloc(x)
#define free(x)         MEM_free(x)
#endif  // ENABLE_NEWLIB


/**
 *  \brief
 *      Return available memory in bytes
 */
u16  MEM_getFree(void);
/**
 *  \brief
 *      Return allocated memory in bytes
 */
u16  MEM_getAllocated(void);
/**
 *  \brief
 *      Return largest free memory block in bytes
 */
u16  MEM_getLargestFreeBlock(void);

/**
 *  \brief
 *      Deallocate space in memory
 *
 *  \param ptr
 *      Pointer to a memory block previously allocated with Mem_alloc to be deallocated.<br>
 *      If a null pointer is passed as argument, no action occurs.
 *
 * A block of memory previously allocated using a call to Mem_alloc is deallocated, making it available again for further allocations.
 * Notice that this function leaves the value of ptr unchanged, hence it still points to the same (now invalid) location, and not to the null pointer.
 */
void MEM_free(void* ptr);
/**
 *  \brief
 *      Allocate memory block
 *
 *  \param size
 *      Number of bytes to allocate
 *  \return
 *      On success, a pointer to the memory block allocated by the function.
 *      The type of this pointer is always void*, which can be cast to the desired type of data pointer in order to be dereferenceable.
 *      If the function failed to allocate the requested block of memory (or if specified size = 0), a <i>NULL</i> pointer is returned.
 *
 * Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 * The content of the newly allocated block of memory is not initialized, remaining with indeterminate values.
 */
void* MEM_alloc(u16 size);
/**
 *  \brief
 *      Allocate memory block at a specific address (useful for short addressing or fixed low level working address)
 *
 *  \param addr
 *      Address where we want to allocate memory
 *  \param size
 *      Number of bytes to allocate
 *  \return
 *      On success, a pointer to the memory block allocated by the function (should match the <i>addr</i> parameter).
 *      The type of this pointer is always void*, which can be cast to the desired type of data pointer in order to be dereferenceable.
 *      If the function failed to allocate the requested block of memory (or if specified size = 0), a <i>NULL</i> pointer is returned.
 *
 * Allocates a block of size bytes of memory at the given address, returning a pointer to the beginning of the block (equal to the address here).
 * The content of the newly allocated block of memory is not initialized, remaining with indeterminate values.
 */
 void* MEM_allocAt(u32 addr, u16 size);

/**
 *  \brief
 *      Pack all free blocks and reset allocation search from start of heap.<br>
 *      You can call this method before trying to allocate small block of memory to reduce memory fragmentation.<br>
 *      Note that since SGDK 2.12 the memory manager is doing "auto packing" operation so you don't need to call this method anymore.
 */
void MEM_pack(void);

/**
 *  \brief
 *      Performs a integrity test on memory manager and stack pointer.<br>
 *      Note that the method may not detect all possible corruption but if it fails then for sure
 *      you have a memory corruption somewhere.
 *
 * Return FALSE if the test integrity failed (some logs are generated)
 */
bool MEM_checkIntegrity();
/**
 *  \brief
 *      Show memory dump
 *
 * Do a memory dump in GensKMod console (useful for debugging).
 */
void MEM_dump(void);

#if (ENABLE_NEWLIB == 0)
/**
 *  \brief
 *      Fill block of memory
 *
 *  \param to
 *      Pointer to the block of memory to fill.
 *  \param value
 *      Value to be set.
 *  \param len
 *      Number of u8 (byte) to be set to the value.
 *
 * Sets the first num bytes of the block of memory pointed by to with the specified value.
 */
void memset(void* to, u8 value, u16 len);
#endif  // ENABLE_NEWLIB

/**
 *  \brief
 *      Fill block of memory (optimized for u16)
 *
 *  \param to
 *      Pointer to the block of memory to fill.
 *  \param value
 *      Value to be set.
 *  \param len
 *      Number of (u16) short to be set to the value.
 *
 * Sets the first num shorts of the block of memory pointed by to with the specified value.
 */
void memsetU16(u16* to, u16 value, u16 len);
/**
 *  \brief
 *      Fill block of memory (optimized for u32)
 *
 *  \param to
 *      Pointer to the block of memory to fill.
 *  \param value
 *      Value to be set.
 *  \param len
 *      Number of u32 (long) to be set to the value.
 *
 * Sets the first num longs of the block of memory pointed by to with the specified value.
 */
void memsetU32(u32* to, u32 value, u16 len);

#if (ENABLE_NEWLIB == 0)
/**
 *  \brief
 *      Copy block of memory
 *
 *  \param to
 *      Pointer to the destination array where the content is to be copied, type-casted to a pointer of type void*.
 *  \param from
 *      Pointer to the source of data to be copied, type-casted to a pointer of type void*.
 *  \param len
 *      Number of bytes to copy.
 *
 * Copies the values of len long from the location pointed by from directly to the memory block pointed by to.
 * The underlying type of the objects pointed by both the source and destination pointers are irrelevant for this function
 * as the result is a binary copy of the data.
 */
void memcpy(void* to, const void* from, u16 len);

/**
 *  \brief
 *      Compare 2 blocks of memory
 *
 *  \param pointer1
 *      Pointer of the first block of memory
 *  \param pointer2
 *      Pointer of the second block of memory
 *  \param len
 *      Number of bytes to compare.
 *
 * Compares the first <i>len</i> bytes of the memory blocks pointed to by <i>pointer1</i> and <i>pointer2</i>. The comparison is done lexicographically.
 * The sign of the result is the sign of the difference between the values of the first pair of bytes (both interpreted as unsigned char)
 * that differ in the objects being compared. 
 */
s8 memcmp(const void* pointer1, const void* pointer2, size_t len);
#endif  // ENABLE_NEWLIB

/**
 *  \deprecated Uses memcpy(void *to, const void *from, u16 len) instead.
 */
#define memcpyU16(to, from, len)        _Pragma("GCC error \"This method is deprecated, use memcpy(..) instead.\"")
/**
 *  \deprecated Uses memcpy(void *to, const void *from, u16 len) instead.
 */
#define memcpyU32(to, from, len)        _Pragma("GCC error \"This method is deprecated, use memcpy(..) instead.\"")
/**
 *  \deprecated Uses memset(void *to, u8 value, u16 len) instead.
 */
#define fastMemset(to, value, len)      _Pragma("GCC error \"This method is deprecated, use memset(..) instead.\"")
/**
 *  \deprecated Uses memsetU16(void *to, u16 value, u16 len) instead.
 */
#define fastMemsetU16(to, value, len)   _Pragma("GCC error \"This method is deprecated, use memsetU16(..) instead.\"")
/**
 *  \deprecated Uses memsetU32(void *to, u32 value, u16 len) instead.
 */
#define fastMemsetU32(to, value, len)   _Pragma("GCC error \"This method is deprecated, use memsetU32(..) instead.\"")
/**
 *  \deprecated Uses memcpy(void *to, const void *from, u16 len) instead.
 */
#define astMemcpy(to, from, len)        _Pragma("GCC error \"This method is deprecated, use memcpy(..) instead.\"")
/**
 *  \deprecated Uses memcpy(void *to, const void *from, u16 len) instead.
 */
#define fastMemcpyU16(to, from, len)    _Pragma("GCC error \"This method is deprecated, use memcpy(..) instead.\"")
/**
 *  \deprecated Uses memcpy(void *to, const void *from, u16 len) instead.
 */
#define fastMemcpyU32(to, from, len)    _Pragma("GCC error \"This method is deprecated, use memcpy(..) instead.\"")


#endif // _MEMORY_H_
````

## File: inc/object.h
````c
/**
 *  \file object.h
 *  \brief Base object management unit.
 *  \author Stephane Dallongeville
 *  \date 02/2022
 *
 * This unit provides methods to manage objects.<br>
 * It works in concert with the <i>pool.h</i> unit which provide dynamic object allocation.<br>
 * <br>
 * The idea of <i>Object</i> is that you can use it as base structure for your own object (entity, enemy, character, whatever you want..).<br>
 * To do that the idea is to declare your new object structure by embedding the Object into it can be anonymous) and always at the first position:<pre>
 * struct entity_
 * {
 *     Object;
 *     f32 posX;
 *     f32 posY;
 *     Sprite* sprite;
 *     ...
 * };</pre>
 *
 * Doing that your Entity structure can be used through OBJ_xxx methods.
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "pool.h"


/**
 * \brief Allocated state for object (mainly useful for debugging)
 */
#define OBJ_ALLOCATED       0x8000



// forward
typedef struct Object_ Object;

/**
 *  \brief
 *      Object function callback
 *
 *  \param obj
 *      Object
 */
typedef void ObjectCallback(Object* obj);

/**
 *  \brief
 *      Base object structure
 *
 *  \param internalState
 *      Object internal state, you can use it but you should save bit 15 as it's used internally to detect invalid object
 *  \param type
 *      Object type, can be used to recognize the underlying object / structure type.
 *  \param init
 *      Initialisation function callback, should be only called once after object creation
 *  \param update
 *      Update function callback, usually called once per frame
 *  \param end
 *      Ending function callback, should be only called once before object release
 */
typedef struct Object_
{
    u16 internalState;
    u16 type;
    ObjectCallback* init;
    ObjectCallback* update;
    ObjectCallback* end;
} Object;


/**
 *  \brief
 *      Create and allocate the new object pool (this method is an alias of #POOL_create(..))
 *
 *  \param size
 *      the capacity of the pool (in number of object)
 *  \param objectSize
 *      the size of a single object (usually you should use sizeof(Struct) here)
 *
 *  \return the new created object pool or NULL if there is not enough memory available for that.
 *
 *  \see POOL_create(..)
 */
Pool* OBJ_createObjectPool(u16 size, u16 objectSize);

/**
 *  \brief
 *      Create a new objet from the given object pool (object must extend basic #Object structure)
 *
 *  \param pool
 *      Object pool to allocate from (see pool.h unit)
 *
 *  \return the created object or NULL if an error occured (no more available object in pool or invalid pool).
 *  The returned object is initialized to 0
 *
 *  \see OBJ_release(..)
 */
Object* OBJ_create(Pool* pool);
/**
 *  \brief
 *      Release an objet from the given object pool (object must extend basic #Object structure)
 *
 *  \param pool
 *      Object pool allocator to release from (see pool.h unit)
 *  \param object
 *      Object to release (must extend basic #Object structure)
 *  \param maintainCoherency
 *      set it to <i>TRUE</i> if you want to keep coherency for stack iteration and use #OBJ_updateAll().<br>
 *      Set it to <i>FALSE</i> for faster release process if you don't require object iteration through alloc stack.
 *
 *  \see OBJ_create(..)
 *  \see OBJ_updateAll(..)
 */
void OBJ_release(Pool* pool, Object* object, bool maintainCoherency);

/**
 *  \brief
 *      Iterate over all active objects from the given object pool and call <i>update</i> method for each of them.<br>
 *
 *  \param pool
 *      Object pool to update all objects from.
 *
 *  \warning You need to always set 'maintainCoherency' to <i>TRUE</i> when using #OBJ_release(..) otherwise stack iteration won't work correctly.
 */
void OBJ_updateAll(Pool* pool);

/**
 *  \brief
 *      Set the initialization method for the given object
 *
 *  \param object
 *      Object to set <i>init</i> method for
 *  \param initMethod
 *      the method to set for object initialization (should be called after object creation)
 *
 *  \see OBJ_setUpdateMethod(..)
 *  \see OBJ_setEndMethod(..)
 */
void OBJ_setInitMethod(Object* object, ObjectCallback* initMethod);
/**
 *  \brief
 *      Set the update method for the given object
 *
 *  \param object
 *      Object to set <i>update</i> method for
 *  \param updateMethod
 *      the method to set for object update (should be called once per frame)
 *
 *  \see OBJ_setInitMethod(..)
 *  \see OBJ_setEndMethod(..)
 */
void OBJ_setUpdateMethod(Object* object, ObjectCallback* updateMethod);
/**
 *  \brief
 *      Set the ending method for the given object
 *
 *  \param object
 *      Object to set <i>end</i> method for
 *  \param endMethod
 *      the method to set for object destruction (should be called just before object is released)
 *
 *  \see OBJ_setInitMethod(..)
 *  \see OBJ_setUpdateMethod(..)
 */
void OBJ_setEndMethod(Object* object, ObjectCallback* endMethod);


#endif // _OBJECT_H_
````

## File: inc/pal.h
````c
/**
 *  \file pal.h
 *  \brief Palette support (herited from vdp_pal.h unit)
 *  \author Stephane Dallongeville
 *  \date 06/2019
 *
 * This unit provides methods to manipulate the VDP Color Palette.<br>
 * The Sega Genesis VDP has 4 palettes of 16 colors.<br>
 * Color is defined with 3 bits for each component : xxxxBBBxGGGxRRRx
 */

#include "config.h"
#include "types.h"
#include "dma.h"

#ifndef _PAL_H_
#define _PAL_H_

#define VDPPALETTE_REDSFT           1
#define VDPPALETTE_GREENSFT         5
#define VDPPALETTE_BLUESFT          9

#define VDPPALETTE_REDMASK          0x000E
#define VDPPALETTE_GREENMASK        0x00E0
#define VDPPALETTE_BLUEMASK         0x0E00
#define VDPPALETTE_COLORMASK        0x0EEE

/**
 *  \brief
 *      Convert a RGB 24 bits color to VDP color
 *
 *  \param color
 *      RGB 24 bits color
 */
#define RGB24_TO_VDPCOLOR(color)    ((((((color) + 0x100000) < 0xFF0000 ? (color) + 0x100000 : 0xFF0000) >> (20)) & VDPPALETTE_REDMASK) | ((((((color) & 0xff00) + 0x1000) < 0xFF00 ? ((color) & 0xff00) + 0x1000 : 0xFF00) >> ((1 * 4) + 4)) & VDPPALETTE_GREENMASK) | ((((((color) & 0xff) + 0x10) < 0xFF ? ((color) & 0xff) + 0x10 : 0xFF) << 4) & VDPPALETTE_BLUEMASK))

/**
 *  \brief
 *      Convert a RGB333 color to VDP color (VDP uses RGB333 internally)
 *
 *  \param r
 *      Red intensity (0-7)
 *  \param g
 *      Green intensity (0-7)
 *  \param b
 *      Blue intensity (0-7)
 */
#define RGB3_3_3_TO_VDPCOLOR(r, g, b) ((((r) & 7) << VDPPALETTE_REDSFT) | (((g) & 7) << VDPPALETTE_GREENSFT) | (((b) & 7) << VDPPALETTE_BLUESFT))

/**
 *  \brief
 *      Convert a RGB888 color to VDP color (VDP uses RGB333 internally)
 *
 *  \param r
 *      Red intensity (0-255)
 *  \param g
 *      Green intensity (0-255)
 *  \param b
 *      Blue intensity (0-255)
 */
#define RGB8_8_8_TO_VDPCOLOR(r, g, b) RGB24_TO_VDPCOLOR(((((b) << 0) & 0xFF) | (((g) & 0xFF) << 8) | (((r) & 0xFF) << 16)))

/**
 *  \brief
 *      Palette structure contains color data.
 *
 *  \param length
 *      Size of this palette.
 *  \param dat
 *      Color data.
 */
typedef struct
{
    u16 length;
    u16* data;
} Palette;


/**
 *  \brief
 *      Default black palette.
 */
extern const u16* const palette_black;
/**
 *  \brief
 *      Default grey palette.
 */
extern const u16 palette_grey[16];
/**
 *  \brief
 *      Default red palette.
 */
extern const u16 palette_red[16];
/**
 *  \brief
 *      Default green palette.
 */
extern const u16 palette_green[16];
/**
 *  \brief
 *      Default blue palette.
 */
extern const u16 palette_blue[16];

/**
 *  \brief
 *      Current fade palette
 */
extern u16 fadeCurrentPal[64];
/**
 *  \brief
 *      End fade palette
 */
extern u16 fadeEndPal[64];

/**
 *  \brief
 *      Returns RGB color value from CRAM for the specified palette entry.
 *
 *  \param index
 *      Color index (0-63).
 *  \return RGB intensity for the specified color index.
 */
u16  PAL_getColor(u16 index);
/**
 *  \brief
 *      Read count RGB colors from CRAM starting at specified index and store them in specified destination palette.
 *
 *  \param index
 *      Color index where start to read (0-63).
 *  \param dest
 *      Destination palette where to write read the RGB color values (should be large enough to store count colors).
 *  \param count
 *      Number of color to get.
 */
void  PAL_getColors(u16 index, u16* dest, u16 count);
/**
 *  \brief
 *      Get a complete palette (16 colors) from CRAM.
 *
 *  \param numPal
 *      Palette number: PAL0, PAL1, PAL2 or PAL3
 *  \param dest
 *      Destination where to write palette colors (should be 16 words long at least)
 */
void PAL_getPalette(u16 numPal, u16* dest);

/**
 *  \brief
 *      Set RGB color into CRAM for the specified palette entry.
 *
 *  \param index
 *      Color index to set (0-63).
 *  \param value
 *      RGB intensity to set at the specified color index.
 */
void PAL_setColor(u16 index, u16 value);
/**
 *  \brief
 *      Write RGB colors into CRAM for the specified palette entries.
 *
 *  \param index
 *      Color index where to start to write (0-63).
 *  \param pal
 *      RGB intensities to set.
 *  \param count
 *      Number of color to set.
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 */
void PAL_setColors(u16 index, const u16* pal, u16 count, TransferMethod tm);
/**
 *  \brief
 *      Write the given Palette RGB colors into CRAM for the specified palette entries.
 *
 *  \param index
 *      Color index where to start to write (0-63).
 *  \param pal
 *      Source Palette.
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 */
void PAL_setPaletteColors(u16 index, const Palette* pal, TransferMethod tm);
/**
 *  \brief
 *      Set a complete palette (16 colors) into CRAM.
 *
 *  \param numPal
 *      Palette number: PAL0, PAL1, PAL2 or PAL3
 *  \param pal
 *      Source palette.
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 */
void PAL_setPalette(u16 numPal, const u16* pal, TransferMethod tm);

/**
 *  \deprecated
 *      Use PAL_setColors(..) instead
 */
#define PAL_setColorsDMA(index, pal, count)     _Pragma("GCC error \"This method is deprecated, use PAL_setColors(..) instead.\"")
/**
 *  \deprecated
 *      Use PAL_setPaletteColors(..) instead
 */
#define PAL_setPaletteColorsDMA(index, pal)     _Pragma("GCC error \"This method is deprecated, use PAL_setPaletteColors(..) instead.\"")
/**
 *  \deprecated
 *      Use PAL_setPalette(..) instead
 */
#define PAL_setPaletteDMA(numPal, pal)          _Pragma("GCC error \"This method is deprecated, use PAL_setPalette(..) instead.\"")

/**
 *  \brief
 *      Initialize a fading operation that will be manually controlled through #PAL_doFadeStep() calls
 *      IMPORTANT: note that start palette is actually updated on next SYS_doVBlankProcess() call
 *
 *  \param fromCol
 *      Start color index for the fade operation (0-63).
 *  \param toCol
 *      End color index for the fade operation (0-63 and >= fromCol).
 *  \param palSrc
 *      Fade departure palette.
 *  \param palDst
 *      Fade arrival palette.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *
 * \see PAL_doFadeStep(..)
 * \see PAL_isManualFadeDone(..)
 */
bool PAL_initFade(u16 fromCol, u16 toCol, const u16* palSrc, const u16* palDst, u16 numFrame);
/**
 *  \brief
 *      Update palette to process one frame/iteration of current fade operation (see #PAL_initFade(..) method)
 *      IMPORTANT: note that palette is actually updated on next SYS_doVBlankProcess() call
 *
 *  \return TRUE if fading operation is not yet complete, FALSE otherwise
 *
 * \see PAL_initFade(..)
 * \see PAL_isManualFadeDone(..)
 */
bool PAL_doFadeStep(void);
/**
 *  \brief
 *      Returns TRUE if the *manual* fading operation is complete, FALSE otherwise.<br>
 *      WARNING: This method is not related to #PAL_isDoingFade() which is about *asynchronous* fading process.
 *
 * \see PAL_initFade(..)
 * \see PAL_doFadeStep(..)
 */
bool PAL_isManualFadeDone(void);

/**
 *  \brief
 *      General palette fading effect.
 *
 *  \param fromCol
 *      Start color index for the fade effect (0-63).
 *  \param toCol
 *      End color index for the fade effect (0-63 and >= fromCol).
 *  \param palSrc
 *      Fade departure palette.
 *  \param palDst
 *      Fade arrival palette.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  This function does general palette fading effect.<br>
 *  The fade operation is done to all palette entries between 'fromCol' and 'toCol'.<br>
 *  Example: fading to all palette entries --> fromCol = 0  and  toCol = 63
 */
void PAL_fade(u16 fromCol, u16 toCol, const u16* palSrc, const u16* palDst, u16 numFrame, bool async);
/**
 *  \brief
 *      Fade current color palette to specified one.
 *
 *  \param fromCol
 *      Start color index for the fade operation (0-63).
 *  \param toCol
 *      End color index for the fade operation (0-63 and >= fromCol).
 *  \param pal
 *      Fade arrival palette.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  See PAL_fade() for more informations.
 */
void PAL_fadeTo(u16 fromCol, u16 toCol, const u16* pal, u16 numFrame, bool async);
/**
 *  \brief
 *      Fade out (current color to black) effect.
 *
 *  \param fromCol
 *      Start color index for the fade operation (0-63).
 *  \param toCol
 *      End color index for the fade operation (0-63 and >= fromCol).
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  See PAL_fade() for more informations.
 */
void PAL_fadeOut(u16 fromCol, u16 toCol, u16 numFrame, bool async);
/**
 *  \brief
 *      Fade in (black to specified color) effect.
 *
 *  \param fromCol
 *      Start color index for the fade operation (0-63).
 *  \param toCol
 *      End color index for the fade operation (0-63 and >= fromCol).
 *  \param pal
 *      Fade arrival palette.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  See PAL_fade() for more informations.
 */
void PAL_fadeIn(u16 fromCol, u16 toCol, const u16* pal, u16 numFrame, bool async);

/**
 *  \brief
 *      Do palette fade effect.
 *
 *  \param numPal
 *      Palette number to use for fade effect.
 *  \param palSrc
 *      Fade departure palette.
 *  \param palDst
 *      Fade arrival palette.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  The fade operation is done to all specified palette entries.<br>
 *  See PAL_fade() for more informations.
 */
void PAL_fadePalette(u16 numPal, const u16* palSrc, const u16* palDst, u16 numFrame, bool async);
/**
 *  \brief
 *      Fade current palette to specified one.
 *
 *  \param numPal
 *      Palette to fade.
 *  \param pal
 *      Fade arrival palette.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  See PAL_fadePal() for more informations.
 */
void PAL_fadeToPalette(u16 numPal, const u16* pal, u16 numFrame, bool async);
/**
 *  \brief
 *      Fade out (current color to black) effect.
 *
 *  \param numPal
 *      Palette to fade.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  See PAL_fadePal() for more informations.
 */
void PAL_fadeOutPalette(u16 numPal, u16 numFrame, bool async);
/**
 *  \brief
 *      Fade in (black to specified color) effect.
 *
 *  \param numPal
 *      Palette to fade.
 *  \param pal
 *      Fade arrival palette.
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  See PAL_fadePal() for more informations.
 */
void PAL_fadeInPalette(u16 numPal, const u16* pal, u16 numFrame, bool async);

/**
 *  \brief
 *      Global palette fading effect.
 *
 *  \param palSrc
 *      Fade departure palette (should contains 64 colors entries).
 *  \param palDst
 *      Fade arrival palette (should contains 64 colors entries).
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  The fade operation is done to all palette entries.
 */
void PAL_fadeAll(const u16* palSrc, const u16* palDst, u16 numFrame, bool async);
/**
 *  \brief
 *      Palettes fade to specified one.
 *
 *  \param pal
 *      Fade arrival palette (should contains 64 entries).
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  The fade operation is done to all palette entries.<br>
 *  See PAL_fadeAll().
 */
void PAL_fadeToAll(const u16* pal, u16 numFrame, bool async);
/**
 *  \brief
 *      Fade out (current color to black) effect.
 *
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  The fade operation is done to all palette entries.<br>
 *  See PAL_fadeAll().
 */
void PAL_fadeOutAll(u16 numFrame, bool async);
/**
 *  \brief
 *      Fade in (black to specified color) effect.
 *
 *  \param pal
 *      Fade arrival palette (should contains 64 entries).
 *  \param numFrame
 *      Duration of palette fading in number of frame.
 *  \param async
 *      Async process.<br>
 *      If set the function return immediatly else the function wait for fading to complete.
 *
 *  The fade operation is done to all palette entries.<br>
 *  See PAL_fadeAll().
 */
void PAL_fadeInAll(const u16* pal, u16 numFrame, bool async);

/**
 *  \brief
 *      Returns TRUE if we are currently doing an asynchronous palette fading operation.<br>
 *      WARNING: This method is not related to #PAL_isManualFadeDone() which is about *manual fading* processed with #PAL_doFadeStep()
 */
bool PAL_isDoingFade(void);
/**
 *  \brief
 *      Wait for (asynchronous) palette fading operation to complete.
 */
void PAL_waitFadeCompletion(void);
/**
 *  \brief
 *      Interrupt any asynchronous palette fading effect.
 */
void PAL_interruptFade(void);


#endif // _VDP_PAL_H_
````

## File: inc/pool.h
````c
/**
 *  \file pool.h
 *  \brief Pool object management unit.
 *  \author Stephane Dallongeville
 *  \date 02/2022
 *
 * This unit provides methods to manage dynamic object allocation.<br>
 * <br>
 * You can use <i>Pool</i> object to handle dynamic allocation from a fixed set of objects.<br>
 * For instance if you may need to handle dynamically bullets for your game and you want to have
 * at max 20 bullets, you can handle it that way:<pre>
 * Pool* bulletPool = POOL_create(20, sizeof(Bullet));
 * ...
 * // create a new bullet
 * Bullet* bullet = POOL_allocate(bulletPool);
 * // check if bullet was correctly created and do your stuff..
 * if (bullet != NULL)
 * {
 *    ...
 * }
 * ...
 * // release your bullet
 * POOL_release(bulletPool, bullet);
 * </pre>
 * <i>Pool</i> object is also very useful for fast iteration over allocated objects:<pre>
 * Bullet** bullets = POOL_getFirst(bulletPool);
 * u16 num = POOL_getNumAllocated(bulletPool);
 * while(num--)
 * {
 *    Bullet* bullet = *bullets++;
 *    // do whatever you need on your bullet
 *    ...
 * }</pre>
 *
 */

#ifndef _POOL_H_
#define _POOL_H_


/**
 *  \brief
 *      Object pool allocator structure
 *
 *  \param bank
 *      bank data
 *  \param allocStack
 *      allocation stack used for fast allocate / release operation
 *  \param free
 *      point on first available object in the allocation stack
 *  \param objectSize
 *      size of a single object (in bytes)
 *  \param size
 *      size of the object pool (in number of object)
 */
typedef struct
{
    void* bank;
    void** allocStack;
    void** free;
    u16 objectSize;
    u16 size;
} Pool;


/**
 *  \brief
 *      Create and allocate a new object pool allocator
 *
 *  \param size
 *      the capacity of the pool (in number of object)
 *  \param objectSize
 *      the size of a single object (usually you should use sizeof(Struct) here, always aligned on 2)
 *
 *  \return the new created object pool or NULL if there is not enough memory available for that.
 *
 *  \see POOL_destroy(..)
 *  \see POOL_reset(..)
 */
Pool* POOL_create(u16 size, u16 objectSize);
/**
 *  \brief
 *      Release the specified object pool allocator
 *
 *  \param pool
 *      Object pool allocator to release
 *
 *  \see POOL_create(..)
 */
void POOL_destroy(Pool* pool);

/**
 *  \brief
 *      Reset the 'object' pool allocator
 *
 *  \param pool
 *      Object pool allocator to reset
 *  \param clear
 *      if set to TRUE then objects memory is cleared (initialized to 0)
 */
void POOL_reset(Pool* pool, bool clear);

/**
 *  \brief
 *      Allocate a new 'object' from the specified object pool
 *
 *  \param pool
 *      Object pool allocator
 *
 *  \return the allocated object or NULL if an error occured (no more available object in pool or invalid pool)
 *
 *  \see POOL_release(..)
 */
void* POOL_allocate(Pool* pool);
/**
 *  \brief
 *      Release an objet from the specified object pool
 *
 *  \param pool
 *      Object pool allocator
 *  \param object
 *      Object to release
 *  \param maintainCoherency
 *      set it to <i>TRUE</i> if you want to keep coherency for stack iteration (see #POOL_getFirst()).<br>
 *      Set it to <i>FALSE</i> for faster release process if you don't require object iteration through alloc stack.
 *
 *  \see POOL_allocate(..)
 */
void POOL_release(Pool* pool, void* object, bool maintainCoherency);

/**
 *  \return
 *      the number of free object in the pool
 *
 *  \param pool
 *      Object pool allocator
 */
u16 POOL_getFree(Pool* pool);
/**
 *  \return
 *      the number of allocated object in the pool
 *
 *  \param pool
 *      Object pool allocator
 */
u16 POOL_getNumAllocated(Pool* pool);

/**
 *  \return
 *      the start position of allocated objects in the alloc stack (useful to iterate over all allocated objects)
 *
 *  \param pool
 *      Object pool allocator
 *
 *  \see POOL_getNumAllocated(..)
 */
void** POOL_getFirst(Pool* pool);

/**
 *  \return
 *      the position of an object in the alloc stack or -1 if the object isn't found (useful for debug purpose mainly)
 *
 *  \param pool
 *      Object pool allocator
 *  \param object
 *      Object to get slot position
 */
s16 POOL_find(Pool* pool, void* object);


#endif // _POOL_H_
````

## File: inc/psg.h
````c
/**
 *  \file psg.h
 *  \brief PSG support
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides access to the PSG through the 68000 CPU
 */

#ifndef _PSG_H_
#define _PSG_H_

/**
 *  \brief
 *      PSG port address.
 */
#define PSG_PORT            0xC00011

/**
 *  \brief
 *      Minimum PSG envelope value.
 */
#define PSG_ENVELOPE_MIN    15
/**
 *  \brief
 *      Maximum PSG envelope value.
 */
#define PSG_ENVELOPE_MAX    0

/**
 *  \brief
 *      Periodic noise type (like low-frequency tone).
 */
#define PSG_NOISE_TYPE_PERIODIC 0
/**
 *  \brief
 *      White noise type (hiss).
 */
#define PSG_NOISE_TYPE_WHITE    1

/**
 *  \brief
 *      Noise frequency = PSG clock / 2 (less coarse).
 */
#define PSG_NOISE_FREQ_CLOCK2   0
/**
 *  \brief
 *      Noise frequency = PSG clock / 4.
 */
#define PSG_NOISE_FREQ_CLOCK4   1
/**
 *  \brief
 *      Noise frequency = PSG clock / 8 (more coarse).
 */
#define PSG_NOISE_FREQ_CLOCK8   2
/**
 *  \brief
 *      Noise frequency = Tone generator #3.
 */
#define PSG_NOISE_FREQ_TONE3    3


/**
 *  \deprecated use PSG_reset() instead
 */
#define PSG_init()  _Pragma("GCC error \"This method is deprecated, use PSG_reset() instead.\"")

/**
 *  \brief
 *      Reset PSG chip
 */
void PSG_reset(void);

/**
 *  \brief
 *      Write to PSG port.
 *
 *  \param data
 *      value to write to the port.
 *
 * Write the specified value to PSG data port.
 *
 */
void PSG_write(u8 data);

/**
 *  \brief
 *      Set envelope level.
 *
 *  \param channel
 *      Channel we want to set envelope (0-3).
 *  \param value
 *      Envelope level to set (#PSG_ENVELOPE_MIN - #PSG_ENVELOPE_MAX).
 *
 * Set envelope level for the specified PSG channel.
 */
void PSG_setEnvelope(u8 channel, u8 value);
/**
 *  \brief
 *      Set tone.
 *
 *  \param channel
 *      Channel we want to set tone (0-3).
 *  \param value
 *      Tone value to set (0-1023).
 *
 * Set direct tone value for the specified PSG channel.
 */
void PSG_setTone(u8 channel, u16 value);
/**
 *  \brief
 *      Partial set tone (low bit only b3-b0).
 *
 *  \param channel
 *      Channel we want to set tone (0-3).
 *  \param value
 *      Low bit (b3-b0) of tone value to set (0-15)
 *
 * Set low part of tone value for the specified PSG channel.
 */
void PSG_setToneLow(u8 channel, u8 value);
/**
 *  \brief
 *      Set frequency.
 *
 *  \param channel
 *      Channel we want to set frequency (0-3).
 *  \param value
 *      Frequency value to set in Hz (0-4095).
 *
 * Set frequency for the specified PSG channel.<br>
 * This method actually converts the specified frequency value in PSG tone value.
 */
void PSG_setFrequency(u8 channel, u16 value);
/**
 *  \brief
 *      Set noise type and frequency.
 *
 *  \param type
 *      Noise type, accepted values are:<br>
 *      #PSG_NOISE_TYPE_PERIODIC<br>
 *      #PSG_NOISE_TYPE_WHITE
 *  \param frequency
 *      Noise frequency, accepted values are:<br>
 *      #PSG_NOISE_FREQ_CLOCK2<br>
 *      #PSG_NOISE_FREQ_CLOCK4<br>
 *      #PSG_NOISE_FREQ_CLOCK8<br>
 *      #PSG_NOISE_FREQ_TONE3
 */
void PSG_setNoise(u8 type, u8 frequency);


#endif // _PSG_H_
````

## File: inc/sprite_eng_legacy.h
````c
/**
 *  \file sprite_eng_legacy.h
 *  \brief Sprite engine legacy - september 2022 version
 *  \author Stephane Dallongeville
 *  \date 10/2013
 *
 * Sprite engine providing advanced sprites manipulation and operations.<br>
 * This unit use both the vram memory manager (see vram.h file for more info)<br>
 * and the Sega Genesis VDP sprite capabilities (see vdp_spr.h file for more info).
 */

#if     LEGACY_SPRITE_ENGINE

#ifndef _SPRITE_ENG_LEGACY_H_
#define _SPRITE_ENG_LEGACY_H_

#include "vdp_tile.h"
#include "vdp_spr.h"
#include "pal.h"
#include "pool.h"


/**
 *  \brief
 *      No collision type
 */
#define COLLISION_TYPE_NONE     0
/**
 *  \brief
 *      Bouding box collision type (Box structure)
 */
#define COLLISION_TYPE_BOX      1
/**
 *  \brief
 *      Round circle collision type (Circle structure)
 */
#define COLLISION_TYPE_CIRCLE   2

/**
 *  \brief
 *      Special flag to indicate that we want to add the sprite at position 0 (head) in the list<br>
 *      instead of adding it in last position (default)
 */
#define SPR_FLAG_INSERT_HEAD                    0x4000
/**
 *  \brief
 *      Disable animation auto loop.<br>
 *      By default animation always restart after the last frame has been played.
 *      This flag prevent the animation to restart and so the animation end on the last frame forever (see #SPR_getAnimationDone(..))
 */
#define SPR_FLAG_DISABLE_ANIMATION_LOOP         0x2000
/**
 *  \brief
 *      Disable delaying of frame update when we are running out of DMA capacity.<br>
 *      By default we delay the frame update when DMA is already full so the frame update happen as soon we have enough DMA capacity to do it.
 *      This flag forces frame update to always happen immediately but that may cause graphical glitches.
 */
#define SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE   0x0080
/**
 *  \brief
 *      Enable automatic VRAM allocation
 */
#define SPR_FLAG_AUTO_VRAM_ALLOC                0x1000
/**
 *  \brief
 *      Enable automatic hardware sprite allocation
 */
#define SPR_FLAG_AUTO_SPRITE_ALLOC              0x0800
/**
 *  \brief
 *      Enable automatic upload of sprite tiles data into VRAM
 */
#define SPR_FLAG_AUTO_TILE_UPLOAD               0x0400
/**
 *  \brief
 *      Enable automatic visibility calculation
 */
#define SPR_FLAG_AUTO_VISIBILITY                0x0200
/**
 *  \brief
 *      Enable fast visibility calculation (only meaningful if SPR_FLAG_AUTO_VISIBILITY is used).<br>
 *      If you set this flag the automatic visibility calculation will be done globally for the (meta) sprite and not per internal
 *      hardware sprite. This result in faster visibility computation at the expense of some waste of hardware sprite.
 */
#define SPR_FLAG_FAST_AUTO_VISIBILITY           0x0100

/**
 *  \brief
 *      Mask for sprite flag
 */
#define SPR_FLAG_MASK                           (SPR_FLAG_INSERT_HEAD | SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_SPRITE_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_FAST_AUTO_VISIBILITY)

/**
 *  \brief
 *      Minimum depth for a sprite (always above others sprites)
 */
#define SPR_MIN_DEPTH       (-0x8000)
/**
 *  \brief
 *      Maximum depth for a sprite (always below others sprites)
 */
#define SPR_MAX_DEPTH       0x7FFF

/**
 *  \brief
 *      Sprite visibility enumeration
 */
typedef enum
{
    VISIBLE,        /**< Sprite is visible (no computation needed) */
    HIDDEN,         /**< Sprite is hidden (no computation needed) */
    AUTO_FAST,      /**< Automatic visibility calculation - FAST (computation made on global meta sprite) */
    AUTO_SLOW,      /**< Automatic visibility calculation - SLOW (computation made per hardware sprite) */
} SpriteVisibility;

/**
 *  \brief
 *      Simple Box structure for collision
 *
 *  \param x
 *      X position (left)
 *  \param y
 *      Y position (top)
 *  \param w
 *      width
 *  \param h
 *      heigth
 */
typedef struct
{
    s8 x;
    s8 y;
    u8 w;
    u8 h;
} BoxCollision;

/**
 *  \brief
 *      Simple Circle structure (can be used for collision detection)
 *
 *  \param x
 *      X center position
 *  \param y
 *      Y center position
 *  \param ray
 *      circle ray
 */
typedef struct
{
    s8 x;
    s8 y;
    u16 ray;
} CircleCollision;

/**
 *  \struct Collision
 *  \brief
 *      Collision definition union.
 *
 *  \param typeHit
 *      Collision type for hit collision:<br>
 *      Allowed values are #COLLISION_TYPE_BOX or #COLLISION_TYPE_CIRCLE.
 *  \param typeAttack
 *      Collision type for attack collision (can be used as alternative hit collision):<br>
 *      Allowed values are #COLLISION_TYPE_BOX or #COLLISION_TYPE_CIRCLE.
 *  \param box
 *      BoxCollision definition if type = #COLLISION_TYPE_BOX
 *  \param circle
 *      CircleCollision definition if type = #COLLISION_TYPE_CIRCLE
 *  \param inner
 *      if current collision is verified then we test inner for more precise collision if needed
 *  \param next
 *      if current collision is not verified then we test next for next collision if needed
 */
typedef struct _collision
{
    u8 typeHit;
    u8 typeAttack;
    union
    {
        BoxCollision box;
        CircleCollision circle;
    } hit;
    union
    {
        BoxCollision box;
        CircleCollision circle;
    } attack;
} Collision;

/**
 *  \brief
 *      Single VDP sprite info structure for sprite animation frame.
 *
 *  \param offsetY
 *      Y offset for this VDP sprite relative to global Sprite position
 *  \param offsetYFlip
 *      Y offset (flip version) for this VDP sprite relative to global Sprite position
 *  \param size
 *      sprite size (see SPRITE_SIZE macro)
 *  \param offsetX
 *      X offset for this VDP sprite relative to global Sprite position
 *  \param offsetXFlip
 *      X offset (flip version) for this VDP sprite relative to global Sprite position
 *  \param numTile
 *      number of tile for this VDP sprite (should be coherent with the given size field)
 */
typedef struct
{
    u8 offsetY;          // respect VDP sprite field order, may help
    u8 offsetYFlip;
    u8 size;
    u8 offsetX;
    u8 offsetXFlip;
    u8 numTile;
}  FrameVDPSprite;

/**
 *  \brief
 *      Sprite animation frame structure.
 *
 *  \param numSprite
 *      number of VDP sprite which compose this frame.
 *      bit 7 is used as a special flag for the sprite engine so always use 'numSprite & 0x7F' to just retrieve the number of sprite
 *  \param timer
 *      active time for this frame (in 1/60 of second)
 *  \param tileset
 *      tileset containing tiles for this animation frame (ordered for sprite)
 *  \param collision
 *      collision structure (not used currently)
 *  \param frameSprites
 *      array of VDP sprites info composing the frame
 */
typedef struct
{
    s8 numSprite;
    u8 timer;
    TileSet* tileset;                   // TODO: have a tileset per VDP sprite --> probably not a good idea performance wise
    Collision* collision;               // Require many DMA queue operations and fast DMA flush as well, also bring extra computing in calculating delayed update
    FrameVDPSprite frameVDPSprites[];
} AnimationFrame;

/**
 *  \brief
 *      Sprite animation structure.
 *
 *  \param numFrame
 *      number of different frame for this animation (max = 255)
 *  \param loop
 *      frame index for loop (last index if no loop)
 *  \param frames
 *      frames composing the animation
 */
typedef struct
{
    u8 numFrame;
    u8 loop;
    AnimationFrame** frames;
} Animation;

/**
 *  \brief
 *      Sprite definition structure.
 *
 *  \param w
 *      frame cell width in pixel
 *  \param h
 *      frame cell height in pixel
 *  \param palette
 *      default palette data
 *  \param numAnimation
 *      number of animation for this sprite
 *  \param animations
 *      animation definitions
 *  \param maxNumTile
 *      maximum number of tile used by a single animation frame (used for VRAM tile space allocation)
 *  \param maxNumSprite
 *      maximum number of VDP sprite used by a single animation frame (used for VDP sprite allocation)
 *
 *  Contains all animations for a Sprite and internal informations.
 */
typedef struct
{
    u16 w;
    u16 h;
    Palette* palette;
    u16 numAnimation;
    Animation** animations;
    u16 maxNumTile;
    u16 maxNumSprite;
} SpriteDefinition;

/**
 *  \brief
 *      Sprite structure used by the Sprite Engine to store state for a sprite.<br>
 *      WARNING: always use the #SPR_addSprite(..) method to allocate Sprite object.<br>
 *
 *  \param status
 *      Internal state and automatic allocation information (internal)
 *  \param visibility
 *      visibility information of current frame for each VDP sprite (max = 16)
 *  \param spriteDef
 *      Sprite definition pointer
 *  \param onFrameChange
 *      Custom callback on frame change event (see #SPR_setFrameChangeCallback(..) method)
 *  \param animation
 *      Animation pointer cache (internal)
 *  \param frame
 *      AnimationFrame pointer cache (internal)
 *  \param animInd
 *      current animation index (internal)
 *  \param frameInd
 *      current frame animation index (internal)
 *  \param timer
 *      timer for current frame (internal)
 *  \param x
 *      current sprite X position on screen offseted by 0x80 (internal VDP position)
 *  \param y
 *      current sprite Y position on screen offseted by 0x80 (internal VDP position)
 *  \param depth
 *      current sprite depth (Z) position used for Z sorting
 *  \param attribut
 *      sprite specific attribut and allocated VRAM tile index (see TILE_ATTR_FULL() macro)
 *  \param VDPSpriteIndex
 *      index of first allocated VDP sprite (0 when no yet allocated)<br>
 *      Number of allocated VDP sprite is defined by definition->maxNumSprite
 *  \param lastVDPSprite
 *      Pointer to last VDP sprite used by this Sprite (used internally to update link between sprite)
 *  \param lastNumSprite
 *      the number of VDP sprite used by the current frame (internal)
 *  \param spriteToHide
 *      internal
 *  \param data
 *      this is a free field for user data, use it for whatever you want (flags, pointer...)
 *  \param prev
 *      pointer on previous Sprite in list
 *  \param next
 *      pointer on next Sprite in list
 *
 *  Used to manage an active sprite in game condition.
 */
typedef struct Sprite
{
    u16 status;
    u16 visibility;
    const SpriteDefinition* definition;
    void (*onFrameChange)(struct Sprite* sprite);
    Animation* animation;
    AnimationFrame* frame;
    s16 animInd;
    s16 frameInd;
    s16 timer;
    s16 x;
    s16 y;
    s16 depth;
    u16 attribut;
    u16 VDPSpriteIndex;
    VDPSprite* lastVDPSprite;
    u16 lastNumSprite;
    s16 spriteToHide;
    u32 data;
    struct Sprite* prev;
    struct Sprite* next;
} Sprite;

/**
 *  \brief
 *      Sprite frame change event callback.<br>
 *
 *  \param sprite
 *      The sprite for which frame just changed.
 *
 *      This event occurs onframe change process during #SPR_update() call (CAUTION: sprite->status field is not up to date at this point).<br>
 *      It let opportunity to the developer to apply special behavior or process when sprite frame just changed:<br>
 *      for instance we can disable animation looping by setting sprite->timer to 0 when we meet the last animation frame.
 */
typedef void FrameChangeCallback(Sprite* sprite);

/**
 * Sprites object pool for the sprite engine
 */
extern Pool* spritesPool;
/**
 * First allocated sprite (NULL if no sprite allocated)
 */
extern Sprite* firstSprite;
/**
 * Last allocated sprite (NULL if no sprite allocated)
 */
extern Sprite* lastSprite;
/**
 * Allocated VRAM (in tile) for Sprite Engine
 */
extern u16 spriteVramSize;


/**
 *  \brief
 *      Initialize the Sprite engine with default parameters.
 *
 *      Initialize the sprite engine using default parameters (420 reserved tiles in VRAM).<br>
 *      This also initialize the hardware sprite allocation system.
 *
 *  \see SPR_initEx(void)
 *  \see SPR_end(void)
 */
void SPR_init(void);
/**
 *  \brief
 *      Init the Sprite engine with specified advanced parameters (VRAM allocation size and decompression buffer size).
 *
 *  \param vramSize
 *      size (in tile) of the VRAM region for the automatic VRAM tile allocation.<br>
 *      If set to 0 the default size is used (420 tiles)
 *
 *      Initialize the sprite engine.<br>
 *      This allocates a VRAM region for sprite tiles and initialize hardware sprite allocation system.
 *
 *  \see SPR_init(void)
 *  \see SPR_end(void)
 */
void SPR_initEx(u16 vramSize);
/**
 *  \brief
 *      End the Sprite engine.
 *
 *      End the sprite engine and release attached resources.<br>
 *      This releases the allocated VRAM region and hardware sprites.
 */
void SPR_end(void);
/**
 *  \brief
 *      FALSE if sprite cache engine is not initialized, TRUE otherwise.
 */
bool SPR_isInitialized(void);

/**
 *  \brief
 *      Reset the Sprite engine.<br>
 *
 *      This method releases all allocated sprites and their resources.
 */
void SPR_reset(void);

/**
 *  \brief
 *      Adds a new sprite with specified parameters and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \param spriteIndex
 *      index of the first sprite in the VDP sprite table used to display this Sprite (should be in [1..79] range.<br>
 *      IMPORTANT: this value is used only if you use manual VDP Sprite allocation (see the <i>flags</i> parameter).<br>
 *  \param flag
 *      specific settings for this sprite:<br>
 *      #SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE = Disable delaying of frame update when we are running out of DMA capacity.<br>
 *          If you set this flag then sprite frame update always happen immediately but may lead to some graphical glitches (tiles data and sprite table data not synchronized).
 *          You can use SPR_setDelayedFrameUpdate(..) method to change this setting.<br>
 *      #SPR_FLAG_AUTO_VISIBILITY = Enable automatic sprite visibility calculation (you can also use SPR_setVisibility(..) method).<br>
 *      #SPR_FLAG_FAST_AUTO_VISIBILITY = Enable fast computation for the automatic visibility calculation (disabled by default)<br>
 *          If you set this flag the automatic visibility calculation will be done globally for the (meta) sprite and not per internal
 *          hardware sprite. This result in faster visibility computation at the expense of some waste of hardware sprite.
 *          You can set the automatic visibility computation by using SPR_setVisibility(..) method.<br>
 *      #SPR_FLAG_AUTO_VRAM_ALLOC = Enable automatic VRAM allocation (enabled by default)<br>
 *          If you don't set this flag you will have to manually define VRAM tile index position for this sprite with the <i>attribut</i> parameter or by using the #SPR_setVRAMTileIndex(..) method<br>
 *      #SPR_FLAG_AUTO_SPRITE_ALLOC = Enable automatic hardware/VDP sprite allocation (enabled by default)<br>
 *          If you don't set this flag you will have to manually define the hardware sprite table index to reserve with the <i>spriteIndex</i> parameter or by using the #SPR_setSpriteTableIndex(..) method<br>
 *      #SPR_FLAG_AUTO_TILE_UPLOAD = Enable automatic upload of sprite tiles data into VRAM (enabled by default)<br>
 *          If you don't set this flag you will have to manually upload tiles data of sprite into the VRAM (you can change this setting using #SPR_setAutoTileUpload(..) method).<br>
 *      #SPR_FLAG_INSERT_HEAD = Allow to insert the sprite at the start/head of the list.<br>
 *          When you use this flag the sprite will be inserted at the head of the list making it top most (equivalent to #SPR_setDepth(#SPR_MIN_DEPTH))<br>
 *          while default insertion position is at the end of the list (equivalent to #SPR_setDepth(#SPR_MAX_DEPTH))<br>
 *      <br>
 *      It's recommended to use the following default settings:<br>
 *      SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_SPRITE_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD<br>
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses the provided flag setting for automatic resources allocation and sprite visibility computation.<br>
 *      If auto visibility is not enabled then sprite is considered as not visible by default (see SPR_setVisibility(..) method).<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).<br>
 *      IMPORTANT NOTE: sprite allocation can fail (return NULL) when you are using auto VRAM allocation (SPR_FLAG_AUTO_VRAM_ALLOC) even if there is enough VRAM available,<br>
 *      this can happen because of the VRAM fragmentation. You can use #SPR_addSpriteExSafe(..) method instead so it take care about VRAM fragmentation.
 *
 *  \see SPR_addSprite(..)
 *  \see SPR_addSpriteExSafe(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSpriteEx(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut, u16 spriteIndex, u16 flag);
/**
 *  \brief
 *      Adds a new sprite with auto resource allocation enabled and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses automatic resources allocation (VRAM and hardware sprite) and visibility is set to ON.<br>
 *      You can change these defaults settings later by calling SPR_setVRAMTileIndex(..), SPR_setSpriteTableIndex(..), SPR_setAutoTileUpload(..) and SPR_setVisibility(..) methods.<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).<br>
 *      IMPORTANT NOTE: sprite allocation can fail (return NULL) because of automatic VRAM allocation even if there is enough VRAM available,
 *      this can happen because of the VRAM fragmentation.<br> You can use #SPR_addSpriteSafe(..) method instead so it take care about VRAM fragmentation.
 *
 *  \see SPR_addSpriteEx(..)
 *  \see SPR_addSpriteSafe(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSprite(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut);
/**
 *  \brief
 *      Adds a new sprite with specified parameters and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \param spriteIndex
 *      index of the first sprite in the VDP sprite table used to display this Sprite (should be > 0 and < 128).<br>
 *      IMPORTANT: this value is used only if you use manual VDP Sprite allocation (see the <i>flag</i> parameter).<br>
 *  \param flag
 *      specific settings for this sprite:<br>
 *      #SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE = Disable delaying of frame update when we are running out of DMA capacity.<br>
 *          If you set this flag then sprite frame update always happen immediately but may lead to some graphical glitches (tiles data and sprite table data not synchronized).
 *          You can use SPR_setDelayedFrameUpdate(..) method to change this setting.<br>
 *      #SPR_FLAG_AUTO_VISIBILITY = Enable automatic sprite visibility calculation (you can also use SPR_setVisibility(..) method).<br>
 *      #SPR_FLAG_FAST_AUTO_VISIBILITY = Enable fast computation for the automatic visibility calculation (disabled by default)<br>
 *          If you set this flag the automatic visibility calculation will be done globally for the (meta) sprite and not per internal
 *          hardware sprite. This result in faster visibility computation at the expense of some waste of hardware sprite (scanline limit).
 *          You can set the automatic visibility computation by using SPR_setVisibility(..) method.<br>
 *      #SPR_FLAG_AUTO_VRAM_ALLOC = Enable automatic VRAM allocation (enabled by default)<br>
 *          If you don't set this flag you will have to manually define VRAM tile index position for this sprite with the <i>attribut</i> parameter or by using the #SPR_setVRAMTileIndex(..) method<br>
 *      #SPR_FLAG_AUTO_SPRITE_ALLOC = Enable automatic hardware/VDP sprite allocation (enabled by default)<br>
 *          If you don't set this flag you will have to manually define the hardware sprite table index to reserve with the <i>spriteIndex</i> parameter or by using the #SPR_setSpriteTableIndex(..) method<br>
 *      #SPR_FLAG_AUTO_TILE_UPLOAD = Enable automatic upload of sprite tiles data into VRAM (enabled by default)<br>
 *          If you don't set this flag you will have to manually upload tiles data of sprite into the VRAM (you can change this setting using #SPR_setAutoTileUpload(..) method).<br>
 *      #SPR_FLAG_INSERT_HEAD = Allow to insert the sprite at the start/head of the list.<br>
 *          When you use this flag the sprite will be inserted at the head of the list making it top most (equivalent to #SPR_setDepth(#SPR_MIN_DEPTH))<br>
 *          while default insertion position is at the end of the list (equivalent to #SPR_setDepth(#SPR_MAX_DEPTH))<br>
 *      <br>
 *      It's recommended to use the following default settings:<br>
 *      SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_SPRITE_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD<br>
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses the provided flag setting for automatic resources allocation and sprite visibility computation.<br>
 *      If auto visibility is not enabled then sprite is considered as not visible by default (see SPR_setVisibility(..) method).<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).
 *
 *  \see SPR_addSpriteSafe(..)
 *  \see SPR_addSpriteEx(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSpriteExSafe(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut, u16 spriteIndex, u16 flag);
/**
 *  \brief
 *      Adds a new sprite with auto resource allocation enabled and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses automatic resources allocation (VRAM and hardware sprite) and visibility is set to ON.<br>
 *      You can change these defaults settings later by calling SPR_setVRAMTileIndex(..), SPR_setSpriteTableIndex(..), SPR_setAutoTileUpload(..) and SPR_setVisibility(..) methods.<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).<
 *
 *  \see SPR_addSpriteSafeEx(..)
 *  \see SPR_addSprite(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSpriteSafe(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut);

/**
 *  \brief
 *      Release the specified sprite (no more visible and release its resources).
 *
 *  \param sprite
 *      Sprite to release
 *
 *      This method release resources for the specified Sprite object and remove it from the screen at next SPR_update() call.
 *
 *  \see SPR_releasesSprite(..)
 */
void SPR_releaseSprite(Sprite* sprite);
/**
 *  \brief
 *      Returns the number of active sprite (number of sprite added with SPR_addSprite(..) or SPR_addSpriteEx(..) methods).
 */
u16 SPR_getNumActiveSprite(void);
/**
 *  \brief
 *      Defragment allocated VRAM for sprites, that can help when sprite allocation fail (SPR_addSprite(..) or SPR_addSpriteEx(..) return <i>NULL</i>).
 */
void SPR_defragVRAM(void);
/**
 *  \brief
 *      Load all frames of SpriteDefinition (using DMA) at specified VRAM tile index and return the indexes table.<br>
 *      <b>WARNING: This function should be call at init/loading time as it can be quite long (several frames)</b>
 *
 *  \param sprDef
 *      the SpriteDefinition we want to load frame data in VRAM.
 *  \param index
 *      the tile position in VRAM where we will upload all sprite frame tiles data.
 *  \param totalNumTile
 *      if not NULL then the function will store here the total number of tile used to load all animation frames.
 *
 *   Load all frames of spriteDef (using DMA) at specified VRAM tile index and return the indexes table.<br>
 *   The returned index table is a dynamically allocated 2D table[anim][frame] so you need to release it using #MEM_free(..)
 *   when you don't need the table anymore.<br>
 *   You can use the frame change callback (see #SPR_setFrameChangeCallback(..)) to automatically update the VRAM index using the indexes table:<br>
 *   <code>frameIndexes = SPR_loadAllFrames(sprite->definition, ind);<br>
 *   SPR_setFrameChangeCallback(sprite, &frameChanged);<br>
 *   ....<br>
 *   void frameChanged(Sprite* sprite)<br>
 *   {<br>
 *       u16 tileIndex = frameIndexes[sprite->animInd][sprite->frameInd];<br>
 *       SPR_setVRAMTileIndex(sprite, tileIndex);<br>
 *   }</code>
 *
 *  \return the 2D indexes table or NULL if there is not enough memory to allocate the table.
 *  \see SPR_setFrameChangeCallback(...);
 */
u16** SPR_loadAllFrames(const SpriteDefinition* sprDef, u16 index, u16* totalNumTile);

/**
 *  \brief
 *      Set the Sprite Definition.
 *
 *  \param sprite
 *      Sprite to set definition for.
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *
 *   Set the Sprite Definition for this sprite.<br>
 *   By default the first frame of the first animation from Sprite Definition is loaded.
 *
 *  \return FALSE if auto resource allocation failed, TRUE otherwise.
 */
bool SPR_setDefinition(Sprite* sprite, const SpriteDefinition* spriteDef);
/**
 *  \brief
 *      Get sprite position X.
 */
s16 SPR_getPositionX(Sprite* sprite);
/**
 *  \brief
 *      Get sprite position Y.
 */
s16 SPR_getPositionY(Sprite* sprite);
/**
 *  \brief
 *      Set sprite position.
 *
 *  \param sprite
 *      Sprite to set position for
 *  \param x
 *      X position
 *  \param y
 *      Y position
 */
void SPR_setPosition(Sprite* sprite, s16 x, s16 y);
/**
 *  \brief
 *      Set sprite Horizontal Flip attribut.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The horizontal flip attribut value (TRUE or FALSE)
 */
void SPR_setHFlip(Sprite* sprite, bool value);
/**
 *  \brief
 *      Set sprite Vertical Flip attribut.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The vertical flip attribut value (TRUE or FALSE)
 */
void SPR_setVFlip(Sprite* sprite, bool value);
/**
 *  \brief
 *      Set sprite Palette index to use.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The palette index to use for this sprite (PAL0, PAL1, PAL2 or PAL3)
 */
void SPR_setPalette(Sprite* sprite, u16 value);
/**
 *  \brief
 *      Set sprite Priority attribut.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The priority attribut value (TRUE or FALSE)
 */
void SPR_setPriority(Sprite* sprite, bool value);
/**
 *  \deprecated
 *      Use #SPR_setPriority(..) instead
 */
#define SPR_setPriorityAttribut(sprite, value)      _Pragma("GCC error \"This method is deprecated, use SPR_setPriority(..) instead.\"")
/**
 *  \brief
 *      Set sprite depth (for sprite display ordering)
 *
 *  \param sprite
 *      Sprite to set depth for
 *  \param value
 *      The depth value (SPR_MIN_DEPTH to set always on top)
 *
 *  Sprite having lower depth are display in front of sprite with higher depth.<br>
 *  The sprite is *immediately* sorted when its depth value is changed.
 */
void SPR_setDepth(Sprite* sprite, s16 value);
/**
 *  \brief
 *      Same as #SPR_setDepth(..)
 */
void SPR_setZ(Sprite* sprite, s16 value);
/**
 *  \brief
 *      Set sprite depth so it remains above others sprite - same as SPR_setDepth(SPR_MIN_DEPTH)
 *
 *  \param sprite
 *      Sprite to set depth for
 *
 *  Sprite having lower depth are display in front of sprite with higher depth.<br>
 *  The sprite is *immediately* sorted when its depth value is changed.
 */
void SPR_setAlwaysOnTop(Sprite* sprite);
/**
 *  \brief
 *      Set sprite depth so it remains behind others sprite - same as SPR_setDepth(SPR_MAX_DEPTH)
 *
 *  \param sprite
 *      Sprite to change depth for
 *
 *  \see SPR_setDepth(Sprite*)
 */
void SPR_setAlwaysAtBottom(Sprite* sprite);
/**
 *  \brief
 *      Set current sprite animation and frame.
 *
 *  \param sprite
 *      Sprite to set animation and frame for
 *  \param anim
 *      animation index to set
 *  \param frame
 *      frame index to set
 */
void SPR_setAnimAndFrame(Sprite* sprite, s16 anim, s16 frame);
/**
 *  \brief
 *      Set current sprite animation.
 *
 *  \param sprite
 *      Sprite to set animation for
 *  \param anim
 *      animation index to set.
 */
void SPR_setAnim(Sprite* sprite, s16 anim);
/**
 *  \brief
 *      Set current sprite frame.
 *
 *  \param sprite
 *      Sprite to set frame for
 *  \param frame
 *      frame index to set.
 */
void SPR_setFrame(Sprite* sprite, s16 frame);
/**
 *  \brief
 *      Pass to the next sprite frame.
 *
 *  \param sprite
 *      Sprite to pass to next frame for
 */
void SPR_nextFrame(Sprite* sprite);
/**
 *  \brief
 *      Enable/disable auto animation (default is on).<br>
 *
 *  \param sprite
 *      Sprite we want to enable/disable auto animation.
 *  \param value
 *      TRUE to enable auto animation (default), FALSE otherwise
 *
 *  \see #SPR_getAutoAnimation(Sprite*)
 */
void SPR_setAutoAnimation(Sprite* sprite, bool value);
/**
 *  \brief
 *      Return TRUE if auto animation is enabled, FALSE otherwise.
 *
 *  \see #SPR_setAnimationLoop(Sprite*, bool)
 */
bool SPR_getAutoAnimation(Sprite* sprite);
/**
 *  \brief
 *      Enable/disable animation loop (default is on).<br>
 *      When disable the sprite will stay on the last animation frame when animation ended instead of restarting it.
 *
 *  \param sprite
 *      Sprite we want to enable/disable animation loop for.
 *  \param value
 *      TRUE to enable animation loop (default), FALSE otherwise
 *
 *  \see SPR_FLAG_DISABLE_ANIMATION_LOOP
 *  \see #SPR_getAnimationDone(Sprite*)
 */
void SPR_setAnimationLoop(Sprite* sprite, bool value);
/**
 *  \brief
 *      Returns TRUE if the sprite reached the end of the current animation.<br>
 *      When auto animation is enabled (see SPR_setAutoAnimation(..)) the function returns TRUE only when we reached
 *      the last *tick* of the last animation frame.<br>
 *      When auto animation is disabled the function returns TRUE as soon we are on last animation frame.
 *
 *  \see #SPR_setAutoAnimation(Sprite*, bool)
 */
bool SPR_isAnimationDone(Sprite* sprite);
/**
 *  \brief
 *      Set the VRAM tile position reserved for this sprite.
 *
 *  \param sprite
 *      Sprite to set the VRAM tile position for
 *  \param value
 *      the tile position in VRAM where we will upload the sprite tiles data.<br>
 *      Use <b>-1</b> for auto allocation.<br>
 *  \return FALSE if auto allocation failed (can happen only if sprite is currently active), TRUE otherwise
 *
 *  By default the Sprite Engine auto allocate VRAM for sprites tiles but you can force
 *  manual allocation and fix the sprite tiles position in VRAM with this method.
 */
bool SPR_setVRAMTileIndex(Sprite* sprite, s16 value);
/**
 *  \brief
 *      Set the VDP sprite index to use for this sprite.
 *
 *  \param sprite
 *      Sprite to set the VDP Sprite index for
 *  \param value
 *      the index of the first sprite in the VDP sprite table used to display this Sprite (should be > 0 and < 128).<br>
 *      Use <b>-1</b> for auto allocation.<br>
 *  \return FALSE if auto allocation failed (can happen only if sprite is currently active), TRUE otherwise
 *
 *  By default the Sprite Engine auto allocate VDP sprite but you can force
 *  manual allocation and fix the index of the first VDP sprite to use with this method.<br>
 *  If you set the index manually you need to ensure you have enough available contiguous VDP sprites at this
 *  index so it can fit the current sprite requirement in VDP sprite.
 *  <b>WARNING: you cannot use sprite 0 as it is internally reserved.</b>
 */
bool SPR_setSpriteTableIndex(Sprite* sprite, s16 value);
/**
 *  \brief
 *      Enable/disable the automatic upload of sprite tiles data into VRAM.
 *
 *  \param sprite
 *      Sprite we want to enable/disable auto tile upload for
 *  \param value
 *      TRUE to enable the automatic upload of sprite tiles data into VRAM.<br>
 *      FALSE to disable it (mean you have to handle that on your own).<br>
 */
void SPR_setAutoTileUpload(Sprite* sprite, bool value);
/**
 *  \brief
 *      Enable/disable the delayed frame update.
 *
 *  \param sprite
 *      Sprite we want to enable/disable delayed frame update
 *  \param value
 *      TRUE to enable the delayed frame update when DMA is running out of transfert capacity (default).<br>
 *      FALSE to disable it. In which case sprite frame is always updated immediately but that may cause graphical glitches (update during active display).<br>
 *
 *  \see #SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE
 */
void SPR_setDelayedFrameUpdate(Sprite* sprite, bool value);
/**
 *  \brief
 *      Set the frame change event callback for this sprite.
 *
 *  \param sprite
 *      Sprite we want to set the frame change callback
 *  \param callback
 *      the callback (function pointer) to call when we just changed the animation frame for this sprite.
 *
 *      The callback (if not NULL) will be called on frame change process during #SPR_update() call (CAUTION: sprite->status field is not up to date at this point).<br>
 *      It let opportunity to the developer to apply special behavior or process when sprite frame just changed:<br>
 *      for instance we can disable animation looping by setting sprite->timer to 0 when we meet the last animation frame.
 *
 *  \see #FrameChangeCallback
 */
void SPR_setFrameChangeCallback(Sprite* sprite, FrameChangeCallback* callback);

/**
 *  \brief
 *      Return the <i>visibility</i> state for this sprite.<br>
 *      WARNING: this is different from SPR_isVisible(..) method, possible value are:<br>
 *      SpriteVisibility.VISIBLE        = sprite is visible<br>
 *      SpriteVisibility.HIDDEN         = sprite is not visible<br>
 *      SpriteVisibility.AUTO_FAST      = visibility is automatically computed - global visibility (only using sprite X position)<br>
 *      SpriteVisibility.AUTO_SLOW      = visibility is automatically computed - per hardware sprite visibility (only using sprite X position)<br>
 *
 *  \param sprite
 *      Sprite to return <i>visibility</i> state
 *
 *  \return
 *       the <i>visibility</i> state for this sprite.
 *
 *  \see SPR_isVisible(...)
 *  \see SPR_setVisibility(...)
 */
SpriteVisibility SPR_getVisibility(Sprite* sprite);
/**
 *  \brief
 *      Return the visible state for this sprite (meaningful only if AUTO visibility is enabled, see #SPR_setVisibility(..) method).<br>
 *
 *  \param sprite
 *      Sprite to return <i>visible</i> state
 *  \param recompute
 *      Force visibility computation.<br>
 *      Only required if SPR_update() wasn't called since last sprite position change (note that can force the frame update processing).
 *
 *  Note that only the sprite X position is used to determine if it's visible (see #SPR_setVisibility(..) method for more information)

 *  \return
 *      the <i>visible</i> state for this sprite.
 *
 *  \see SPR_setVisibility(...)
 */
bool SPR_isVisible(Sprite* sprite, bool recompute);

/**
 *  \brief
 *      Set the <i>visibility</i> state for this sprite.
 *
 *  \param sprite
 *      Sprite to set the <i>visibility</i> information
 *  \param value
 *      Visibility value to set.<br>
 *      SpriteVisibility.VISIBLE        = sprite is visible<br>
 *      SpriteVisibility.HIDDEN         = sprite is not visible<br>
 *      SpriteVisibility.AUTO_FAST      = visibility is automatically computed - global visibility (only using sprite X position)<br>
 *      SpriteVisibility.AUTO_SLOW      = visibility is automatically computed - per hardware sprite visibility (only using sprite X position)<br>
 *
 *      The only interest in having a sprite hidden / not visible is to avoid having it consuming scanline sprite budget:<br>
 *      The VDP is limited to a maximum of 20 sprites or 320 pixels of sprite per scanline (16 sprites/256 px in H32 mode).<br>
 *      If we reach this limit, following sprites won't be renderer so it's important to try to optimize the number of sprites on a single scanline.<br>
 *      When a sprite is set to not visible (automatically or manually), its position is set offscreen *vertically* so it won't eat
 *      anymore the scanline sprite rendering budget, only X position test is required for that so a sprite is determined not visible only using its X coordinate.
 *
 *  \see SPR_computeVisibility(...)
 *  \see SPR_getVisibility(...)
 *  \see SPR_isVisible(...)
 */
void SPR_setVisibility(Sprite* sprite, SpriteVisibility value);
/**
 *  \deprecated Use #SPR_setVisibility(..) method instead.
 */
#define SPR_setAlwaysVisible(sprite, value)     _Pragma("GCC error \"This method is deprecated, use SPR_setVisibility(..) instead.\"")
/**
 *  \deprecated Use #SPR_setVisibility(..) method instead.
 */
#define SPR_setNeverVisible(sprite, value)      _Pragma("GCC error \"This method is deprecated, use SPR_setVisibility(..) instead.\"")
/**
 *  \deprecated Use SPR_isVisible(..) instead.
 */
#define SPR_computeVisibility(sprite)           _Pragma("GCC error \"This method is deprecated, use SPR_isVisible(sprite, TRUE) instead.\"")

/**
 *  \brief
 *      Clear all displayed sprites.
 *
 *  This method allow to quickly hide all sprites (without releasing their resources).<br>
 *  Sprites can be displayed again just by calling SPR_update().
 */
void SPR_clear(void);
/**
 *  \brief
 *      Update and display the active list of sprite.
 *
 *  This actually updates all internal active sprites states and prepare the sprite list
 *  cache to send it to the hardware (VDP) at Vint.
 *
 *  \see #SPR_addSprite(..)
 */
void SPR_update(void);

/**
 *  \brief
 *      Log the profil informations (when enabled) in the KMod message window.
 */
void SPR_logProfil(void);
/**
 *  \brief
 *      Log the sprites informations (when enabled) in the KMod message window.
 */
void SPR_logSprites(void);


#endif // _SPRITE_ENG_LEGACY_H_

#endif
````

## File: inc/sprite_eng.h
````c
/**
 *  \file sprite_eng.h
 *  \brief Sprite engine
 *  \author Stephane Dallongeville
 *  \date 10/2013
 *
 * Sprite engine providing advanced sprites manipulation and operations.<br>
 * This unit use both the vram memory manager (see vram.h file for more info)<br>
 * and the Sega Genesis VDP sprite capabilities (see vdp_spr.h file for more info).
 */

#if     !LEGACY_SPRITE_ENGINE

#ifndef _SPRITE_ENG_H_
#define _SPRITE_ENG_H_

#include "vdp_tile.h"
#include "vdp_spr.h"
#include "pal.h"
#include "pool.h"


/**
 *  \brief
 *      No collision type
 */
#define COLLISION_TYPE_NONE     0
/**
 *  \brief
 *      Bouding box collision type (Box structure)
 */
#define COLLISION_TYPE_BOX      1
/**
 *  \brief
 *      Round circle collision type (Circle structure)
 */
#define COLLISION_TYPE_CIRCLE   2

/**
 *  \brief
 *      Special flag to indicate that we want to add the sprite at position 0 (head / top) in the list<br>
 *      instead of adding it in last / bottom position (default)
 */
#define SPR_FLAG_INSERT_HEAD                    0x4000
/**
 *  \brief
 *      Disable animation auto loop.<br>
 *      By default animation always restart after the last frame has been played.
 *      This flag prevent the animation to restart and so the animation end on the last frame forever (see #SPR_getAnimationDone(..))
 */
#define SPR_FLAG_DISABLE_ANIMATION_LOOP         0x2000
/**
 *  \brief
 *      Disable delaying of frame update when we are running out of DMA capacity.<br>
 *      By default we delay the frame update when DMA is already full so the frame update happen as soon we have enough DMA capacity to do it.
 *      This flag forces frame update to always happen immediately but that may cause graphical glitches.
 */
#define SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE   0x1000
/**
 *  \brief
 *      Enable automatic VRAM allocation
 */
#define SPR_FLAG_AUTO_VRAM_ALLOC                0x0800
/**
 *  \brief
 *      Enable automatic upload of sprite tiles data into VRAM
 */
#define SPR_FLAG_AUTO_TILE_UPLOAD               0x0400
/**
 *  \brief
 *      Enable automatic visibility calculation
 */
#define SPR_FLAG_AUTO_VISIBILITY                0x0200
/**
 *  \brief
 *      Enable fast visibility calculation (only meaningful if SPR_FLAG_AUTO_VISIBILITY is used).<br>
 *      If you set this flag the automatic visibility calculation will be done globally for the (meta) sprite and not per internal
 *      hardware sprite. This result in faster visibility computation at the expense of using extra (wasting) hardware sprites.
 */
#define SPR_FLAG_FAST_AUTO_VISIBILITY           0x0100

/**
 *  \brief
 *      Mask for sprite flag
 */
#define SPR_FLAG_MASK                           (SPR_FLAG_INSERT_HEAD | SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_FAST_AUTO_VISIBILITY)

/**
 *  \brief
 *      Minimum depth for a sprite (always above others sprites)
 */
#define SPR_MIN_DEPTH       (-0x8000)
/**
 *  \brief
 *      Maximum depth for a sprite (always below others sprites)
 */
#define SPR_MAX_DEPTH       0x7FFF

/**
 *  \brief
 *      Sprite visibility enumeration
 */
typedef enum
{
    VISIBLE,        /**< Sprite is visible (no computation needed) */
    HIDDEN,         /**< Sprite is hidden (no computation needed) */
    AUTO_FAST,      /**< Automatic visibility calculation - FAST (computation made on global meta sprite) */
    AUTO_SLOW,      /**< Automatic visibility calculation - SLOW (computation made per hardware sprite) */
} SpriteVisibility;

/**
 *  \brief
 *      Simple Box structure for collision
 *
 *  \param x
 *      X position (left)
 *  \param y
 *      Y position (top)
 *  \param w
 *      width
 *  \param h
 *      heigth
 */
typedef struct
{
    s8 x;
    s8 y;
    u8 w;
    u8 h;
} BoxCollision;

/**
 *  \brief
 *      Simple Circle structure (can be used for collision detection)
 *
 *  \param x
 *      X center position
 *  \param y
 *      Y center position
 *  \param ray
 *      circle ray
 */
typedef struct
{
    s8 x;
    s8 y;
    u16 ray;
} CircleCollision;

/**
 *  \struct Collision
 *  \brief
 *      Collision definition union.
 *
 *  \param typeHit
 *      Collision type for hit collision:<br>
 *      Allowed values are #COLLISION_TYPE_BOX or #COLLISION_TYPE_CIRCLE.
 *  \param typeAttack
 *      Collision type for attack collision (can be used as alternative hit collision):<br>
 *      Allowed values are #COLLISION_TYPE_BOX or #COLLISION_TYPE_CIRCLE.
 *  \param box
 *      BoxCollision definition if type = #COLLISION_TYPE_BOX
 *  \param circle
 *      CircleCollision definition if type = #COLLISION_TYPE_CIRCLE
 *  \param inner
 *      if current collision is verified then we test inner for more precise collision if needed
 *  \param next
 *      if current collision is not verified then we test next for next collision if needed
 */
typedef struct _collision
{
    u8 typeHit;
    u8 typeAttack;
    union
    {
        BoxCollision box;
        CircleCollision circle;
    } hit;
    union
    {
        BoxCollision box;
        CircleCollision circle;
    } attack;
} Collision;

/**
 *  \brief
 *      Single VDP sprite info structure for sprite animation frame.
 *
 *  \param offsetY
 *      Y offset for this VDP sprite relative to global Sprite position
 *  \param offsetYFlip
 *      Y offset (flip version) for this VDP sprite relative to global Sprite position
 *  \param size
 *      sprite size (see SPRITE_SIZE macro)
 *  \param offsetX
 *      X offset for this VDP sprite relative to global Sprite position
 *  \param offsetXFlip
 *      X offset (flip version) for this VDP sprite relative to global Sprite position
 *  \param numTile
 *      number of tile for this VDP sprite (should be coherent with the given size field)
 */
typedef struct
{
    u8 offsetY;          // respect VDP sprite field order, may help
    u8 offsetYFlip;
    u8 size;
    u8 offsetX;
    u8 offsetXFlip;
    u8 numTile;
}  FrameVDPSprite;

/**
 *  \brief
 *      Sprite animation frame structure.
 *
 *  \param numSprite
 *      number of VDP sprite which compose this frame.
 *      bit 7 is used as a special flag for the sprite engine so always use 'numSprite & 0x7F' to just retrieve the number of sprite
 *  \param timer
 *      active time for this frame (in 1/60 of second)
 *  \param tileset
 *      tileset containing tiles for this animation frame (ordered for sprite)
 *  \param collision
 *      collision structure (not used currently)
 *  \param frameSprites
 *      array of VDP sprites info composing the frame
 */
typedef struct
{
    s8 numSprite;
    u8 timer;
    TileSet* tileset;                   // TODO: have a tileset per VDP sprite --> probably not a good idea performance wise
    Collision* collision;               // Require many DMA queue operations and fast DMA flush as well, also bring extra computing in calculating delayed update
    FrameVDPSprite frameVDPSprites[];
} AnimationFrame;

/**
 *  \brief
 *      Sprite animation structure.
 *
 *  \param numFrame
 *      number of different frame for this animation (max = 255)
 *  \param loop
 *      frame index for loop (last index if no loop)
 *  \param frames
 *      frames composing the animation
 */
typedef struct
{
    u8 numFrame;
    u8 loop;
    AnimationFrame** frames;
} Animation;

/**
 *  \brief
 *      Sprite definition structure.
 *
 *  \param w
 *      frame cell width in pixel
 *  \param h
 *      frame cell height in pixel
 *  \param palette
 *      default palette data
 *  \param numAnimation
 *      number of animation for this sprite
 *  \param animations
 *      animation definitions
 *  \param maxNumTile
 *      maximum number of tile used by a single animation frame (used for VRAM tile space allocation)
 *  \param maxNumSprite
 *      maximum number of VDP sprite used by a single animation frame (used for VDP sprite allocation)
 *
 *  Contains all animations for a Sprite and internal informations.
 */
typedef struct
{
    u16 w;
    u16 h;
    Palette* palette;
    u16 numAnimation;
    Animation** animations;
    u16 maxNumTile;
    u16 maxNumSprite;
} SpriteDefinition;

/**
 *  \brief
 *      Sprite structure used by the Sprite Engine to store state for a sprite.<br>
 *      WARNING: always use the #SPR_addSprite(..) method to allocate Sprite object.<br>
 *
 *  \param status
 *      Internal state and automatic allocation information (internal)
 *  \param visibility
 *      visibility information of current frame for each VDP sprite (max = 16)
 *  \param spriteDef
 *      Sprite definition pointer
 *  \param onFrameChange
 *      Custom callback on frame change event (see #SPR_setFrameChangeCallback(..) method)
 *  \param animation
 *      Animation pointer cache (internal)
 *  \param frame
 *      AnimationFrame pointer cache (internal)
 *  \param animInd
 *      current animation index (internal)
 *  \param frameInd
 *      current frame animation index (internal)
 *  \param timer
 *      timer for current frame (internal)
 *  \param x
 *      current sprite X position on screen offseted by 0x80 (internal VDP position)
 *  \param y
 *      current sprite Y position on screen offseted by 0x80 (internal VDP position)
 *  \param depth
 *      current sprite depth (Z) position used for Z sorting
 *  \param attribut
 *      sprite specific attribut and allocated VRAM tile index (see TILE_ATTR_FULL() macro)
 *  \param data
 *      this is a free field for user data, use it for whatever you want (flags, pointer...)
 *  \param prev
 *      pointer on previous Sprite in list
 *  \param next
 *      pointer on next Sprite in list
 *
 *  Used to manage an active sprite in game condition.
 */
typedef struct Sprite
{
    u16 status;
    u16 visibility;
    const SpriteDefinition* definition;
    void (*onFrameChange)(struct Sprite* sprite);
    Animation* animation;
    AnimationFrame* frame;
    s16 animInd;
    s16 frameInd;
    s16 timer;
    s16 x;
    s16 y;
    s16 depth;
    u16 attribut;
    u32 data;
    struct Sprite* prev;
    struct Sprite* next;
} Sprite;

/**
 *  \brief
 *      Sprite frame change event callback.<br>
 *
 *  \param sprite
 *      The sprite for which frame just changed.
 *
 *      This event occurs onframe change process during #SPR_update() call (CAUTION: sprite->status field is not up to date at this point).<br>
 *      It let opportunity to the developer to apply special behavior or process when sprite frame just changed:<br>
 *      for instance we can disable animation looping by setting sprite->timer to 0 when we meet the last animation frame.
 */
typedef void FrameChangeCallback(Sprite* sprite);

/**
 * Sprites object pool for the sprite engine
 */
extern Pool* spritesPool;
/**
 * First allocated sprite (NULL if no sprite allocated)
 */
extern Sprite* firstSprite;
/**
 * Last allocated sprite (NULL if no sprite allocated)
 */
extern Sprite* lastSprite;
/**
 * Allocated VRAM (in tile) for Sprite Engine
 */
extern u16 spriteVramSize;


/**
 *  \brief
 *      Initialize the Sprite engine with default parameters.
 *
 *      Initialize the sprite engine using default parameters (420 reserved tiles in VRAM).<br>
 *      This also initialize the hardware sprite allocation system.
 *
 *  \see SPR_initEx(void)
 *  \see SPR_end(void)
 */
void SPR_init(void);
/**
 *  \brief
 *      Init the Sprite engine with specified advanced parameters (VRAM allocation size and decompression buffer size).
 *
 *  \param vramSize
 *      size (in tile) of the VRAM region for the automatic VRAM tile allocation.<br>
 *      If set to 0 the default size is used (420 tiles)
 *
 *      Initialize the sprite engine.<br>
 *      This allocates a VRAM region for sprite tiles and initialize hardware sprite allocation system.
 *
 *  \see SPR_init(void)
 *  \see SPR_end(void)
 */
void SPR_initEx(u16 vramSize);
/**
 *  \brief
 *      End the Sprite engine.
 *
 *      End the sprite engine and release attached resources.<br>
 *      This releases the allocated VRAM region and hardware sprites.
 */
void SPR_end(void);
/**
 *  \brief
 *      FALSE if sprite cache engine is not initialized, TRUE otherwise.
 */
bool SPR_isInitialized(void);

/**
 *  \brief
 *      Reset the Sprite engine.<br>
 *
 *      This method releases all allocated sprites and their resources.
 */
void SPR_reset(void);

/**
 *  \brief
 *      Adds a new sprite with specified parameters and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \param flag
 *      specific settings for this sprite:<br>
 *      #SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE = Disable delaying of frame update when we are running out of DMA capacity.<br>
 *          If you set this flag then sprite frame update always happen immediately but may lead to some graphical glitches (tiles data and sprite table data not synchronized).
 *          You can use SPR_setDelayedFrameUpdate(..) method to change this setting.<br>
 *      #SPR_FLAG_AUTO_VISIBILITY = Enable automatic sprite visibility calculation (you can also use SPR_setVisibility(..) method).<br>
 *      #SPR_FLAG_FAST_AUTO_VISIBILITY = Enable fast computation for the automatic visibility calculation (disabled by default)<br>
 *          If you set this flag the automatic visibility calculation will be done globally for the (meta) sprite and not per internal
 *          hardware sprite. This result in faster visibility computation at the expense of some waste of hardware sprite.
 *          You can set the automatic visibility computation by using SPR_setVisibility(..) method.<br>
 *      #SPR_FLAG_AUTO_VRAM_ALLOC = Enable automatic VRAM allocation (enabled by default)<br>
 *          If you don't set this flag you will have to manually define VRAM tile index position for this sprite with the <i>attribut</i> parameter or by using the #SPR_setVRAMTileIndex(..) method<br>
 *      #SPR_FLAG_AUTO_TILE_UPLOAD = Enable automatic upload of sprite tiles data into VRAM (enabled by default)<br>
 *          If you don't set this flag you will have to manually upload tiles data of sprite into the VRAM (you can change this setting using #SPR_setAutoTileUpload(..) method).<br>
 *      #SPR_FLAG_INSERT_HEAD = Allow to insert the sprite at the start/head of the list.<br>
 *          When you use this flag the sprite will be inserted at the head of the list making it top most (equivalent to #SPR_setDepth(#SPR_MIN_DEPTH))<br>
 *          while default insertion position is at the end of the list (equivalent to #SPR_setDepth(#SPR_MAX_DEPTH))<br>
 *      <br>
 *      It's recommended to use the following default settings:<br>
 *      SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD<br>
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses the provided flag setting for automatic resources allocation and sprite visibility computation.<br>
 *      If auto visibility is not enabled then sprite is considered as not visible by default (see SPR_setVisibility(..) method).<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).<br>
 *      IMPORTANT NOTE: sprite allocation can fail (return NULL) when you are using auto VRAM allocation (SPR_FLAG_AUTO_VRAM_ALLOC) even if there is enough VRAM available,<br>
 *      this can happen because of the VRAM fragmentation. You can use #SPR_addSpriteExSafe(..) method instead so it take care about VRAM fragmentation.
 *
 *  \see SPR_addSprite(..)
 *  \see SPR_addSpriteExSafe(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSpriteEx(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut, u16 flag);
/**
 *  \brief
 *      Adds a new sprite with auto resource allocation enabled and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses automatic resources allocation (VRAM and hardware sprite) and visibility is set to ON.<br>
 *      You can change these defaults settings later by calling SPR_setVRAMTileIndex(..), SPR_setAutoTileUpload(..) and SPR_setVisibility(..) methods.<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).<br>
 *      IMPORTANT NOTE: sprite allocation can fail (return NULL) because of automatic VRAM allocation even if there is enough VRAM available,
 *      this can happen because of the VRAM fragmentation.<br> You can use #SPR_addSpriteSafe(..) method instead so it take care about VRAM fragmentation.
 *
 *  \see SPR_addSpriteEx(..)
 *  \see SPR_addSpriteSafe(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSprite(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut);
/**
 *  \brief
 *      Adds a new sprite with specified parameters and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \param flag
 *      specific settings for this sprite:<br>
 *      #SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE = Disable delaying of frame update when we are running out of DMA capacity.<br>
 *          If you set this flag then sprite frame update always happen immediately but may lead to some graphical glitches (tiles data and sprite table data not synchronized).
 *          You can use SPR_setDelayedFrameUpdate(..) method to change this setting.<br>
 *      #SPR_FLAG_AUTO_VISIBILITY = Enable automatic sprite visibility calculation (you can also use SPR_setVisibility(..) method).<br>
 *      #SPR_FLAG_FAST_AUTO_VISIBILITY = Enable fast computation for the automatic visibility calculation (disabled by default)<br>
 *          If you set this flag the automatic visibility calculation will be done globally for the (meta) sprite and not per internal
 *          hardware sprite. This result in faster visibility computation at the expense of some waste of hardware sprite (scanline limit).
 *          You can set the automatic visibility computation by using SPR_setVisibility(..) method.<br>
 *      #SPR_FLAG_AUTO_VRAM_ALLOC = Enable automatic VRAM allocation (enabled by default)<br>
 *          If you don't set this flag you will have to manually define VRAM tile index position for this sprite with the <i>attribut</i> parameter or by using the #SPR_setVRAMTileIndex(..) method<br>
 *      #SPR_FLAG_AUTO_TILE_UPLOAD = Enable automatic upload of sprite tiles data into VRAM (enabled by default)<br>
 *          If you don't set this flag you will have to manually upload tiles data of sprite into the VRAM (you can change this setting using #SPR_setAutoTileUpload(..) method).<br>
 *      #SPR_FLAG_INSERT_HEAD = Allow to insert the sprite at the start/head of the list.<br>
 *          When you use this flag the sprite will be inserted at the head of the list making it top most (equivalent to #SPR_setDepth(#SPR_MIN_DEPTH))<br>
 *          while default insertion position is at the end of the list (equivalent to #SPR_setDepth(#SPR_MAX_DEPTH))<br>
 *      <br>
 *      It's recommended to use the following default settings:<br>
 *      SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD<br>
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses the provided flag setting for automatic resources allocation and sprite visibility computation.<br>
 *      If auto visibility is not enabled then sprite is considered as not visible by default (see SPR_setVisibility(..) method).<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).
 *
 *  \see SPR_addSpriteSafe(..)
 *  \see SPR_addSpriteEx(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSpriteExSafe(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut, u16 flag);
/**
 *  \brief
 *      Adds a new sprite with auto resource allocation enabled and returns it.
 *
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *  \param x
 *      default X position.
 *  \param y
 *      default Y position.
 *  \param attribut
 *      sprite attribut (see TILE_ATTR() macro).
 *  \return the new sprite or <i>NULL</i> if the operation failed (some logs can be generated in the KMod console in this case)
 *
 *      By default the sprite uses automatic resources allocation (VRAM and hardware sprite) and visibility is set to ON.<br>
 *      You can change these defaults settings later by calling SPR_setVRAMTileIndex(..), SPR_setAutoTileUpload(..) and SPR_setVisibility(..) methods.<br>
 *      You can release all sprite resources by using SPR_releaseSprite(..) or SPR_reset(..).<
 *
 *  \see SPR_addSpriteSafeEx(..)
 *  \see SPR_addSprite(..)
 *  \see SPR_releaseSprite(..)
 */
Sprite* SPR_addSpriteSafe(const SpriteDefinition* spriteDef, s16 x, s16 y, u16 attribut);

/**
 *  \brief
 *      Release the specified sprite (no more visible and release its resources).
 *
 *  \param sprite
 *      Sprite to release
 *
 *      This method release resources for the specified Sprite object and remove it from the screen at next SPR_update() call.
 *
 *  \see SPR_releasesSprite(..)
 */
void SPR_releaseSprite(Sprite* sprite);
/**
 *  \brief
 *      Returns the number of active sprite (number of sprite added with SPR_addSprite(..) or SPR_addSpriteEx(..) methods).
 */
u16 SPR_getNumActiveSprite(void);
/**
 *  \brief
 *      Returns the (maximum) number of used VDP sprite from current active sprites (sum of maximum hardware sprite usage from all active sprites).
 */
u16 SPR_getUsedVDPSprite(void);
/**
 *  \brief
 *      Returns the current remaining free VRAM (in tile) for the sprite engine.
 */
u16 SPR_getFreeVRAM(void);
/**
 *  \brief
 *      Return the current largest free VRAM block size (in tile) for the sprite engine.
 */
u16 SPR_getLargestFreeVRAMBlock(void);

/**
 *  \brief
 *      Indicate that we want to prevent adding a new sprite if there is possibly not enough hardware sprite to display it
 *      (counting the maximum hardware sprite usage from all active sprites) and so this ensures to always display all of them.<br>
 *      By default the sprite engine allow to add a new sprite even if we may run out of hardware sprite to display all of them,
 *      that can lead to some sprite disappearence in some situations but ultimately it allows to handle more active sprites at once.
 *
 *  \see SPR_disableVDPSpriteChecking(..)
 *  \see SPR_getNumVDPSprite(..)
 */
void SPR_enableVDPSpriteChecking();
/**
 *  \brief
 *      Indicate that we allow the sprite engine to add a new sprite even if we may run out of hardware sprite to display all of them,
 *      that can lead to some sprite disappearence in some situation but ultimately it allows to handle more active sprites at once (default behavior).
 *
 *  \see SPR_enableVDPSpriteChecking(..)
 *  \see SPR_getNumVDPSprite(..)
 */
void SPR_disableVDPSpriteChecking();
/**
 *  \brief
 *      Defragment allocated VRAM for sprites, that can help when sprite allocation fail (SPR_addSprite(..) or SPR_addSpriteEx(..) return <i>NULL</i>).
 */
void SPR_defragVRAM(void);

/**
 *  \brief
 *      Load all frames of SpriteDefinition using DMA at specified VRAM tile index and return the indexes table.<br>
 *      <b>WARNING: This function should be call at init/loading time as it can be quite long (several frames)</b>
 *
 *  \param sprDef
 *      the SpriteDefinition we want to load frame data in VRAM.
 *  \param index
 *      the tile position in VRAM where we will upload all sprite frame tiles data.
 *  \param totalNumTile
 *      if not NULL then the function will store here the total number of tile used to load all animation frames.
  *  \param tm
 *      Transfer method to upload sprite frame data.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
*
 *   Load all frames of spriteDef at specified VRAM tile index and return the indexes table.<br>
 *   The returned index table is a dynamically allocated 2D table[anim][frame] so you need to release it using #MEM_free(..)
 *   when you don't need the table anymore.<br>
 *   You can use the frame change callback (see #SPR_setFrameChangeCallback(..)) to automatically update the VRAM index using the indexes table:<br>
 *   <code>frameIndexes = SPR_loadAllFrames(sprite->definition, ind);<br>
 *   SPR_setFrameChangeCallback(sprite, &frameChanged);<br>
 *   ....<br>
 *   void frameChanged(Sprite* sprite)<br>
 *   {<br>
 *       u16 tileIndex = frameIndexes[sprite->animInd][sprite->frameInd];<br>
 *       SPR_setVRAMTileIndex(sprite, tileIndex);<br>
 *   }</code>
 *
 *  \return the 2D indexes table or NULL if there is not enough memory to allocate the table.
 *  \see SPR_setFrameChangeCallback(...);
 */
u16** SPR_loadAllFramesEx(const SpriteDefinition* sprDef, u16 index, u16* totalNumTile, TransferMethod tm);
/**
 *  \brief
 *      Same as all frames of SpriteDefinition at specified VRAM tile index and return the indexes table.<br>
 *      <b>WARNING: This function should be call at init/loading time as it can be quite long (several frames)</b>
 *
 *  \param sprDef
 *      the SpriteDefinition we want to load frame data in VRAM.
 *  \param index
 *      the tile position in VRAM where we will upload all sprite frame tiles data.
 *  \param totalNumTile
 *      if not NULL then the function will store here the total number of tile used to load all animation frames.
 *
 *   Load all frames of spriteDef (using DMA) at specified VRAM tile index and return the indexes table.<br>
 *   The returned index table is a dynamically allocated 2D table[anim][frame] so you need to release it using #MEM_free(..)
 *   when you don't need the table anymore.<br>
 *   You can use the frame change callback (see #SPR_setFrameChangeCallback(..)) to automatically update the VRAM index using the indexes table:<br>
 *   <code>frameIndexes = SPR_loadAllFrames(sprite->definition, ind);<br>
 *   SPR_setFrameChangeCallback(sprite, &frameChanged);<br>
 *   ....<br>
 *   void frameChanged(Sprite* sprite)<br>
 *   {<br>
 *       u16 tileIndex = frameIndexes[sprite->animInd][sprite->frameInd];<br>
 *       SPR_setVRAMTileIndex(sprite, tileIndex);<br>
 *   }</code>
 *
 *  \return the 2D indexes table or NULL if there is not enough memory to allocate the table.
 *  \see SPR_setFrameChangeCallback(...);
 */
u16** SPR_loadAllFrames(const SpriteDefinition* sprDef, u16 index, u16* totalNumTile);
/**
 *  \brief
 *      Same as #SPR_loadAllFrames(..) but only computes the indexes table without actually loading the Sprite frame data to VRAM (see #SPR_loadAllTiles(..) for that).
 *
 *  \param sprDef
 *      the SpriteDefinition we want to compute the indexes table.
 *  \param index
 *      the tile position in VRAM where we want to upload all sprite frame data (note that VRAM data upload won't be done here).
 *  \param totalNumTile
 *      if not NULL then the function will store here the total number of tile used to load all animation frames.
 *
 *  \return the 2D indexes table or NULL if there is not enough memory to allocate the table.
 *  \see SPR_loadAllFrames(...)
 *  \see SPR_loadAllTiles(...)
 */
u16** SPR_loadAllIndexes(const SpriteDefinition* sprDef, u16 index, u16* totalNumTile);
/**
 *  \brief
 *      Same as #SPR_loadAllFrames(..) but only perform the Sprite tile data upload process, SPR_loadAllIndexes(..) should be called first to compute the indexes table.
 *
 *  \param sprDef
 *      the SpriteDefinition we want to load frame data in VRAM.
 *  \param index
 *      the tile position in VRAM where we will upload all sprite frame tiles data.
 *  \param index
 *      the 2D indexes table providing the tile index position in VRAM for each sprite frame (previously build using #SPR_loadAllIndexes(..) method).
 *  \param tm
 *      Transfer method to upload sprite frame data.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *
 *  \return the number of tile uploaded to VRAM.
 *  \see SPR_loadAllFrames(...)
 *  \see SPR_loadAllIndexes(...)
 */
u16 SPR_loadAllTiles(const SpriteDefinition* sprDef, u16 index, u16** indexes, const TransferMethod tm);

/**
 *  \brief
 *      Set the Sprite Definition.
 *
 *  \param sprite
 *      Sprite to set definition for.
 *  \param spriteDef
 *      the SpriteDefinition data to assign to this sprite.
 *
 *   Set the Sprite Definition for this sprite.<br>
 *   By default the first frame of the first animation from Sprite Definition is loaded.
 *
 *  \return FALSE if auto resource allocation failed, TRUE otherwise.
 */
bool SPR_setDefinition(Sprite* sprite, const SpriteDefinition* spriteDef);
/**
 *  \brief
 *      Get sprite position X.
 */
s16 SPR_getPositionX(Sprite* sprite);
/**
 *  \brief
 *      Get sprite position Y.
 */
s16 SPR_getPositionY(Sprite* sprite);
/**
 *  \brief
 *      Set sprite position.
 *
 *  \param sprite
 *      Sprite to set position for
 *  \param x
 *      X position
 *  \param y
 *      Y position
 */
void SPR_setPosition(Sprite* sprite, s16 x, s16 y);
/**
 *  \brief
 *      Set sprite Horizontal Flip attribut.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The horizontal flip attribut value (TRUE or FALSE)
 */
void SPR_setHFlip(Sprite* sprite, bool value);
/**
 *  \brief
 *      Set sprite Vertical Flip attribut.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The vertical flip attribut value (TRUE or FALSE)
 */
void SPR_setVFlip(Sprite* sprite, bool value);
/**
 *  \brief
 *      Set sprite Palette index to use.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The palette index to use for this sprite (PAL0, PAL1, PAL2 or PAL3)
 */
void SPR_setPalette(Sprite* sprite, u16 value);
/**
 *  \brief
 *      Set sprite Priority attribut.
 *
 *  \param sprite
 *      Sprite to set attribut for
 *  \param value
 *      The priority attribut value (TRUE or FALSE)
 */
void SPR_setPriority(Sprite* sprite, bool value);
/**
 *  \deprecated
 *      Use #SPR_setPriority(..) instead
 */
#define SPR_setPriorityAttribut(sprite, value)      _Pragma("GCC error \"This method is deprecated, use SPR_setPriority(..) instead.\"")
/**
 *  \brief
 *      Set sprite depth (for sprite display ordering)
 *
 *  \param sprite
 *      Sprite to set depth for
 *  \param value
 *      The depth value (SPR_MIN_DEPTH to set always on top)
 *
 *  Sprite having lower depth are display in front of sprite with higher depth.<br>
 *  The sprite is *immediately* sorted when its depth value is changed.
 */
void SPR_setDepth(Sprite* sprite, s16 value);
/**
 *  \brief
 *      Same as #SPR_setDepth(..)
 */
void SPR_setZ(Sprite* sprite, s16 value);
/**
 *  \brief
 *      Set sprite depth so it remains above others sprite - same as SPR_setDepth(SPR_MIN_DEPTH)
 *
 *  \param sprite
 *      Sprite to change depth for
 *
 *  \see SPR_setDepth(Sprite*)
 */
void SPR_setAlwaysOnTop(Sprite* sprite);
/**
 *  \brief
 *      Set sprite depth so it remains behind others sprite - same as SPR_setDepth(SPR_MAX_DEPTH)
 *
 *  \param sprite
 *      Sprite to change depth for
 *
 *  \see SPR_setDepth(Sprite*)
 */
void SPR_setAlwaysAtBottom(Sprite* sprite);
/**
 *  \brief
 *      Set current sprite animation and frame.
 *
 *  \param sprite
 *      Sprite to set animation and frame for
 *  \param anim
 *      animation index to set
 *  \param frame
 *      frame index to set
 */
void SPR_setAnimAndFrame(Sprite* sprite, s16 anim, s16 frame);
/**
 *  \brief
 *      Set current sprite animation.
 *
 *  \param sprite
 *      Sprite to set animation for
 *  \param anim
 *      animation index to set.
 */
void SPR_setAnim(Sprite* sprite, s16 anim);
/**
 *  \brief
 *      Set current sprite frame.
 *
 *  \param sprite
 *      Sprite to set frame for
 *  \param frame
 *      frame index to set.
 */
void SPR_setFrame(Sprite* sprite, s16 frame);
/**
 *  \brief
 *      Pass to the next sprite frame.
 *
 *  \param sprite
 *      Sprite to pass to next frame for
 */
void SPR_nextFrame(Sprite* sprite);
/**
 *  \brief
 *      Enable/disable auto animation for the current animation (default is on).<br>
 *
 *  \param sprite
 *      Sprite we want to enable/disable auto animation.
 *  \param value
 *      TRUE to enable auto animation (default), FALSE otherwise
 *
 *  \see #SPR_getAutoAnimation(Sprite*)
 */
void SPR_setAutoAnimation(Sprite* sprite, bool value);
/**
 *  \brief
 *      Return TRUE if auto animation is enabled, FALSE otherwise.
 *
 *  \see #SPR_setAnimationLoop(Sprite*, bool)
 */
bool SPR_getAutoAnimation(Sprite* sprite);
/**
 *  \brief
 *      Enable/disable animation loop (default is on).<br>
 *      When disable the sprite will stay on the last animation frame when animation ended instead of restarting it.
 *
 *  \param sprite
 *      Sprite we want to enable/disable animation loop for.
 *  \param value
 *      TRUE to enable animation loop (default), FALSE otherwise
 *
 *  \see SPR_FLAG_DISABLE_ANIMATION_LOOP
 *  \see #SPR_getAnimationDone(Sprite*)
 */
void SPR_setAnimationLoop(Sprite* sprite, bool value);
/**
 *  \brief
 *      Returns TRUE if the sprite reached the end of the current animation.<br>
 *      When auto animation is enabled (see SPR_setAutoAnimation(..)) the function returns TRUE only when we reached
 *      the last *tick* of the last animation frame.<br>
 *      When auto animation is disabled the function returns TRUE as soon we are on last animation frame.
 *
 *  \see #SPR_setAutoAnimation(Sprite*, bool)
 */
bool SPR_isAnimationDone(Sprite* sprite);
/**
 *  \brief
 *      Set the VRAM tile position reserved for this sprite.
 *
 *  \param sprite
 *      Sprite to set the VRAM tile position for
 *  \param value
 *      the tile position in VRAM where we will upload the sprite tiles data.<br>
 *      Use <b>-1</b> for auto allocation.<br>
 *  \return FALSE if auto allocation failed (can happen only if sprite is currently active), TRUE otherwise
 *
 *  By default the Sprite Engine auto allocate VRAM for sprites tiles but you can force
 *  manual allocation and fix the sprite tiles position in VRAM with this method.
 */
bool SPR_setVRAMTileIndex(Sprite* sprite, s16 value);
/**
 *  \brief
 *      Enable/disable the automatic upload of sprite tiles data into VRAM.
 *
 *  \param sprite
 *      Sprite we want to enable/disable auto tile upload for
 *  \param value
 *      TRUE to enable the automatic upload of sprite tiles data into VRAM.<br>
 *      FALSE to disable it (mean you have to handle that on your own).<br>
 */
void SPR_setAutoTileUpload(Sprite* sprite, bool value);
/**
 *  \brief
 *      Enable/disable the delayed frame update.
 *
 *  \param sprite
 *      Sprite we want to enable/disable delayed frame update
 *  \param value
 *      TRUE to enable the delayed frame update when DMA is running out of transfert capacity (default).<br>
 *      FALSE to disable it. In which case sprite frame is always updated immediately but that may cause graphical glitches (update during active display).<br>
 *
 *  \see #SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE
 */
void SPR_setDelayedFrameUpdate(Sprite* sprite, bool value);
/**
 *  \brief
 *      Set the frame change event callback for this sprite.
 *
 *  \param sprite
 *      Sprite we want to set the frame change callback
 *  \param callback
 *      the callback (function pointer) to call when we just changed the animation frame for this sprite.
 *
 *      The callback (if not NULL) will be called on frame change process during #SPR_update() call (CAUTION: sprite->status field is not up to date at this point).<br>
 *      It let opportunity to the developer to apply special behavior or process when sprite frame just changed:<br>
 *      for instance we can disable animation looping by setting sprite->timer to 0 when we meet the last animation frame.
 *
 *  \see #FrameChangeCallback
 */
void SPR_setFrameChangeCallback(Sprite* sprite, FrameChangeCallback* callback);

/**
 *  \brief
 *      Return the <i>visibility</i> state for this sprite.<br>
 *      WARNING: this is different from SPR_isVisible(..) method, possible value are:<br>
 *      SpriteVisibility.VISIBLE        = sprite is visible<br>
 *      SpriteVisibility.HIDDEN         = sprite is not visible<br>
 *      SpriteVisibility.AUTO_FAST      = visibility is automatically computed - global visibility (only using sprite X position)<br>
 *      SpriteVisibility.AUTO_SLOW      = visibility is automatically computed - per hardware sprite visibility (only using sprite X position)<br>
 *
 *  \param sprite
 *      Sprite to return <i>visibility</i> state
 *
 *  \return
 *       the <i>visibility</i> state for this sprite.
 *
 *  \see SPR_isVisible(...)
 *  \see SPR_setVisibility(...)
 */
SpriteVisibility SPR_getVisibility(Sprite* sprite);
/**
 *  \brief
 *      Return the visible state for this sprite (meaningful only if AUTO visibility is enabled, see #SPR_setVisibility(..) method).<br>
 *
 *  \param sprite
 *      Sprite to return <i>visible</i> state
 *  \param recompute
 *      Force visibility computation.<br>
 *      Only required if SPR_update() wasn't called since last sprite position change (note that can force the frame update processing).
 *
 *  Note that only the sprite X position is used to determine if it's visible (see #SPR_setVisibility(..) method for more information)

 *  \return
 *      the <i>visible</i> state for this sprite.
 *
 *  \see SPR_setVisibility(...)
 */
bool SPR_isVisible(Sprite* sprite, bool recompute);

/**
 *  \brief
 *      Set the <i>visibility</i> state for this sprite.
 *
 *  \param sprite
 *      Sprite to set the <i>visibility</i> information
 *  \param value
 *      Visibility value to set.<br>
 *      SpriteVisibility.VISIBLE        = sprite is visible<br>
 *      SpriteVisibility.HIDDEN         = sprite is not visible<br>
 *      SpriteVisibility.AUTO_FAST      = visibility is automatically computed - global visibility (only using sprite X position)<br>
 *      SpriteVisibility.AUTO_SLOW      = visibility is automatically computed - per hardware sprite visibility (only using sprite X position)<br>
 *
 *      The only interest in having a sprite hidden / not visible is to avoid having it consuming scanline sprite budget:<br>
 *      The VDP is limited to a maximum of 20 sprites or 320 pixels of sprite per scanline (16 sprites/256 px in H32 mode).<br>
 *      If we reach this limit, following sprites won't be renderer so it's important to try to optimize the number of sprites on a single scanline.<br>
 *      When a sprite is set to not visible (automatically or manually), its position is set offscreen *vertically* so it won't eat
 *      anymore the scanline sprite rendering budget, only X position test is required for that so a sprite is determined not visible only using its X coordinate.
 *
 *  \see SPR_computeVisibility(...)
 *  \see SPR_getVisibility(...)
 *  \see SPR_isVisible(...)
 */
void SPR_setVisibility(Sprite* sprite, SpriteVisibility value);
/**
 *  \deprecated Use #SPR_setVisibility(..) method instead.
 */
#define SPR_setAlwaysVisible(sprite, value)     _Pragma("GCC error \"This method is deprecated, use SPR_setVisibility(..) instead.\"")
/**
 *  \deprecated Use #SPR_setVisibility(..) method instead.
 */
#define SPR_setNeverVisible(sprite, value)      _Pragma("GCC error \"This method is deprecated, use SPR_setVisibility(..) instead.\"")
/**
 *  \deprecated Use SPR_isVisible(..) instead.
 */
#define SPR_computeVisibility(sprite)           _Pragma("GCC error \"This method is deprecated, use SPR_isVisible(sprite, TRUE) instead.\"")

/**
 *  \brief
 *      Clear all displayed sprites.
 *
 *  This method allow to quickly hide all sprites (without releasing their resources).<br>
 *  Sprites can be displayed again just by calling SPR_update().
 */
void SPR_clear(void);
/**
 *  \brief
 *      Update and display the active list of sprite.
 *
 *  This actually updates all internal active sprites states and prepare the sprite list
 *  cache to send it to the hardware (VDP) at Vint.
 *
 *  \see #SPR_addSprite(..)
 */
void SPR_update(void);

/**
 *  \brief
 *      Log the profil informations (when enabled) in the KMod message window.
 */
void SPR_logProfil(void);
/**
 *  \brief
 *      Log the sprites informations (when enabled) in the KMod message window.
 */
void SPR_logSprites(void);


#endif // _SPRITE_ENG_H_

#endif
````

## File: inc/sram.h
````c
/**
 *  \file sram.h
 *  \brief SRAM (Static RAM used for save backup) support.
 *  \author Chilly Willy & Stephane Dallongeville
 *  \date 08/2012
 *
 * This unit provides methods to read from or write to SRAM.<br>
 * By default we suppose SRAM is 8bit and connected to odd address.<br>
 * You can change to even address by changing SRAM_BASE from 0x200001 to 0x200000 and rebuild the library.<br>
 *<br>
 * Informations about SRAM (taken from Segaretro.org):<br>
 * The regions specified by 0xA130F9-0xA130FF (0x200000-0x3FFFFF) can be either ROM or RAM and can be write-protected.<br>
 * Here is the layout of the register as far as I know:<br>
 *<br>
 *        7  6  5  4  3  2  1  0<br>
 *      +-----------------------+<br>
 *      |??|??|??|??|??|??|WP|MD|<br>
 *      +-----------------------+<br>
 *<br>
 *      MD:     Mode -- 0 = ROM, 1 = RAM<br>
 *      WP:     Write protect -- 0 = writable, 1 = not writable
 */

#ifndef _SRAM_H_
#define _SRAM_H_


#include "mapper.h"


#define SRAM_CONTROL    MAPPER_BASE
#define SRAM_BASE       0x200001


/**
 *  \brief
 *      Enable SRAM in Read Write mode.
 */
void SRAM_enable(void);
/**
 *  \brief
 *      Enable SRAM in Read Only mode.
 */
void SRAM_enableRO(void);
/**
 *  \brief
 *      Disable SRAM.
 */
void SRAM_disable(void);

/**
 *  \brief
 *      Read a byte from the SRAM.
 *
 *  \param offset
 *      Offset where we want to read.
 *  \return value.
 */
u8 SRAM_readByte(u32 offset);
/**
 *  \brief
 *      Read a word from the SRAM.
 *
 *  \param offset
 *      Offset where we want to read.
 *  \return value.
 */
u16 SRAM_readWord(u32 offset);
/**
 *  \brief
 *      Read a long from the SRAM.
 *
 *  \param offset
 *      Offset where we want to read.
 *  \return value.
 */
u32 SRAM_readLong(u32 offset);
/**
 *  \brief
 *      Write a byte to the SRAM.
 *
 *  \param offset
 *      Offset where we want to write.
 *  \param val
 *      Value wto write.
 */
void SRAM_writeByte(u32 offset, u8 val);
/**
 *  \brief
 *      Write a word to the SRAM.
 *
 *  \param offset
 *      Offset where we want to write.
 *  \param val
 *      Value wto write.
 */
void SRAM_writeWord(u32 offset, u16 val);
/**
 *  \brief
 *      Write a long to the SRAM.
 *
 *  \param offset
 *      Offset where we want to write.
 *  \param val
 *      Value wto write.
 */
void SRAM_writeLong(u32 offset, u32 val);


#endif // _SRAM_H_
````

## File: inc/string.h
````c
/**
 *  \file string.h
 *  \brief String manipulations
 *  \author Stephane Dallongeville
 *  \author Paspallas Dev
 *  \author Jack Nolddor
 *  \author Jesus Alonso (doragasu)
 *  \date 08/2011
 *
 * This unit provides basic null terminated string operations and type conversions.
 */

#if (ENABLE_NEWLIB != 0) && !defined(_NEWLIB_STRING_H_)
#define _NEWLIB_STRING_H_
#include_next <string.h> // Include string.h from newlib
#undef _STRING_H_        // Will be defined again just below
#endif

#ifndef _STRING_H_
#define _STRING_H_

#if (ENABLE_NEWLIB == 0)

/**
 *  \brief
 *      Test if specified character is a digit or not
 */
#define isdigit(c)      ((c) >= '0' && (c) <= '9')


typedef void *__gnuc_va_list;
typedef __gnuc_va_list va_list;

#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)


/**
 *  \brief
 *      Calculate the length of a string (limited to 65535 characters maximum).
 *
 *  \param str
 *      The string we want to calculate the length.
 *  \return length of string
 *
 * This function calculates and returns the length of the specified string (limited to 65535 characters maximum).
 */
u16 strlen(const char *str);
/**
 *  \brief
 *      Compute the length of a string, to a maximum number of bytes.
 *
 *  \param str
 *      The string whose length you want to calculate.
 *  \param maxlen
 *      The maximum length to check.
 *
 *  \return The minimum of 'maxlen' and the number of characters that precede the terminating null character.
 *
 *  The strnlen() function computes the length of the string pointed to by 'str', not including the terminating null character ('\0'), <br>
 *  up to a maximum of 'maxlen' bytes. The function doesn't check any more than the first 'maxlen' bytes.
 */
u16 strnlen(const char *str, u16 maxlen);
/**
 *  \brief
 *      Compare the 2 strings.
 *
 *  \param str1
 *      The string we want to compare.
 *  \param str2
 *      The string we want to compare.
 *  \return an integral value indicating the relationship between the strings:<br>
 *      A zero value indicates that both strings are equal.<br>
 *      A value greater than zero indicates that the first character that does not match has a greater value in str1 than in str2<br>
 *      A value less than zero indicates the opposite.
 *
 * This function starts comparing the first character of each string.<br>
 * If they are equal to each other, it continues with the following pairs until
 * the characters differ or until a terminating null-character is reached.
 */
s8 strcmp(const char *str1, const char *str2);

/**
 *  \brief
 *      Copies a string.
 *
 *  \param dest
 *      Destination string (it must be large enough to receive the copy).
 *  \param src
 *      Source string.
 *  \return pointer on destination string.
 *
 * Copies the source string to destination.
 */
char* strcpy(char *dest, const char *src);
/**
 *  \brief
 *      Copies the first 'len' characters of string.
 *
 *  \param dest
 *      Destination string (its size must be >= (len + 1)).
 *  \param src
 *      Source string.
 *  \param len
 *      Maximum number of character to copy.
 *  \return pointer on destination string.
 *
 * Copies the first 'len' characters source string to destination.
 */
char* strncpy(char *dest, const char *src, u16 len);
/**
 *  \brief
 *      Concatenate two strings.
 *
 *  \param dest
 *      Destination string (it must be large enough to receive appending).
 *  \param src
 *      Source string.
 *  \return pointer on destination string.
 *
 * Appends the source string to the destination string.
 */
char* strcat(char *dest, const char *src);
/**
 *  \brief
 *      Search for a character in the specified string.
 *
 *  \param dest
 *      Destination string (it must be large enough to receive appending).
 *  \param src
 *      Source string.
 *  \return pointer on destination string.
 *
 * Returns a pointer to the first occurrence of <i>c</i> in the given string.<br>
 * The function returns NULL if the specified character is not found.
 */
char* strchr(const char *from, char c);

/**
 *  \brief
 *      Composes a string with the same text that would be printed if format was used on printf,
 *      but instead of being printed, the content is stored as a C string in the buffer pointed by str.
 *
 *  \param buf
 *      Destination string (it must be large enough to receive result).
 *  \param fmt
 *      C string that contains the text to be written to destination string.<br />
 *      It can optionally contain embedded format specifiers.
 *  \param args
 *      Optional argument(s). Depending on the format string, the function may expect a sequence of additional arguments,<br>
 *      each containing a value to be used to replace a format specifier in the format string.
 *
 *      There should be at least as many of these arguments as the number of values specified in the format specifiers.<br>
 *      Additional arguments are ignored by the function.
 *
 *  \return On success, the total number of characters written is returned..
 *
 *  Copy the string pointed by 'fmt' param to the 'buffer' param.<br>
 *  If 'fmt' includes format specifiers (subsequences beginning with %), the additional arguments following format are
 *  formatted and inserted in the resulting string replacing their respective specifiers
 */
int vsprintf(char *buffer, const char *fmt, va_list args);
/**
 *  \brief
 *      Composes a string with the same text that would be printed if format was used on printf,
 *      but instead of being printed, the content is stored as a C string in the buffer pointed by str.
 *
 *  \param buffer
 *      Destination string (it must be large enough to receive result).
 *  \param fmt
 *      C string that contains the text to be written to destination string.<br />
 *      It can optionally contain embedded format specifiers.
 *
 *  \param ... (additional arguments)
 *      Depending on the format string, the function may expect a sequence of additional arguments, <br>
 *      each containing a value to be used to replace a format specifier in the format string.
 *
 *      There should be at least as many of these arguments as the number of values specified in the format specifiers. <br>
 *      Additional arguments are ignored by the function.
 *
 *  \return On success, the total number of characters written is returned..
 *
 *  Copy the string pointed by 'fmt' param to the 'buffer' param.<br>
 *  If 'fmt' includes format specifiers (subsequences beginning with %), the additional arguments following format are
 *  formatted and inserted in the resulting string replacing their respective specifiers
 */
int sprintf(char *buffer, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

#endif  // ENABLE_NEWLIB

/**
 *  \brief
 *      Clear a string.
 *
 *  \param str
 *      string to clear.
 *  \return pointer on the given string.
 *
 * Clear the specified string.
 */
char* strclr(char *str);
/**
 *  \brief
 *      Replace the given character in a string.
 *
 *  \param str
 *      The string to operate on.
 *  \param oldc
 *      The character being replaced.
 *  \param newc
 *      The character 'oldc' is replaced with.
 *  \return pointer to the null byte at the end of 'str'.
 *
 * Replace all occurrences of character in a null-terminated string.
 */
char *strreplacechar(char *str, char oldc, char newc);
/**
 *  \brief
 *      Convert a s32 value to string (input value should be in [-500000000..500000000] range).
 *
 *  \param value
 *      The s32 integer value to convert to string (input value should be in [-500000000..500000000] range).
 *  \param str
 *      Destination string (it must be large enough to receive result).
 *  \param minsize
 *      Minimum size of resulting string.
 *  \return string length
 *
 * Converts the specified s32 value to string.<br>
 * If resulting value is shorter than requested minsize the method prepends result with '0' character.
 */
u16 intToStr(s32 value, char *str, u16 minsize);
/**
 *  \brief
 *      Convert a u32 value to string (input value should be in [0..500000000] range).
 *
 *  \param value
 *      The u32 integer value to convert to string (input value should be in [0..500000000] range).
 *  \param str
 *      Destination string (it must be large enough to receive result).
 *  \param minsize
 *      Minimum size of resulting string.
 *  \return string length
 *
 * Converts the specified u32 value to string.<br>
 * If resulting value is shorter than requested minsize the method prepends result with '0' character.
 */
u16 uintToStr(u32 value, char *str, u16 minsize);
/**
 *  \brief
 *      Convert a u32 value to hexadecimal string.
 *
 *  \param value
 *      The u32 integer value to convert to hexadecimal string.
 *  \param str
 *      Destination string (it must be large enough to receive result).
 *  \param minsize
 *      Minimum size of resulting string.
 *  \return string length
 *
 * Converts the specified u32 value to hexadecimal string.<br>
 * If resulting value is shorter than requested minsize the method prepends result with '0' character.
 */
u16 intToHex(u32 value, char *str, u16 minsize);

/**
 *  \brief
 *      Convert a fix16 value to string.
 *
 *  \param value
 *      The fix16 value to convert to string.
 *  \param str
 *      Destination string (it must be large enough to receive result).
 *  \param numdec
 *      Number of wanted decimal.
 *
 * Converts the specified fix16 value to string.<br>
 */
void fix16ToStr(fix16 value, char *str, u16 numdec);
/**
 *  \brief
 *      Convert a fix32 value to string.
 *
 *  \param value
 *      The fix32 value to convert to string.
 *  \param str
 *      Destination string (it must be large enough to receive result).
 *  \param numdec
 *      Number of wanted decimal.
 *
 * Converts the specified fix32 value to string.<br>
 */
void fix32ToStr(fix32 value, char *str, u16 numdec);
/**
 *  \brief
 *      Convert a fastfix16 value to string.
 *
 *  \param value
 *      The fastfix16 value to convert to string.
 *  \param str
 *      Destination string (it must be large enough to receive result).
 *  \param numdec
 *      Number of wanted decimal.
 *
 * Converts the specified fastfix16 value to string.<br>
 */
void fastFix16ToStr(fastfix16 value, char *str, u16 numdec);
/**
 *  \brief
 *      Convert a fastfix32 value to string.
 *
 *  \param value
 *      The fastfix32 value to convert to string.
 *  \param str
 *      Destination string (it must be large enough to receive result).
 *  \param numdec
 *      Number of wanted decimal.
 *
 * Converts the specified fastfix32 value to string.<br>
 */
void fastFix32ToStr(fastfix32 value, char *str, u16 numdec);
/**
 *  \brief
 *      Same as fix16ToStr(..)
 *  \see fix16ToStr
 */
void F16_toStr(fix16 value, char *str, u16 numdec);
/**
 *  \brief
 *      Same as fix32ToStr(..)
 *  \see fix32ToStr
 */
void F32_toStr(fix32 value, char *str, u16 numdec);
/**
 *  \brief
 *      Same as fastFix16ToStr(..)
 *  \see fastFix16ToStr
 */
void FF16_toStr(fastfix16 value, char *str, u16 numdec);
/**
 *  \brief
 *      Same as fastFix32ToStr(..)
 *  \see fastFix32ToStr
 */
void FF32_toStr(fastfix32 value, char *str, u16 numdec);

#endif // _STRING_H_
````

## File: inc/sys.h
````c
/**
 *  \file sys.h
 *  \brief Entry point unit / Interrupt callback / System
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit contains SGDK initialization / reset methods, IRQ callbacks and others system stuff.
 */

#ifndef _SYS_H_
#define _SYS_H_


#define PROCESS_PALETTE_FADING      (1 << 0)
#define PROCESS_BITMAP_TASK         (1 << 1)
#define PROCESS_DMA_TASK            (1 << 2)
#define PROCESS_VDP_SCROLL_TASK     (1 << 3)


#define ROM_ALIGN_BIT               17
#define ROM_ALIGN                   (1 << ROM_ALIGN_BIT)
#define ROM_ALIGN_MASK              (ROM_ALIGN - 1)

#define ROM_START                   ROM
#define ROM_END                     (((u32) &_stext) + ((u32) &_sdata))
#define ROM_SIZE                    ((ROM_END + ROM_ALIGN_MASK) & (~ROM_ALIGN_MASK))

/**
 *  \brief
 *      To force method inlining (not sure that GCC does actually care of it)
 */
#define FORCE_INLINE                inline __attribute__((always_inline))

/**
 *  \brief
 *      To force no inlining for this method
 */
#define NO_INLINE                   __attribute__((noinline))

/**
 *  \brief
 *      Put function in .data (RAM) instead of the default .text
 */
#define RAM_SECT                    __attribute__((section(".ramprog")))

/**
 *  \brief
 *      Declare function for the hint callback (generate a RTE to return from interrupt instead of RTS)
 */
#define HINTERRUPT_CALLBACK         __attribute__((interrupt)) void

/**
 *  \brief
 *      Macro for packing structures and enumerates
 */
#define PACKED		                __attribute__((__packed__))


// exist through rom_head.c
typedef struct
{
    char console[16];               /* Console Name (16) */
    char copyright[16];             /* Copyright Information (16) */
    char title_local[48];           /* Domestic Name (48) */
    char title_int[48];             /* Overseas Name (48) */
    char serial[14];                /* Serial Number (2, 12) */
    u16 checksum;                   /* Checksum (2) */
    char IOSupport[16];             /* I/O Support (16) */
    u32 rom_start;                  /* ROM Start Address (4) */
    u32 rom_end;                    /* ROM End Address (4) */
    u32 ram_start;                  /* Start of Backup RAM (4) */
    u32 ram_end;                    /* End of Backup RAM (4) */
    char sram_sig[2];               /* "RA" for save ram (2) */
    u16 sram_type;                  /* 0xF820 for save ram on odd bytes (2) */
    u32 sram_start;                 /* SRAM start address - normally 0x200001 (4) */
    u32 sram_end;                   /* SRAM end address - start + 2*sram_size (4) */
    char modem_support[12];         /* Modem Support (24) */
    char notes[40];                 /* Memo (40) */
    char region[16];                /* Country Support (16) */
} ROMHeader;

extern const ROMHeader rom_header;

// size of text segment --> start of initialized data (RO)
extern u32 _stext;
// size of initialized data segment
extern u32 _sdata;

/**
 *  \brief
 *      Define at which period to do VBlank process (see #SYS_doVBlankProcess() method)
 */
typedef enum
{
    IMMEDIATELY,        /** Start VBlank process immediately whatever we are in blanking period or not */
    ON_VBLANK ,         /** Start VBlank process on VBlank period, start immediatly in we are already in VBlank */
    ON_VBLANK_START     /** Start VBlank process on VBlank *start* period, means that we wait the next *start* of VBlank period if we missed it */
} VBlankProcessTime;

#if LEGACY_ERROR_HANDLER
/**
 *  \brief
 *      Bus error interrupt callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *busErrorCB;
/**
 *  \brief
 *      Address error interrupt callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *addressErrorCB;
/**
 *  \brief
 *      Illegal instruction exception callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *illegalInstCB;
/**
 *  \brief
 *      Division by zero exception callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *zeroDivideCB;
/**
 *  \brief
 *      CHK instruction interrupt callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *chkInstCB;
/**
 *  \brief
 *      TRAPV instruction interrupt callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *trapvInstCB;
/**
 *  \brief
 *      Privilege violation exception callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *privilegeViolationCB;
/**
 *  \brief
 *      Trace interrupt callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *traceCB;
/**
 *  \brief
 *      Line 1x1x exception callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *line1x1xCB;
/**
 *  \brief
 *      Error exception callback.
 *
 * You can modify it to use your own callback (for debug purpose).
 */
extern VoidCallback *errorExceptionCB;
#endif

/**
 *  \brief
 *      Level interrupt callback.
 *
 * You can modify it to use your own callback.
 */
extern VoidCallback *intCB;


/**
 *  \brief
 *      Assert reset
 *
 * Assert reset pin on the 68000 CPU.
 * This is needed to reset some attached hardware.
 */
void SYS_assertReset(void);
/**
 *  \brief
 *      Soft reset
 *
 * Software reset
 */
void SYS_reset(void);
/**
 *  \brief
 *      Hard reset
 *
 * Reset with forced hardware init and memory clear / reset operation.
 */
void SYS_hardReset(void);

/**
 *  \brief
 *      Wait for start of VBlank and do all the VBlank processing (DMA transfers, XGM driver tempo, Joypad pooling..)
 *  \return FALSE if process was canceled because the method was called from V-Int (vertical interrupt) callback
 *      in which case we exit the function as V-Int will be triggered immediately.<br>
 *
 * Do all the SGDK VBlank process.<br>
 * Some specific processing should be done during the Vertical Blank period as the VDP is idle at this time.
 * This is always where we should do all VDP data transfer (using the DMA preferably) but we can also do the processes which
 * has to be done at a frame basis (joypad polling, sound driver sync/update..)<br>
 * In the case of SGDK, calling this method will actually do the following tasks:<br>
 * - flush the DMA queue<br>
 * - process asynchronous palette fading operation<br>
 * - joypad polling<br>
 * <br>
 * Note that VBlank process may be delayed to next VBlank if we missed the start of the VBlank period so that will cause a frame miss.
 */
bool SYS_doVBlankProcess(void);
/**
 *  \brief
 *      Do all the VBlank processing (DMA transfers, XGM driver tempo, Joypad pooling..)
 *  \param processTime
 *      Define at which period we start VBlank process, accepted values are:<br>
 *      <b>IMMEDIATELY</b>      Start VBlank process immediatly whatever we are in blanking period or not
 *                              (*highly discouraged* unless you really know what you're doing !)<br>
 *      <b>ON_VBLANK</b>        Start VBlank process on VBlank period, if we already are in VBlank period
 *                              it starts immediately (discouraged as VBlank period may be shortened and all
 *                              processes cannot be completed in time)<br>
 *      <b>ON_VBLANK_START</b>  Start VBlank process on VBlank *start* period (recommanded as default value).
 *                              That means that if #SYS_doVBlankProcess() is called too late (after the start
 *                              of VBlank) then we force a passive wait for the next start of VBlank so we can
 *                              align the processing with the beggining of VBlank period to ensure fast DMA
 *                              transfert and avoid possible graphical glitches due to VRAM update during active display.<br>
 *  \return FALSE if process was canceled because we forced Start VBlank process (<i>time = ON_VBLANK_START</i>)
 *      and the method was called from V-Int (vertical interrupt) callback in which case we exit the function
 *      as V-Int will be triggered immediately.<br>
 *
 * Wait for Vblank and does all the SGDK VBlank process.<br>
 * Some specific processing should be done during the Vertical Blank period as the VDP is idle at this time.
 * This is always where we should do all VDP data transfer (using the DMA preferably) but we can also do the processes which
 * has to be done at a frame basis (joypad polling, sound driver sync/update..)<br>
 * In the case of SGDK, calling this method will actually do the following tasks:<br>
 * - flush the DMA queue<br>
 * - process asynchronous palette fading operation<br>
 * - joypad polling<br>
 * <br>
 * Note that depending the used <i>time</i> parameter, VBlank process may be delayed to next VBlank so that will wause a frame miss.
 */
bool SYS_doVBlankProcessEx(VBlankProcessTime processTime);

/**
 *  \brief
 *      End the current frame (alias for #SYS_doVBlankProcess(void)).
 *
 *  End the current frame and does all the internal SGDK VBlank process (DMA flush, VDP data upload, async palette fade, scroll update..)
 *
 *  \see SYS_doVBlankProcess(void)
 */
bool SYS_nextFrame(void);

/**
 *  \brief
 *      Returns the current value of the stack pointer register (A7)
 */
u32 SYS_getStackPointer();

/**
 *  \brief
 *      Return current interrupt mask level.
 *
 * See SYS_setInterruptMaskLevel() for more informations about interrupt mask level.
 */
u16 SYS_getInterruptMaskLevel(void);
/**
 *  \brief
 *      Set interrupt mask level.
 *
 * You can disable interrupt depending their level.<br>
 * Interrupt with level <= interrupt mask level are ignored.<br>
 * We have 3 different interrupts:<br>
 * <b>Vertical interrupt (V-INT): level 6</b><br>
 * <b>Horizontal interrupt (H-INT): level 4</b><br>
 * <b>External interrupt (EX-INT): level 2</b><br>
 * Vertical interrupt has the highest level (and so priority) where external interrupt has lowest one.<br>
 * For instance to disable Vertical interrupt just use SYS_setInterruptMaskLevel(6).<br>
 *
 * \see SYS_getInterruptMaskLevel()
 * \see SYS_getAndSetInterruptMaskLevel()
 * \see SYS_setVIntCallback()
 * \see SYS_setHIntCallback()
 */
void SYS_setInterruptMaskLevel(u16 value);

/**
 *  \brief
 *      Set the interrupt mask level to given value and return previous level.
 *
 * You can disable interrupt depending their level.<br>
 * Interrupt with level <= interrupt mask level are ignored.<br>
 * We have 3 different interrupts:<br>
 * <b>Vertical interrupt (V-INT): level 6</b><br>
 * <b>Horizontal interrupt (H-INT): level 4</b><br>
 * <b>External interrupt (EX-INT): level 2</b><br>
 * Vertical interrupt has the highest level (and so priority) where external interrupt has lowest one.<br>
 * For instance to disable Vertical interrupt just use SYS_setInterruptMaskLevel(6).<br>
 *
 * \see SYS_getInterruptMaskLevel()
 * \see SYS_setInterruptMaskLevel()
 * \see SYS_setVIntCallback()
 * \see SYS_setHIntCallback()
 */
u16 SYS_getAndSetInterruptMaskLevel(u16 value);

/**
 *  \brief
 *      Disable interrupts (Vertical, Horizontal and External).
 *
 *
 * This method is used to temporary disable interrupts to protect some processes and should always be followed by SYS_enableInts().<br>
 * You need to protect against interrupts any processes than can be perturbed / corrupted by the interrupt callback code (IO ports access in general but not only).<br>
 * Now by default SGDK doesn't do anything armful in its interrupts handlers (except with the Bitmap engine) so it's not necessary to protect from interrupts by default
 * but you may need it if your interrupts callback code does mess with VDP for instance.<br>
 * Note that you can nest #SYS_disableInts / #SYS_enableInts() calls.
 *
 * \see SYS_enableInts(void)
 */
void SYS_disableInts(void);
/**
 *  \brief
 *      Re-enable interrupts (Vertical, Horizontal and External).
 *
 * This method is used to reenable interrupts after a call to #SYS_disableInts().<br>
 * Note that you can nest #SYS_disableInts / #SYS_enableInts() calls.
 *
 * \see SYS_disableInts(void)
 */
void SYS_enableInts(void);

/**
 *  \brief
 *      Set user 'Vertical Blank' callback method.
 *
 *  \param CB
 *      Pointer to the method to call on Vertical Blank period.<br>
 *      You can remove current callback by passing a <i>NULL</i> pointer here.
 *
 * Vertical blank period starts right at the end of display period.<br>
 * This period is usually used to prepare next frame data (refresh sprites, scrolling ...).<br>
 * SGDK handle that in the #SYS_doVBlankProcess() method and will call the user 'Vertical Blank' from this method after all major tasks.<br>
 * It's recommended to use the 'Vertical Blank' callback instead of the 'VInt' callback if you need to do some VDP accesses.
 *
 * \see SYS_setVIntCallback(VoidCallback *CB);
 */
void SYS_setVBlankCallback(VoidCallback *CB);

/**
 *  \brief
 *      Set 'Vertical Interrupt' callback method, prefer #SYS_setVBlankCallback(..) when possible.
 *
 *  \param CB
 *      Pointer to the method to call on Vertical Interrupt.<br>
 *      You can remove current callback by passing a <i>NULL</i> pointer here.
 *
 * Vertical interrupt happen at the end of display period at the start of the vertical blank period.<br>
 * This period is usually used to prepare next frame data (refresh sprites, scrolling ...) though now
 * SGDK handle most of these process using #SYS_doVBlankProcess() so you can control it manually (do it from main loop or put it in Vint callback).<br>
 * The only things that SGDK always handle from the vint callback is the XGM sound driver music tempo and Bitmap engine phase reset.<br>
 * It's recommended to keep your code as fast as possible as it will eat precious VBlank time, nor you should touch the VDP from your Vint callback
 * otherwise you will need to protect any VDP accesses from your main loop (which is painful), use the SYS_setVIntCallback(..) instead for that.
 *
 * \see SYS_setVBlankCallback(VoidCallback *CB);
 * \see SYS_setHIntCallback(VoidCallback *CB);
 */
void SYS_setVIntCallback(VoidCallback *CB);
/**
 *  \brief
 *      Set 'Horizontal Interrupt' callback method (need to be prefixed by HINTERRUPT_CALLBACK).
 *
 *  \param CB
 *      Pointer to the method to call on Horizontal Interrupt.<br>
 *      You can remove current callback by passing a NULL pointer here.<br>
 *      You need to prefix your hint method with <i>HINTERRUPT_CALLBACK</i>:<br>
 *      <p>HINTERRUPT_CALLBACK myHIntFunction()
 *      {
 *          ...
 *      }</p>
 * <br>
 * Horizontal interrupt happen at the end of scanline display period right before Horizontal blank.<br>
 * This period is usually used to do mid frame changes (palette, scrolling or others raster effect).<br>
 * When you do that, don't forget to protect your VDP access from your main loop using
 * #SYS_disableInts() / #SYS_enableInts() otherwise you may corrupt your VDP writes.
 */
void SYS_setHIntCallback(VoidCallback *CB);
/**
 *  \brief
 *      Set External interrupt callback method.
 *
 *  \param CB
 *      Pointer to the method to call on External Interrupt.<br>
 *      You can remove current callback by passing a null pointer here.
 *
 * External interrupt happen on Light Gun trigger (HVCounter is locked).
 */
void SYS_setExtIntCallback(VoidCallback *CB);

/**
 *  \brief
 *      Return TRUE if we are in the V-Interrupt process.
 *
 * This method tests if we are currently processing a Vertical retrace interrupt (V-Int callback).
 */
bool SYS_isInVInt(void);

/**
 *  \brief
 *      Return != 0 if we are on a NTSC system.
 *
 * Better to use the IS_PAL_SYSTEM
 */
u16 SYS_isNTSC(void);
/**
 *  \brief
 *      Return != 0 if we are on a PAL system.
 *
 * Better to use the IS_PAL_SYSTEM
 */
u16 SYS_isPAL(void);

/**
 *  \brief
 *      Returns number of Frame Per Second.
 *
 * This function actually returns the number of time it was called in the last second.<br>
 * i.e: for benchmarking you should call this method only once per frame update.
 */
u32 SYS_getFPS(void);
/**
 *  \brief
 *      Returns number of Frame Per Second (fix32 form).
 *
 * This function actually returns the number of time it was called in the last second.<br>
 * i.e: for benchmarking you should call this method only once per frame update.
 */
fix32 SYS_getFPSAsFloat(void);
/**
 *  \brief
 *      Return an estimation of CPU frame load (in %)
 *
 * Return an estimation of CPU load (in %, mean value computed on 8 frames) based of idle time spent in #VDP_waitVSync() / #VDP_waitVInt() methods.<br>
 * The method can return value above 100% you CPU load is higher than 1 frame.
 *
 * \see VDP_waitVSync(void)
 * \see VDP_waitVInt(void)
 */
u16 SYS_getCPULoad(void);
/**
 *  \brief
 *      Returns TRUE if frame load is currently displayed, FALSE otherwise

 * \see SYS_showFrameLoad(void)
 */
bool SYS_getShowFrameLoad();
/**
 *  \brief
 *      Show a cursor indicating current frame load level in scanline (top = 0% load, bottom = 100% load)
 *
 *  \param mean
 *      frame load level display is averaged on 8 frames (mean load)
 *
 *  Show current frame load using a cursor indicating the scanline reached when #VDP_waitVSync() / #VDP_waitVInt() method was called.<br>
 *  Note that internally sprite 0 is used to display to cursor (palette 0 and color 15) as it is not directly used by the Sprite Engine but
 *  if you're using the low level VDP sprite methods then you should know that sprite 0 will be used here.
 *
 * \see SYS_hideFrameLoad(void)
 */
void SYS_showFrameLoad(bool mean);
/**
 *  \brief
 *      Hide the frame load cursor previously enabled using #SYS_showFrameLoad() method.

 * \see SYS_showFrameLoad(void)
 */
void SYS_hideFrameLoad(void);

/**
 *  \brief
 *      Computes full ROM checksum and return it.<br>
 *      The checksum is a custom fast 32 bit checksum converted to 16 bit at end
 */
u16 SYS_computeChecksum(void);
/**
 *  \brief
 *      Returns TRUE if ROM checksum is ok (correspond to rom_head.checksum field)
 */
bool SYS_isChecksumOk(void);

/**
 *  \brief
 *      Die with the specified error message.<br>
 *      Program execution is interrupted.<br>
 *      Accepts a list of strings. The list must end with a NULL value.
 *
 * This actually display an error message and program ends execution.
 */
void SYS_die(char *err, ...);

#endif // _SYS_H_
````

## File: inc/tab_cnv.h
````c
#ifndef _TAB_CNV_H_
#define _TAB_CNV_H_

extern const u8 cnv_2to4_tab[0x4];
extern const u8 cnv_2to8_tab[0x4];
extern const u16 cnv_2to16_tab[0x4];
extern const u32 cnv_2to32_tab[0x4];

extern const u8 cnv_4to8_tab[0x10];
extern const u16 cnv_4to16_tab[0x10];
extern const u32 cnv_4to32_tab[0x10];

extern const u16 cnv_8to16_tab[0x100];
extern const u32 cnv_8to32_tab[0x100];

extern const u8 cnv_bcd_tab[100];


#endif // _TAB_CNV_H_
````

## File: inc/task_cst.h
````c
/**
 * \file task_cst.h
 * \brief User task constantes definition
 * \date 12/2021
 * \author doragasu
 *
 * This module define constantes for the task unit (see task.h file).
 * This needs to be separated as we include it from task.a assembly file !
 */

#ifndef __TASK_CST_H__
#define __TASK_CST_H__

/**
 *  \brief
 *      Byte length for the user task stack (should be less than STACK_SIZE define din memory_base.h)
 */
#define USER_STACK_LENGTH   512

/**
 *  \brief
 *      User task registers save buffer size
 */
#define UTSK_REGS_LEN       (15 * 4)

/**
 *  \brief
 *      Timeout value to use for infinite waits
 */
#define TSK_PEND_FOREVER    -1


#endif /*__TASK_CST_H__*/
````

## File: inc/task.h
````c
/**
 * \file task.h
 * \brief User task support
 * \date 12/2021
 * \author doragasu
 *
 * This module allows assigning a function that will run as an user task.
 * CPU time is given to that function in several ways:
 * - When the supervisor task calls TSK_userYield(), the user task executes
 *   immediately until the next VBlank interrupt. Then supervisor task is
 *   resumed.
 * - When the supervisor task call TSK_superPend(), the user task executes
 *   immediately until one of the following happens:
 *   a) The user task calls TSK_superPend().
 *   b) The timeout specified in TSK_superPost() expires.
 *
 * For the scheduler to work, VBlank interrupts must be enabled.
 *
 * These functions are implemented in task.s
 */

#ifndef __TASK_H__
#define __TASK_H__

#include "task_cst.h"


/**
 * \brief Initialize the task sub system (reset internal variables).
 */
void TSK_init();

/**
 * \brief Configure the user task callback function.<br>
 *  Must be set with a not NULL callback before calling any TSK_xxx functions.
 *
 * \param task A function pointer to the user task (or NULL to disable multitasking).
 */
void TSK_userSet(VoidCallback *task);

/**
 * \brief Stop the user task.<br>
 *  This has the same effect than using TSK_setUser(NULL).
 */
void TSK_stop(void);

/**
 * \brief Yield from supervisor task to user task. The user task will resume
 * and will use all the available CPU time until the next vertical blanking
 * interrupt, that will resume the supervisor task.
 */
void TSK_userYield(void);

/**
 * \brief Block supervisor task and resume user task. Supervisor task will
 * not resume execution until #TSK_superPost() is called from user task or a
 * timeout happens..
 *
 * \param wait Maximum number of frames to wait while blocking. Use
 *            TSK_PEND_FOREVER for an infinite wait, or a positive number
 *            (greater than 0) for a specific number of frames.
 *
 * \return false if task was awakened from user task, or true if timeout
 * occurred.
 */
bool TSK_superPend(s16 wait);

/**
 * \brief Resume a blocked supervisor task. Must be called from user task.
 *
 * \param immediate If true, immediately causes a context switch to
 *            supervisor task. If false, context switch will not occur until
 *            the VBLANK interrupt.
 */
void TSK_superPost(bool immediate);

#endif /*__TASK_H__*/
````

## File: inc/timer.h
````c
/**
 *  \file timer.h
 *  \brief Timer support
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides basic timer functions (useful for profiling).<br>
 * This unit uses V-Int to count frame so disabling V-Int will make timer methods to not work anymore.
 */

#ifndef _TIMER_H_
#define _TIMER_H_


/**
 *  \brief
 *      Number of subtick per second.
 */
#define SUBTICKPERSECOND    76800
/**
 *  \brief
 *      Number of tick per second.
 */
#define TICKPERSECOND       300
/**
 *  \brief
 *      Time sub division per second.
 */
#define TIMEPERSECOND       256

/**
 *  \brief
 *      Maximum number of timer.
 */
#define MAXTIMER            16

extern vu32 vtimer;


/**
 *  \brief
 *      Returns elapsed subticks from console reset.
 *
 * Returns elapsed subticks from console reset (1/76800 second based).<br>
 * <b>WARNING:</b> this function isn't accurate during VBlank (return fixed value for the whole VBlank).<br>
 * This is to avoid issue with the VCounter rollback during VBlank.
 */
u32  getSubTick(void);
/**
 *  \brief
 *      Returns elapsed ticks from console reset.
 *
 * Returns elapsed ticks from console reset (1/300 second based).
 */
u32  getTick(void);

/**
 *  \brief
 *      Returns elapsed time from console reset.
 *
 *  \param fromTick
 *      Choose tick or sub tick (more accurate) calculation.
 *
 * Returns elapsed time from console reset (1/256 second based).
 */
u32  getTime(u16 fromTick);
/**
 *  \brief
 *      Returns elapsed time in second from console reset.
 *
 *  \param fromTick
 *      Choose tick or sub tick (more accurate) calculation.
 *
 * Returns elapsed time in second from console reset.<br>
 * Value is returned as fix32.
 */
fix32 getTimeAsFix32(u16 fromTick);

/**
 *  \brief
 *      Start internal timer (0 <= numtimer < MAXTIMER)
 *
 *  \param numTimer
 *      Timer number (0-MAXTIMER)
 */
void startTimer(u16 numTimer);
/**
 *  \brief
 *      Get elapsed subticks for specified timer.
 *
 *  \param numTimer
 *      Timer number (0-MAXTIMER)
 *  \param restart
 *      Restart timer if TRUE
 *
 * Returns elapsed subticks from last call to startTimer(numTimer).
 */
u32  getTimer(u16 numTimer, u16 restart);

/**
 *  \brief
 *      Wait for a certain amount of subticks (1/76800 second based).
 *
 *  \param subtick
 *      Number of subtick to wait for.
 *
 * <b>WARNING:</b> this function isn't accurate during VBlank (always wait until the end of VBlank) because of the VCounter rollback.
 */
void waitSubTick(u32 subtick);
/**
 *  \brief
 *      Wait for a certain amount of ticks (1/300 second based).
 *
 *  \param tick
 *      Number of tick to wait for.
 *
 * <b>WARNING:</b> 5/6 (PAL/NTSC) ticks based timer so use 5 or 6 ticks as minimum wait value.
 */
void waitTick(u32 tick);
/**
 *  \brief
 *      Wait for a certain amount of millisecond.
 *
 *  \param ms
 *      Number of millisecond to wait for.
 *
 * <b>WARNING:</b> ~3.33 ms based timer is used when 'ms' is >= 100.
 */
void waitMs(u32 ms);


#endif // _TIMER_H_
````

## File: inc/tools.h
````c
/**
 *  \file tools.h
 *  \brief Misc tools methods
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides some misc tools methods as getFPS(), unpack()...
 */

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "bmp.h"
#include "vdp.h"
#include "vdp_tile.h"
#include "vdp_bg.h"
#include "map.h"


/**
 *  \brief
 *      No compression.
 */
#define COMPRESSION_NONE        0
/**
 *  \brief
 *      Use aplib (appack or sixpack) compression scheme.
 */
#define COMPRESSION_APLIB       1
/**
 *  \brief
 *      Use LZ4W compression scheme.
 */
#define COMPRESSION_LZ4W        2


/**
 *  \brief
 *      Simple cycle counter tool from BlastEm - start cycle count (see #BLASTEM_PROFIL_END)
 */
#define BLASTEM_PROFIL_START    VDP_setReg(0x9F, 0xC0);
/**
 *  \brief
 *      Simple cycle counter tool from BlastEm - stop cycle count and display result in console
 */
#define BLASTEM_PROFIL_END      VDP_setReg(0x9F, 0x00);


/**
 *  \brief
 *      Callback for QSort comparaison
 *
 * This callback is used to compare 2 objects.<br>
 * Return value should be:<br>
 * < 0 if o1 is below o2<br>
 * = 0 if o1 is equal to o2<br>
 * > 0 if o1 is above o2
 */
typedef s16 _comparatorCallback(void* o1, void* o2);


/**
 *  \brief
 *      Set the randomizer seed (to allow different reproductible series)
 */
void setRandomSeed(u16 seed);
/**
 *  \brief
 *      Returns a random u16 integer value.
 */
u16 random(void);


/**
 *  \brief
 *      Composes a string with the same text that would be printed if format was used on printf,
 *      but instead of being printed to screen, the content is printed in KMod console.
 *
 *  \param fmt
 *      C string that contains the text to be written to destination string.<br />
 *      It can optionally contain embedded format specifiers.
 *
 *  \param ... (additional arguments)
 *      Depending on the format string, the function may expect a sequence of additional arguments, <br>
 *      each containing a value to be used to replace a format specifier in the format string.
 *
 *      There should be at least as many of these arguments as the number of values specified in the format specifiers. <br>
 *      Additional arguments are ignored by the function.
 *
 *  \return On success, the total number of characters written (limited to 255 max)
 *
 *  Copy the string pointed by 'fmt' param to KMod console.<br>
 *  If 'fmt' includes format specifiers (subsequences beginning with %), the additional arguments following format are
 *  formatted and inserted in the resulting string replacing their respective specifiers.<br>
 *  Note that internally a buffer of 255 characters is allocated so consider this limitation !
 *
 */
int kprintf(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

/**
 *  \brief
 *      KDebug log helper methods
 *  \deprecated Use kprintf(..) instead
 */
void KLog(char* text);
void KLog_U1(char* t1, u32 v1);
void KLog_U2(char* t1, u32 v1, char* t2, u32 v2);
void KLog_U3(char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3);
void KLog_U4(char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3, char* t4, u32 v4);
void KLog_U1_(char* t1, u32 v1, char* t2);
void KLog_U2_(char* t1, u32 v1, char* t2, u32 v2, char* t3);
void KLog_U3_(char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3, char* t4);
void KLog_U4_(char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3, char* t4, u32 v4, char* t5);
void KLog_U1x(u16 minSize, char* t1, u32 v1);
void KLog_U2x(u16 minSize, char* t1, u32 v1, char* t2, u32 v2);
void KLog_U3x(u16 minSize, char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3);
void KLog_U4x(u16 minSize, char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3, char* t4, u32 v4);
void KLog_U1x_(u16 minSize, char* t1, u32 v1, char* t2);
void KLog_U2x_(u16 minSize, char* t1, u32 v1, char* t2, u32 v2, char* t3);
void KLog_U3x_(u16 minSize, char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3, char* t4);
void KLog_U4x_(u16 minSize, char* t1, u32 v1, char* t2, u32 v2, char* t3, u32 v3, char* t4, u32 v4, char* t5);
void KLog_S1(char* t1, s32 v1);
void KLog_S2(char* t1, s32 v1, char* t2, s32 v2);
void KLog_S3(char* t1, s32 v1, char* t2, s32 v2, char* t3, s32 v3);
void KLog_S4(char* t1, s32 v1, char* t2, s32 v2, char* t3, s32 v3, char* t4, s32 v4);
void KLog_S1_(char* t1, s32 v1, char* t2);
void KLog_S2_(char* t1, s32 v1, char* t2, s32 v2, char* t3);
void KLog_S3_(char* t1, s32 v1, char* t2, s32 v2, char* t3, s32 v3, char* t4);
void KLog_S4_(char* t1, s32 v1, char* t2, s32 v2, char* t3, s32 v3, char* t4, s32 v4, char* t5);
void KLog_S1x(u16 minSize, char* t1, s32 v1);
void KLog_S2x(u16 minSize, char* t1, s32 v1, char* t2, s32 v2);
void KLog_S3x(u16 minSize, char* t1, s32 v1, char* t2, s32 v2, char* t3, s32 v3);
void KLog_S4x(u16 minSize, char* t1, s32 v1, char* t2, s32 v2, char* t3, s32 v3, char* t4, s32 v4);
void KLog_f1(char* t1, fix16 v1);
void KLog_f2(char* t1, fix16 v1, char* t2, fix16 v2);
void KLog_f3(char* t1, fix16 v1, char* t2, fix16 v2, char* t3, fix16 v3);
void KLog_f4(char* t1, fix16 v1, char* t2, fix16 v2, char* t3, fix16 v3, char* t4, fix16 v4);
void KLog_f1x(s16 numDec, char* t1, fix16 v1);
void KLog_f2x(s16 numDec, char* t1, fix16 v1, char* t2, fix16 v2);
void KLog_f3x(s16 numDec, char* t1, fix16 v1, char* t2, fix16 v2, char* t3, fix16 v3);
void KLog_f4x(s16 numDec, char* t1, fix16 v1, char* t2, fix16 v2, char* t3, fix16 v3, char* t4, fix16 v4);
void KLog_F1(char* t1, fix32 v1);
void KLog_F2(char* t1, fix32 v1, char* t2, fix32 v2);
void KLog_F3(char* t1, fix32 v1, char* t2, fix32 v2, char* t3, fix32 v3);
void KLog_F4(char* t1, fix32 v1, char* t2, fix32 v2, char* t3, fix32 v3, char* t4, fix32 v4);
void KLog_F1x(s16 numDec, char* t1, fix32 v1);
void KLog_F2x(s16 numDec, char* t1, fix32 v1, char* t2, fix32 v2);
void KLog_F3x(s16 numDec, char* t1, fix32 v1, char* t2, fix32 v2, char* t3, fix32 v3);
void KLog_F4x(s16 numDec, char* t1, fix32 v1, char* t2, fix32 v2, char* t3, fix32 v3, char* t4, fix32 v4);


/**
 *  \brief
 *      Allocate a new Bitmap structure which can receive unpacked bitmap data of the specified Bitmap.<br>
 *      There is no memory allocated for the palette data as it assumes to always use a reference for Palette field.
 *
 *  \param bitmap
 *      Source Bitmap we want to allocate the unpacked Bitmap object.
 *  \return
 *      The new allocated Bitmap object which can receive the unpacked Bitmap, note that returned bitmap
 *      is allocated in a single bloc and can be released with Mem_Free(bitmap).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked bitmap.
 */
Bitmap *allocateBitmap(const Bitmap *bitmap);
/**
 *  \brief
 *      Allocate a new Bitmap structure which can receive the bitmap data for the specified Bitmap dimension.<br>
 *      There is no memory allocated for the palette data as it assumes to always use a reference for Palette field.
 *
 *  \param width
 *      Width in pixel of the bitmap structure we want to allocate.
 *  \param heigth
 *      heigth in pixel of the bitmap structure we want to allocate.
 *  \return
 *      The new allocated Bitmap object which can receive an unpacked Bitmap for the specified dimension.<br>
 *      Note that returned bitmap is allocated in a single bloc and can be released with Mem_Free(bitmap).<br>
 *      <i>NULL</i> is returned if there is not enough memory to allocate the bitmap.
 */
Bitmap *allocateBitmapEx(u16 width, u16 heigth);
/**
 *  \brief
 *      Allocate TileSet structure which can receive unpacked tiles data of the specified TileSet.
 *
 *  \param tileset
 *      Source TileSet we want to allocate the unpacked TileSet object.
 *  \return
 *      The new allocated TileSet object which can receive the unpacked TileSet, note that returned tile set
 *      is allocated in a single bloc and can be released with Mem_Free(tb).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked tiles.
 */
TileSet *allocateTileSet(const TileSet *tileset);
/**
 *  \brief
 *      Allocate a new TileSet structure which can receive the data for the specified number of tile.
 *
 *  \param numTile
 *      Number of tile this tileset can contain
 *  \return
 *      The new allocated TileSet object which can receive the specified number of tile.<br>
 *      Note that returned tileset is allocated in a single bloc and can be released with Mem_Free(tileset).<br>
 *      <i>NULL</i> is returned if there is not enough memory to allocatee the tileset.
 */
TileSet *allocateTileSetEx(u16 numTile);
/**
 *  \brief
 *      Allocate TileMap structure which can receive unpacked tilemap data of the specified TileMap.
 *
 *  \param tilemap
 *      Source TileMap we want to allocate the unpacked TileMap object.
 *  \return
 *      The new allocated TileMap object which can receive the unpacked TileMap, note that returned tilemap
 *      is allocated in a single bloc and can be released with Mem_Free(tilemap).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked tilemap.
 */
TileMap *allocateTileMap(const TileMap *tilemap);
/**
 *  \brief
 *      Allocate a new TileMap structure which can receive tilemap data for the specified TileMap dimension.
 *
 *  \param width
 *      Width in tile of the TileMap structure we want to allocate.
 *  \param heigth
 *      heigth in tile of the TileMap structure we want to allocate.
 *  \return
 *      The new allocated TileMap object which can receive data for the specified TileMap dimension.<br>
 *      Note that returned tilemap is allocated in a single bloc and can be released with Mem_Free(tilemap).<br>
 *      <i>NULL</i> is returned if there is not enough memory to allocate the tilemap.
 */
TileMap *allocateTileMapEx(u16 width, u16 heigth);
/**
 *  \brief
 *      Allocate Image structure which can receive unpacked image data of the specified Image.
 *      There is no memory allocated for the palette data as it assumes to always use a reference for Palette field.
 *
 *  \param image
 *      Source Image we want to allocate the unpacked Image object.
 *  \return
 *      The new allocated Image object which can receive the unpacked Image, note that returned image
 *      is allocated in a single bloc and can be released with Mem_Free(image).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked image.
 */
Image *allocateImage(const Image *image);
/**
 *  \brief
 *      Allocate Map structure which can receive unpacked data of the specified MapDefinition.
 *
 *  \param mapDef
 *      Source MapDefinition we want to allocate Map object for.
 *  \return
 *      The new allocated Map object which can receive the (unpacked) MapDefinition data, note that returned map
 *      is allocated in a single bloc and can be released with Mem_Free(image).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the data for given MapDefinition.
 */
Map *allocateMap(const MapDefinition *mapDef);

/**
 *  \brief
 *      Unpack the specified source Bitmap and return result in a new allocated Bitmap.
 *
 *  \param src
 *      bitmap to unpack.
 *  \param dest
 *      Destination bitmap where to store unpacked data, be sure to allocate enough space in image buffer.<br>
 *      If set to NULL then a dynamic allocated Bitmap is returned.
 *  \return
 *      The unpacked Bitmap.<br>
 *      If <i>dest</i> was set to NULL then the returned bitmap is allocated in a single bloc and can be released with Mem_Free(bitmap).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked bitmap.
 */
Bitmap *unpackBitmap(const Bitmap *src, Bitmap *dest);
/**
 *  \brief
 *      Unpack the specified TileSet structure and return result in a new allocated TileSet.
 *
 *  \param src
 *      tiles to unpack.
 *  \param dest
 *      Destination TileSet structure where to store unpacked data, be sure to allocate enough space in tiles and tilemap buffer.<br>
 *      If set to NULL then a dynamic allocated TileSet is returned.
 *  \return
 *      The unpacked TileSet.<br>
 *      If <i>dest</i> was set to NULL then the returned tiles base is allocated in a single bloc and can be released with Mem_Free(tb).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked tiles.
 */
TileSet *unpackTileSet(const TileSet *src, TileSet *dest);
/**
 *  \brief
 *      Unpack the specified TileMap structure and return result in a new allocated TileMap.
 *
 *  \param src
 *      tilemap to unpack.
 *  \param dest
 *      Destination tilemap where to store unpacked data, be sure to allocate enough space in tiles and tilemap buffer.<br>
 *      If set to NULL then a dynamic allocated TileMap is returned.
 *  \return
 *      The unpacked TileMap.<br>
 *      If <i>dest</i> was set to NULL then the returned tilemap is allocated in a single bloc and can be released with Mem_Free(tilemap).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked tilemap.
 */
TileMap *unpackTileMap(const TileMap *src, TileMap *dest);
/**
 *  \brief
 *      Unpack the specified Image structure and return result in a new allocated Image.
 *
 *  \param src
 *       image to unpack.
 *  \param dest
 *      Destination Image where to store unpacked data.<br>
 *      If set to NULL then a dynamic allocated Image is returned.
 *  \return
 *      The unpacked Image.<br>
 *      If <i>dest</i> was set to NULL then the returned image is allocated in a single bloc and can be released with Mem_Free(image).<br>
 *      <i>NULL</i> is returned if there is not enough memory to store the unpacked image.
 */
Image *unpackImage(const Image *src, Image *dest);

/**
 *  \brief
 *      Unpack the specified source data buffer in the specified destination buffer.<br>
 *      if source is not packed then nothing is done.
 *
 *  \param compression
 *      compression type, accepted values:<br>
 *      <b>COMPRESSION_APLIB</b><br>
 *      <b>COMPRESSION_LZ4W</b><br>
 *  \param src
 *      Source data buffer containing the packed data to unpack.
 *  \param dest
 *      Destination buffer where to store unpacked data, be sure to allocate enough space.
 *  \return
 *      Unpacked size.
 */
u32 unpack(u16 compression, u8 *src, u8 *dest);

/**
 *  \brief
 *      Unpack (aplib packer) the specified source data buffer in the specified destination buffer.
 *
 *  \param src
 *      Source data buffer containing the packed data (aplib packer) to unpack.
 *  \param dest
 *      Destination buffer where to store unpacked data, be sure to allocate enough space.
 *  \return
 *      Unpacked size.
 */
u32 aplib_unpack(u8 *src, u8 *dest);
/**
 *  \brief
 *      Unpack (LZ4W) the specified source data buffer in the specified destination buffer.
 *
 *  \param src
 *      Source data buffer containing the packed data (LZ4W packed) to unpack.
 *  \param dest
 *      Destination buffer where to store unpacked data, be sure to allocate enough space.<br>
 *      The size of unpacked data is contained in the first 4 bytes of 'src'.
 *  \return
 *      Unpacked size.
 */
u32 lz4w_unpack(const u8 *src, u8 *dest);


/**
 *  \brief
 *      Quick sort algo on u8 data array.
 *
 *  \param data
 *      u8 data pointer.
 *  \param left
 *      left index (should be 0).
 *  \param right
 *      right index (should be table size - 1).
 */
void qsort_u8(u8 *data, u16 left, u16 right);
/**
 *  \brief
 *      Quick sort algo on s8 data array.
 *
 *  \param data
 *      s8 data pointer.
 *  \param left
 *      left index (should be 0).
 *  \param right
 *      right index (should be table size - 1).
 */
void qsort_s8(s8 *data, u16 left, u16 right);
/**
 *  \brief
 *      Quick sort algo on u16 data array.
 *
 *  \param data
 *      u16 data pointer.
 *  \param left
 *      left index (should be 0).
 *  \param right
 *      right index (should be table size - 1).
 */
void qsort_u16(u16 *data, u16 left, u16 right);
/**
 *  \brief
 *      Quick sort algo on s16 data array.
 *
 *  \param data
 *      s16 data pointer.
 *  \param left
 *      left index (should be 0).
 *  \param right
 *      right index (should be table size - 1).
 */
void qsort_s16(s16 *data, u16 left, u16 right);
/**
 *  \brief
 *      Quick sort algo on u32 data array.
 *
 *  \param data
 *      u32 data pointer.
 *  \param left
 *      left index (should be 0).
 *  \param right
 *      right index (should be table size - 1).
 */
void qsort_u32(u32 *data, u16 left, u16 right);
/**
 *  \brief
 *      Quick sort algo on s32 data array.
 *
 *  \param data
 *      s32 data pointer.
 *  \param left
 *      left index (should be 0).
 *  \param right
 *      right index (should be table size - 1).
 */
void qsort_s32(s32 *data, u16 left, u16 right);

/**
 *  \brief
 *      Quick sort algo on array of pointer (object)
 *
 *  \param data
 *      array of pointer (pointer of object to sort).
 *  \param len
 *      number of element in the data array
 *  \param cb
 *      comparator callback used to compare 2 objects.
 */
void qsort(void** data, u16 len, _comparatorCallback* cb);



#endif // _TOOLS_H_
````

## File: inc/types.h
````c
/**
 *  \file types.h
 *  \brief Types definition
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * SGDK Types definition.
 */

#ifndef _TYPES_H_
#define _TYPES_H_

/**
 *  \brief
 *      FALSE define (equivalent to 0).
 */
#ifndef FALSE
#define FALSE   0
#endif
/**
 *  \brief
 *      TRUE define (equivalent to 1).
 */
#ifndef TRUE
#define TRUE    1
#endif
/**
 *  \brief
 *      NULL define (equivalent to 0).
 */
#ifndef NULL
#define NULL    0
#endif

#ifndef U8_MIN
#define U8_MIN  ((u8) 0x00)
#endif
#ifndef U8_MAX
#define U8_MAX  ((u8) 0xFF)
#endif
#ifndef S8_MIN
#define S8_MIN  ((s8) (-0x80))
#endif
#ifndef S8_MAX
#define S8_MAX  ((s8) 0x7F)
#endif

#ifndef U16_MIN
#define U16_MIN ((u16) 0x0000)
#endif
#ifndef U16_MAX
#define U16_MAX ((u16) 0xFFFF)
#endif
#ifndef S16_MIN
#define S16_MIN ((s16) (-0x8000))
#endif
#ifndef S16_MAX
#define S16_MAX ((s16) 0x7FFF)
#endif

#ifndef U32_MIN
#define U32_MIN ((u32) 0x00000000)
#endif
#ifndef U32_MAX
#define U32_MAX ((u32) 0xFFFFFFFF)
#endif
#ifndef S32_MIN
#define S32_MIN ((s32) (-0x80000000))
#endif
#ifndef S32_MAX
#define S32_MAX ((s32) 0x7FFFFFFF)
#endif


/**
 *  \typedef s8
 *      8 bits signed integer (equivalent to char).
 */
typedef char s8;
/**
 *  \typedef s16
 *      16 bits signed integer (equivalent to short).
 */
typedef short s16;
/**
 *  \typedef s32
 *      32 bits signed integer (equivalent to long).
 */
typedef long s32;

/**
 *  \typedef u8
 *      8 bits unsigned integer (equivalent to unsigned char).
 */
typedef unsigned char u8;
/**
 *  \typedef u16
 *      16 bits unsigned integer (equivalent to unsigned short).
 */
typedef unsigned short u16;
/**
 *  \typedef u32
 *      32 bits unsigned integer (equivalent to unsigned long).
 */
typedef unsigned long u32;

/**
 *  \typedef size_t
 *      size type (equivalent to unsigned long).
 */
 typedef unsigned long size_t;


#if !defined(__cplusplus) && (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202300L))

/**
 *  \typedef bool
 *      boolean type, to be used with TRUE/true and FALSE/false constants.
 *      (internally set as unsigned char)
 */
typedef u8 bool;
/**
 *  \brief
 *      false define (equivalent to 0).
 */
#ifndef false
#define false   FALSE
#endif
/**
 *  \brief
 *      true define (equivalent to 1).
 */
#ifndef true
#define true    TRUE
#endif

#endif

/**
 *  \typedef vs8
 *      volatile 8 bits signed integer.
 */
typedef volatile s8 vs8;
/**
 *  \typedef vs16
 *      volatile 16 bits signed integer.
 */
typedef volatile s16 vs16;
/**
 *  \typedef vs32
 *      volatile 32 bits signed integer.
 */
typedef volatile s32 vs32;

/**
 *  \typedef vu8
 *      volatile 8 bits unsigned integer.
 */
typedef volatile u8 vu8;
/**
 *  \typedef vu16
 *      volatile 16 bits unsigned integer.
 */
typedef volatile u16 vu16;
/**
 *  \typedef vu32
 *      volatile 32 bits unsigned integer.
 */
typedef volatile u32 vu32;

/**
 *  \typedef vbool
 *      volatile boolean type.
 *      (internally set as volatile unsigned char)
 */
typedef vu8 vbool;

/**
 *  \typedef p16
 *      short pointer for fast 16 bit addressing (GCC does correctly cast that to pointer).
 *      Limited to 0xFFFF8000-0x00007FFF memory region (first 32KB bank of ROM, and last 32KB of RAM)
 */
typedef s16 p16;


#if !defined(uint8_t) && !defined(__int8_t_defined)
#define uint8_t     u8
#define int8_t      s8
#endif
#if !defined(uint16_t) && !defined(__int16_t_defined)
#define uint16_t    u16
#define int16_t     s16
#endif
#if !defined(uint32_t) && !defined(__int32_t_defined)
#define uint32_t    u32
#define int32_t     s32
#endif
#if !defined(size_t)
#define size_t      u32
#endif
#if !defined(ptrdiff_t)
#define ptrdiff_t   u32
#endif


/**
 *  \typedef fix16
 *      16 bits fixed point (10.6) type
 */
typedef s16 fix16;
/**
 *  \typedef fix32
 *      32 bits fixed point (22.10) type
 */
typedef s32 fix32;
/**
 *  \typedef f16
 *      16 bits fixed point (10.6) type - short version
 */
typedef s16 f16;
/**
 *  \typedef f32
 *      32 bits fixed point (22.10) type - short version
 */
typedef s32 f32;

/**
 *  \typedef fastfix16
 *      "fast" 16 bits fixed point (8.8) type
 */
typedef s16 fastfix16;
/**
 *  \typedef fastfix32
 *      "fast" 32 bits fixed point (16.16) type
 */
typedef s32 fastfix32;
/**
 *  \typedef ff16
 *      "fast" 16 bits fixed point (8.8) type - short version
 */
typedef s16 ff16;
/**
 *  \typedef ff32
 *      "fast" 32 bits fixed point (16.16) type - short version
 */
typedef s32 ff32;

/**
 *  \typedef vfix16
 *      volatile 16 bits fixed point (10.6) type.
 */
typedef vs16 vfix16;
/**
 *  \typedef vfix32
 *      volatile 32 bits fixed point (22.10) type.
 */
typedef vs32 vfix32;
/**
 *  \typedef vf16
 *      volatile 16 bits fixed point (10.6) type - short version
 */
typedef vs16 vf16;
/**
 *  \typedef vf32
 *      volatile 32 bits fixed point (22.10) type - short version
 */
typedef vs32 vf32;


#define FASTCALL

/**
 *  \brief
 *      Simple Box structure
 *
 *  \param x
 *      X position (left)
 *  \param y
 *      Y position (top)
 *  \param w
 *      width
 *  \param h
 *      heigth
 */
typedef struct
{
    s16 x;
    s16 y;
    u16 w;
    u16 h;
} Box;

/**
 *  \brief
 *      Simple Circle structure
 *
 *  \param x
 *      X center position
 *  \param y
 *      Y center position
 *  \param ray
 *      circle ray
 */
typedef struct
{
    s16 x;
    s16 y;
    u16 ray;
} Circle;


typedef void VoidCallback(void);


u8  getZeroU8(void);
u16 getZeroU16(void);
u32 getZeroU32(void);


#endif // _TYPES_H_
````

## File: inc/vdp_bg.h
````c
/**
 *  \file vdp_bg.h
 *  \brief VDP background plane support
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides plane A & plane B facilities :
 * - set scrolling
 * - clear plane
 * - draw text in plane
 */

#ifndef _VDP_BG_H_
#define _VDP_BG_H_

#include "bmp.h"
#include "vdp.h"
#include "vdp_tile.h"


/**
 *  \brief
 *      Image structure which contains all data to define an image in a background plane.<br>
 *      Use the unpackImage() method to unpack if compression is enabled in TileSet or TileMap structure.
 *
 *  \param palette
 *      Palette data.
 *  \param tileset
 *      TileSet data structure (contains tiles definition for the image).
 *  \param tilemap
 *      TileMap data structure (contains tilemap definition for the image).
 */
typedef struct
{
    Palette *palette;
    TileSet *tileset;
    TileMap *tilemap;
} Image;


/**
 *  Contains current VRAM tile position where we will upload next tile data.
 *
 *  \see VDP_drawBitmap()
 *  \see VDP_drawImage()
 */
extern u16 curTileInd;

/**
 *  \brief
 *      Set plane horizontal scroll (plain scroll mode).<br>
 *      3 horizontal scroll modes are supported:<br>
 *      - Plain (whole plane)<br>
 *      - Tile (8 pixels bloc)<br>
 *      - Line (per pixel scroll)<br>
 *
 *  \param plane
 *      Plane we want to set the horizontal scroll.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param value
 *      H scroll offset.<br>
 *      Negative value will move the plane to the left while positive
 *      value will move it to the right.
 *
 *  \see VDP_setScrollingMode() function to change scroll mode.
 *  \see VDP_setHorizontalScrollVSync()
 */
void VDP_setHorizontalScroll(VDPPlane plane, s16 value);
/**
 *  \brief
 *      Same as #VDP_setHorizontalScroll(..) except that it will delay scroll update on VSync.
 *
 *  \param plane
 *      Plane we want to set the horizontal scroll.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param value
 *      H scroll offset.<br>
 *      Negative value will move the plane to the left while positive
 *      value will move it to the right.
 *
 *  \see VDP_setHorizontalScroll()
 */
void VDP_setHorizontalScrollVSync(VDPPlane plane, s16 value);
/**
 *  \brief
 *      Set plane horizontal scroll (tile scroll mode).<br>
 *      3 horizontal scroll modes are supported:<br>
 *      - Plain (whole plane)<br>
 *      - Tile (8 pixels bloc)<br>
 *      - Line (per pixel scroll)<br>
 *
 *  \param plane
 *      Plane we want to set the horizontal scroll.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param tile
 *      First tile we want to set the horizontal scroll.
 *  \param values
 *      H scroll offsets.<br>
 *      Negative values will move the plane to the left while positive
 *      values will move it to the right.
 *  \param len
 *      Number of tile to set.
 *  \param tm
 *      Transfer method, it's recommended to use DMA_QUEUE so it will be executed as fast as possible during VBlank.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  \see VDP_setScrollingMode() function to change scroll mode.
 */
void VDP_setHorizontalScrollTile(VDPPlane plane, u16 tile, s16* values, u16 len, TransferMethod tm);
/**
 *  \brief
 *      Set plane horizontal scroll (line scroll mode).<br>
 *      3 horizontal scroll modes are supported:<br>
 *      - Plain (whole plane)<br>
 *      - Tile (8 pixels bloc)<br>
 *      - Line (per pixel scroll)<br>
 *
 *  \param plane
 *      Plane we want to set the horizontal scroll.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param line
 *      First line we want to set the horizontal scroll.
 *  \param values
 *      H scroll offsets.<br>
 *      Negative values will move the plane to the left while positive values will move it to the right.
 *  \param len
 *      Number of line to set.
 *  \param tm
 *      Transfer method, it's recommended to use DMA_QUEUE so it will be executed as fast as possible during VBlank.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  \see VDP_setScrollingMode()
 */
void VDP_setHorizontalScrollLine(VDPPlane plane, u16 line, s16* values, u16 len, TransferMethod tm);

/**
 *  \brief
 *      Set plane vertical scroll (plain scroll mode).
 *      2 vertical scroll modes are supported:<br>
 *      - Plain (whole plane)<br>
 *      - 2-Tiles (16 pixels bloc)<br>
 *
 *  \param plane
 *      Plane we want to set the vertical scroll.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param value
 *      V scroll offset.<br>
 *      Negative value will move the plane down while positive value will move it up.
 *
 *  \see VDP_setScrollingMode()
 */
void VDP_setVerticalScroll(VDPPlane plane, s16 value);
/**
 *  \brief
 *      Same as #VDP_setVerticalScroll(..) except that it will delay scroll update on VSync.
 *
 *  \param plane
 *      Plane we want to set the vertical scroll.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param value
 *      V scroll offset.<br>
 *      Negative value will move the plane down while positive value will move it up.
 *
 *  \see VDP_setHorizontalScroll()
 */
void VDP_setVerticalScrollVSync(VDPPlane plane, s16 value);
/**
 *  \brief
 *      Set plane vertical scroll (2-Tiles scroll mode).
 *      2 vertical scroll modes are supported:<br>
 *      - Plain (whole plane)<br>
 *      - 2-Tiles (16 pixels bloc)<br>
 *
 *  \param plane
 *      Plane we want to set the vertical scroll.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *  \param tile
 *      First tile we want to set the vertical scroll.
 *  \param values
 *      V scroll offsets.<br>
 *      Negative values will move the plane down while positive values will move it up.
 *  \param len
 *      Number of tile to set.
 *  \param tm
 *      Transfer method, it's recommended to use DMA_QUEUE so it will be executed as fast as possible during VBlank.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  \see VDP_setScrollingMode()
 */
void VDP_setVerticalScrollTile(VDPPlane plane, u16 tile, s16* values, u16 len, TransferMethod tm);

/**
 *  \brief
 *      Clear specified plane (using DMA).
 *
 *  \param plane
 *      Plane we want to clear.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param wait
 *      Wait the operation to complete when set to TRUE otherwise it returns immediately
 *      but then you will require to wait for DMA completion (#DMA_waitCompletion()) before accessing the VDP.
 */
void VDP_clearPlane(VDPPlane plane, bool wait);

/**
 *  \brief
 *      Returns the plane used to display text.
 *
 *  Returned value should be either equals to BG_A, BG_B or WINDOW.
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 */
VDPPlane VDP_getTextPlane(void);
/**
 *  \brief
 *      Returns the palette number used to display text.
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 */
u16 VDP_getTextPalette(void);
/**
 *  \brief
 *      Returns the priority used to display text.
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 */
u16 VDP_getTextPriority(void);

/**
 *  \brief
 *      Define the plane to use to display text.
 *
 *  \param plane
 *      Plane where to display text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 */
void VDP_setTextPlane(VDPPlane plane);
/**
 *  \brief
 *      Define the palette to use to display text.
 *
 *  \param palette
 *      Palette number.
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 */
void VDP_setTextPalette(u16 palette);
/**
 *  \brief
 *      Define the priority to use to display text.
 *
 *  \param prio
 *      Priority:<br>
 *      1 = HIGH PRIORITY TILE.<br>
 *      0 = LOW PRIORITY TILE.<br>
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 */
void VDP_setTextPriority(u16 prio);

/**
 *  \brief
 *      Draw text in specified plane (advanced method).
 *
 *  \param plane
 *      Plane where we want to draw text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param str
 *      String to draw.
 *  \param basetile
 *      Base tile attributes data (see TILE_ATTR() macro).
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param tm
 *      Transfer method, using DMA_QUEUE or DMA_QUEUE_COPY ensure that it will be executed during VBlank.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  \see VDP_clearText(..)
 *  \see VDP_setTextPalette(..)
 *  \see VDP_setTextPriority(..)
 *  \see VDP_setTextPlane(..)
 */
void VDP_drawTextEx(VDPPlane plane, const char* str, u16 basetile, u16 x, u16 y, TransferMethod tm);
/**
 *  \brief
 *      Clear a single line portion of text (advanced method).
 *
 *  \param plane
 *      Plane where we want to clear text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param basetile
 *      Base tile attributes data (see TILE_ATTR() macro).
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param w
 *      width to clear (in tile).
 *  \param tm
 *      Transfer method, using DMA_QUEUE or DMA_QUEUE_COPY ensure that it will be executed during VBlank.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearTextArea(..)
 *  \see VDP_clearTextLine(..)
 */
void VDP_clearTextEx(VDPPlane plane, u16 basetile, u16 x, u16 y, u16 w, TransferMethod tm);
/**
 *  \brief
 *      Clear a specific area of text (advanced method).
 *
 *  \param plane
 *      Plane where we want to clear text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param basetile
 *      Base tile attributes data (see TILE_ATTR() macro).
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param w
 *      width to clear (in tile).
 *  \param h
 *      heigth to clear (in tile).
 *  \param tm
 *      Transfer method, using DMA_QUEUE or DMA_QUEUE_COPY ensure that it will be executed during VBlank.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 */
void VDP_clearTextAreaEx(VDPPlane plane, u16 basetile, u16 x, u16 y, u16 w, u16 h, TransferMethod tm);

/**
 *  \brief
 *      Draw text in specified plane.
 *
 *  \param plane
 *      Plane where we want to draw text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param str
 *      String to draw.
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *
 *  \see VDP_drawTextEx(..)
 *  \see VDP_clearText(..)
 *  \see VDP_setTextPalette(..)
 *  \see VDP_setTextPriority(..)
 *  \see VDP_setTextPlane(..)
 */
void VDP_drawTextBG(VDPPlane plane, const char* str, u16 x, u16 y);
/**
 *  \brief
 *      Clear a single line portion of text.
 *
 *  \param plane
 *      Plane where we want to clear text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param w
 *      width to clear (in tile).
 *
 *  \see VDP_clearTextEx(..)
 *  \see VDP_drawText(..)
 *  \see VDP_clearTextArea(..)
 *  \see VDP_clearTextLine(..)
 */
void VDP_clearTextBG(VDPPlane plane, u16 x, u16 y, u16 w);
/**
 *  \brief
 *      Clear a specific area of text.
 *
 *  \param plane
 *      Plane where we want to clear text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param w
 *      width to clear (in tile).
 *  \param h
 *      heigth to clear (in tile).
 *
 *  \see VDP_clearTextAreaEx(..)
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 *  \see VDP_clearTextLine(..)
 */
void VDP_clearTextAreaBG(VDPPlane plane, u16 x, u16 y, u16 w, u16 h);
/**
 *  \brief
 *      Clear a complete line of text.
 *
 *  \param plane
 *      Plane where we want to clear text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param y
 *      y position (in tile).
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 *  \see VDP_clearTextArea(..)
 */
void VDP_clearTextLineBG(VDPPlane plane, u16 y);

/**
 *  \brief
 *      Draw text.
 *
 *  \param str
 *      String to draw.
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *
 *  \see VDP_clearText(..)
 *  \see VDP_setTextPalette(..)
 *  \see VDP_setTextPriority(..)
 *  \see VDP_setTextPlane(..)
 */
void VDP_drawText(const char* str, u16 x, u16 y);

/**
 *  \brief
 *      Draw text in specified plane and clear remaining unused space.<br>
 *      If the new line is shorter than the one previously drawn in this place, the old characters will be removed.<br>
 *      This function works significantly faster than calling VDP_clearText() and then VDP_drawText().
 *      It is suitable when a lot of updating information needs to be displayed on the screen.
 *
 *  \param plane
 *      Plane where we want to draw text.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param str
 *      String to draw. For correct operation, it must not exceed 40 characters.
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param len
 *      Fixed string length. For correct operation, it must not exceed 40 characters.
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 *  \see VDP_setTextPalette(..)
 *  \see VDP_setTextPriority(..)
 *  \see VDP_setTextPlane(..)
 */
void VDP_drawTextBGFill(VDPPlane plane, const char* str, u16 x, u16 y, u16 len);

/**
 *  \brief
 *      Draw text and clear remaining unused space.<br>
 *      If the new line is shorter than the one previously drawn in this place, the old characters will be removed.<br>
 *      This function works significantly faster than calling VDP_clearText() and then VDP_drawText().
 *      It is suitable when a lot of updating information needs to be displayed on the screen.
 *
 *  \param str
 *      String to draw. For correct operation, it must not exceed 40 characters.
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param len
 *      Fixed string length. For correct operation, it must not exceed 40 characters.
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 *  \see VDP_setTextPalette(..)
 *  \see VDP_setTextPriority(..)
 *  \see VDP_setTextPlane(..)
 */
void VDP_drawTextFill(const char* str, u16 x, u16 y, u16 len);
/**
 *  \brief
 *      Clear a single line portion of text.
 *
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param w
 *      width to clear (in tile).
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearTextArea(..)
 *  \see VDP_clearTextLine(..)
 */
void VDP_clearText(u16 x, u16 y, u16 w);
/**
 *  \brief
 *      Clear a specific area of text.
 *
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *  \param w
 *      width to clear (in tile).
 *  \param h
 *      heigth to clear (in tile).
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 *  \see VDP_clearTextLine(..)
 */
void VDP_clearTextArea(u16 x, u16 y, u16 w, u16 h);
/**
 *  \brief
 *      Clear a complete line of text.
 *
 *  \param y
 *      y position (in tile).
 *
 *  \see VDP_drawText(..)
 *  \see VDP_clearText(..)
 *  \see VDP_clearTextArea(..)
 */
void VDP_clearTextLine(u16 y);

/**
 *  \brief
 *      Draw Bitmap in specified background plane and at given position.
 *
 *  \param plane
 *      Plane where we want to draw the bitmap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param bitmap
 *      Genesis bitmap (the width and height should be aligned to 8).<br>
 *      The Bitmap is unpacked "on the fly" if needed (require some memory).
 *  \param x
 *      Plane X position (in tile).
 *  \param y
 *      Plane Y position (in tile).
 *  \return
 *      FALSE if there is not enough memory to unpack the specified Bitmap (only if compression was enabled).
 *
 *  This function does "on the fly" 4bpp bitmap conversion to tile data and transfert them to VRAM.<br>
 *  It's very helpful when you use bitmap images but the conversion eats sometime so you should use it only for static screen only.<br>
 *  For "in-game" condition you should use VDP_loadTileData() method with prepared tile data.
 *
 *  \see VDP_loadBMPTileData()
 */
bool VDP_drawBitmap(VDPPlane plane, const Bitmap *bitmap, u16 x, u16 y);
/**
 *  \brief
 *      Draw Bitmap in specified background plane and at given position.
 *
 *  \param plane
 *      Plane where we want to draw the bitmap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param bitmap
 *      Genesis bitmap (the width and height should be aligned to 8).<br>
 *      The Bitmap is unpacked "on the fly" if needed (require some memory).
 *  \param basetile
 *      Base tile attributes data (see TILE_ATTR_FULL() macro).
 *  \param x
 *      Plane X position (in tile).
 *  \param y
 *      Plane Y position (in tile).
 *  \param loadpal
 *      Load the bitmap palette information when non zero (can be TRUE or FALSE)
 *  \return
 *      FALSE if there is not enough memory to unpack the specified Bitmap (only if compression was enabled).
 *
 *  This function does "on the fly" 4bpp bitmap conversion to tile data and transfert them to VRAM.<br>
 *  It's very helpful when you use bitmap images but the conversion eats sometime so you should use it only for static screen only.<br>
 *  For "in-game" condition you should use VDP_loadTileData() method with prepared tile data.<br>
 *
 *  \see VDP_loadBMPTileData()
 */
bool VDP_drawBitmapEx(VDPPlane plane, const Bitmap *bitmap, u16 basetile, u16 x, u16 y, bool loadpal);

/**
 *  \brief
 *      Draw Image (using DMA) in specified background plane and at given position.
 *
 *  \param plane
 *      Plane where we want to draw the tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param image
 *      Image structure to draw.<br>
 *      The Image is unpacked "on the fly" if needed (require some memory).
 *  \param x
 *      Plane X position (in tile).
 *  \param y
 *      Plane Y position (in tile).
 *  \return
 *      FALSE if there is not enough memory to unpack the specified Image (only if compression was enabled).
 *
 *  Load the image tiles data in VRAM and display it at specified plane position.
 *
 *  \see VDP_drawImageEx()
 */
bool VDP_drawImage(VDPPlane plane, const Image *image, u16 x, u16 y);
/**
 *  \brief
 *      Draw Image in specified background plane and at given position.
 *
 *  \param plane
 *      Plane where we want to load tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param image
 *      Image structure to draw.<br>
 *      The Image is unpacked "on the fly" if needed (require some memory).
 *  \param basetile
 *      Base tile attributes data (see TILE_ATTR_FULL() macro).
 *  \param x
 *      Plane X position (in tile).
 *  \param y
 *      Plane Y position (in tile).
 *  \param loadpal
 *      Load the bitmap palette information when non zero (can be TRUE or FALSE)
 *  \param dma
 *      use DMA
 *  \return
 *      FALSE if there is not enough memory to unpack the specified Image (only if image was packed).
 *
 *  Load the image tiles data in VRAM and display it at specified plane position.
 *
 *  \see VDP_drawImage()
 */
bool VDP_drawImageEx(VDPPlane plane, const Image *image, u16 basetile, u16 x, u16 y, bool loadpal, bool dma);

/**
 * \brief
 * Draw a signed 32-bit integer value in specified plane (advanced method).
 *
 * \param plane
 * Plane where we want to draw the number.<br>
 * Accepted values are:<br>
 * - BG_A<br>
 * - BG_B<br>
 * - WINDOW<br>
 * \param value
 * The signed 32-bit integer value to draw.
 * \param basetile
 * Base tile attributes data (see TILE_ATTR() macro).
 * \param x
 * X position (in tile).
 * \param y
 * y position (in tile).
 * \param tm
 * Transfer method, using DMA_QUEUE or DMA_QUEUE_COPY ensure that it will be executed during VBlank.<br>
 * Accepted values are:<br>
 * - CPU<br>
 * - DMA<br>
 * - DMA_QUEUE<br>
 * - DMA_QUEUE_COPY
 *
 * \see VDP_drawNumBG(..)
 */
void VDP_drawNumEx(VDPPlane plane, s32 value, u16 basetile, u16 x, u16 y, TransferMethod tm);
/**
 * \brief
 * Draw a signed 32-bit integer value in specified plane.
 *
 * \param plane
 * Plane where we want to draw the number.<br>
 * Accepted values are:<br>
 * - BG_A<br>
 * - BG_B<br>
 * - WINDOW<br>
 * \param value
 * The signed 32-bit integer value to draw.
 * \param x
 * X position (in tile).
 * \param y
 * y position (in tile).
 *
 * \see VDP_drawNumEx(..)
 * \see VDP_drawNum(..)
 */
void VDP_drawNumBG(VDPPlane plane, s32 value, u16 x, u16 y);
/**
 * \brief
 * Draw a signed 32-bit integer value.
 *
 * \param value
 * The signed 32-bit integer value to draw.
 * \param x
 * X position (in tile).
 * \param y
 * y position (in tile).
 *
 * \see VDP_drawNumBG(..)
 */
void VDP_drawNum(s32 value, u16 x, u16 y);

#endif // _VDP_BG_H_
````

## File: inc/vdp_pal.h
````c
/**
 *  \deprecated Use pal.h unit instead
 */

#ifndef _VDP_PAL_H_
#define _VDP_PAL_H_


/**
 *  \deprecated Use #PAL_getColor(..) instead
 */
#define VDP_getPaletteColor(index)     _Pragma("GCC error \"This definition is deprecated, use PAL_getColor(..) instead.\"")

/**
 *  \deprecated Use #PAL_getColors(..) instead
 */
#define VDP_getPaletteColors(index, dest, count)     _Pragma("GCC error \"This definition is deprecated, use PAL_getColors(..) instead.\"")
/**
 *  \deprecated Use #PAL_getPalette(..) instead
 */
#define VDP_getPalette(num, pal)     _Pragma("GCC error \"This definition is deprecated, use PAL_getPalette(..) instead.\"")

/**
 *  \deprecated Use #PAL_setColor(..) instead
 */
#define VDP_setPaletteColor(index, value)     _Pragma("GCC error \"This definition is deprecated, use PAL_setColor(..) instead.\"")
/**
 *  \deprecated Use #PAL_setColors(..) instead
 */
#define VDP_setPaletteColors(index, values, count)     _Pragma("GCC error \"This definition is deprecated, use PAL_setColors(..) instead.\"")
/**
 *  \deprecated Use #PAL_setPalette(..) instead
 */
#define VDP_setPalette(num, pal)     _Pragma("GCC error \"This definition is deprecated, use PAL_setPalette(..) instead.\"")

/**
 *  \deprecated Use #PAL_fade(..) instead
 */
#define VDP_fade(fromcol, tocol, palsrc, paldst, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fade(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeTo(..) instead
 */
#define VDP_fadeTo(fromcol, tocol, pal, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeTo(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeOut(..) instead
 */
#define VDP_fadeOut(fromcol, tocol, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeOut(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeIn(..) instead
 */
#define VDP_fadeIn(fromcol, tocol, pal, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeIn(..) instead.\"")

/**
 *  \deprecated Use #PAL_fadePalette(..) instead
 */
#define VDP_fadePal(numpal, palsrc, paldst, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadePalette(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeToPalette(..) instead
 */
#define VDP_fadeToPal(numpal, pal, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeToPalette(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeOutPalette(..) instead
 */
#define VDP_fadeOutPal(numpal, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeOutPalette(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeInPalette(..) instead
 */
#define VDP_fadeInPal(numpal, pal, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeInPalette(..) instead.\"")

/**
 *  \deprecated Use #PAL_fadeAll(..) instead
 */
#define VDP_fadeAll(palsrc, paldst, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeAll(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeToAll(..) instead
 */
#define VDP_fadeToAll(pal, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeToAll(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeOutAll(..) instead
 */
#define VDP_fadeOutAll(numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeOutAll(..) instead.\"")
/**
 *  \deprecated Use #PAL_fadeInAll(..) instead
 */
#define VDP_fadeInAll(pal, numframe, async)     _Pragma("GCC error \"This definition is deprecated, use PAL_fadeInAll(..) instead.\"")

/**
 *  \deprecated Use #PAL_isDoingFade(..) instead
 */
#define VDP_isDoingFade()     _Pragma("GCC error \"This definition is deprecated, use PAL_isDoingFade(..) instead.\"")
/**
 *  \deprecated Use #PAL_waitFadeCompletion(..) instead
 */
#define VDP_waitFadeCompletion()     _Pragma("GCC error \"This definition is deprecated, use PAL_waitFadeCompletion(..) instead.\"")
/**
 *  \deprecated Use #PAL_interruptFade() instead
 */
#define VDP_interruptFade()     _Pragma("GCC error \"This definition is deprecated, use PAL_interruptFade(..) instead.\"")


#endif // _VDP_PAL_H_
````

## File: inc/vdp_spr.h
````c
/**
 *  \file vdp_spr.h
 *  \brief VDP Sprite support
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides methods to allocate and manipulate VDP Sprite at low level.<br>
 * The Sega Genesis VDP can handle up to 80 simultanous sprites of 4x4 tiles (32x32 pixels).
 */

#include "config.h"
#include "types.h"
#include "dma.h"

#ifndef _VDP_SPR_H_
#define _VDP_SPR_H_

/**
 *  \brief
 *      Maximum size of Sprite Attribute Table (128 in VRAM but limited to 80 in VDP anyway)
 */
#define SAT_MAX_SIZE            80

/**
 *  \brief
 *      Helper to define sprite size in sprite definition structure.
 *
 *  \param w
 *      sprite width (in tile).
 *  \param h
 *      sprite height (in tile).
 */
#define SPRITE_SIZE(w, h)   ((((w) - 1) << 2) | ((h) - 1))


/**
 *  \brief
 *      VDP sprite definition structure replicating VDP hardware sprite.
 *
 *  \param y
 *      Y position - 0x80 (0x80 = 0 on screen). Valid values: [0 - 1023]
 *  \param size
 *      sprite size (see SPRITE_SIZE macro)
 *    \param sizeH
 *      horizontal size. Valid values: 0 -> 8, 1 -> 16, 2 -> 24, 3 -> 32
 *    \param sizeV
 *      vertical size. Valid values: 0 -> 8, 1 -> 16, 2 -> 24, 3 -> 32
 *  \param link
 *      sprite link, this information is used to define sprite drawing order (use 0 to force end of list)
 *  \param attribut
 *      tile index and sprite attribut (priority, palette, H/V flip), see TILE_ATTR_FULL macro
 *    \param priority
 *      sprite priority. Valid values: 0 -> low, 1 -> high
 *    \param palette
 *      palette index. Valid values: [0, 3]
 *    \param flipV
 *      vertical flip. Valid values: 0 -> normal, 1 -> flipped
 *    \param flipH
 *      horizontal flip. Valid values: 0 -> normal, 1 -> flipped
 *    \param tile
 *      tile index. Valid values: [0, 2047]
 *  \param x
 *      X position - 0x80 (0x80 = 0 on screen). Valid values: [0 - 1023]
 */
typedef struct
{
    s16 y;  // 10 bits
    union
    {
        struct
        {
            u16 unused1  : 4;
            u16 sizeH    : 2;
            u16 sizeV    : 2;
            u16 unused2  : 1;
            u16 linkData : 7;
        };
        struct
        {
            u8 size;
            u8 link;
        };
        u16 size_link;
    };
    union
    {
        u16 attribut;
        struct
        {
            u16 priority : 1;
            u16 palette  : 2;
            u16 flipV    : 1;
            u16 flipH    : 1;
            u16 tile     : 11;
        };
    };
    s16 x;  // 10 bits
}  VDPSprite;


/**
 *  \brief VDP sprite cache
 */
extern VDPSprite vdpSpriteCache[SAT_MAX_SIZE + 16];

/**
 *  \brief Pointer to last allocated sprite after calling VDP_allocateSprites(..) method.<br>
 *    This can be used to do the link from the last allocated VDP sprite.
 */
extern VDPSprite* lastAllocatedVDPSprite;
/**
 *  \brief Highest index of allocated VDP sprite since the last call to VDP_resetSprites() or VDP_releaseAllSprites().<br>
 *      A value of <i>-1</i> mean no VDP Sprite were allocated..<br>
 *      This can be used to define the number of sprite to transfer with VDP_updateSprites(..) method.<br>
 *      <b>WARNING:</b> this value is not correctly updated on sprite release operation so it may gives an higher index than reality.<br>
 *      You can ue currently VDP_refreshHighestAllocatedSpriteIndex() method to force recomputation of highest index (costs a bit of time).
 *
 *  \see VDP_refreshHighestAllocatedSpriteIndex()
 */
extern s16 highestVDPSpriteIndex;


/**
 *  \brief
 *      Clear all sprites and reset VDP sprite allocation (if any).
 */
void VDP_resetSprites(void);

/**
 *  \brief
 *      Release all VDP sprite allocation
 */
void VDP_releaseAllSprites(void);

/**
 *  \brief
 *      Allocate the specified number of hardware VDP sprites and link them together.<br>
 *
 *  \param num
 *      Number of VDP sprite to allocate (need to be > 0)
 *  \return the first VDP sprite index where allocation was made.<br>
 *      -1 if there is not enough available VDP sprite remaining.<br>
 *      <b>WARNING:</b> VDP can display up to 80 sprites at once on screen in H40 mode only, in H32 mode
 *      it's limited to 64 sprites even if we allow to allocate up to 80 (SAT size).
 *
 *  This method allocates the specified number of VDP sprite and returns the index of the
 *  first allocated sprite in VDP sprite table (see vdpSpriteCache).<br>
 *  Sprites are linked together using <i>link</i> field (last sprite ends with link 0).<br>
 *  If there is not enough available VDP sprites the allocation operation fails and return -1.
 *  NOTE: The last sprite from the allocated list can be retrieved with <i>lastAllocatedVDPSprite</i>, this is
 *  to avoid parsing all the list to find it, if we want to link it to a specific sprite for instance.<br>
 *
 *  \see VDP_releaseSprites(..)
 */
s16 VDP_allocateSprites(u16 num);
/**
 *  \brief
 *      Release specified number of VDP sprites.
 *
 *  \param index
 *      The index of the first VDP sprite to release (0 <= index < SAT_MAX_SIZE)
 *  \param num
 *      Number of VDP sprite to release (should be > 0)
 *
 *  This method release the specified number of VDP sprite from the specified index using
 *  the <i>link</i> field information to determine which sprites to release when more than
 *  1 sprite is released.
 *
 *  \see VDP_allocateSprites(..)
 */
void VDP_releaseSprites(u16 index, u16 num);
/**
 *  \brief
 *      Returns the number of available VDP sprite from the SAT.
 *      <b>WARNING:</b> the SAT maximum size is 128 entries in VRAM <b>but</b> the VDP can only display up to 80 (H40 mode) or
 *      64 sprites (H32 mode) at once on the screen.
 *
 *  \see VDP_allocateSprites(..)
 *  \see VDP_releaseSprites(..)
 */
u16 VDP_getAvailableSprites(void);
/**
 *  \brief
 *      Compute and return the highest index of currently allocated VDP sprite.<br>
 *      A value of <i>-1</i> mean no VDP Sprite are allocated.<br>
 *      This value can be used to define the number of sprite to transfer with VDP_updateSprites(..) method.
 *
 *  \see VDP_allocateSprites(..)
 *  \see VDP_releaseSprites(..)
 *  \see highestVDPSpriteIndex
 */
s16 VDP_refreshHighestAllocatedSpriteIndex(void);

/**
 *  \brief
 *      Clear all sprites.
 */
void VDP_clearSprites(void);
/**
 *  \brief
 *      Set a sprite (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to set (should be < SAT_MAX_SIZE).
 *  \param x
 *      Sprite position X on screen.
 *  \param y
 *      Sprite position Y on screen.
 *  \param size
 *      Sprite size (see SPRITE_SIZE() macro).
 *  \param attribut
 *      Sprite tile attributes (see TILE_ATTR_FULL() macro).
 *  \param link
 *      Sprite link (index of next sprite, 0 for end)<br>
 *      Be careful to not modify link made by VDP_allocateSprite(..), use VDP_setSprite(..) instead in that case.
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteFull(u16 index, s16 x, s16 y, u8 size, u16 attribut, u8 link);
/**
 *  \brief
 *      Set a sprite (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to set (should be < SAT_MAX_SIZE).
 *  \param x
 *      Sprite position X on screen.
 *  \param y
 *      Sprite position Y on screen.
 *  \param size
 *      Sprite size (see SPRITE_SIZE() macro).
 *  \param attribut
 *      Sprite tile attributes (see TILE_ATTR_FULL() macro).
 *
 *  \see VDP_setSpriteFull(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSprite(u16 index, s16 x, s16 y, u8 size, u16 attribut);
/**
 *  \brief
 *      Set sprite position (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify position (should be < SAT_MAX_SIZE).
 *  \param x
 *      Sprite position X.
 *  \param y
 *      Sprite position Y.
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpritePosition(u16 index, s16 x, s16 y);
/**
 *  \brief
 *      Set sprite size (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify size (should be < SAT_MAX_SIZE).
 *  \param size
 *      Sprite size (see SPRITE_SIZE() macro).
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteSize(u16 index, u8 size);
/**
 *  \brief
 *      Set sprite attributes (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 *  \param attribut
 *      Sprite tile attributes (see TILE_ATTR_FULL() macro).
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteAttribut(u16 index, u16 attribut);
/**
 *  \brief
 *      Set sprite link (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify link (should be < SAT_MAX_SIZE).
 *  \param link
 *      Sprite link (index of next sprite, 0 for end).
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteLink(u16 index, u8 link);
/**
 *  \brief
 *      Link sprites starting at the specified index.<br>
 *      Links are created in simple ascending order (1 --> 2 --> 3 --> ...)
 *
 *  \param index
 *      Index of the first sprite we want to link (should be < SAT_MAX_SIZE).
 *  \param num
 *      Number of link to create (if you want to link 2 sprites you should use 1 here)
 *  \return
 *      The last linked sprite
 */
VDPSprite* VDP_linkSprites(u16 index, u16 num);

/**
 *  \brief
 *      Send the cached sprite list to the VDP.
 *
 *  \param num
 *      Number of sprite to transfer starting at index 0 (max = MAX_SPRITE).<br>
 *      If you use dynamic VDP Sprite allocation you may use 'highestVDPSpriteIndex + 1' here
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  \see highestVDPSpriteIndex
 *  \see VDP_refreshHighestAllocatedSpriteIndex()
 */
void VDP_updateSprites(u16 num, TransferMethod tm);
/**
 *  \brief
 *      Set sprite priority (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 *  \param priority
 *      sprite priority. Valid values: 0 -> low, 1 -> high
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpritePriority(u16 index, bool priority);
/**
 *  \brief
 *      Get sprite priority (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 */
bool VDP_getSpritePriority(u16 index);
/**
 *  \brief
 *      Set sprite palette (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 *  \param palette
 *      palette index. Valid values: [0, 3]
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpritePalette(u16 index, u16 palette);
/**
 *  \brief
 *      Get sprite palette (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 */
u16 VDP_getSpritePalette(u16 index);
/**
 *  \brief
 *      Set sprite horizontal and vertical flip (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 *  \param flipH
 *      Horizontal flip. Valid values: 0 -> normal, 1 -> flipped.
 *  \param flipV
 *      Vertical flip. Valid values: 0 -> normal, 1 -> flipped.
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteFlip(u16 index, bool flipH, bool flipV);
/**
 *  \brief
 *      Set sprite horizontal flip (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 *  \param flipH
 *      Horizontal flip. Valid values: 0 -> normal, 1 -> flipped.
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteFlipH(u16 index, bool flipH);
/**
 *  \brief
 *      Set sprite vertical flip (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 *  \param flipV
 *      Vertical flip. Valid values: 0 -> normal, 1 -> flipped.
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteFlipV(u16 index, bool flipV);
/**
 *  \brief
 *      Get sprite horizontal flip (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 */
bool VDP_getSpriteFlipH(u16 index);
/**
 *  \brief
 *      Get sprite vertical flip (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 */
bool VDP_getSpriteFlipV(u16 index);
/**
 *  \brief
 *      Set sprite tile index (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 *  \param tile
 *      Tile index. Valid values: [0, 2047].
 *
 *  \see VDP_setSprite(..)
 *  \see VDP_updateSprites(..)
 */
void VDP_setSpriteTile(u16 index, u16 tile);
/**
 *  \brief
 *      Get sprite tile index (use sprite list cache).
 *
 *  \param index
 *      Index of the sprite to modify attributes (should be < SAT_MAX_SIZE).
 */
u16 VDP_getSpriteTile(u16 index);

#endif // _VDP_SPR_H_
````

## File: inc/vdp_tile.h
````c
/**
 *  \file vdp_tile.h
 *  \brief VDP General Tile / Tilemap operations
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides methods to manipulate VDP tiles and tilemap :<br>
 * - upload tiles to VDP memory<br>
 * - upload tiles to VDP memory from bitmap data<br>
 * - clear / fill / set tile map data<br>
 */

#ifndef _VDP_TILE_H_
#define _VDP_TILE_H_


#include "vdp.h"
#include "dma.h"


/**
 *  \brief
 *      Bit shift for the tile priority attribute in tilemap data.
 */
#define TILE_ATTR_PRIORITY_SFT      15
/**
 *  \brief
 *      Bit shift for the tile palette attribute in tilemap data.
 */
#define TILE_ATTR_PALETTE_SFT       13
/**
 *  \brief
 *      Bit shift for the tile V flip attribute in tilemap data.
 */
#define TILE_ATTR_VFLIP_SFT         12
/**
 *  \brief
 *      Bit shift for the tile H flip attribute in tilemap data.
 */
#define TILE_ATTR_HFLIP_SFT         11
/**
 *  \brief
 *      Mask for the tile priority attribute in tilemap data.
 */
#define TILE_ATTR_PRIORITY_MASK     (1 << TILE_ATTR_PRIORITY_SFT)
/**
 *  \brief
 *      Mask for the tile palette attribute in tilemap data.
 */
#define TILE_ATTR_PALETTE_MASK      (3 << TILE_ATTR_PALETTE_SFT)
/**
 *  \brief
 *      Mask for the tile V flip attribute in tilemap data.
 */
#define TILE_ATTR_VFLIP_MASK        (1 << TILE_ATTR_VFLIP_SFT)
/**
 *  \brief
 *      Mask for the tile H flip attribute in tilemap data.
 */
#define TILE_ATTR_HFLIP_MASK        (1 << TILE_ATTR_HFLIP_SFT)
/**
 *  \brief
 *      Mask for the tile attributes (priority, palette and flip) in tilemap data.
 */
#define TILE_ATTR_MASK              (TILE_ATTR_PRIORITY_MASK | TILE_ATTR_PALETTE_MASK | TILE_ATTR_VFLIP_MASK | TILE_ATTR_HFLIP_MASK)

/**
 *  \brief
 *      Encode tile attributes for tilemap data.
 *
 *  \param pal
 *      Palette index
 *  \param prio
 *      Tile priority
 *  \param flipV
 *      Vertical flip
 *  \param flipH
 *      Horizontal flip
 */
#define TILE_ATTR(pal, prio, flipV, flipH)               (((flipH) << TILE_ATTR_HFLIP_SFT) + ((flipV) << TILE_ATTR_VFLIP_SFT) + ((pal) << TILE_ATTR_PALETTE_SFT) + ((prio) << TILE_ATTR_PRIORITY_SFT))
/**
 *  \brief
 *      Encode tile attributes for tilemap data.
 *
 *  \param pal
 *      Palette index
 *  \param prio
 *      Tile priority
 *  \param flipV
 *      Vertical flip
 *  \param flipH
 *      Horizontal flip
 *  \param index
 *      Tile index
 */
#define TILE_ATTR_FULL(pal, prio, flipV, flipH, index)   (((flipH) << TILE_ATTR_HFLIP_SFT) + ((flipV) << TILE_ATTR_VFLIP_SFT) + ((pal) << TILE_ATTR_PALETTE_SFT) + ((prio) << TILE_ATTR_PRIORITY_SFT) + (index))

/**
 *  \brief
 *      Tile set structure which contains tiles definition.<br>
 *      Use the unpackTileSet() method to unpack if compression is enabled.
 *
 *  \param compression
 *      compression type, accepted values:<br>
 *      <b>COMPRESSION_NONE</b><br>
 *      <b>COMPRESSION_APLIB</b><br>
 *      <b>COMPRESSION_LZ4W</b><br>
 *  \param numTile
 *      number of tile in the <i>tiles</i> buffer.
 *  \param tiles
 *      Tiles data (packed or not depending compression field).
 */
typedef struct
{
    u16 compression;
    u16 numTile;
    u32 *tiles;
} TileSet;

/**
 *  \brief
 *      TileMap structure which contains tilemap background definition.<br>
 *      Use the unpackTileMap() method to unpack if compression is enabled.
 *  \param compression
 *      compression type, accepted values:<br>
 *      <b>COMPRESSION_NONE</b><br>
 *      <b>COMPRESSION_APLIB</b><br>
 *      <b>COMPRESSION_LZ4W</b><br>
 *  \param w
 *      tilemap width in tile.
 *  \param h
 *      tilemap height in tile.
 *  \param tilemap
 *      Tilemap data.
 */
typedef struct
{
    u16 compression;
    u16 w;
    u16 h;
    u16 *tilemap;
} TileMap;

/**
 *  \brief
 *      Return the VRAM tilemap address for the specified plane position
 *
 *  \param plane
 *      Plane we want to get the VRAM tilemap address for a given position.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param x
 *      X position (in tile).
 *  \param y
 *      Y position (in tile).
 */
u16 VDP_getPlaneAddress(VDPPlane plane, u16 x, u16 y);

/**
 *  \brief
 *      Load tile data (pattern) in VRAM.
 *
 *  \param data
 *      Pointer to tile data.
 *  \param index
 *      Tile index where start tile data load (use TILE_USER_INDEX as base user index).
 *  \param num
 *      Number of tile to load.
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Transfert rate:<br>
 *  ~90 bytes per scanline in software (during blanking)<br>
 *  ~190 bytes per scanline in hardware (during blanking)
 */
void VDP_loadTileData(const u32 *data, u16 index, u16 num, TransferMethod tm);
/**
 *  \brief
 *      Load tile data (pattern) in VRAM.
 *
 *  \param tileset
 *      Pointer to TileSet structure.<br>
 *      The TileSet is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param index
 *      Tile index where start tile data load (use TILE_USER_INDEX as base user index).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *  \return
 *      FALSE if there is not enough memory to unpack the specified TileSet (only if compression was enabled).
 *
 *  Transfert rate:<br>
 *  ~90 bytes per scanline in software (during blanking)<br>
 *  ~190 bytes per scanline in hardware (during blanking)
 */
bool VDP_loadTileSet(const TileSet *tileset, u16 index, TransferMethod tm);
/**
 *  \brief
 *      Load a subset of tile data (pattern) in VRAM. Useful to only reload a part of a tileSet if it;s been overwritten
 *      or in case of streaming maps, to get the required tiles out of the full tile atlas
 *
 *  \param tileset
 *      Pointer to TileSet structure.<br>
 *      The TileSet is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param index
 *      Tile index where start tile data load (use TILE_USER_INDEX as base user index).
 *  \param fromTile
 *      Start tile index to be loaded from the set.
 *  \param count
 *      Number of tiles to Load.
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *  \return
 *      FALSE if there is not enough memory to unpack the specified TileSet (only if compression was enabled).
 *
 *  Transfert rate:<br>
 *  ~90 bytes per scanline in software (during blanking)<br>
 *  ~190 bytes per scanline in hardware (during blanking)
 */
bool VDP_loadTileSetEx(const TileSet *tileset, u16 index, u16 fromTile, u16 count, TransferMethod tm);
/**
 *  \brief
 *      Load font tile data in VRAM.<br>
 *      Note that you should prefer the VDP_loadFont(..) method to this one (easier to use).
 *
 *  \param font
 *      Pointer to font tile data.
 *  \param length
 *      Number of characters of the font (max = FONT_LENGTH).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  This fonction permits to replace system font by user font.<br>
 *  The font tile data are loaded to TILE_FONT_INDEX and can contains FONT_LENGTH characters at max.<br>
 *  Each character should fit in one tile (8x8 pixels bloc).<br>
 *  See also VDP_loadFont(..) and VDP_loadTileData(..)
 */
void VDP_loadFontData(const u32 *font, u16 length, TransferMethod tm);
/**
 *  \brief
 *      Load font from the specified <i>Font</i> tileset in VRAM.
 *
 *  \param font
 *      TileSet containing the font (should be 16*6 characters font, see SGDK default font at <i>res\image\font_default.png</i>).<br>
 *      The TileSet is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *  \return
 *      FALSE if there is not enough memory to unpack the specified font (only if compression was enabled).
 *
 *  This fonction permits to replace system font by user font.<br>
 *  The font tile data are loaded to TILE_FONT_INDEX and can contains FONT_LENGTH characters at max.<br>
 *  Each character should fit in one tile (8x8 pixels bloc).<br>
 *  See also VDP_loadFontData(..)
 */
bool VDP_loadFont(const TileSet *font, TransferMethod tm);
/**
 *  \brief
 *      Load the default <i>Font</i> tileset in VRAM.
 *
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *  \return
 *      FALSE if there is not enough memory to unpack the specified font (only if compression was enabled).
 *
 *  The font tile data are loaded to TILE_FONT_INDEX and can contains FONT_LENGTH characters at max.<br>
 *  Each character should fit in one tile (8x8 pixels bloc).<br>
 *  See also VDP_loadFontData(..)
 */
bool VDP_loadDefaultFont(TransferMethod tm);

/**
 *  \brief
 *      Load 4bpp bitmap tile data in VRAM.
 *
 *  \param data
 *      Pointer to 4bpp bitmap tile data.
 *  \param index
 *      Tile index where start tile data load (use TILE_USER_INDEX as base user index).
 *  \param w
 *      Width of bitmap region to load (in tile).
 *  \param h
 *      Heigh of bitmap region to load (in tile).
 *  \param bmp_w
 *      Width of bitmap (in tile), it can differ from 'w' parameter.
 *
 *  This function does "on the fly" 4bpp bitmap conversion to tile data and transfert them to VRAM.<br>
 *  It's very helpful when you use bitmap images but the conversion eats sometime so you should use it only for static screen only.<br>
 *  For "in-game" condition you should use VDP_loadTileData() method with converted tile data.<br>
 *  See also VDP_loadBMPTileDataEx().
 */
void VDP_loadBMPTileData(const u32 *data, u16 index, u16 w, u16 h, u16 bmp_w);
/**
 *  \brief
 *      Load 4bpp bitmap tile data in VRAM.
 *
 *  \param data
 *      Pointer to 4bpp bitmap tile data.
 *  \param index
 *      Tile index where start tile data load (use TILE_USER_INDEX as base user index).
 *  \param x
 *      X start position of bitmap region to load (in tile).
 *  \param y
 *      Y start position of bitmap region to load (in tile).
 *  \param w
 *      Width of bitmap region to load (in tile).
 *  \param h
 *      Heigh of bitmap region to load (in tile).
 *  \param bmp_w
 *      Width of bitmap (in tile), it can differ from 'w' parameter.
 *
 *  This function does "on the fly" 4bpp bitmap conversion to tile data and transfert them to VRAM.<br>
 *  It's very helpful when you use bitmap images but the conversion eats sometime so you should use it only for static screen only.<br>
 *  For "in-game" condition you should use VDP_loadTileData() method with converted tile data.
 *  See also VDP_loadBMPTileData()
 */
void VDP_loadBMPTileDataEx(const u32 *data, u16 index, u16 x, u16 y, u16 w, u16 h, u16 bmp_w);

/**
 *  \brief
 *      Fill tile data in VRAM.
 *
 *  \param value
 *      Value (byte) used to fill VRAM tile data.
 *  \param index
 *      Tile index where start tile data fill (use TILE_USER_INDEX as base user index).
 *  \param num
 *      Number of tile to fill.
 *  \param wait
 *      Wait the operation to complete when set to TRUE otherwise it returns immediately
 *      but then you will require to wait for DMA completion (#DMA_waitCompletion()) before accessing the VDP.
 *
 *  This function is generally used to clear tile data in VRAM.
 */
void VDP_fillTileData(u8 value, u16 index, u16 num, bool wait);

/**
 *  \brief
 *      Clear tilemap.
 *
 *  \param planeAddr
 *      Plane address where we want to clear tilemap.<br>
 *      Accepted values are:<br>
 *      - VDP_BG_A<br>
 *      - VDP_BG_B<br>
 *      - VDP_WINDOW<br>
 *  \param ind
 *      Tile index where to start clear.
 *  \param num
 *      Number of tile to clear.
 *  \param wait
 *      Wait the operation to complete when set to TRUE otherwise it returns immediately
 *      but then you will require to wait for DMA completion (#DMA_waitCompletion()) before accessing the VDP.
 *
 *  \see VDP_clearTileMapRect()
 *  \see VDP_fillTileMap()
 */
void VDP_clearTileMap(u16 planeAddr, u16 ind, u16 num, bool wait);
/**
 *  \brief
 *      Fill tilemap.
 *
 *  \param planeAddr
 *      Plane address where we want to fill tilemap.<br>
 *      Accepted values are:<br>
 *      - VDP_BG_A<br>
 *      - VDP_BG_B<br>
 *      - VDP_WINDOW<br>
 *  \param tile
 *      Tile attributes (see TILE_ATTR_FULL() and TILE_ATTR() macros).
 *  \param ind
 *      tile index where to start fill.
 *  \param num
 *      Number of tile to fill.
 *
 *  \see VDP_fillTileMapRect()
 */
void VDP_fillTileMap(u16 planeAddr, u16 tile, u16 ind, u16 num);
/**
 *  \brief
 *      Set tilemap data at specified index.
 *
 *  \param planeAddr
 *      Plane address where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - VDP_BG_A<br>
 *      - VDP_BG_B<br>
 *      - VDP_WINDOW<br>
 *  \param data
 *      Tile attributes data (see TILE_ATTR_FULL() and TILE_ATTR() macros).
 *  \param ind
 *      Tile index where to start to set tilemap data.
 *  \param num
 *      Number of tile to set.
 *  \param vramStep
 *      VRAM address increment after each write (default value = 2)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Set the specified tilemap with specified tile attributes values.<br>
 *  You can use this method when you are using the 'mapbase' parameter on your resource definition to set the base attributes<br>
 *  (palette, priority and base tile index) so you don't need to provide them here.<br>
 *  This method is faster than using #VDP_setTileMapDataEx(..) which allow to override base tile attributes though the 'basetile' parameter.
 *
 *  \see VDP_setTileMapDataEx().
 *  \see VDP_setTileMapDataRect().
 */
void VDP_setTileMapData(u16 planeAddr, const u16 *data, u16 ind, u16 num, u16 vramStep, TransferMethod tm);
/**
 *  \brief
 *      Set tilemap data at specified index (extended version).
 *
 *  \param planeAddr
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - VDP_BG_A<br>
 *      - VDP_BG_B<br>
 *      - VDP_WINDOW<br>
 *  \param data
 *      Tile attributes data (see TILE_ATTR_FULL() and TILE_ATTR() macros).
 *  \param basetile
 *      Base tile index and flag for tile attributes (see TILE_ATTR_FULL() macro).
 *  \param ind
 *      Tile index where to start to set tilemap data.
 *  \param num
 *      Number of tile to set.
 *  \param vramStep
 *      VRAM address increment after each write (default value = 2).
 *
 *  Set the specified tilemap with specified tile attributes values.<br>
 *  Unlike #VDP_setTileMapData(..) this method let you to override the base tile attributes (priority, palette and base index)<br>
 *  at the expense of more computation time. If you want faster tilemap processing (using #VDP_setTileMapData(..)), you can use<br>
 *  the 'mapbase' parameter when declaring your IMAGE resource to set base tile attributes but then you have fixed/static tile allocation.
 *
 *  \see VDP_setTileMapData()
 *  \see VDP_setTileMapDataRectEx()
 */
void VDP_setTileMapDataEx(u16 planeAddr, const u16 *data, u16 basetile, u16 ind, u16 num, u16 vramStep);

/**
 *  \brief
 *      Set tilemap data (single position).
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tile
 *      tile attributes (see TILE_ATTR_FULL() and TILE_ATTR() macros).
 *  \param x
 *      X position (in tile).
 *  \param y
 *      y position (in tile).
 *
 *  Set the specified tilemap position (tilemap wrapping supported) with given tile attributes.
 */
void VDP_setTileMapXY(VDPPlane plane, u16 tile, u16 x, u16 y);
/**
 *  \brief
 *      Clear specified region of tilemap.
 *
 *  \param plane
 *      Plane where we want to clear tilemap region.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param x
 *      Region X start position (in tile).
 *  \param y
 *      Region Y start position (in tile).
 *  \param w
 *      Region Width (in tile).
 *  \param h
 *      Region Heigh (in tile).
 *
 *  \see VDP_clearTileMap() (faster method)
 */
void VDP_clearTileMapRect(VDPPlane plane, u16 x, u16 y, u16 w, u16 h);
/**
 *  \brief
 *      Fill speficied region of tilemap.
 *
 *  \param plane
 *      Plane where we want to fill tilemap region.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tile
 *      tile attributes (see TILE_ATTR_FULL() and TILE_ATTR() macros).
 *  \param x
 *      Region X start position (in tile).
 *  \param y
 *      Region Y start position (in tile).
 *  \param w
 *      Region Width (in tile).
 *  \param h
 *      Region Heigh (in tile).
 *
 *  Fill the specified tilemap region with specified tile attributes value.
 *
 *  \see VDP_fillTileMap() (faster method)
 *  \see VDP_fillTileMapRectInc()
 */
void VDP_fillTileMapRect(VDPPlane plane, u16 tile, u16 x, u16 y, u16 w, u16 h);
/**
 *  \brief
 *      Fill tilemap with index auto increment at specified region.
 *
 *  \param plane
 *      Plane where we want to fill tilemap region.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param basetile
 *      Base tile attributes (see TILE_ATTR_FULL() and TILE_ATTR() macros).
 *  \param x
 *      Region X start position (in tile).
 *  \param y
 *      Region Y start position (in tile).
 *  \param w
 *      Region Width (in tile).
 *  \param h
 *      Region Heigh (in tile).
 *
 *  Set the specified tilemap region with specified tile attributes value.<br>
 *  The function auto increments tile index in tile attributes like this:<br>
 *  tilemap line 0 : basetile, basetile+1, basetile+2, basetile+3, ...<br>
 *  tilemap line 1 : basetile+w, basetile+w+1, basetile+w+2, ...<br>
 *  ...<br>
 *  So this function is convenient to display generated image or simulate a frame buffer.<br>
 *
 *  \see also VDP_fillTileMapRect()
 */
void VDP_fillTileMapRectInc(VDPPlane plane, u16 basetile, u16 x, u16 y, u16 w, u16 h);

/**
 *  \brief
 *      Set tilemap data for specified region.
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param data
 *      Source tilemap data containing tile attributes (see TILE_ATTR_FULL() macro).
 *  \param x
 *      Region X start position (in tile).
 *  \param y
 *      Region Y start position (in tile).
 *  \param w
 *      Region Width (in tile).
 *  \param h
 *      Region Heigh (in tile).
 *  \param wm
 *      Source tilemap width (in tile).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Set the specified tilemap region (tilemap wrapping supported) with specified tile attributes values.<br>
 *  You can use this method when you are using the 'mapbase' parameter on your resource definition to set the base attributes<br>
 *  (palette, priority and base tile index) so you don't need to provide them here.<br>
 *  This method is faster than using #VDP_setTileMapDataRectEx(..) which allow to override base tile attributes though the 'basetile' parameter.
 *
 *  \see VDP_setTileMapDataRectEx().
 *  \see VDP_setTileMapData().
 */
void VDP_setTileMapDataRect(VDPPlane plane, const u16 *data, u16 x, u16 y, u16 w, u16 h, u16 wm, TransferMethod tm);
/**
 *  \brief
 *      Set tilemap data for specified region (extended version).
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param data
 *      Source tilemap data containing tile attributes (see TILE_ATTR_FULL() macro).
 *  \param basetile
 *      Base index and flag for tile attributes (see TILE_ATTR_FULL() macro).
 *  \param x
 *      Region X start position (in tile).
 *  \param y
 *      Region Y start position (in tile).
 *  \param w
 *      Region Width (in tile).
 *  \param h
 *      Region Heigh (in tile).
 *  \param wm
 *      Source tilemap width (in tile).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *      But i highly discourage of using DMA here as VDP_setTileMapDataRectEx(..) requires to prepare data in a temporary buffer first<br>
 *      to use DMA, resulting in a slower process than using CPU. However DMA_QUEUE is very useful as it wil prepare the data<br>
 *      and transfer the data as fast as possible during VBlank.
 *
 *  Set the specified tilemap region (tilemap wrapping supported) with specified tile attributes values.<br>
 *  Unlike #VDP_setTileMapDataRect(..) this method let you to override the base tile attributes (priority, palette and base index)<br>
 *  at the expense of more computation time. If you want faster tilemap processing (using #VDP_setTileMapDataRect(..)), you can use<br>
 *  the 'mapbase' parameter when declaring your IMAGE resource to set base tile attributes but then you have fixed/static tile allocation.
 *
 *  \see VDP_setTileMapDataRect()
 *  \see VDP_setTileMapDataEx()
 */
void VDP_setTileMapDataRectEx(VDPPlane plane, const u16 *data, u16 basetile, u16 x, u16 y, u16 w, u16 h, u16 wm, TransferMethod tm);

/**
 *  \brief
 *      Set a row of tilemap data.
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param data
 *      Source tilemap data containing tile attributes (see TILE_ATTR_FULL() macro).
 *  \param row
 *      Plane row we want to set data
 *  \param x
 *      Row X start position (in tile)
 *  \param w
 *      Row width to update (in tile)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Set a row of tilemap data (tilemap wrapping supported) with given tile attributes values.
 *  You can use this method when you are using the 'mapbase' parameter on your resource definition to set the base attributes<br>
 *  (palette, priority and base tile index) so you don't need to provide them here.<br>
 *  This method is faster than using #VDP_setTileMapDataRowEx(..) which allow to override base tile attributes though the 'basetile' parameter.
 *
 *  \see VDP_setTileMapDataRowPartEx()
 *  \see VDP_setTileMapDataRow()
 */
void VDP_setTileMapDataRow(VDPPlane plane, const u16 *data, u16 row, u16 x, u16 w, TransferMethod tm);
/**
 *  \brief
 *      Set a row of tilemap data - extended version.
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param data
 *      Source tilemap data containing tile attributes (see TILE_ATTR_FULL() macro).
 *  \param basetile
 *      Base index and flag for tile attributes (see TILE_ATTR_FULL() macro).
 *  \param row
 *      Plane row we want to set data
 *  \param x
 *      Row X start position (in tile)
 *  \param w
 *      Row width to update (in tile)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *      But i highly discourage of using DMA here as VDP_setTileMapDataRowEx(..) requires to prepare data in a temporary buffer first<br>
 *      to use DMA, resulting in a slower process than using CPU. However DMA_QUEUE is very useful as it wil prepare the data<br>
 *      and transfer the data as fast as possible during VBlank.
 *
 *  Set a row of tilemap data (tilemap wrapping supported) with given tile attributes values.<br>
 *  Unlike #VDP_setTileMapDataRow(..) this method let you to override the base tile attributes (priority, palette and base index)<br>
 *  at the expense of more computation time. If you want faster tilemap processing (using #VDP_setTileMapDataRow(..)), you can use<br>
 *  the 'mapbase' parameter when declaring your IMAGE resource to set base tile attributes but then you have fixed/static tile allocation.
 *
 *  \see VDP_setTileMapDataRowPart()
 *  \see VDP_setTileMapDataRow()
 */
void VDP_setTileMapDataRowEx(VDPPlane plane, const u16 *data, u16 basetile, u16 row, u16 x, u16 w, TransferMethod tm);
/**
 *  \brief
 *      Set a complete column of pre-arranged tilemap data (not supported when plane width is set to 128).
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param data
 *      Prepared tile attributes data (see TILE_ATTR_FULL() macro).<br>
 *      Column data are already arranged to be transferred as a single contiguous data block.
 *  \param column
 *      Plane column we want to set data
 *  \param y
 *      Column Y start position (in tile)
 *  \param h
 *      Column height to update (in tile)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Set a complete column of tilemap data with given tile attributes values.<br>
 *  This method is faster than #VDP_setTileMapDataColumn(..) or #VDP_setTileMapDataColumnEx(..) as it assumes<br>
 *  that data buffer is properly prepared and arranged to be directly copied as it.<br>
 *  <b>WARNING:</b> this function doesn't work when plane width is set to 128 (see #VDP_setPlaneSize(..) method).
 *
 *  \see VDP_setTileMapDataRowFast()
 *  \see VDP_setTileMapDataColumn()
 *  \see VDP_setTileMapData()
 */
void VDP_setTileMapDataColumnFast(VDPPlane plane, u16* data, u16 column, u16 y, u16 h, TransferMethod tm);
/**
 *  \brief
 *      Set a column of tilemap data (not supported when plane width is set to 128).
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param data
 *      Source tilemap data containing tile attributes (see TILE_ATTR_FULL() macro).
 *  \param column
 *      Plane column we want to set data
 *  \param y
 *      Column Y start position (in tile)
 *  \param h
 *      Column height to update (in tile)
 *  \param wm
 *      Source tilemap width (in tile).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Set a column of tilemap data (tilemap wrapping supported) with given tile attributes values.<br>
 *  You can use this method when you are using the 'mapbase' parameter on your resource definition to set the base tile attributes<br>
 *  (palette, priority and base tile index) so you don't need to provide them here.<br>
 *  This method is faster than using #VDP_setTileMapDataColumnEx(..) which allow to override base tile attributes though the 'basetile' parameter.<br>
 *  <b>WARNING:</b> this function doesn't work when plane width is set to 128 (see #VDP_setPlaneSize(..) method)
 *
 *  \see VDP_setTileMapDataColumnPartEx()
 *  \see VDP_setTileMapDataColumn()
 */
void VDP_setTileMapDataColumn(VDPPlane plane, const u16 *data, u16 column, u16 y, u16 h, u16 wm, TransferMethod tm);
/**
 *  \brief
 *      Set a column of tilemap data - extended version (not supported when plane width is set to 128).
 *
 *  \param plane
 *      Plane where we want to set tilemap data.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param data
 *      Source tilemap data containing tile attributes (see TILE_ATTR_FULL() macro).
 *  \param basetile
 *      Base index and flag for tile attributes (see TILE_ATTR_FULL() macro).
 *  \param column
 *      Plane column we want to set data
 *  \param y
 *      Column Y start position (in tile)
 *  \param h
 *      Column height to update (in tile)
 *  \param wm
 *      Source tilemap width (in tile).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *      But i highly discourage of using DMA here as VDP_setTileMapDataColumnEx(..) requires to prepare data in a temporary buffer first<br>
 *      to use DMA, resulting in a slower process than using CPU. However DMA_QUEUE is very useful as it wil prepare the data<br>
 *      and transfer the data as fast as possible during VBlank.
 *
 *  Set a column of tilemap data (tilemap wrapping supported) with given tile attributes values.<br>
 *  Unlike #VDP_setTileMapDataColumn(..) this method let you to override the base tile attributes (priority, palette and base index)<br>
 *  at the expense of more computation time. If you want faster tilemap processing (using #VDP_setTileMapDataColumn(..)), you can use<br>
 *  the 'mapbase' parameter when declaring your IMAGE resource to set base tile attributes but then you have fixed/static tile allocation.
 *  <b>WARNING:</b> this function doesn't work when plane width is set to 128 (see #VDP_setPlaneSize(..) method)
 *
 *  \see VDP_setTileMapDataColumnPart()
 *  \see VDP_setTileMapDataColumnEx()
 *  \see VDP_setTileMapData()
 */
void VDP_setTileMapDataColumnEx(VDPPlane plane, const u16 *data, u16 basetile, u16 column, u16 y, u16 h, u16 wm, TransferMethod tm);

/**
 *  \brief
 *      Load tilemap region.
 *
 *  \param plane
 *      Plane where we want to load tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tilemap
 *      Source tilemap to load.<br>
 *      The TileMap is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param x
 *      Region X start position (in tile).
 *  \param y
 *      Region Y start position (in tile).
 *  \param w
 *      Region Width (in tile).
 *  \param h
 *      Region Heigh (in tile).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Load the specified tilemap region at equivalent plane position (tilemap wrapping supported).<br>
 *  You can use this method when you are using the 'mapbase' parameter on your resource definition to set the base attributes<br>
 *  (palette, priority and base tile index) so you don't need to provide them here.<br>
 *  This method is faster than using #VDP_setTileMapEx(..) which allow to override base tile attributes though the 'basetile' parameter.
 *
 *  \see VDP_setTileMapData()
 *  \see VDP_setTileMapDataEx()
 */
bool VDP_setTileMap(VDPPlane plane, const TileMap *tilemap, u16 x, u16 y, u16 w, u16 h, TransferMethod tm);
/**
 *  \brief
 *      Load tilemap region at specified plane position.
 *
 *  \param plane
 *      Plane where we want to load tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tilemap
 *      Source tilemap to load.<br>
 *      The TileMap is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param basetile
 *      Base index and flag for tile attributes (see TILE_ATTR_FULL() macro).
 *  \param xp
 *      Plane X destination position (in tile).
 *  \param yp
 *      Plane Y destination position (in tile).
 *  \param x
 *      Region X start position (in tile).
 *  \param y
 *      Region Y start position (in tile).
 *  \param w
 *      Region Width (in tile).
 *  \param h
 *      Region Heigh (in tile).
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *      But i highly discourage of using DMA here as VDP_setTileMapEx(..) requires to prepare data in a temporary buffer first<br>
 *      to use DMA, resulting in a slower process than using CPU. However DMA_QUEUE is very useful as it wil prepare the data<br>
 *      and transfer the data as fast as possible during VBlank.
 *
 *  Load the specified tilemap region at specified plane position (tilemap wrapping supported).<br>
 *  Unlike #VDP_setTileMap(..) this method let you to override the base tile attributes (priority, palette and base index)<br>
 *  at the expense of more computation time. If you want faster tilemap processing (using #VDP_setTileMap(..)), you can use<br>
 *  the 'mapbase' parameter when declaring your IMAGE resource to set base tile attributes but then you have fixed/static tile allocation.
 *
 *  \see VDP_setTileMapDataRect()
 *  \see VDP_setTileMapDataRectEx()
 */
bool VDP_setTileMapEx(VDPPlane plane, const TileMap *tilemap, u16 basetile, u16 xp, u16 yp, u16 x, u16 y, u16 w, u16 h, TransferMethod tm);
/**
 *  \brief
 *      Load tilemap row.
 *
 *  \param plane
 *      Plane where we want to load tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tilemap
 *      Source tilemap to set row from.<br>
 *      The TileMap is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param row
 *      Plane row we want to set data
 *  \param x
 *      Source tilemap X start position (in tile).
 *  \param w
 *      Row width to update (in tile)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Load a complete row of data from tilemap at equivalent plane position (wrapped around if needed).<br>
 *  You can use this method when you are using the 'mapbase' parameter on your resource definition to set the base attributes<br>
 *  (palette, priority and base tile index) so you don't need to provide them here.<br>
 *  This method is faster than using #VDP_setTileMapRowEx(..) which allow to override base tile attributes though the 'basetile' parameter.
 *
 *  \see VDP_setTileMapRowEx()
 *  \see VDP_setMapColumn()
 *  \see VDP_setTileMapDataRow()
 */
bool VDP_setTileMapRow(VDPPlane plane, const TileMap *tilemap, u16 row, u16 x, u16 w, TransferMethod tm);
/**
 *  \brief
 *      Load tilemap row (extended version).
 *
 *  \param plane
 *      Plane where we want to load tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tilemap
 *      Source tilemap to set row from.<br>
 *      The TileMap is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param basetile
 *      Base index and flag for tile attributes (see TILE_ATTR_FULL() macro).
 *  \param row
 *      Plane row we want to set data
 *  \param x
 *      Source tilemap X start position (in tile)
 *  \param y
 *      Source tilemap Y / row position (in tile), can be different that plane row if desired.
 *  \param w
 *      Row width to update (in tile)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *      But i highly discourage of using DMA here as VDP_setTileMapRowEx(..) requires to prepare data in a temporary buffer first<br>
 *      to use DMA, resulting in a slower process than using CPU. However DMA_QUEUE is very useful as it wil prepare the data<br>
 *      and transfer the data as fast as possible during VBlank.
 *
 *  Load a complete row of data from tilemap at equivalent plane position (wrapped around if needed).<br>
 *  Unlike #VDP_setTileMapRow(..) this method let you to override the base tile attributes (priority, palette and base index)<br>
 *  at the expense of more computation time. If you want faster tilemap processing (using #VDP_setTileMapRow(..)), you can use<br>
 *  the 'mapbase' parameter when declaring your IMAGE resource to set base tile attributes but then you have fixed/static tile allocation.
 *
 *  \see VDP_setTileMapRow()
 *  \see VDP_setMapColumnEx()
 *  \see VDP_setTileMapDataRowEx()
 */
bool VDP_setTileMapRowEx(VDPPlane plane, const TileMap *tilemap, u16 basetile, u16 row, u16 x, u16 y, u16 w, TransferMethod tm);
/**
 *  \brief
 *      Load tilemap column (not supported when plane width is set to 128).
 *
 *  \param plane
 *      Plane where we want to load tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tilemap
 *      Source tilemap to set column from.<br>
 *      The TileMap is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param column
 *      Plane column we want to set data
 *  \param y
 *      Source tilemap Y start position (in tile).
 *  \param h
 *      Column height to update (in tile)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY
 *
 *  Load a complete column of data from tilemap at equivalent plane position (wrapped around if needed).<br>
 *  You can use this method when you are using the 'mapbase' parameter on your resource definition to set the base attributes<br>
 *  (palette, priority and base tile index) so you don't need to provide them here.<br>
 *  This method is faster than using #VDP_setTileMapColumnEx(..) which allow to override base tile attributes though the 'basetile' parameter.<br>
 *  <b>WARNING:</b> this function doesn't work when plane width is set to 128 (see #VDP_setPlaneSize(..) method)
 *
 *  \see VDP_setTileMapColumnEx()
 *  \see VDP_setMapRow()
 *  \see VDP_setTileMapDataColumn()
 */
bool VDP_setTileMapColumn(VDPPlane plane, const TileMap *tilemap, u16 column, u16 y, u16 h, TransferMethod tm);
/**
 *  \brief
 *      Load tilemap column - extended version (not supported when plane width is set to 128).
 *
 *  \param plane
 *      Plane where we want to load tilemap.<br>
 *      Accepted values are:<br>
 *      - BG_A<br>
 *      - BG_B<br>
 *      - WINDOW<br>
 *  \param tilemap
 *      Source tilemap to set column from.<br>
 *      The TileMap is unpacked "on-the-fly" if needed (require some memory).<br>
 *      Using DMA_QUEUE for packed resource is unsafe as the resource will be released and eventually
 *      can be overwritten before DMA operation so use DMA_QUEUE_COPY in that case or unpack the resource first.
 *  \param basetile
 *      Base index and flag for tile attributes (see TILE_ATTR_FULL() macro).
 *  \param column
 *      Plane column we want to set data
 *  \param x
 *      Source tilemap X / column position (in tile), can be different than plane column if desired.
 *  \param y
 *      Source tilemap Y start position (in tile).
 *  \param h
 *      Column height to update (in tile)
 *  \param tm
 *      Transfer method.<br>
 *      Accepted values are:<br>
 *      - CPU<br>
 *      - DMA<br>
 *      - DMA_QUEUE<br>
 *      - DMA_QUEUE_COPY<br>
 *      But i highly discourage of using DMA here as VDP_setTileMapColumnEx(..) requires to prepare data in a temporary buffer first<br>
 *      to use DMA, resulting in a slower process than using CPU. However DMA_QUEUE is very useful as it wil prepare the data<br>
 *      and transfer the data as fast as possible during VBlank.
 *
 *  Load a complete column of data from tilemap at equivalent plane position (wrapped around if needed).<br>
 *  Unlike #VDP_setTileMapColumn(..) this method let you to override the base tile attributes (priority, palette and base index)<br>
 *  at the expense of more computation time. If you want faster tilemap processing (using #VDP_setTileMapColumn(..)), you can use<br>
 *  the 'mapbase' parameter when declaring your IMAGE resource to set base tile attributes but then you have fixed/static tile allocation.<br>
 *  <b>WARNING:</b> this function doesn't work when plane width is set to 128 (see #VDP_setPlaneSize(..) method)
 *
 *  \see VDP_setTileMapColumn()
 *  \see VDP_setMapRowEx()
 *  \see VDP_setTileMapDataColumnEx()
 */
bool VDP_setTileMapColumnEx(VDPPlane plane, const TileMap *tilemap, u16 basetile, u16 column, u16 x, u16 y, u16 h, TransferMethod tm);

/**
 *  \deprecated
 *      Use #VDP_setTileMap() instead.
 */
bool VDP_setMap(VDPPlane plane, const TileMap *tilemap, u16 basetile, u16 x, u16 y);
/**
 *  \deprecated
 *      Use #VDP_setTileMapEx() instead.
 */
bool VDP_setMapEx(VDPPlane plane, const TileMap *tilemap, u16 basetile, u16 x, u16 y, u16 xm, u16 ym, u16 wm, u16 hm);


#endif // _VDP_TILE_H_
````

## File: inc/vdp.h
````c
/**
 *  \file vdp.h
 *  \brief VDP main
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *  \addtogroup VDP VDP
 *  \{ 
 *
 * This unit provides general VDP (Video Display Processor) methods:<br>
 * - initialisation<br>
 * - get / set register<br>
 * - get / set resolution<br>
 * - enable / disable VDP features<br>
 * <br>
 * <b>WARNING:</b> It's very important that VRAM is organized with tile data being located before tilemaps and tables:<br>
 * 0000-XXXX = tile data<br>
 * XXXX-FFFF = tilemaps & tables (H scroll table, sprite table, B/A plane and window plane).<br>
 * <br>
 * If you don't respect that you may get in troubles as SGDK expect it ;)
 */

#ifndef _VDP_H_
#define _VDP_H_


/**
 *  \brief
 *      VDP Data port address.
 */
#define VDP_DATA_PORT           0xC00000
/**
 *  \brief
 *      VDP Control port address.
 */
#define VDP_CTRL_PORT           0xC00004
/**
 *  \brief
 *      VDP HV counter port address.
 */
#define VDP_HVCOUNTER_PORT      0xC00008

/**
 *  \deprecated
 *      Use #VDP_DATA_PORT instead
 */
#define GFX_DATA_PORT           _Pragma("GCC error \"This definition is deprecated, use VDP_DATA_PORT instead.\"")

/**
 *  \deprecated
 *      Use #VDP_CTRL_PORT instead
 */
#define GFX_CTRL_PORT           _Pragma("GCC error \"This definition is deprecated, use VDP_CTRL_PORT instead.\"")
/**
 *  \deprecated
 *      Use #VDP_HVCOUNTER_PORT instead
 */
#define GFX_HVCOUNTER_PORT      _Pragma("GCC error \"This definition is deprecated, use VDP_HVCOUNTER_PORT instead.\"")


/**
 *  \brief
 *      VDP FIFO empty flag.
 */
#define VDP_FIFOEMPTY_FLAG      (1 << 9)
/**
 *  \brief
 *      VDP FIFO full flag.
 */
#define VDP_FIFOFULL_FLAG       (1 << 8)
/**
 *  \brief
 *      VDP Vertical interrupt pending flag.
 */
#define VDP_VINTPENDING_FLAG    (1 << 7)
/**
 *  \brief
 *      VDP sprite overflow flag.
 */
#define VDP_SPROVERFLOW_FLAG    (1 << 6)
/**
 *  \brief
 *      VDP sprite collision flag.
 */
#define VDP_SPRCOLLISION_FLAG   (1 << 5)
/**
 *  \brief
 *      VDP odd frame flag.
 */
#define VDP_ODDFRAME_FLAG       (1 << 4)
/**
 *  \brief
 *      VDP Vertical blanking flag.
 */
#define VDP_VBLANK_FLAG         (1 << 3)
/**
 *  \brief
 *      VDP Horizontal blanking flag.
 */
#define VDP_HBLANK_FLAG         (1 << 2)
/**
 *  \brief
 *      VDP DMA busy flag.
 */
#define VDP_DMABUSY_FLAG        (1 << 1)
/**
 *  \brief
 *      VDP PAL mode flag.
 */
#define VDP_PALMODE_FLAG        (1 << 0)

/**
 *  \deprecated
 *      Use VDP_BG_A instead
 */
#define VDP_PLAN_A              _Pragma("GCC error \"This definition is deprecated, use VDP_BG_A instead.\"")
/**
 *  \deprecated
 *      Use VDP_BG_B instead
 */
#define VDP_PLAN_B              _Pragma("GCC error \"This definition is deprecated, use VDP_BG_B instead.\"")
/**
 *  \deprecated
 *      Use VDP_WINDOW instead
 */
#define VDP_PLAN_WINDOW         _Pragma("GCC error \"This definition is deprecated, use VDP_WINDOW instead.\"")

/**
 *  \brief
 *      VDP background A tilemap address in VRAM.
 */
#define VDP_BG_A                bga_addr
/**
 *  \brief
 *      VDP background B tilemap address in VRAM.
 */
#define VDP_BG_B                bgb_addr
/**
 *  \brief
 *      VDP window tilemap address in VRAM.
 */
#define VDP_WINDOW              window_addr
/**
 *  \brief
 *      VDP horizontal scroll table address in VRAM.
 */
#define VDP_HSCROLL_TABLE       hscrl_addr
/**
 *  \brief
 *      VDP sprite list table address in VRAM.
 */
#define VDP_SPRITE_TABLE        slist_addr
/**
 *  \brief
 *      Address in VRAM where tilemaps start (= address of first tilemap / table in VRAM).
 */
#define VDP_MAPS_START          maps_addr

/**
 *  \brief
 *      Definition to set horizontal scroll to mode plane.
 */
#define HSCROLL_PLANE           0
/**
 *  \brief
 *      Definition to set horizontal scroll to mode tile.
 */
#define HSCROLL_TILE            2
/**
 *  \brief
 *      Definition to set horizontal scroll to mode line.
 */
#define HSCROLL_LINE            3

/**
 *  \brief
 *      Definition to set vertical scroll to mode plane.
 */
#define VSCROLL_PLANE           0
/**
 *  \brief
 *      Definition to set vertical scroll to mode column (2 tiles width).
 */
#define VSCROLL_COLUMN          1
/**
 *  \deprecated
 *      Use VSCROLL_COLUMN instead
 */
#define VSCROLL_2TILE           _Pragma("GCC error \"This definition is deprecated, use VSCROLL_COLUMN instead.\"")

/**
 *  \brief
 *      Interlaced scanning mode disabled.<br>
 *      That is the default mode for the VDP.
 */
#define INTERLACED_NONE         0
/**
 *  \brief
 *      Interlaced Scanning Mode 1 - 8x8 dots per cell (normal vertical resolution)<br>
 *      In Interlaced Mode 1, the same pattern will be displayed on the adjacent lines of even and odd numbered fields.
 */
#define INTERLACED_MODE1        1
/**
 *  \brief
 *      Interlaced Scanning Mode 2 - 8x16 dots per cell (double vertical resolution)<br>
 *      In Interlaced Mode 2, different patterns can be displayed on the adjacent lines of even and odd numbered fields.
 */
#define INTERLACED_MODE2        2

/**
 *  \brief
 *      SGDK font length
 */
#define FONT_LENGTH             96
#define FONT_LEN                _Pragma("GCC error \"This definition is deprecated, use FONT_LENGTH instead.\"")

/**
 *  \brief
 *      Size of a single tile in byte.
 */
#define TILE_SIZE               32
#define TILE_INDEX_MASK         (0xFFFF / TILE_SIZE)

/**
 *  \brief
 *      Space in byte for tile in VRAM (tile space ends where tilemaps starts)
 */
#define TILE_SPACE              VDP_MAPS_START

/**
 *  \brief
 *      Maximum number of tile in VRAM (related to TILE_SPACE).
 */
#define TILE_MAX_NUM            (TILE_SPACE / TILE_SIZE)
/**
 *  \brief
 *      Maximum tile index in VRAM (related to TILE_MAXNUM).
 */
#define TILE_MAX_INDEX          (TILE_MAXNUM - 1)
/**
 *  \brief
 *      System base tile index in VRAM.
 */
#define TILE_SYSTEM_INDEX       0x0000
/**
 *  \brief
 *      Number of system tile.
 */
#define TILE_SYSTEM_LENGTH      16
/**
 *  \brief
 *      User base tile index.
 */
#if (LEGACY_FONT_LOCATION != 0)
#define TILE_USER_INDEX         (TILE_SYSTEM_INDEX + TILE_SYSTEM_LENGTH)
#else
#define TILE_USER_INDEX         (TILE_SYSTEM_INDEX + TILE_SYSTEM_LENGTH + FONT_LENGTH)
#endif
/**
 *  \brief
 *      Font base tile index.
 */
#if (LEGACY_FONT_LOCATION != 0)
#define TILE_FONT_INDEX         (TILE_MAX_NUM - FONT_LENGTH)
#else
#define TILE_FONT_INDEX         (TILE_SYSTEM_INDEX + TILE_SYSTEM_LENGTH)
#endif
/**
 *  \brief
 *      Sprite engine base tile index (should be located at the end of tileset space)
 */
#if (LEGACY_FONT_LOCATION != 0)
#define TILE_SPRITE_INDEX       (TILE_FONT_INDEX - spriteVramSize)
#else
#define TILE_SPRITE_INDEX       (TILE_MAX_NUM - spriteVramSize)
#endif
/**
 *  \brief
 *      Number of available user tile.
 */
#define TILE_USER_LENGTH        ((userTileMaxIndex - TILE_USER_INDEX) + 1)
/**
 *  \brief
 *      Maximum tile index in VRAM reserved for user (for background and user managed sprites)
 */
#define TILE_USER_MAX_INDEX     userTileMaxIndex

/**
 *  \deprecated
 *      Use TILE_MAX_NUM instead
 */
#define TILE_MAXNUM             _Pragma("GCC error \"This definition is deprecated, use TILE_MAX_NUM instead.\"")
/**
 *  \deprecated
 *      Use TILE_MAX_INDEX instead
 */
#define TILE_MAXINDEX           _Pragma("GCC error \"This definition is deprecated, use TILE_MAX_INDEX instead.\"")
/**
 *  \deprecated Use TILE_SYSTEM_LENGTH instead.
 */
#define TILE_SYSTEM_LENGHT      _Pragma("GCC error \"This definition is deprecated, use TILE_SYSTEM_LENGTH instead.\"")
/**
 *  \deprecated
 *      Use TILE_SYSTEM_LENGTH instead
 */
#define TILE_SYSTEMLENGTH       _Pragma("GCC error \"This definition is deprecated, use TILE_SYSTEM_LENGTH instead.\"")
/**
 *  \deprecated
 *      Use TILE_SYSTEM_INDEX instead
 */
#define TILE_SYSTEMINDEX        _Pragma("GCC error \"This definition is deprecated, use TILE_SYSTEM_INDEX instead.\"")
/**
 *  \deprecated
 *      Use TILE_USER_INDEX instead
 */
#define TILE_USERINDEX          _Pragma("GCC error \"This definition is deprecated, use TILE_USER_INDEX instead.\"")
/**
 *  \deprecated
 *      Use TILE_FONT_INDEX instead
 */
#define TILE_FONTINDEX          _Pragma("GCC error \"This definition is deprecated, use TILE_FONT_INDEX instead.\"")
/**
 *  \deprecated
 *      Use TILE_SPRITE_INDEX instead
 */
#define TILE_SPRITEINDEX        _Pragma("GCC error \"This definition is deprecated, use TILE_SPRITE_INDEX instead.\"")
/**
 *  \deprecated
 *      Use TILE_USER_LENGTH instead
 */
#define TILE_USERLENGTH         _Pragma("GCC error \"This definition is deprecated, use TILE_USER_LENGTH instead.\"")
/**
 *  \deprecated
 *      Use TILE_USER_MAX_INDEX instead
 */
#define TILE_USERMAXINDEX       _Pragma("GCC error \"This definition is deprecated, use TILE_USER_MAX_INDEX instead.\"")

/**
 *  \brief
 *      System tile address in VRAM.
 */
#define TILE_SYSTEM             (TILE_SYSTEM_INDEX * TILE_SIZE)
/**
 *  \brief
 *      User tile address in VRAM.
 */
#define TILE_USER               (TILE_USER_INDEX * TILE_SIZE)
/**
 *  \brief
 *      Font tile address in VRAM.
 */
#define TILE_FONT               (TILE_FONT_INDEX * TILE_SIZE)

/**
 *  \brief
 *      Palette 0
 */
#define PAL0                    0
/**
 *  \brief
 *      Palette 1
 */
#define PAL1                    1
/**
 *  \brief
 *      Palette 2
 */
#define PAL2                    2
/**
 *  \brief
 *      Palette 3
 */
#define PAL3                    3

/**
 *  \brief
 *      Set VDP command to read specified VRAM address.
 */
#define VDP_READ_VRAM_ADDR(adr)     (((0x0000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00))
/**
 *  \brief
 *      Set VDP command to read specified CRAM address.
 */
#define VDP_READ_CRAM_ADDR(adr)     (((0x0000 + ((adr) & 0x7F)) << 16) + 0x20)
/**
 *  \brief
 *      Set VDP command to read specified VSRAM address.
 */
#define VDP_READ_VSRAM_ADDR(adr)    (((0x0000 + ((adr) & 0x7F)) << 16) + 0x10)

/**
 *  \brief
 *      Set VDP command to write at specified VRAM address.
 */
#define VDP_WRITE_VRAM_ADDR(adr)    (((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00))
/**
 *  \brief
 *      Set VDP command to write at specified CRAM address.
 */
#define VDP_WRITE_CRAM_ADDR(adr)    (((0xC000 + ((adr) & 0x7F)) << 16) + 0x00)
/**
 *  \brief
 *      Set VDP command to write at specified VSRAM address.
 */
#define VDP_WRITE_VSRAM_ADDR(adr)   (((0x4000 + ((adr) & 0x7F)) << 16) + 0x10)

/**
 *  \brief
 *      Set VDP command to issue a DMA transfert to specified VRAM address.
 */
#define VDP_DMA_VRAM_ADDR(adr)      (((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x80))
/**
 *  \brief
 *      Set VDP command to issue a DMA transfert to specified CRAM address.
 */
#define VDP_DMA_CRAM_ADDR(adr)      (((0xC000 + ((adr) & 0x7F)) << 16) + 0x80)
/**
 *  \brief
 *      Set VDP command to issue a DMA transfert to specified VSRAM address.
 */
#define VDP_DMA_VSRAM_ADDR(adr)     (((0x4000 + ((adr) & 0x7F)) << 16) + 0x90)

/**
 *  \brief
 *      Set VDP command to issue a DMA VRAM copy to specified VRAM address.
 */
#define VDP_DMA_VRAMCOPY_ADDR(adr)  (((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0xC0))

/**
 *  \brief
 *      Helper to write in vertical scroll table (same as VDP_WRITE_VSRAM_ADDR).
 */
#define VDP_VERT_SCROLL(adr)        VDP_WRITE_VSRAM_ADDR(adr)
/**
 *  \brief
 *      Helper to write in horizontal scroll table (same as VDP_WRITE_VRAM_ADDR(VDP_SCROLL_H + adr)).
 */
#define VDP_HORZ_SCROLL(adr)        VDP_WRITE_VRAM_ADDR(VDP_SCROLL_H + (adr))

/**
 *  \deprecated
 *      Use #VDP_READ_VRAM_ADDR instead
 */
#define GFX_READ_VRAM_ADDR(adr)     _Pragma("GCC error \"This definition is deprecated, use VDP_READ_VRAM_ADDR instead.\""))
/**
 *  \deprecated
 *      Use #VDP_READ_CRAM_ADDR instead
 */
#define GFX_READ_CRAM_ADDR(adr)     _Pragma("GCC error \"This definition is deprecated, use VDP_READ_CRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_READ_VSRAM_ADDR instead
 */
#define GFX_READ_VSRAM_ADDR(adr)    _Pragma("GCC error \"This definition is deprecated, use VDP_READ_VSRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_WRITE_VRAM_ADDR instead
 */
#define GFX_WRITE_VRAM_ADDR(adr)    _Pragma("GCC error \"This definition is deprecated, use VDP_WRITE_VRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_WRITE_CRAM_ADDR instead
 */
#define GFX_WRITE_CRAM_ADDR(adr)    _Pragma("GCC error \"This definition is deprecated, use VDP_WRITE_CRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_WRITE_VSRAM_ADDR instead
 */
#define GFX_WRITE_VSRAM_ADDR(adr)   _Pragma("GCC error \"This definition is deprecated, use VDP_WRITE_VSRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_DMA_VRAM_ADDR instead
 */
#define GFX_DMA_VRAM_ADDR(adr)      _Pragma("GCC error \"This definition is deprecated, use VDP_DMA_VRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_DMA_CRAM_ADDR instead
 */
#define GFX_DMA_CRAM_ADDR(adr)      _Pragma("GCC error \"This definition is deprecated, use VDP_DMA_CRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_DMA_VSRAM_ADDR instead
 */
#define GFX_DMA_VSRAM_ADDR(adr)     _Pragma("GCC error \"This definition is deprecated, use VDP_DMA_VSRAM_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_DMA_VRAMCOPY_ADDR instead
 */
#define GFX_DMA_VRAMCOPY_ADDR(adr)  _Pragma("GCC error \"This definition is deprecated, use VDP_DMA_VRAMCOPY_ADDR instead.\"")
/**
 *  \deprecated
 *      Use #VDP_VERT_SCROLL instead
 */
#define GFX_VERT_SCROLL(adr)        _Pragma("GCC error \"This definition is deprecated, use VDP_VERT_SCROLL instead.\"")
/**
 *  \deprecated
 *      Use #VDP_HORZ_SCROLL instead
 */
#define GFX_HORZ_SCROLL(adr)        _Pragma("GCC error \"This definition is deprecated, use VDP_HORZ_SCROLL instead.\"")


/**
 *  \brief
 *      Tests VDP status against specified flag (see VDP_XXX_FLAG).
 */
#define GET_VDP_STATUS(flag)         ((*(vu16*)(VDP_CTRL_PORT)) & (flag))
/**
 *  \brief
 *      Tests if current system is a PAL system (50 Hz).
 */
#define IS_PAL_SYSTEM                GET_VDP_STATUS(VDP_PALMODE_FLAG)

/**
 *  \brief
 *      Returns HV counter.
 */
#define GET_HVCOUNTER               (*(vu16*)(VDP_HVCOUNTER_PORT))
/**
 *  \brief
 *      Returns Horizontal counter.
 */
#define GET_HCOUNTER                (GET_HVCOUNTER & 0xFF)
/**
 *  \brief
 *      Returns Vertical counter.
 */
#define GET_VCOUNTER                (GET_HVCOUNTER >> 8)


/**
 *  \brief
 *      Type used to define on which plane to work (used by some methods).
 */
typedef enum
{
    BG_A = 0, BG_B = 1, WINDOW = 2
} VDPPlane;


// used by define
extern u16 window_addr;
extern u16 bga_addr;
extern u16 bgb_addr;
extern u16 hscrl_addr;
extern u16 slist_addr;
extern u16 maps_addr;
extern u16 userTileMaxIndex;

/**
 *  \brief
 *      Current screen width (horizontale resolution)
 */
extern u16 screenWidth;
/**
 *  \brief
 *      Current screen height (verticale resolution)
 */
extern u16 screenHeight;
/**
 *  \brief
 *      Current background plane width (in tile)
 *
 *  Possible values are: 32, 64, 128
 */
extern u16 planeWidth;
/**
 *  \brief
 *      Current background plane height (in tile)
 *
 *  Possible values are: 32, 64, 128
 */
extern u16 planeHeight;
/**
 *  \brief
 *      Current window width (in tile)
 *
 *  Possible values are: 32, 64
 */
extern u16 windowWidth;
/**
 *  \brief
 *      Current background plane width bit shift
 *
 *  Possible values are: 5, 6 or 7 (corresponding to plane width 32, 64 and 128)
 */
extern u16 planeWidthSft;
/**
 *  \brief
 *      Current background plane height bit shift
 *
 *  Possible values are: 5, 6 or 7 (corresponding to plane height 32, 64 and 128)
 */
extern u16 planeHeightSft;
/**
 *  \brief
 *      Current window width bit shift
 *
 *  Possible values are: 5 or 6 (corresponding to window width 32 or 64)
 */
extern u16 windowWidthSft;


/**
 *  \brief
 *      Initialize the whole VDP sub system.
 *
 * Reset VDP registers, reset sprites then call #VDP_resetScreen() to reset BG and palettes.
 */
void VDP_init(void);

/**
 *  \brief
 *      Reset background planes and palettes.
 *
 *  Reset VRAM (clear BG planes and reload font), reset scrolls and reset palettes (set to default grey / red / green / blue ramps).
 */
void VDP_resetScreen(void);

/**
 *  \brief
 *      Get VDP register value.
 *
 *  \param reg
 *      Register number we want to retrieve value.
 *  \return specified register value.
 */
u8   VDP_getReg(u16 reg);
/**
 *  \brief
 *      Set VDP register value.
 *
 *  \param reg
 *      Register number we want to set value.
 *      Note that setting bit 7 of register number (reg | 0x80) allows the value to be written to the VDP
 *      without being 'cached' in the SGDK register array so VDP_getReg(reg) will return previous value.
 *  \param value
 *      value to set.
 */
void VDP_setReg(u16 reg, u8 value);

/**
 *  \brief
 *      Returns VDP enable state.
 */
bool VDP_getEnable(void);
/**
 *  \brief
 *      Returns VDP enable state.
 */
bool VDP_isEnable(void);
/**
 *  \brief
 *      Set VDP enable state.
 *
 *  You can temporary disable VDP to speed up VDP memory transfert.
 */
void VDP_setEnable(bool value);

/**
 *  \brief
 *      Returns number of total scanline.
 *
 *  312 for PAL system and 262 for NTSC system.
 */
u16  VDP_getScanlineNumber(void);
/**
 *  \brief
 *      Returns vertical screen resolution.
 *
 *  Always returns 224 on NTSC system as they only support this mode.<br>
 *  PAL system supports 240 pixels mode.
 */
u16  VDP_getScreenHeight(void);
/**
 *  \brief
 *      Set vertical resolution to 224 pixels.
 *
 *  This is the only accepted mode for NTSC system.
 */
void VDP_setScreenHeight224(void);
/**
 *  \brief
 *      Set vertical resolution to 240 pixels.
 *
 *  Only work on PAL system.
 */
void VDP_setScreenHeight240(void);
/**
 *  \brief
 *      Returns horizontal screen resolution.
 *
 *  Returns 320 or 256 depending current horizontal resolution mode.
 */
u16  VDP_getScreenWidth(void);
/**
 *  \brief
 *      Set horizontal resolution to 256 pixels.
 */
void VDP_setScreenWidth256(void);
/**
 *  \brief
 *      Set horizontal resolution to 320 pixels.
 */
void VDP_setScreenWidth320(void);

/**
 *  \brief
 *      Return background plane width (in tile).
 */
u16  VDP_getPlaneWidth(void);
/**
 *  \brief
 *      Return background plane height (in tile).
 */
u16  VDP_getPlaneHeight(void);
/**
 *  \brief
 *      Set background plane size (in tile).<br>
 *      WARNING: take attention to properly setup VRAM so tilemaps has enough space.
 *
 *  \param w
 *      width in tile.<br>
 *      Possible values are 32, 64 or 128.
 *  \param h
 *      height in tile.<br>
 *      Possible values are 32, 64 or 128.
 *  \param setupVram
 *      If set to TRUE then tilemaps and tables will be automatically remapped in VRAM depending
 *      the plane size. If you don't know what that means then it's better to keep this value to TRUE :p<br>
 *      Be careful to redraw your backgrounds, also the sprite engine may need to re-allocate its VRAM region if location changed.
 */
void VDP_setPlaneSize(u16 w, u16 h, bool setupVram);
/**
 *  \deprecated
 *      Use #VDP_setPlaneSize(..) instead.
 */
#define VDP_setPlanSize(w, h)      _Pragma("GCC error \"This definition is deprecated, use VDP_setPlaneSize(..) instead.\"")

/**
 *  \brief
 *      Returns plane horizontal scrolling mode.
 *
 *  Possible values are: HSCROLL_PLANE, HSCROLL_TILE, HSCROLL_LINE
 *
 *  \see VDP_setScrollingMode for more informations about scrolling mode.
 */
u8 VDP_getHorizontalScrollingMode(void);
/**
 *  \brief
 *      Returns plane vertical scrolling mode.
 *
 *  Possible values are: VSCROLL_PLANE, VSCROLL_2TILE
 *
 *  \see VDP_setScrollingMode for more informations about scrolling mode.
 */
u8 VDP_getVerticalScrollingMode(void);
/**
 *  \brief
 *      Set plane scrolling mode.
 *
 *  \param hscroll
 *      Horizontal scrolling mode :<br>
 *      <b>HSCROLL_PLANE</b> = Scroll offset is applied to the whole plane.<br>
 *      <b>HSCROLL_TILE</b> = Scroll offset is applied on a tile basis granularity (8 pixels bloc).<br>
 *      <b>HSCROLL_LINE</b> = Scroll offset is applied on a line basis granularity (1 pixel).<br>
 *  \param vscroll
 *      Vertical scrolling mode :<br>
 *      <b>VSCROLL_PLANE</b> = Scroll offset is applied to the whole plane.<br>
 *      <b>VSCROLL_2TILE</b> = Scroll offset is applied on 2 tiles basis granularity (16 pixels bloc).<br>
 *
 *  \see VDP_setHorizontalScroll() to set horizontal scroll offset in mode plane.<br>
 *  \see VDP_setHorizontalScrollTile() to set horizontal scroll offset(s) in mode tile.<br>
 *  \see VDP_setHorizontalScrollLine() to set horizontal scroll offset(s) in mode line.<br>
 *  \see VDP_setVerticalScroll() to set vertical scroll offset in mode plane.<br>
 *  \see VDP_setVerticalScrollTile() to set vertical scroll offset(s) in mode 2-tile.<br>
 */
void VDP_setScrollingMode(u16 hscroll, u16 vscroll);

/**
 *  \brief
 *      Returns the background color index.
 */
u8 VDP_getBackgroundColor(void);
/**
 *  \brief
 *      Set the background color index.
 */
void VDP_setBackgroundColor(u8 value);

/**
 *  \brief
 *      Returns auto increment register value.
 */
u8   VDP_getAutoInc(void);
/**
 *  \brief
 *      Set auto increment register value.
 */
void VDP_setAutoInc(u8 value);

/**
 *  \brief
 *      Returns DMA enabled state
 */
u8 VDP_getDMAEnabled(void);
/**
 *  \brief
 *      Set DMA enabled state.
 *
 *  Note that by default SGDK always enable DMA (there is no reason to disable it)
 */
void VDP_setDMAEnabled(bool value);
/**
 *  \brief
 *      Returns HV counter latching on INT2 (used for light gun)
 */
u8 VDP_getHVLatching(void);
/**
 *  \brief
 *      Set HV counter latching on INT2 (used for light gun)
 *
 *  You can ask the HV Counter to fix its value on INT2 for accurate light gun positionning.
 */
void VDP_setHVLatching(bool value);
/**
 *  \brief
 *      Enable or Disable Vertical interrupt (it's *strongly* recommanded to keep it enabled).
 *
 *  \see VDP_setHInterrupt()
 */
void VDP_setVInterrupt(bool value);
/**
 *  \brief
 *      Enable or Disable Horizontal interrupt.
 *
 *  \see VDP_setHIntCounter()
 */
void VDP_setHInterrupt(bool value);
/**
 *  \brief
 *      Enable or Disable External interrupt.
 *
 *  \see VDP_setExtIntCounter()
 */
void VDP_setExtInterrupt(bool value);
/**
 *  \brief
 *      Enable or Disable Hilight / Shadow effect.
 */
void VDP_setHilightShadow(bool value);

/**
 *  \brief
 *      Get Horizontal interrupt counter value.
 */
u8   VDP_getHIntCounter(void);
/**
 *  \brief
 *      Set Horizontal interrupt counter value.
 *
 *  When Horizontal interrupt is enabled, setting 5 here means that H int will occurs each (5+1) scanline.<br>
 *  Set value 0 to get H int at each scanline.
 */
void VDP_setHIntCounter(u8 value);

/**
 *  \brief
 *      Get VRAM address (location) of BG A tilemap.
 */
u16 VDP_getBGAAddress(void);
/**
 *  \brief
 *      Get VRAM address (location) of BG B tilemap.
 */
u16 VDP_getBGBAddress(void);
/**
 *  \deprecated
 *      Use #VDP_getBGAAddress(..) instead.
 */
#define VDP_getAPlanAddress() _Pragma("GCC error \"This definition is deprecated, use VDP_getBGAAddress() instead.\"")
/**
 *  \deprecated
 *      Use #VDP_getBGBAddress(..) instead.
 */
#define VDP_getBPlanAddress() _Pragma("GCC error \"This definition is deprecated, use VDP_getBGBAddress() instead.\"")

/**
 *  \brief
 *      Get VRAM address (location) of Window tilemap.
 */
u16 VDP_getWindowAddress(void);
/**
 *  \deprecated
 *      Use #VDP_getWindowAddress(..) instead.
 */
#define VDP_getWindowPlanAddress() _Pragma("GCC error \"This definition is deprecated, use VDP_getWindowAddress() instead.\"")
/**
 *  \brief
 *      Get VRAM address (location) of Sprite list.
 */
u16 VDP_getSpriteListAddress(void);
/**
 *  \brief
 *      Get VRAM address (location) of H SCroll table.
 */
u16 VDP_getHScrollTableAddress(void);

/**
 *  \brief
 *      Set VRAM address (location) of BG A tilemap.
 *      The address should be at multiple of $2000<br>
 *      <br>
 *      Ex:<br>
 *      VDP_setBGAAddress(0xC000)<br>
 *      Will set the BG A to at address 0xC000 in VRAM.
 */
void VDP_setBGAAddress(u16 value);
/**
 *  \brief
 *      Set VRAM address (location) of BG B tilemap.<br>
 *      The address should be at multiple of $2000<br>
 *      <br>
 *      Ex:<br>
 *      VDP_setBGBAddress(0xE000)<br>
 *      Will set the BG B tilemap at address 0xE000 in VRAM.
 */
void VDP_setBGBAddress(u16 value);
/**
 *  \deprecated
 *      Use #VDP_setBGAAddress(..) instead.
 */
#define VDP_setAPlanAddress(value)      \
_Pragma("GCC error \"This definition is deprecated, use VDP_setBGAAddress(..) instead.\"")
/**
 *  \deprecated
 *      Use #VDP_setBGBAddress(..) instead.
 */
#define VDP_setBPlanAddress(value)      \
_Pragma("GCC error \"This definition is deprecated, use VDP_setBGBAddress(..) instead.\"")
/**
 *  \brief
 *      Set VRAM address (location) of Window tilemap.<br>
 *      The address should be at multiple of $1000 in H40 and $800 in H32<br>
 *      <br>
 *      Ex:<br>
 *      VDP_setWindowAddress(0xA000)<br>
 *      Will set the Window tilemap at address 0xA000 in VRAM.
 */
void VDP_setWindowAddress(u16 value);
/**
 *  \deprecated
 *      Use #VDP_setWindowAddress(..) instead.
 */
#define VDP_setWindowPlanAddress(value)     _Pragma("GCC error \"This definition is deprecated, use VDP_setWindowAddress(..) instead.\"")
/**
 *  \brief
 *      Set VRAM address (location) of Sprite list.<br>
 *      The address should be at multiple of $400 in H40 and $200 in H32<br>
 *      <br>
 *      Ex:<br>
 *      VDP_setSpriteListAddress(0xD800)<br>
 *      Will set the Sprite list to at address 0xD800 in VRAM.
 */
void VDP_setSpriteListAddress(u16 value);
/**
 *  \brief
 *      Set VRAM address (location) of H Scroll table.<br>
 *      The address should be at multiple of $400<br>
 *      <br>
 *      Ex:<br>
 *      VDP_setHScrollTableAddress(0xD400)<br>
 *      Will set the HScroll table to at address 0xD400 in VRAM.
 */
void VDP_setHScrollTableAddress(u16 value);

/**
 *  \brief
 *      Sets the scan mode of the display.
 *
 *  \param mode
 *      Accepted values : #INTERLACED_NONE, #INTERLACED_MODE1, #INTERLACED_MODE2
 *
 * This function changes the scanning mode on the next display blanking period.<br>
 * In Interlaced Mode 1, the same pattern will be displayed on the adjacent lines of even and odd numbered fields.<br>
 * In Interlaced Mode 2, different patterns can be displayed on the adjacent lines of even and odd numbered fields.<br>
 * The number of cells on the screen stays the same regardless of which scanning mode is active.
 */
void VDP_setScanMode(u16 mode);

/**
 *  \brief
 *      Sets the window Horizontal position.
 *
 *  \param right
 *      If set to <i>FALSE</i> the window is displayed from column 0 up to column <i>pos</i>
 *      If set to <i>TRUE</i> the window is displayed from column <i>pos</i> up to last column
 *  \param pos
 *      The Horizontal position of the window in 2 tiles unit (16 pixels).
 */
void VDP_setWindowHPos(u16 right, u16 pos);
/**
 *  \brief
 *      Sets the window Vertical position.
 *
 *  \param down
 *      If set to <i>FALSE</i> the window is displayed from row 0 up to row <i>pos</i>
 *      If set to <i>TRUE</i> the window is displayed from row <i>pos</i> up to last row
 *  \param pos
 *      The Vertical position of the window in 1 tile unit (8 pixels).
 */
void VDP_setWindowVPos(u16 down, u16 pos);
/**
 *  \brief
 *      Turns off the window.
 */
void VDP_setWindowOff();
/**
 *  \brief
 *      Positions the window from the top edge of the screen by the specified number of rows (tiles).
 *
 *  \param rows
 *      The number of rows, expressed in tiles.
 */
void VDP_setWindowOnTop(u16 rows);
/**
 *  \brief
 *      Positions the window from the bottom edge of the screen by the specified number of rows (tiles).
 *
 *  \param rows
 *      The number of rows, expressed in tiles.
 */
void VDP_setWindowOnBottom(u16 rows);
/**
 *  \brief
 *      Positions the window from the left edge of the screen by the specified number of columns, each 2 tiles wide (16 pixels).
 *
 *  \param cols
 *      The number of columns, expressed in double tiles.
 */
void VDP_setWindowOnLeft(u16 cols);
/**
 *  \brief
 *      Positions the window from the right edge of the screen by the specified number of columns, each 2 tiles wide (16 pixels).
 *
 *  \param cols
 *      The number of columns, expressed in double tiles.
 */
void VDP_setWindowOnRight(u16 cols);
/**
 *  \brief
 *      Positions the window to full screen.
 */
void VDP_setWindowFullScreen();

/**
 *  \brief
 *      Wait for DMA operation to complete - same as #DMA_waitCompletion()
 */
void VDP_waitDMACompletion(void);
/**
 *  \brief
 *      Wait for VDP FIFO to be empty.
 */
void VDP_waitFIFOEmpty(void);

/**
 *  \brief
 *      Wait for next Vertical Interruption.
 *  \return
 *      TRUE if a frame miss was detected (more than 1 frame elapsed since last call)
 *
 *  The method actually wait for the start of Vertical Interruption.
 *  It returns immediately if we are already in V-Int handler.
 */
bool VDP_waitVInt(void);
/**
 *  \brief
 *      Wait for next vertical blank period (same as #VDP_waitVSync())
 *  \return
 *      TRUE if a frame miss was detected (more than 1 frame elapsed since last call)
 *
 *  \param forceNext
 *      Force waiting for next start of VBlank if we are already in VBlank period when calling the method.
 *
 *  The method wait until we are in Vertical blanking area/period.
 */
bool VDP_waitVBlank(bool forceNext);
/**
 *  \brief
 *      Wait for Vertical Synchro.
 *  \return
 *      TRUE if a frame miss was detected (more than 1 frame elapsed since last call)
 *
 *  The method actually wait for the *next* start of Vertical blanking.
 */
bool VDP_waitVSync(void);
/**
 *  \brief
 *      Wait for next vertical active area (end of vertical blank period)
 *
 *  \param forceNext
 *      Force waiting for next start of V-Active if we are already in V-Active period when calling the method.
 *
 *  The method wait until we are in Vertical active area/period.
 */
void VDP_waitVActive(bool forceNext);

/**
 *  \brief
 *      Return an enhanced V Counter representation.
 *
 *  Using direct V counter from VDP may give troubles as the VDP V-Counter rollback during V-Blank period.<br>
 *  This function aim to make ease the use of V-Counter by adjusting it to a [0-255] range where 0 is the start of VBlank area and 255 the end of active display area.
 */
u16 VDP_getAdjustedVCounter(void);

/**
 *  \brief
 *      Display number of Frame Per Second.
 *
 *  \param asFloat
 *      Display in float number format.
 *  \param x
 *      X coordinate (in tile).
 *  \param y
 *      y coordinate (in tile).
 *
 * This function actually display the number of time it was called in the last second.<br>
 * i.e: for benchmarking you should call this method only once per frame update.
 *
 * \see #SYS_getFPS(..)
 */
void VDP_showFPS(u16 asFloat, u16 x, u16 y);
/**
 *  \brief
 *      Display the estimated CPU load (in %).
 * 
*  \param x
 *      X coordinate (in tile).
 *  \param y
 *      y coordinate (in tile).
 *
 * This function actually display an estimation of the CPU load (in %) for the last frame.
 *
 * \see #SYS_getCPULoad()
 */
void VDP_showCPULoad(u16 x, u16 y);

#endif // _VDP_H_

/** \} */
````

## File: inc/vram.h
````c
/**
 *  \file vram.h
 *  \brief SGDK VRAM (Video Memory) management unit
 *  \author Stephane Dallongeville
 *  \date 11/2015
 *
 * Video Memory management unit.<br>
 * It offerts methods to manage dynamic VRAM allocation for tile data.<br>
 * Tile data should always be located before tilemap in VRAM (0000-XXXX = tile data, XXXX-FFFF = tilemaps).<br>
 *<br>
 *<pre>
 * VRAMRegion structure define a VRAM region where we want to use dynamic allocation.
 * 'vram' field is a buffer representing the VRAM region usage. For each entry:
 *  b14-b0 = size of the bloc (in tile)
 *  b15    = 1:used, 0:free
 *
 *  address           value
 *
 *                  +-------------------+
 *  free = 0        | cacheSize  (free) |
 *                  |                   |
 *                  |                   |
 *  cacheSize - 1   |                   |
 *                  +-------------------+
 *  cacheSize       | 0                 |
 *                  +-------------------+
 *
 *
 *  1. Before allocation (with cacheSize = 1000)
 *
 *                  +-------------------+
 *  free = 0        | 1000       (free) |
 *                  |                   |
 *                  |                   |
 *  999             |                   |
 *                  +-------------------+
 *  1000            | 0                 |
 *                  +-------------------+
 *
 *  cache = ???
 *  free = cache            *free = cacheSize
 *  end = cache+cacheSize   *end = 0
 *
 *
 *  2. After allocation of 32 tiles
 *
 *                  +------------------------+
 *  0               | 32              (used) |
 *  free = 32       | 968             (free) |
 *                  |                        |
 *                  |                        |
 *  999             |                        |
 *                  +------------------------+
 *  1000            | 0                      |
 *                  +------------------------+
 *
 *  cache = ???
 *  free = cache + 32       *free = cacheSize - 32
 *
 *
 *  3. After allocation of 128 tiles
 *
 *                  +------------------------+
 *  0               | 32              (used) |
 *  32              | 128             (used) |
 *  free = 32+128   | 840             (free) |
 *                  |                        |
 *                  |                        |
 *  999             |                        |
 *                  +------------------------+
 *  1000            | 0                      |
 *                  +------------------------+
 *
 *
 *  4. After allocation of 64, 500, 100 tiles
 *
 *                  +------------------------+
 *  0               | 32              (used) |
 *  32              | 128             (used) |
 *  160             | 64              (used) |
 *  224             | 500             (used) |
 *  724             | 100             (used) |
 *  free = 824      | 176             (free) |
 *                  |                        |
 *                  |                        |
 *  999             |                        |
 *                  +------------------------+
 *  1000            | 0                      |
 *                  +------------------------+
 *
 *
 *  5. After release of allocation #3 (64 tiles)
 *
 *                  +------------------------+
 *  0               | 32              (used) |
 *  32              | 128             (used) |
 *  160             | 64              (free) |
 *  224             | 500             (used) |
 *  724             | 100             (used) |
 *  free = 824      | 176             (free) |
 *                  |                        |
 *                  |                        |
 *  999             |                        |
 *                  +------------------------+
 *  1000            | 0                      |
 *                  +------------------------+
 *
 *
 *  6. After release of allocation #4 (500 tiles)
 *
 *                  +------------------------+
 *  0               | 32              (used) |
 *  32              | 128             (used) |
 *  160             | 64              (free) |
 *  224             | 500             (free) |
 *  724             | 100             (used) |
 *  free = 824      | 176             (free) |
 *                  |                        |
 *                  |                        |
 *  999             |                        |
 *                  +------------------------+
 *  1000            | 0                      |
 *                  +------------------------+
 *
 *
 *  7. After allocation of 400 tiles
 *
 *                  +------------------------+
 *  0               | 32              (used) |
 *  32              | 128             (used) |
 *  160             | 400             (used) |
 *  560             | 164             (free) |
 *  724             | 100             (used) |
 *  free = 824      | 176             (free) |
 *                  |                        |
 *                  |                        |
 *  999             |                        |
 *                  +------------------------+
 *  1000            | 0                      |
 *                  +------------------------+
 *</pre>
 */

#ifndef _VRAM_H_
#define _VRAM_H_


/**
 *  \brief
 *      VRAM region structure.
 *
 *  \param startIndex
 *      start position in tile for the VRAM region
 *  \param endIndex
 *      end position in tile for the VRAM region
 *  \param free
 *      position of next free area
 *  \param vram
 *      allocation buffer
 *
 * Define cache information for a VRAM region dedicated to tile storage.
 */
typedef struct
{
    u16 startIndex;
    u16 endIndex;
    u16 *free;
    u16 *vram;
} VRAMRegion;


/**
 *  \brief
 *      Initialize a new VRAM region structure.
 *
 *  \param region
 *      Region to initialize.
 *  \param startIndex
 *      Tile start index in VRAM.
 *  \param size
 *      Size in tile of the region.
 *
 * Set parameters and allocate memory for the VRAM region structure.
 *
 * \see VRAM_releaseRegion(..)
 *
 */
void VRAM_createRegion(VRAMRegion *region, u16 startIndex, u16 size);
/**
 *  \brief
 *      Release the VRAM region structure.
 *
 *  \param region
 *      VRAMRegion we want to release.
 *
 * Release memory used by the VRAM region structure.
 *
 * \see VRAM_createRegion(..)
 */
void VRAM_releaseRegion(VRAMRegion *region);
/**
 *  \brief
 *      Release all allocations from specified VRAM region.
 *
 *  \param region
 *      VRAM region we want to clear.
 */
void VRAM_clearRegion(VRAMRegion *region);

/**
 *  \brief
 *      Return the number of free tile remaining in the specified VRAM region.
 *
 *  \param region
 *      VRAM region
 *  \return
 *      the number of free tile in the specified VRAM region
 */
u16 VRAM_getFree(VRAMRegion *region);
/**
 *  \brief
 *      Return the number of allocated tile in the specified VRAM region.
 *
 *  \param region
 *      VRAM region
 *  \return
 *      the number of allocated tile in the specified VRAM region.
 */
u16 VRAM_getAllocated(VRAMRegion *region);
/**
 *  \brief
 *      Return the largest free block size (in tile) in the specified VRAM region.
 *
 *  \param region
 *      VRAM region
 *  \return
 *      the largest free block size (in tile) in the specified VRAM region.
 */
u16 VRAM_getLargestFreeBlock(VRAMRegion *region);

/**
 *  \brief
 *      Try to allocate the specified number of tile in the given VRAM region and return its index.
 *
 *  \param region
 *      VRAM region
 *  \param size
 *      Number of tile we want to allocate in VRAM (need to be > 0).
 *  \return
 *      the index in VRAM where we allocated the bloc of tile.<br>
 *      -1 if there is no enough available VRAM in the region.
 *
 *  \see VRAM_free(..)
 */
s16 VRAM_alloc(VRAMRegion *region, u16 size);
/**
 *  \brief
 *      Release the previously allocated VRAM block at specified index in the given VRAM region.<br>
 *
 *  \param region
 *      VRAM region
 *  \param index
 *      The index of the VRAM block we want to release
 *
 *  \see VRAM_alloc(..)
 */
void VRAM_free(VRAMRegion *region, u16 index);


#endif // _VRAM_H_
````

## File: inc/ym2612.h
````c
/**
 *  \file ym2612.h
 *  \brief YM2612 support
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides access to the YM2612 through the 68000 CPU.
 */

#ifndef _YM2612_H_
#define _YM2612_H_

/**
 *  \brief
 *      YM2612 base port address.
 */
#define YM2612_BASEPORT     0xA04000


/**
 *  \brief
 *      Reset YM2612 chip
 */
void YM2612_reset(void);

/**
 *  \brief
 *      Read YM2612 port.
 *
 *  \param port
 *      Port number (0-3)
 *  \return YM2612 port value.
 *
 *  Reading YM2612 always return YM2612 status (busy and timer flag) whatever is the port read.
 */
u8   YM2612_read(const u16 port);
/**
 *  \brief
 *      Return YM2612 status (busy and timer flag).
 *
 *  \return YM2612 status.
 */
u8   YM2612_readStatus();
/**
 *  \brief
 *      Write YM2612 port.
 *
 *  \param port
 *      Port number (0-3)
 *  \param data
 *      Data to write
 */
void YM2612_write(const u16 port, const u8 data);
/**
 *  \deprecated Use YM2612_write(..) method instead.
 */
#define YM2612_writeSafe(port, data)        _Pragma("GCC error \"This definition is deprecated, use YM2612_write(..) instead.\"")
/**
 *  \brief
 *      Set YM2612 register value.
 *
 *  \param part
 *      part number (0-1)
 *  \param reg
 *      register number
 *  \param data
 *      register value
 */
void YM2612_writeReg(const u16 part, const u8 reg, const u8 data);
/**
 *  \deprecated Use YM2612_writeReg(..) method instead.
 */
#define YM2612_writeRegSafe(part, reg, data)        _Pragma("GCC error \"This definition is deprecated, use YM2612_writeReg(..) instead.\"")

/**
 *  \brief
 *      Enable YM2612 DAC.
 */
void YM2612_enableDAC(void);
/**
 *  \brief
 *      Disable YM2612 DAC.
 */
void YM2612_disableDAC(void);


#endif // _YM2612_H_
````

## File: inc/z80_ctrl.h
````c
/**
 *  \file z80_ctrl.h
 *  \brief Z80 control
 *  \author Stephane Dallongeville
 *  \date 08/2011
 *
 * This unit provides Z80 access from the YM2612:<br>
 * - enable / disable Z80<br>
 * - request / release Z80 BUS<br>
 * - upload / download data to / from Z80 memory<br>
 * - set Z80 external Bank<br>
 * - Z80 driver handling<br>
 */

#ifndef _Z80_CTRL_H_
#define _Z80_CTRL_H_


#define Z80_HALT_PORT                   0xA11100
#define Z80_RESET_PORT                  0xA11200

/**
 *  \brief
 *
 * Z80 RAM start address.
 */
#define Z80_RAM_START                   0xA00000
/**
 *  \brief
 *
 * Z80 RAM end address.
 */
#define Z80_RAM_END                     0xA01FFF
/**
 *  \brief
 *
 * Z80 RAM address.
 */
#define Z80_RAM                         Z80_RAM_START
/**
 *  \brief
 *
 * Z80 RAM length in byte.
 */
#define Z80_RAM_LEN                     ((Z80_RAM_END - Z80_RAM_START) + 1)
/**
 *  \brief
 *
 * Z80 YM2612 port address.
 */
#define Z80_YM2612                      0xA04000
/**
 *  \brief
 *
 * Z80 Bank register address.
 */
#define Z80_BANK_REGISTER               0xA06000

/**
 *  \brief
 *
 * Z80 default driver command address.
 */
#define Z80_DRV_COMMAND                 0xA00100
/**
 *  \brief
 *
 * Z80 default driver status address.
 */
#define Z80_DRV_STATUS                  0xA00102
/**
 *  \brief
 *
 * Z80 default driver parameters address.
 */
#define Z80_DRV_PARAMS                  0xA00104

// default command and status value
#define Z80_DRV_COM_PLAY_SFT            0
#define Z80_DRV_COM_STOP_SFT            4
#define Z80_DRV_STAT_PLAYING_SFT        0
#define Z80_DRV_STAT_READY_SFT          7

/**
 *  \brief
 *      Z80 default driver play command.
 */
#define Z80_DRV_COM_PLAY                (1 << Z80_DRV_COM_PLAY_SFT)
/**
 *  \brief
 *      Z80 default driver stop command.
 */
#define Z80_DRV_COM_STOP                (1 << Z80_DRV_COM_STOP_SFT)
/**
 *  \brief
 *      Z80 default driver play status.
 */
#define Z80_DRV_STAT_PLAYING            (1 << Z80_DRV_STAT_PLAYING_SFT)
/**
 *  \brief
 *      Z80 default driver ready status.
 */
#define Z80_DRV_STAT_READY              (1 << Z80_DRV_STAT_READY_SFT)


#define Z80_DRV_CH0_SFT                 _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH1 instead.\"")
#define Z80_DRV_CH1_SFT                 _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH2 instead.\"")
#define Z80_DRV_CH2_SFT                 _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH3 instead.\"")
#define Z80_DRV_CH3_SFT                 _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH4 instead.\"")

#define Z80_DRV_CH0                     _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH1_MSK instead.\"")
#define Z80_DRV_CH1                     _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH2_MSK instead.\"")
#define Z80_DRV_CH2                     _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH3_MSK instead.\"")
#define Z80_DRV_CH3                     _Pragma("GCC error \"This method is deprecated, use SOUND_PCM_CH4_MSK instead.\"")


/**
 *  \brief
 *      NULL Z80 driver.
 */
#define Z80_DRIVER_NULL                 0
/**
 *  \brief
 *      Variable rate sample player Z80 driver.<br>
 *      It can play a sample (8 bit signed) from 8 Khz up to 32 Khz rate.
 */
#define Z80_DRIVER_PCM                  1
/**
 *  \brief
 *      2 channels PCM sample player Z80 driver.<br>
 *      It can mix 2 samples (4 bit PCM) at a fixed 22 Khz rate.
 */
#define Z80_DRIVER_DPCM2                2
/**
 *  \brief
 *      4 channels sample player Z80 driver with envelop control.<br>
 *      It can mix 4 samples (8 bit signed) at a fixed 16 Khz rate<br>
 *      and handle volume (16 levels) for each channel.
 */
#define Z80_DRIVER_PCM4                 3
/**
 *  \brief
 *      eXtended VGM music player driver.<br>
 *      This driver takes VGM (or XGM) file as input to play music.<br>
 *      It supports 4 PCM channels at a fixed 14 Khz and allows to play SFX through PCM with 16 level of priority.<br>
 *      The driver is designed to avoid DMA contention when possible (depending CPU load).
 */
#define Z80_DRIVER_XGM                  4
/**
 *  \brief
 *      eXtended VGM music player driver version 2.<br>
 *      This driver takes VGM (or XGM2) file as input to play music.<br>
 *      It supports 3 PCM channels at either 13.3 Khz or 6.65 Khz and envelop control for both FM and PSG.<br>
 *      It allows to play SFX through PCM with 16 level of priority.<br>
 *      The driver supports renforced protection against DMA contention.
 */
#define Z80_DRIVER_XGM2                 5
/**
 *  \brief
 *      CUSTOM Z80 driver.
 */
#define Z80_DRIVER_CUSTOM               -1


#define Z80_DRIVER_4PCM_ENV             _Pragma("GCC error \"This definition is deprecated, use Z80_DRIVER_PCM4 instead.\"")
#define Z80_DRIVER_2ADPCM               _Pragma("GCC error \"This definition is deprecated, use Z80_DRIVER_DPCM2 instead.\"")


/**
 *  \brief
 *      Initialize Z80 sub system.
 *
 *  Request Z80 BUS and reset bank number.
 */
void Z80_init(void);

/**
 *  \brief
 *      Return Z80 BUS taken state.
 */
bool Z80_isBusTaken(void);
/**
 *  \brief
 *      Request Z80 BUS.
 *  \param wait
 *      Wait for BUS request operation to complete.
 */
void Z80_requestBus(bool wait);
/**
 *  \brief
 *      Request Z80 BUS if not yet taken.
 *  \param wait
 *      Wait for BUS request operation to complete.
 *  \return
 *      Z80 BUS taken state before calling the function.
 */
bool Z80_getAndRequestBus(bool wait);

/**
 *  \brief
 *      Release Z80 BUS.
 */
void Z80_releaseBus(void);

/**
 *  \brief
 *      Start Z80 reset.
 */
void Z80_startReset(void);
/**
 *  \brief
 *      End Z80 reset.
 */
void Z80_endReset(void);

/**
 *  \brief
 *      Set Z80 memory bank.
 *  \param bank
 *      Bank number to set (0x000-0x1FF)
 */
void Z80_setBank(const u16 bank);

/**
 *  \brief
 *      Read Z80 memory (Z80_RAM).
 *  \param addr
 *      Address to read (relative to start of Z80_RAM).
 *
 *  You need to request Z80 BUS before accessing Z80 memory.
 *
 *  \see Z80_requestBus(bool)
 */
u8 Z80_read(const u16 addr);
/**
 *  \brief
 *      Write to Z80 memory (Z80_RAM).
 *  \param addr
 *      Address to write (relative to start of Z80_RAM).
 *  \param value
 *      Value to write.
 *
 *  You need to request Z80 BUS before accessing Z80 memory.
 *
 *  \see Z80_requestBus(bool)
 */
void Z80_write(const u16 addr, const u8 value);

/**
 *  \brief
 *      Clear Z80 memory.
 *
 *  You need to request Z80 BUS before accessing Z80 memory.
 *
 *  \see Z80_requestBus(bool)
 */
void Z80_clear(void);
/**
 *  \brief
 *      Upload data in Z80 memory.
 *  \param dest
 *      Destination address (Z80 memory).
 *  \param data
 *      Data to upload.
 *  \param size
 *      Size in byte of data to upload.
 */
void Z80_upload(const u16 dest, const u8 *data, const u16 size);
/**
 *  \brief
 *      Read data from Z80 memory.
 *
 *  \param from
 *      Source address (Z80 memory).
 *  \param dest
 *      Destination where to write data.
 *  \param size
 *      Size in byte of data to read.
 */
void Z80_download(const u16 from, u8 *dest, const u16 size);

/**
 *  \brief
 *      Return currently loaded Z80 driver.
 *
 *  Possible returned values are:<br>
 *  - #Z80_DRIVER_NULL<br>
 *  - #Z80_DRIVER_PCM<br>
 *  - #Z80_DRIVER_DPCM2<br>
 *  - #Z80_DRIVER_PCM4<br>
 *  - #Z80_DRIVER_XGM<br>
 *  - #Z80_DRIVER_XGM2<br>
 *  - #Z80_DRIVER_CUSTOM<br>
 */
s16  Z80_getLoadedDriver(void);
/**
 *  \brief
 *      Unload Z80 driver (set the NULL driver).
 */
void Z80_unloadDriver(void);

/**
 *  \deprecated Use the dedicated loadDriver(..) method (as XGM_loadDriver(..) for instance)
 */
#define Z80_loadDriver(driver, waitReady)   _Pragma("GCC error \"This method is deprecated, directly use the dedicated driver 'loadDriver' method (as XGM_loadDriver(..) for instance).\"")

/**
 *  \brief
 *      Load a custom Z80 driver.
 *
 *  \param drv
 *      Pointer to the driver binary to load.
 *  \param size
 *      Size (in bytes) of the driver binary.
 */
void Z80_loadCustomDriver(const u8 *drv, u16 size);

/**
 *  \brief
 *      Return driver ready state (only for non custom driver).
 */
bool Z80_isDriverReady(void);

/**
 *  \brief
 *      Get the Z80 task 'Vertical interrupt' callback method.
 *
 *  \return the pointer of the method called on Vertical interrupt period or NULL if no method are set
 *
 * \see Z80_setVIntCallback(VoidCallback *CB);
 */
VoidCallback* Z80_getVIntCallback(void);
/**
 *  \brief
 *      Set the Z80 task 'Vertical interrupt' callback method.
 *
 *  \param CB
 *      Pointer to the method to call on Vertical interrupt period.<br>
 *      You can remove current callback by passing a <i>NULL</i> pointer here.
 *
 * Vertical interrupt happen at the end of display period at the start of the vertical blank period.<br>
 * The only things that SGDK always handle from the vint callback is sound driver task as music tempo or Bitmap engine phase reset.<br>
 * It's recommended to keep your code as fast as possible as it will eat precious VBlank time, nor you should touch the VDP from your Vint callback
 * otherwise you will need to protect any VDP accesses from your main loop (which is painful).
 *
 * \see SYS_setVIntCallback(VoidCallback* CB);
 */
void Z80_setVIntCallback(VoidCallback* CB);

/**
 *  \brief
 *      Enable/disable 68K bus access protection from Z80 (can be used by any sound driver).
 *
 *  \param signalAddress
 *      Z80 RAM address used (relative to the start of Z80 RAM) to set the BUS protection signal.<br>
 *      Signal is set to 1 when main BUS should not be accesssed from Z80 (DMA operation in progess), set to 0 otherwise.
 */
void Z80_useBusProtection(u16 signalAddress);

/**
 *  \brief
 *      Set temporary 68K BUS protection from Z80 (for sound driver supporting it).<br>
 *      You should protect BUS Access during DMA and restore it after. Ex:<br>
 *      Z80_setBusProtection(TRUE);
 *      DMA_doDma(VRAM, data, 0x1000, 0x100, 2);
 *      Z80_setBusProtection(FALSE);
 *
 *      This way the sound driver will *try* to avoid accessing the 68K BUS during DMA to
 *      avoid execution interruption and so preserve PCM playback quality.<br>
 *      Note that depending the sound driver, the success of the operation is not 100% garantee and can fails in some conditions
 *      (heavy Z80 load, lot of PSG data in XGM music).<br>
 *      In that case you can also try to use the #Z80_setForceDelayDMA() method to help improving the PCM playblack.
 *
 *  \see Z80_useBusProtection(..)
 *  \see Z80_enableBusProtection(..)
 *  \see Z80_disableBusProtection(..)
 */
void Z80_setBusProtection(bool value);
/**
 *  \brief
 *      Enable temporary 68K BUS protection from Z80 (for sound driver supporting it). See #Z80_setBusProtection(..) for more info.
 *
 *  \see Z80_setBusProtection(..)
 *  \see Z80_enableBusProtection(..)
 *  \see Z80_disableBusProtection(..)
 */
void Z80_enableBusProtection();
/**
 *  \brief
 *      Disable temporary 68K BUS protection from Z80 (for sound driver supporting it). See #Z80_setBusProtection(..) for more info.
 *
 *  \see Z80_setBusProtection(..)
 *  \see Z80_enableBusProtection(..)
 *  \see Z80_disableBusProtection(..)
 */
void Z80_disableBusProtection();

/**
 *  \brief
 *      Returns #TRUE if DMA delay is enabled to improve PCM playback.
 *
 *  \see Z80_setForceDelayDMA(bool)
 */
bool Z80_getForceDelayDMA(void);
/**
 *  \brief
 *      This method can be used to improve the PCM playback during music play and while DMA queue is used.<br>
 *      Even using the BUS protection with #Z80_setBusProtection you may still experience altered PCM playback.
 *      With the XGM driver for instance this happens when music contains PSG data (Z80 requires the main BUS to access PSG).<br>
 *      By delaying a bit the DMA execution from the DMA queue we let the Z80 to access main bus for a bit of time thus avoiding any stall.
 *      The delay is about 3 scanlines so using the force delay DMA will reduce the DMA bandwidth for about 3 vblank lines.
 *
 *  \param value TRUE or FALSE
 *
 *  \see Z80_getForceDelayDMA()
 *  \see Z80_setBusProtection()
 */
void Z80_setForceDelayDMA(bool value);

#endif // _Z80_CTRL_H_
````

## File: changelog.txt
````
SGDK 2.11 (April 2025)
----------------------

LIBRARY
* fixed incorrect naming of maths methods leading to errors when using F16_computePosition(..) and F16_computePositionEx(..)
* renamed V2D_F16_movePoint(..) / V2D_F16_movePointEx(..) --> V2D_F16_computePosition(..) / V2D_F16_computePositionEx(..)


SGDK 2.10 (April 2025)
----------------------

COMPILER
* updated GCC to version 13.2 (sometime throw error messages about cached files which can be ignored)
* removed Docker files (see notes about Docker in readme.md file)
* removed vstudio template
* removed 'sjasm' and 'musicg' git submodules (version fixed in SGDK)
* added VSCode template
* automated doxygen documentation generation with GitHub Action (doxygen is now online, see readme.md for more info)
* XGM2TOOL
 - fixed max frame limit
 - fixed special mode / frequencies preservation
 - fixed keys op packing
 - fixed a bug with loop point
 - fixed a bug where START/STOP PCM commands could be swapped
 - fixed a bug in initial PSG state 
 - integrated 'musicg' library sources in XGM2Tool
* XGMTOOL
 - added support to recognize commands from RF5C68 and RF5C164 chips (Sega-CD PCM chip)
* RESCOMP
 - added special marker to enable optimization for specific single VDP sprite
 - added array format support for the animation 'time' field for SPRITE resource (allow defining time on a frame basis)
 - added new 'ordering' option for TILESET, TILEMAP and MAP resources
 - added new 'export' option for TILESET resource
 - added new 'sortby' option for OBJECTS resource (thanks Iratahack)
 - fixed Object boolean fields export
 - improved (again) sprite cutting process
 - avoid re-processing sprite cutting process on duplicated frames
 - now also export binary array size (label form)
 - added support for ProMotion NG JASC .pal file
 - fixed cutting PNG export
 - keep class loader open when loading rescomp_ext.jar extensions to allow correct import of dependencies
* MAKEFILE
 - re-introduced deps + added '-ffunction-sections' and '-fdata-sections' flags
 - fixed default targets
 - cleanup and small fixes
* README
 - updated notes about Doxygen documentation, Docker and VSCode
 - fixed broken links
 - added AWS tutorial link

LIBRARY
* SPRITE
 - added SPR_setAutoAnimation(..)/SPR_getAutoAnimation(..) to enable/disable auto animation.
 - optimized handling of simple sprite (sprite using only a single hardware sprite which fit frame size)
 - renamed SPR_getAnimationDone() --> SPR_isAnimationDone() and changed a bit the logic behind it
 - SPR_loadAllFrame(..) now detects duplicated tilesets and only load them once in VRAM (make the method a bit slower though)
 - fixed auto loop
 - updated legacy sprite engine to include the new features of the new sprite engine
 - minors tweaks
* DMA
 - fixed transfer size of 0 (do not transfer anything instead of transferring 65536 words) 
* MAP
 - added MapDataPatchCallback(..) allowing easier map data patching / dynamic map
 - fixed a bug in MAP_getMetaTilemapRect(..) when crossing block in X dimension
 - added MAP_overridePlaneSize(..) methods to allow overriding plane size internally
 - added 'COLUMN_AHEAD' and 'ROW_AHEAD' definitions so we can easily customize them if needed
* SOUND
 - fixed a small bug with half rate PCM play (XGM2)
 - fixed DAC state on start play music (XGM2)
 - adjust music tempo depending PAL or NTSC track info (XGM2)
 - many bug fixes in XGM2 sound driver
 - fix YM2612 reset
* PAL
 - simplified / improved a bit palette fading process to avoid palette transfer outside vblank
 - added PAL_isManualFadeDone() method (let you know if manual fading process is complete)
 - fixed color overflow in PAL_doFadeStep(..)
 - fixed synchronous palette fading (last palette update was sometime missing)
 - protect from interrupts on palette read operation (often cause issues with raster effects)
* VDP
 - added user friendly setWindowXXX(...), VDP_setWindowOff() and VDP_setWindowFullScreen() methods
 - added coordinates parameters for VDP_showFPS(), VDP_showCPULoad() and BMP_showFPS() methods
 - added VDP_drawTextFill(..) function
* SYS
 - new error handler with backtrace and symbol support (thanks to Vladikcomper for it) !
 - tried to mitigate as much as possible HALT_Z80_ON_IO effect on PCM playback (shortened Z80 interruptions time)
 - SYS_die(..) function supports an extra message parameter
 - halt Z80 on SYS_reset() / SYS_hardReset()
* MATH - WARNING: major revamping (thanks to Daniel Moura the initial PR)
 - all fix16, fix32, fastfix16, fastfix32 functions has been renamed and are now prefixed with F16_, F32_ , FF16_, FF32_
 - M3D_setRotation(..) is now taking rotation angles *in degree*
 - added new F16_tan(..), F16_atan(..) and F16_atan2(..) trigonometric functions
 - added F16_getAngle(..) function to find angle between 2 points
 - added F16_computePosition(..) to compute point position from 'origin', 'angle' and 'distance'
 - added conversion functions between fastfix16, fastfix32, fix16 and fix32 types (thanks werton)
 - added typedef structures: Vect2D_ff16, Vect2D_ff32, Mat2D_ff16, Mat2D_ff32, V2ff16, V2ff32, M2ff16 and M2ff32 (thanks werton)
* MODULE / ENTENSION
 - added support for MegaWifi everdrive x7
 - fixed a bug with flash-save
* optimized a bit LZ4W decompressor (thanks Malachi)
* vsprintf(..) function is now visible
* added string conversion functions for ff16 and ff32 types
* added RAM_SECT definition (used to force a function to be located in RAM)
* smalls tweaks, optimizations and cleanup

SAMPLE
* added MAP patching example (press button Y to enable / disable it) in 'sonic' sample
* added sprite tiles sharing and tile animation examples in 'sample/advanced' folder (thanks werton)
* added sprite masking example in 'sample/fx' folder (thanks werton)
* updated MegaWifi samples
* updated samples to last SGDK

MIGRATION NOTE
* SPR_getAnimationDone() --> SPR_isAnimationDone() (the logic changed quite a bit, don't forget to read the documentation)
* All fix16, fix32, fastfix16, fastfix32 functions has been renamed and are now prefixed with F16_, F32_ , FF16_, FF32_
 - intToFix16(x) is simply replaced by FIX16(x) or F16(x)
 - fix16ToInt(x) --> F16_toInt(..)
 - fix16Rount(x) --> F16_round(..)
 - fix16ToFix32(x) --> F16_toFix32(..)
 - fix32ToFix16(x) --> F32_toFix16(..)
 - ...
* M3D_setRotation(..) is now taking rotation angles *in degree* (fix16 format)
 
 
SGDK 2.00 (January 2024)
------------------------

COMPILER
* added new xgm2tool
* RESCOMP
 - added new XGM2 resource type
 - MAP: fixed TMX high priority layer definition from "<layer name> high" (thanks cidoku)
 - TILESET: fixed a small bug with empty tileset
 - SPRITE: small improvement in sprite cutting processing
 - changed sound drivers name
 - improved clarity of console output (thanks danielt3)
* MAKEFILE
 - improved support for newer Linux based GCC (thanks Craig Hackney)
 - support for deeper file structure
* README
 - added alternates sound drivers references

LIBRARY
* SPRITE
 - sprites are now initialized with an intermediate depth level (leave more freedom for afterward depth adjustment)
 - added SPR_setAnimationLoop(..) method to enable/disable animation loop
 - added SPR_setAlwaysAtBottom(..) method because why not ^^
* OBJECT
 - optimized object release operation when maintaining coherency
* MATH
 - refactoring / improvements on fix16 / fix32 operations
 - added fastfix16 (ff16) and fastfix32 (ff32) types
* MEM
 - fixed MEM_allocAt(..) function
* VDP
 - fixed VDP VSRAM read / write / DMA command
 - text clear operation is now done by using the 'space' char instead of tile #0
 - added VDP_setVInterrupt(bool) method to enable/disable V-Interrupt
 - small fix on RGB24_TO_VDPCOLOR(..) macro (thanks Werton)
* PAL
 - PAL_getColor(..) / PAL_getPalette(..) / PAL_getColors(..) return forced RGB333 value (undefined bits are set to 0)
* SYS
 - removed z80 halt on reset (already done in sega.s boot)
* SOUND
 - major refactor in sound drivers structure
 - added new XGM2 sound driver - beta (XGM2_xxx methods)
 - added 'minimusic' sound driver from Sik
 - removed fractal driver support (driver is discontinued) and enable conso
* TYPE
 - fixed bool declaration to be compliant with C23
 - added size_t and ptrdiff_t definitions
* 'console' module is now enabled by default
* added new SGDK logo from Adrian Spoz (thanks for making it !)
* smalls fixes, general cleanup, some refactoring..

SAMPLE
* moved 'sonic' and 'platformer' samples into 'sample/game' folder
* moved 'sound' sample into 'sample/snd/sound-test' and moved 'xgm-player' into 'sample/snd'
* moved 'console' and 'multitasking' samples into 'sample/sys' folder
* renamed 'bench' sample to 'benchmark'
* updated samples to last SGDK

SGDK 2.00 MIGRATION NOTE

* SND_xxx_<DRV> methods --> SND_<DRV>_xxx
* SND_xxx_XGM methods removed (use XGM_xxx methods instead)
* Z80_DRIVER_2ADPCM --> Z80_DRIVER_DPCM2
* Z80_DRIVER_4PCM  --> Z80_DRIVER_PCM4
 
 
SGDK 1.90 (July 2023)
---------------------

COMPILER
* RESCOMP
 - MAP: added support for 8x8 tile priority information for TMX maps (was fixed to TMX map tile size before)
 - OBJECT: objects are now sorted on 'id' field by default (if it exists)
 - OBJECT: added 'width' and 'height' fields export
 - OBJECT: fixed 'name' field export
 - OBJECT: fixed bug with f32 field value
 - SPRITE: added support for empty frame in the middle of a sprite animation
 - SPRITE: fixed bug with dummy empty frame when NONE optimisation level selected
 - SPRITE: fixed a small bug in sprite frame duplicate detection
 - SPRITE: added new optimization level for cutting process (replaced 'iteration' with 'optLevel')
 - SPRITE: several fixes / improvements to sprite cutting algorithm
 - PAL: better JASC .pal file support
 - fixed a small bug with empty / dummy tileset
 - added info in output log for duplicated objects
 - rescomp.txt documentation updated and fixes
 - several others tweaks and fixes
* XGMTOOL
 - fixed compilation on linux (thanks to Andrew DeRosier)
 - updated XGMTool binary to support ZGM output
* MAKEFILE
 - made ext.mk optional
* LINK
 - added .ramprog section for easier moving code into RAM
* DOCKER
 - fixed windows docker build container (thanks to Allion Benjamin)
 - removed wine32 dependency, support Apple M1 (arm64) and native tools too
* README
 - fixed assembly (asm target) generation
 - added links + smalls updates here and there

LIBRARY
* SPRITE:
 Rewrote the sprite engine to get rid of hardware sprite allocation (note that the legacy sprite engine
 is still accessible through the config.h file as it can be useful for sprite multiplexing).
 - better hardware sprite usage
 - general simplification of the API
 - lighter 'Sprite' structure
 - re-introduced Y position in visibility computation
 - lot of optimisations done to maintain same or get higher level of performance
 - added SPR_getAnimationDone(..) method to know if current playing animation reached the last frame
 - added SPR_getUsedVDPSprite() method to know how many hardware sprite are currently in use.
 - added SPR_enableVDPSpriteChecking() / SPR_disableVDPSpriteChecking() methods
 - added SPR_getFreeVRAM() and SPR_getLargestFreeVRAMBlock() methods
 - keep same behavior in debug / release build
* OBJECT
 - small fix in POOL_find(..) method
* MATH
 - transformed some macros to methods for better type checking (can also help the compiler)
* CONSOLE 
 - added TTY console support (added as module - thanks to Andreas Dietrich for it)
* VDP
 - modified MAX_VDP_SPRITE to SAT_MAX_SIZE as max displayable sprite depend from horizontal reolution
 - disable VDP during VDP_resetScreen() process (faster operation)
 - small fix on VDP_waitVSync() and VDP_waitVActive() methods to take care of VDP disable state
* SYS
 - added SYS_nextFrame() method (just an alias to SYS_doVBlankProcess)
 - added SYS_resetBanks(), SYS_getNextFarAccessRegion() and SYS_setNextFarAccessRegion() methods
 - fixed a bug with soft reset + general safer soft reset operation
 - added SYS_getShowFrameLoad() method
 - fixed small bug in TSK_init()
* Z80
 - several fixes and method updates for safer operation
* SOUND
 - added XGM_setPCM_FAR(..) and XGM_setPCMFast_FAR(..) to allow playing PCM through bank switch
 - added YM2612_readStatus() method
* JOY
 - added JOY_getEventHandler() method (useful for saving / restoring callback)
* SAVE
 - added flash save support (added as module - thanks to doragasu for it)
* MISC
 - tweaked / improved readability of lz4w_unpack (thanks to HpMan)
* deprecated methods and definitions now generate compilation error
* added NO_INLINE definition when we want to force no inlining
* forced NO_INLINE on some larges functions to avoid code growing too much
* added megaWifi README file
* documentation updates
* smalls fixes, general cleanup, some refactoring..

SAMPLE
* BENCHMARK
 - added new large scroll performance test
 - added new big sprite tests
 - reworked scoring (making it not comparable to previous versions unfortunately)
 - small fixes
* updated samples to last SGDK

SGDK 1.90 MIGRATION NOTE

* Removed 'resetz80' parameter from Z80_upload(..) function (it's always reseted now)
* Z80_clear(..) doesn't take any parameters now (it always clears all the Z80 RAM now)
 
 
SGDK 1.80 (September 2022)
--------------------------

COMPILER
* RESCOMP
 - added new OBJECTS resource (see rescomp.txt for more infos)
 - added new TILEMAP resource (see rescomp.txt for more infos)
 - added Tiled TMX map and object support (TILEMAP, MAP and OBJECT resources)
 - added Tiled TSX tileset support (TILESET resource)
 - added global RGB image support (see rescomp.txt for more infos)
 - added .pal file and RGB image support for PALETTE resource
 - added extension support so you can add your own custom resource (see rescomp.txt for more infos)
 - better management of duplicated resources
 - better compression logic (don't compress if gain is too low)
 - better logic for extended attributes from tile (MAP and TILEMAP resources)
 - improved tile indexing performance
 - improved compilation reports
 - some tweaks and fixes
* XGMTOOL
 - added support to split VGM and PCM streams in VGM file (Thanks to Vic_)
 - added ZGM (compressed VGM) export support (Thanks to Vic_, useful for 32X dev)
* MAKEFILE
 - added '-fms-extensions' flag to allow usage of anonymous embedded structures and enums
 - added '--register-prefix-optional,--bitwise-or' flags to help with assembly code (can use '|' as OR operator)
 - added 'ext.mk' and 'ext_lib.mk' files for makefile extension and customization
 - removed '-ggdb' flag for .s files compilation. It makes AS crashing when using 'dc.b' data declaration.
* README
 - added 'useful tools' section
 - some rewrites and tweaks (thanks to MrBen89)
 - updated tutorial section
 - updated thanks section
 - added special thanks section

LIBRARY
* OBJECT
 - added new 'pool' unit for easy and fast object pool management
 - added new 'object' unit for easy general object implementation
* VDP
 - renamed many constants for better coherency in naming convention (ex: TILE_USERINDEX --> TILE_USER_INDEX)
 - renamed all GFX_xxx constants to VDP_xxx 
 - added bit fields to VDPSprite structure for easier field manipulation (thanks to Carlos Aragones)
 - added many methods to manipulate these fields (thanks to Carlos Aragones)
* MAP
 - removed 'palette' and 'tileset' fields from Map structure (better to handle that externally)
 - fixed allocateMap(..) method
* MEM
 - added MEM_allocAt(..) function to allocate memory at a fixed address
 - splitted memory.h file --> memory.h and memory_base.h (easier to include in .s file)
* SYS
 - faster H-int callback implementation (callback method need to be prefixed with HINTERRUPT_CALLBACK now)
 - added TSK_init() method for easier proper multi tasking system initialization 
 - modified random() implementation to allow reproducible numbers sequence
* Z80
 - added generic and customizable Z80 bus protection methods (Z80_useBusProtection / Z80_setBusProtection)
 - added generic Z80_setForceDelayDMA() so it can be used by all sound driver (not only reserved to XGM sound driver)
* SOUND
 - added Fractal sound driver support (https://gitlab.com/Natsumi/Fractal-Sound)
 - added PSG_setToneLow(..) method (only set low part of tone register)
 - renamed PSG_init() --> PSG_reset()
* TYPE
 - added volatile fix16/fix32 type (vfix16, vfix32, vf16, vf32)
 - changed 'bool' to 'u8' instead of 'u16' (don't waste RAM / ROM :p)
 - added p16 type (short pointer for low ROM / high RAM address)
* DMA
 - increased a bit default DMA buffer size
* added macros to create 'enum' type compatible with assembly (.s) files
* several documentation fixes / cleanups / updates
* removed old deprecated units and headers (vdp_pal and vdp_dma)
* general cleanup, some refactoring..

SAMPLE
* rewrote 'sonic' sample with better separation of different units.
* added new 'platformer' sample (thanks to Gerard Gascon).
* tweaked/fixed 'scaling' and 'wobble' samples (using the new fast h-int call)

SGDK 1.80 MIGRATION NOTE
* Add extra '_' character on some constantes (i.e: TILE_USERINDEX --> TILE_USER_INDEX; TILE_FONTINDEX --> TILE_FONT_INDEX; ...)
* Replace all GFX_xxx constants with VDP_xxx
* Add HINTERRUPT_CALLBACK keyword before your hint callback method (see 'scaling' sample for an example)
 
 
 SGDK 1.70 (February 2022)
-------------------------

COMPILER
* fixed bug in sizebnd companion tool
* RESCOMP
 - fixed compression for BIN resource (AUTO cannot be accepted here)
 - fixed a small bug preventing having a transparent tile with high priority or palette index > 0
 - fixed a bug with 'opt' set to NONE in SPRITE resource
 - fixed illegal sun 'WaveFileReader' class usage for WAV file resampling operation
* MAKEFILE
 - added 'EXTRA_FLAGS' entry to makefiles so we can use custom extra compilation flags
 - added 'SGDK_GCC' define to identify SGDK compilation toolchain if needed (when used in larger build frameworks)
 - modifications to support Linux native toolchains (thanks to Doragasu)
* LINK
 - now use objcopy instead of ld to generate rom_head.bin (improve compatibility with others GCC)
* README
 - added Doragasu references / version update
 - added DOA game reference
* DOCKER
 - use unprivileged sgdk user to avoid root privilege usage (thanks to Doragasu)
 - Modifications to support Linux native toolchains (thanks to Doragasu)
 - minors tweaks

LIBRARY
* added multitask support - see 'task' unit for more details (thanks to Doragasu)
* added MegaWifi support - module should be enable in config.h file (thanks to Doragasu)
* SPRITE
* VDP
 - small fix to VDP_resetScreen() function to avoid a stack overflow on failure
 - added VDP_drawTextEx(..), VDP_clearTextEx(..) and VDP_clearTextAreaEx(..) methods (can use DMA / DMA_QUEUE)
* MAP
 - added wrapping support (map size need to be aligned on power of 2)
 - added MAP_scrollToEx(..) method to allow forced full map redraw
 - fixed a bug on initial map refresh
 - fixed a small bug on full map refresh
* MATH
 - added getNextPow2(..) method
 - added casting to int to fix conversion (safer)
* SYS
 - added VDP_setExtInterrupt(..) method and fixed ext interrupt enable stat
 - added new kprintf(..) method for easier KMod debug logging
 - minor tweak on mapper banking logic
* DMA
 - removed Z80 halt/resume hack before triggering DMA (was not preventing bus clash anyway)
* PAL
 - fixed first set palette operation not always done in vblank during fading process (CRAM dots)
 - simpler setFadePalette(..) implementation 
 - updated to use color value rounding instead of truncation for RGB24_TO_VDPCOLOR(..) macro
 - added new RGB_TO_VDPCOLOR macros (thanks to emmanuel)
* STRING
 - modified intToStr(..) to display hexa value for too large number
* for safety HALT_Z80_ON_IO is also enable by default (config.h file)
* some refactoring
* changed u16 to bool in some declarations
* documentation fixes / cleanups / updates
* cleanup, removed old deprecated units / headers

SAMPLE
* added linescroll sample
* added multitasking sample (thanks to Doragasu)
* added MegaWifi sample (thanks to Doragasu)
* BENCHMARK
 - updated to fix tests when using pure C (still not perfect)
 

SGDK 1.65 (July 2021)
---------------------

COMPILER
* rewrote sizebnd tool in Java and added auto checksum feature.
* RESCOMP
 - added new NEAR function to force NEAR location of binary data (read rescomp.txt file for more info)
 - fixed a small bug on MAP resource where it was using flipped version of a tile even if tileset was containing non flipped version
 - better optimisation process for large sprites (close to the limit of 16 hardware sprites)
 - many small fixes, tweaks and improvements to sprite cutting process
 - added duplicated sprite frame optimization
 - fixed typo on 2BPP to 8BPP image conversion
 - added information on MAP resource compilation
 - WAV resource can now be set as FAR (bank switch)
 - fixed sample output override for WAV resource (needed for Bad Apple sample which use a custom ADPCM2 driver)
 - fixed a possible bug in duplicated resource symbol
* APJ
 - fixed cost computation (bit better compression)
 - fixed encoding for short match with long offset
 - more infos on encoding errors
* LINK
 - increased theoretical max ROM size to ~3.8GB (using bank switch)
* README
 - added MarsDev project reference
 - added And-0 documentation references
* DOCKER
 - use lighter docker image
 - improved / fixed docker script (thanks to all contributors for that)

LIBRARY
* SPRITE
 - fixed a bug where some sprites wouldn't be properly hidden in rare occasion
 - automatically update VRAM region location when VRAM mapping changed
 - VRAM region can be remapped if needed on defrag operation
 - added sprite validity check in 'debug' build
* VDP
 - VDP_resetScreen() now also reset VSRAM
 - updated VRAM mapping for 64x64 plane size to allow full window map
 - use new mul/div/mod functions for small performance improvement on some functions
* MAP
 - fixed a decoding bug when we had consecutive same row block offset
 - fixed MAP scrolling update when using TILE/LINE horizontal scrolling mode
* MATH
 - added fix32Avg(..) and fix16Avg(..) methods
 - added mulu/muls/divu/divs/modu/mods/divmodu/divmods(..) functions which allow to directly use the 68000 mul/div instruction (GCC doesn't always use them correctly)
* SYS
 - added ROM_SIZE definition to get ROM size
 - added SYS_getFarDataEx(..) and SYS_getFarDataSafeEx(..) methods
 - added SYS_isCrossingBank(..) method
 - fixed RAM mapping for bank switching with ROM larger than 12MB
 - reset banks only when bank switch is enabled (may help with Everdrive X3)
 - added SYS_setVBlankCallback(..) method
 - added SYS_computeChecksum() and SYS_isChecksumOk() methods
* DMA
 - lowered default PAL buffer capacity (DMA_BUFFER_SIZE_PAL_LOW) to spare memory
 - HALT_Z80_ON_DMA is now enabled by default for the sake of safety
 - added DMA_canQueue(..) method and small fix on transferSize calculation
 - fixed a stupid bug in DMA_doDma(..) on 128K bank crossing
 - removed some VDP_waitDMACompletion() check for performance reason (developer need to do it by himself)
* PAL
 - added TransfertMethod parameter to set palette colors methods
 - made 'fadeCurrentPal' and 'fadeEndPal' variables visible (can be useful)
* SOUND / XGM
 - protected XGM_xxx methods against interrupts
 - fixed XGM_pausePlay(..) which wasn't always muting all sound (thanks to BigEvilCorporation for the fix)
 - fixed potential PENDING_FRM overflow (mutual change)
 - XGM play / pause / resume commands are now exclusive (cannot be pending at same time)
 - full YM reset on YM2612_reset()
* BITMAP
 - fixed VRAM organization for Bitmap mode (restore 64x32 plane size on exit)
 - use new mul/div/mod functions for small performance improvement on BMP_scale(..) function
* ROM HEADER
 - updated default SRAM size to 64 KB
* JOY
 - use the correct bits to detect 3-button.
 - tried to compensate 6 button read to avoid the UP+DOWN issue state (emulator or unofficial controllers)
* small changes for better/safer soft reset behavior
* replaced some waitVSync() by SYS_doVBlankProcess()
* more detailed error / warning logs in debug build
* documentation fixes and updates
* cleanup, removed old deprecated units / headers

SAMPLE
* SONIC
 - fixed DMA buffer size for initial MAP update
 

SGDK 1.62 (March 2021)
----------------------

COMPILER
* RESCOMP
 - fixed a small bug on MAP resource compression
 - implemented independant compression on MAP resource
 - XGM resource are now stored as FAR (accessible through bank switch)
 - tweaked default BALANCED sprite cutting optimization strategy to reduce a bit sprite usage over tile
 - increased maximum SPRITE width / height to 248 pixels (it was 128 pixels)
 - optimized SPRITE metadata size (divided size by 4) at the expense of slower sprite processing
 - force slow sprite cutting processing when specifying a number of iteration different from default (500000)
 - added information (rescomp.txt) about how to provide extra data (priority / palette) on tilemap for MAP and IMAGE resources
 - added support to grayscale image
 - better error reporting
* BATCH
 - several fixes to build_adv.bat file
* MAKEFILE
 - makefile can now define GDK variable on its own (environment variable not anymore needed)
 - fixed copy/paste typo in makelib.gen file
* README
 - smalls changes to alternative OS solutions
 - added Raster Scroll MD graphics guide reference
 - updated credits part
 - fixed line return
* increased maximum ROM size to 12MB
* removed resourcemanager tool which was not used nor complete 
* added prelemenary docker support for easier SGDK usage on alternative OS (thanks to Steve Schnepp for it)
  
LIBRARY
* SYS
 - added SYS_getFarDataSafe(..) method / FAR_SAFE(..) macro allowing safe access to resource crossing 512 KB banks
   SGDK methods now use FAR_SAFE(..) accessor instead of FAR(..) for bank switch mechanism
 - fixed a bug in bank switch initialization
 - better CPU load reporting
 - minor change to main(..) prototype and return execution
 - added new 'ROMHeader rom_header' variable to get easier access to ROM header infos
* SPRITE
 - modified sprite structures to reduce metadata size (but at the expense of a bit slower processing)
 - added SPR_getVisibility(..) and SPR_isVisible(..) methods
 - now automatic sprite visibility is computed using only X position as we use it only scanline sprite usage optimization
 - made 'firstSprite' / 'lastSprite' visible
 - improved profiling
* DMA
 - small tweak + some fixes when DMA is disabled
* VDP
 - added VDP_setHorizontalScrollVSync() and VDP_setVerticalScrollVSync() methods to do delayed scrolling update
 - safier VDP_drawText(..) / VDP_clearText(..) methods (bounds check)
 - prevented tilemap column update when plane width is set to 128 (not possible because auto-inc register is limited to 255)
* MAP
 - added priority info support to baseTile parameter in MAP_create(..)
* MATH
 - removed MATH_BIG_TABLES define as it's not anymore useful with the lto
 - add clamp function (thanks de Devon Powell)
 - fix issue with rounding helpers (fix16)
* MEMORY
 - added malloc(..) / free(..) so they forward to MEM_alloc(..) / MEM_free(..) methods
* SOUND
 - added XGM_startPlay_FAR(..) to allow to play XGM music accessible through bank switch
* TIMER
 - small fix on waitSubTick(..) method to take care of small wait during vblank
* JOY
 - added JOY_writeJoypadX(..) and JOY_writeJoypadY(...) which allow to set a specific position for mouse pointer (Thanks to LIZARDRIVE)
* small change to 'Map' and 'Sprite' structure for better doxygen documentation reference

SAMPLE
* reworked folder structure
* added new basic HELLO_WORLD sample
* added BAD APPLE demo sample (to exhibit SGDK bank switching mechanism)
* added new WOBBLE fx sample (thanks to Andreas Dietrich for it)
* added new SCALING fx sample (thanks to Andreas Dietrich for it)
* SPRITE sample renamed to SONIC
* BENCHMARK
 - better score computation for Sprite part (also now comparable to others tests)

SGDK 1.62 MIGRATION NOTE
* Replace all sprite->seqInd occurence with sprite->frameInd
* Replace all sprite->animation->length occurence with sprite->animation->numFrame
* Sprite frame size is now directly defined in the SpriteDefinition structure (SpriteDefinition->w, SpriteDefinition->y) as it's fixed for all frames.


SGDK 1.6 (December 2020)
------------------------

COMPILER
* APLIB
 - replaced appack tool by new APJ (Java APLIB packer) tool written from scratch
* LZ4W
 - much faster compression speed
 - better exit operation
* RESCOMP
 - added new MAP resource and MAP_xx methods (in map.h unit) to handle large maps / background scrolling easily (finally !)
 - integrated APJ tool for faster ApLib compression
 - added per resource size information, fixed Map resource export...
 - we can now disable sprite cutting optimisation ('NONE' value for opt parameter)
 - fixed a small bug on Sprite cutting optimisation process
 - added 'optimisation' parameter to TILESET resource
 - fixed bug on tilemap generation with base tile index used
 - fixed a mall bug in BIN resource export
 - fixed a small bug on ALIGN directive
 - optimized tile parsing
* XGMROMBUILDER
 - now remember last used folder
* MAKEFILE
 - added deps file (.d) support for better dependencies handling
 - added sources sub folders support (2 depth levels)
 - removed useless and undesired warnings
* README
 - added GET STARTED part
 - major rewrite / revamp to take benefit of Markdown format (thanks to Astrofra)
 - others changes

LIBRARY
* MAJOR CHANGE !WARNING!:
  Added SYS_doVBlankProcess(..) to replace automatic V-Int processing of SGDK.
  This was done to avoid issues with missing interrupts protection and to offer more control in general.
  So basically you should replace all your _VDP_waitVSync()_ references with _SYS_doVBlankProcess()_
* SYS
 - faster user callback interrupt handling
 - modified ROM header to enable SRAM by default
 - fixed small initialization issue
 - minor fix on SSF2 mapper initialization
 - added LOG_LEVEL setting allowing different level of log for debug (default = LOG_LEVEL_WARNING)
 - removed interrupts protection where they are not anymore useful
 - small change for more accurate detection of frame miss
* MAP
 Added new MAP engine (map.h unit) to handle large maps / background scrolling easily from MAP resource:
 - Map* MAP_create(const MapDefinition* mapDef, VDPPlane plane, u16 baseTile);
 - void MAP_scrollTo(Map* map, u32 x, u32 y);
 - u16 MAP_getMetaTile(Map* map, u16 x, u16 y);
 - u16 MAP_getTile(Map* map, u16 x, u16 y);
 - void MAP_getMetaTilemapRect(Map* map, u16 x, u16 y, u16 w, u16 h, u16* dest);
 - void MAP_getTilemapRect(Map* map, u16 x, u16 y, u16 w, u16 h, bool column, u16* dest);
* DMA
 - added DMA_xxxFast() methods (no 128 KB bank cross check)
 - small tweaks for bit faster DMA queue
 - tweaks in general to DMA methods
 - increased default DMA queue size (we can quickly reach 64)
* VDP 
 - fixed a small bug on VDP_drawTextBG(..) method
 - added VDP_waitVBlank() and VDP_waitVActive() methods
 - added VDP_getPlaneAddress() method (was internal first)
* STRING
 - added support to u32/s32 type to sprintf(..) function
 - fixed fix16ToStr(..) and fix32ToStr(..) methods
* SOUND
 - fixed a small bug on PCM command play (thanks to hsk)
 - better restoring of Z80 BUS request state
* Z80
 - added Z80_getAndRequestBus() method
 - added loading of a dummy driver by default to keep Z80 active on init
* replaced some u16 parameter by bool where it makes sense to have it

SAMPLE
* Updated for SGDK 1.6 changes
* BENCHMARK
 - minors changes (updated to version 1.31)
* SPRITE
 - updated background to complete level dimension.
 - updated to take benefit of the new MAP resource and advanced MAP methods
 - added alternate scrolling update mode (still using new MAP unit)

SGDK 1.6 MIGRATION NOTE
* Replace all VDP_waitVSync() references with SYS_doVBlankProcess()
* Replace all VDP_waitVInt() references with SYS_doVBlankProcess()


SGDK 1.51 (April 2020)
----------------------

COMPILER
* APPACK
 - fixed build for 32 and 64 bit linux (thanks doragasu)
* LZ4W
 - minor fix  
* RESCOMP
 - added new sprite optimization options to SPRITE resource (see rescomp.txt for details)
 - minor tweak on binary export order (can save some bytes with LZ4W compression)
 - fixed resource duplication bug
* XGMTOOL
 - fixed VGM loop
  
LIBRARY
* fixed corrupted library binaries which were displaying a blank screen on some MD models
* MEMORY
 - fixed a minor in memory packing operation
 - added MEM_pack() here and there to avoid memory fragmentation
* VDP
 - fixed getAdjustedVCounterInternal(..) which could return value > 255 in some rare situation
 - moved VDP DMA busy checking on reset (better to do it before accessing VDP)
 - VDP_setPlaneSize(..):
   - added constraint on plane size
   - fixed maps start address calculation (when VRAM setup is asked)
 - minor change in VDP_drawImageEx(..) to do setTileMap(..) operation using CPU (DMA is actually slower here)   
* added SYS_showFrameLoad() / SYS_hideFrameLoad() methods to monitor CPU frame load.
* always load font using CPU in reset process (safer)
* fixed String unit build when ENABLE_NEWLIB set (thanks doragasu)
* improved documentation in Joy unit (thanks Chilly Willy) and VDP (regarding DMA QUEUE usage specifically)

SAMPLE
* reduced memory usage on sample which use Bitmap mode by reducing DMA allocated memory
* SPRITE
 - added SYS_showFrameLoad() showcase


SGDK 1.5 (April 2020)
---------------------

COMPILER
* RESCOMP
 - added ALIGN directive (read rescomp.txt for more information about it)
 - added UNGROUP directive (read rescomp.txt for more information about it)
 - added 'compression' and 'far' field to BIN resource
 - replaced Map structure export by TileMap
 - minor optimization in building IMAGE tilemap
   plain tiles are now ignored (taken from system tiles) when using a base tile index offset for tilemap (mapbase parameter in IMAGE resource)
 - more flexible resource compilation
   - ignore palette and priority for transparent pixel
   - sprite can have their palette not starting at index 0
 - reorganized resource data export order for better LZ4W compression and bank switch support
 - added support for 1bpp and 2bpp indexed color images
 - faster LZ4W tool call (embeded in rescomp now)
 - preserve resource order for better BIN data compression with LZ4W
 - more constrained sprite cutting process depending chosen optimization strategy
 - minor fix to allow using bit 7 (color index >= 128) in IMAGE resource as priority bit in tilemap
 - simplified / fixed binary compression block with alignment
 - fixed a bug on possible duplicated resource export
 - fixed Circle collision type export
 - now return -1 as exit code on error
 - replaced FileWriter by StringBuffer (faster and safer)
 - minor changes and improvements to rescomp.txt file
* XGMTOOL
 - fixed a small bug during sample conversion processing
* XGMROMBUILDER
 - updated to last XGMTool and XGM driver version
* LZ4W
 - fixed LZ4W compression which could failed in very are case
* BINTOS
 - fixed data section (it was .text instead of .rodata)
* MAKEFILE
 - updated 'release' target to generate symbol.txt file (always interesting to have)
 - show more warnings
* forced no inlining of memset / memcpy methods to fix LTO agressive optimization issue

LIBRARY
* SYS
 - added bank switch support using SSF2 mapper (allow ROM > 4MB)
   - use ENABLE_BANK_SWITCH flag in config.h file to enable bank siwtch support in SGDK
   - added FAR(..) directive to access a resource through bank switch if required
   - added SYS_getBank(..) / SYS_setBank(..) methods
 - moved RAM initialization to sys.c unit and added support for bank crossing (more control on it)
 - minors changes to reset methods (simpler)
 - added ROM and RAM constants (yeah, why not ^^)
* SPRITE
 - added sprite frame change event callback (using SPR_setFrameChangeCallback(..) method)
 - added SPR_loadAllFrames(..) to (pre)load all frames data of a SpriteDefinition to VRAM
 - removed unpack buffer (replaced by DMA buffer and new DMA_QUEUE_COPY method)
 - fixed a small issue with delayed update
 - fixed a bug on SPR_setDefinition(..) (can display glitches as some sprites weren't always properly hidden)
 - fixed internal sprite link (could occasionaly let some phantom and glitched sprites visible)
 - added out of range index detection for animation and frame (debug build only)
* DMA
 - added new DMA buffer for easier and better DMA queue management
   - added 'bufferSize' parameter to DMA_initEx(..) function
   - added DMA_setBufferSize(..) and DMA_setBufferSizeToDefault() functions to set the temporary DMA buffer size
   - added new DMA_QUEUE_COPY transfer method (TransferMethod enum) to copy data to a temporary buffer before transfer actually occurs
   - added DMA_allocateAndQueueDma(..) function which return a temporary buffer and queue a DMA transfer from it
   - added DMA_allocateTemp(..) and DMA_releaseTemp(..) methods to allocate memory from DMA temporary buffer (use that safely)
   - added DMA_copyAndQueueDma(..) function which copy data to transfer to a temporary buffer and queue the DMA transfer
 - added a new generic DMA_transfer(..) function
 - added DMA_doCPUCopy(..) function to do a CPU copy to VRAM/CRAM/VSRAM
 - added DMA_getMaxQueueSize() and DMA_setMaxQueueSize() to get and set the queue size.
 - passed the DMA queue flush loop in assembly for better control (and also faster operation as GCC was dumb about it)
 - safer DMA operation on DMA_doDMA(..)
 - added DMA_initEx(..) and simplified DMA_init()
 - added new DMA_DISABLED flag in config.h to completely disable DMA support in SGDK (for debug purpose)
 - fix for HALT_Z80_ON_DMA (stupid typo)
* VDP 
 - many refactoring (see refactoring section at bottom)
 - TILE_USERMAXINDEX now take allocated VRAM for sprite engine in account !
 - added TILE_SPRITEINDEX constant to get base tile index for the Sprite Engine
 - added tilemap row update methods
   - VDP_setTileMapDataRow(..) / VDP_setTileMapDataRowEx(..)
   - VDP_setTileMapRow(..) / VDP_setTileMapRowEx(..)
 - added tilemap column update methods
   - VDP_setTileMapDataColumnFast(..) / VDP_setTileMapDataColumn(..) / VDP_setTileMapDataColumnEx(..)
   - VDP_setTileMapColumn(..) / VDP_setTileMapColumnEx(..)
 - added TransferMethod parameter to many tilemap set methods
 - added tilemap wrapping support to VDP_setTileMap(..) and VDP_setTileMapEx(..) methods.
 - added setupVram parameter to VDP_setPlaneSize(..) function
 - removed vdpSpriteCacheQueue table (replaced by new DMA_QUEUE_COPY)
 - re-introduced VDP_loadTileData(..) method in vdp_tile.c unit (no more assembly code for this one)
 - moved font loading in VDP_resetScreen() method (fix)
* PALETTE
 - fixed palette fading so it correctly trigger during vblank (avoid CRAM dot)
 - fixed minor issue in palette fading (sometime not properly doing last fade step) 
* MEMORY
 - fixed MEM_getLargestFreeBlock(..) method
 - added MEM_pack() method to help reducing memory fragmentation
 - removed MEM_init() access as it's not safe to call it externally
 - increased stack size to 0xA00
* MATHS
 - fixed getLog2Int(..) method
 - replaced sin tabs to use FIX32/FIX16 (thanks to FireRat for the generator)
* TYPES
 - added new rorxx(..) / rolxx(..) functions which are correctly turned into ROR / ROL instruction when optimization are enabled
* Z80
 - fixed Z80 enable restoration on DMA
* general cleanup and refactoring

SAMPLE
* Updated for SGDK 1.5 changes
* BENCHMARK
 - added memory information at startup
 - some changes to adapt to last SGDK
* SPRITE
 - updated resources to use large backgrounds
 - updated to support long scrolling > 512 px (vertical and horizontal) !
 - as collision is not implemented, added physic settings using START button so we can play with vertical scrolling too
 - enemies sprites frames preloaded and animated using new Sprite engine features
   Showcase of SPR_loadAllFrames(..) and SPR_setFrameChangeCallback(..) methods
* XGMPlayer
 - updated for easier integration in XGM ROM Builder tool
* added notes and tutorial references to README.txt

REFACTORING
* TILE_USERMAXINDEX now take allocated VRAM for sprite engine in account !
* _voidCallback --> VoidCallback
* _joyEventCallback --> JoyEventCallback
* PLAN_A --> BG_A
* PLAN_B --> BG_B
* PLAN_WINDOW --> WINDOW
* VDP_PLAN_A --> VDP_BG_A
* VDP_PLAN_B --> VDP_BG_B
* VDP_PLAN_WINDOW --> VDP_WINDOW
* all references to 'Plan' keyword --> 'Plane'
 - VDPPlan --> VDPPlane
 - VDP_clearPlan --> VDP_clearPlane
 - VDP_getTextPlan --> VDP_getTextPlane
 - VDP_setTextPlan --> VDP_setTextPlane
 - VDP_setAPlanAddress --> VDP_setBGAAddress
 - VDP_setBPlanAddress --> VDP_setBGBAddress
 - VDP_setPlanSize --> VDP_setPlaneSize
* all references to 'Map' --> 'TileMap'
 - Map --> TileMap
 - unpackMap --> unpackTileMap
 - allocateMap --> allocateTileMap
 - VDP_setMap --> VDP_setTileMap
 - VDP_setMapEx --> VDP_setTileMapEx


SGDK 1.41 (September 2019)
--------------------------

COMPILER
* RESCOMP
 - added new sprite optimization options to SPRITE resource (see rescomp.txt for details)
 - minor tweak on binary export order (can save some bytes with LZ4W compression)
 - fixed resource duplication bug
* XGMTOOL
 - fixed VGM loop
  
LIBRARY
* SYS
 - Safer SYS_setInterruptMaskLevel() so interrupt mask is not lost after SYS_enablesInts() call
 - fixed/updated SGDK logo display code
 - renamed getFPS() / getFPS_f() to SYS_getFPS() / SYS_getFPSAsFloat()
* DMA
 - fixed possible DMA failure on some Megadrive when Z80 access 68K BUS at same time we trigger DMA
 - minor fix on autoInc restoration after DMA_flushQueue()
* VDP 
 - tweaked default VRAM configuration so window can be freely used anywhere.
 - added VDP_getAdjustedVCounter() method to have a consistent [0..255] V-Counter (avoiding rollback issue)
 - tweaked up VDP_resetScreen() method
 - added VDP_setHVLatching() and VDP_setDMAEnabled() methods
* PALETTE
 - fixed VDP_getPaletteColors() and VDP_getPalette(..) methods (regression)
 - fixed a bug with all palette fading methods (regression)
 - renamed vdp_pal unit to pal unit (so all palette methods are now called PAL_xxx)
* JOY
 - added JOY_reset() method to reset controller detection without clearing JOY state change event callback
 - safer JOY_setSupport(..) / gun controller implementation
 - more permissive mouse id detection in mouse pooling code
 - fixed an issue where an EA 4-way multitap could be incorrectly detected
* TIMER
 - safer waitSubTick(..) implementation during VInt.
 - fixed getSubTick() method to take care of HV counter latching when light guns are used.
* RESOURCE
 - added 2 alternate SGDK logo (free feel to use it)
 - minor change on library resources name
 - fixed version and added alias for old resource names
* added ENABLE_NEWLIB define in config.h file for those who want to use newlib within SGDK (you need to build it by yourself)
* changed 'u16' to 'bool' where it makes sense to use 'bool' (internally they are the same type, it's just for readability).
* minors tweaks, changes and fixes here and there

SAMPLE
* XGMPlayer
 - updated to make it work with last SGDK (sprite engine difference mainly)
* SOUND
 - reintroduced cry SFX test for XGM driver


SGDK 1.4 (May 2019)
-------------------

COMPILER
* RESCOMP
 - rewrote from scratch in java for easier evolution and easy multi OS support.
 - added smart sprite cutting (detect empty space in sprite)
 - many changes on SPRITE resource, don't forget to read the rescomp.txt file to see changes about this resource.
 - can now disable map optimization for IMAGE resources (see rescomp.txt for more information)
* LZ4W
 - added code sources (java)
 - fixed compression using previous data block in ROM (updated packer to version 1.4)
* BINTOS
 - fixed a stupid bug on path.
* added XGM ROM builder tool sources.
* removed RESCOMP (C version), WAVTORAW, TFMCOM and Z80DASM tools.
* added Visual Studio template.

LIBRARY
* SYS
 - added SYS_setVIntAligned(..) method to force V-Int callback to align process on VBlank.
   IMPORTANT: by default now SGDK *does* align the V-Int processing to VBlank so you need to disable it if you don't want it !
 - added SYS_getCPULoad() to return CPU load estimation.
 - added SYS_resetMissedFrames() / SYS_getMissedFrames() methods.
* DMA
 - simplified DMA over capacity strategy
 - minor change in debug log message
* SPRITE
 - renamed SPR_init(..) to SPR_initEx(..) so now SPR_init() doesn't require any parameters by default.
 - removed 'maxSprite' parameter from Sprite Engine initialization (alays use max available).
 - added delayed frame update support.
 - added SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE flag to disable the delayed frame update (when we are running out of DMA capacity).
 - added SPR_setDelayedFrameUpdate() to change the delayed frame update state for a sprite.
 - added SPR_FLAG_INSERT_HEAD flag to allow adding new sprite in first position (instead of last position by default)
 - added SPR_defragVRAM() method to force VRAM defragmentation
 - added SPR_addSpriteSafe(..) and SPR_addSpriteExSafe(..) methods (automatically do VRAM defrag if needed)
 - fixed a bug where the last VDP sprite attribute weren't always correctly updated with visibility set to AUTO_SLOW.
 - fixed a bug a sprite couldn't be allocated.
 - fixed sprite visibility state when the number of used sprite changed.
 - many changes to sprite structures (better handling of flip info, better ROM usage...)
* VDP 
 - added VDP_showCPULoad() method to display CPU load
 - added VDP_waitVInt() method to wait until next VInt to happen.
 - removed 'waitvsync' parameter for VDP_initFading(..) and VDP_doStepFading(..) methods (we always want VSync here)
 - simplified / tweaked macros for VDP control writes
* VRAM
 - added VRAM_getAllocated(..) and VRAM_getLargestFreeBlock(..) methods
* TYPE
 - added bool type
 - replaced u16 by bool where it needs to be
* MATHS
 - updated fix32Mul() and fix32Div() definition (again, trying to find the best compromize)
 - added new missing structures as Vect2D_f32, Mat2D_f32, Vect3D_xx...
 - added f16 and f32 as shorcut of fix16 and fix32 types.
 - added short typedefs (V2u16 = Vect2D_u16, V2f32 = Vect2D_f32, M2f16 = Mat2D_f16, M3f32 = Mat3D_f32)
* PALETTE
 - removed index field from Palette structure
* JOY
 - added checking for mouse / multipad read operation to avoid timeout operation when mouse or multipad is not present.
* Z80
 - added volatile access for safety
* TIMER
 - improved waitMs(..) method to be more accurate on small wait.
 - tweaked getSubTick() method (need testing, possible regression)
* STRING
 - fixed and optimized uintToStr() method
* TOOL
 - memory allocation methods for unpacking now always use deep allocation regardless of the compression used (simpler and less bug prone)
 - fixed LZ4W decompression using previous data block
* fixed variable initialization (last byte was not always properly initialized)
* added HALT_Z80_ON_IO define (config.h) to force Z80 halt on IO port access.
* new awesome SGDK logo (Thanks a tons to Lizardrive for making it !)
* removed useless sound drivers (MVS, TFM and VGM)
* removed old TILE_CACHE unit (replaced by VRAM unit)
* removed useless zlib 
* many refactoring (sorry for that, you will need to update your old code)
* many tweaks / cleanup

SAMPLE
* updated all samples to take care of last changes made in SGDK
* BENCHMARK
 - tweaked big sprite test to disable the delayed sprite update (new sprite engine feature)
 - fixed sheet size (new rescomp don't allow it)


SGDK 1.34 (January 2018)
------------------------

LIBRARY
* DMA
 - added DMA queue support for all (or almost all) methods supporting DMA operation
* SPRITE
 - fixed timing issue when changing FRAME or ANIMATION manually.
 - fixed sprite sorting when multiple depth were modified in a single SPR_update(..).
 - safer sprite allocation / release.
* MEMORY
 - added MEM_getLargestFreeBlock() to get the largest available block of memory.
* improved LZ4W compression (better compression rate, faster compression...)
* minors changes on method updating tilemap through X,Y position (safer)
* some cleanup

SAMPLE
* reworked benchmark sample to avoid out of memory error (^^)


SGDK 1.33 (November 2017)
-------------------------

LIBRARY
* DMA
 - added HALT_Z80_ON_DMA flag in config.h to enable Z80 halt on DMA (avoid corruptions or sound issues on Tectoy MD).
 - deprecated 'vdp_dma' unit now forward calls to 'dma' unit.
 - added wait DMA checking (DMA fill or DMA copy operation) before doing a DMA operation.
* SPRITE
 - simplified sprite sorting (always enabled, just need to use SPR_setDepth(..) method if needed)
 - fixed a regression which was causing 1 frame latency in sprite update.

SAMPLE
* minor change to sound sample


SGDK 1.32 (October 2017)
------------------------

COMPILER
* added VS project for easier compilation with Visual Studio (thanks to lab313)

LIBRARY
* TIMER:
 - fixed getSubTick() method (no more possible rollback)
 - minor fix in getFPS() and getFPS_f() methods
* VDP:
 - tweaked default VRAM memory layout
* BITMAP:
 - changed get/setPixelXXX(..) methods so they now work on single pixel (not anymore doubled X pixel resolution)
 - changed drawLine(..) method so it now work on single pixel (not anymore doubled X pixel resolution).
   WARNING: drawLine(..) is not anymore doing clipping, use BMP_clipLine(..) first for that.
 - added get/setPixelXXXFast(..) methods for fast get/set pixel operation (no clipping check)
* SOUND:
 - fixed auto PCM selection when playing sample with driver 2ADPCM and 4PCM.
 - fixed default tempo for PAL system with XGM driver
* SPRITE:
   WARNING: you now require to set the sprite depth to use depth sorting (not anymore using the sprite Y position).
 - replaced SPR_FLAG_AUTO_YSORTING by SPR_FLAG_AUTO_DEPTH_SORTING
 - replaced SPR_setYSorting(..) method by SPR_setDepthSorting(..) / SPR_setZSorting(..)
 - replaced SPR_sortOnYPos() by SPR_sortOnDepth()
 - added void SPR_setDepth(..) / SPR_setZ(..) methods to set sprite depth
 - fixed VDP_updateSprites() with DMA queue operation (prevent sprite table modifications before DMA occurs).
* STRING:
 - added int16ToStr(..) and uint16ToStr(..) methods (faster than intToStr(..) or uintToStr(..) for 16 bit integer, thanks to clbr)
 - optimized intToStr(..) and uintToStr(..) methods (thanks to clbr for that)

SAMPLE
* added XGM Player sample :)
* Bench:
 - added some pixels / line draw tests
* Donuts:
 - updated for new depth sorting refactoring


SGDK 1.31 (July 2017)
---------------------

DOCUMENTATION
* minor fix and updated to last version

COMPILER
* fixed debug build in 'build_lib' batch

LIBRARY
* JOY:
 - fixed joy state variables declaration to avoid issues when GCC -O3 optimization level is used.
* SPRITE:
 - added ALWAYS_ON_TOP flag to keep a sprite above others sprites whatever is sorting order.
 - minor fix on sprite sort


SGDK 1.3 (June 2017)
--------------------

DOCUMENTATION
* updated to last version

COMPILER
* Updated to GCC 6.3 (thanks a tons to Gligli for that !)
 - many bugs fix and new features compared to old GCC 3.4.6
 - much better assembly code generation :)
 - added LTO (Linker Time Optimization) support
* Modified makefile to enable LTO and improve optimization level.
* Rescomp:
 - updated to handle structure changes in the Sprite Engine.

LIBRARY
* DMA:
 - minor optimization to DMA_queue(..) method (thanks to HpMan)
* Memory:
 - default stack size increased to 0x800 bytes (GCC 6.3 requires more stack memory :p)
* Sprite Engine:
 - added automatic Y sorting (per sprite)
 - added SPR_sort(..) for generic sorting
 - added SPR_sortOnY(..) for generic sorting
 - by default now sprite visibility is set to always ON (faster than automatic visibility)
 - updated 'Collision' structure (hierarchical structure)
 - some changes to internal structures to provide better performance
* VDP BG/Tile:
 - fixed a minor bug in VDP_setTileMapDataEx(..) and VDP_setTileMapDataRectEx(..) methods (thanks to Alekmaul for reporting it)
* minors fixes...

SAMPLE
* Bench:
 - fixed math tests for GCC 6.3


SGDK 1.22a (September 2016)
---------------------------

LIBRARY
* VDP: reintroduced the 16 plain system tiles.


SGDK 1.22 (September 2016)
--------------------------

DOCUMENTATION
* minors improvements and fixes

COMPILER
* Rescomp: fixed a minor issue in sprite resource
* XGMTool:
 - added duration information to XD3 tag
 - improved loop
* added the XGM ROM builder tool.
* removed GenRes from makefile

LIBRARY
* Sprite Engine:
 - fixed sprite attribut update for non visible sprite
 - fixed sprite list update in certain condition
* Sound:
 - moved XGM driver method in a specific unit (xgm.c)
 - added XGM_getElapsed(..) method to retrieve elapsed XGM music playing time (in number of frame)
 - added XGM_setLoopNumber(..) to set the wanted number of loop in XGM music play.
 - added interrupt protection for Z80 access
* Maths:
 - reverted fix32Mul() and fix32Div() to previous version to avoid cumulative error
 - added getApproximatedLog2(..) method for fast Log2 calculation (approximated)
 - added getLog2Int(..) method for integer Log2 calculation
* Misc:
 - moved QSort methods to tools
 - added generic qsort with custom comparator callback
* VDP: more flexible VRAM tilemap configuration (window plan don't have to be first map).
* Bitmap mode: can now set bitmap mode in window plan
* changed to MIT license
* refactoring

SAMPLE
* Bench: added sprite donut animation test.


SGDK 1.21 (May 2016)
--------------------

LIBRARY
* SPRITE: fixed a bug causing corrupted sprite after SPR_release(..) operation
* TOOLS: reintroduced zlib_unpack(..) method (accidentally removed from header)

SAMPLE
* Bench:
 - fixed a bug causing address error on real hardware during 'Sprite test'.
 - added 2 tests in 'BG test'


SGDK 1.2 (May 2016)
-------------------

DOCUMENTATION
* several update and fixes here and there (some tags were not correctly recognized in later doxygen version).

COMPILER
* Rescomp: updated to version 1.5 (with updated documentation)
 - updated SPRITE resource compilation to the new SGDK Sprite structures.
 - fixed minor issue on IMAGE resource packing.
 - now using constants for sound driver (less confusing, got annoying bug because of that).
 - changed header 'define' name generation to avoid conflict between 2 identical named file.
 - fixed a bug in tilemap optimization for flipped tiles.
 - removed all packers not anymore used in SGDK.
 - minor change to XGM resource to support extra parameter
 - fixed a minor bug in sprite structure definition
* XGMTool: updated to version 1.64
 - major structure changes for faster conversion / optimization operations
 - improved VGM to XGM conversion.
 - added options to disable some PCM auto processing
 - new options available to handle specific case and improve conversion process
 - added GD3 tag support.
 - better handling of PAL/NTSC timing
 - fixed pal information lost during XGC conversion.
 - more accurate loop position
 - fixed a minor issue in offset calculation.
 - fixed VGM loop information export (when using VGM optimization)
* WavToRaw:
 - fix 64-bit issues, it is still not endian safe.
 - check for read errors.
* Appack: minor fix for silent parameter.
* Added new custom LZ4W packer (require Java to be installed).
* Removed GenRes tool (not anymore used and can confuse with rescomp).
* added GDB tool (not yet really used yet)
* Separated 'debug' and 'release' library build for easier profile switch.

LIBRARY
* BITMAP:
 - allow to change the plan used for bitmap rendering
 - added buffer preservation option (severe impact on performance)
 - fixed minors issues on reset/initialization process
* DMA: added new DMA queue system in 'dma' unit, you can consider 'vdp_dma' unit as deprecated (still provided for backward compatibility).
 - we can now limit the max transfer capabilities in a single frame with DMA_setMaxTransferSize(..)
 - can now define the DMA queue size with DMA_init(..) method.
 - added DMA_setIgnoreOverCapacity(..) to change DMA strategy when reaching max capacity.
* GRAPHIC:
 - added news methods for allocation and compression stuff (see TOOLS section)
 - added dynamic VRAM allocation ('vram' unit which replace 'tilecache' unit)
* PALETTE:
 - fixed RGB24_TO_VDPCOLOR so it does what it says.
 - better palette fading using rounding.
 - fixed issue using sync fading locking interrupts (can cause XGM music lag).
* MATHS:
 - added abs(..) method.
 - fixed fix32ToRoundedInt() and fix32Round() defines (added parenthesis around)
 - replaced distance_approx(..) by getApproximatedDistance(..)
 - changed fix32 div/mul calculation strategy for better value preservation
* MEMORY:
 - added MEM_getAllocated() to return current dynamically allocated memory.
 - added MEM_dump() to dump in Gend KMod console the memory allocation table
* SOUND:
 - renamed Z80_DRIVER_4PCM_ENV --> Z80_DRIVER_4PCM
* SPRITE: complete rewrite of sprite engine !
 - many changes including the API.
 - should be faster but will be more optimized in future.
* STRING:
 - added isdigit(c), strnlen(..) and the very useful sprintf(..) ma
 - added strncpy(..) method.
 - replaced strreplace(..) --> strreplacechar(..)
 - fixed fix32ToStr(..) and fix16ToStr(..) methods
* SYSTEM:
 - tried to more more compatible with default GCC stdint.h definitions
 - added SYS_setVIntPreCallback(..) so you can have your method called at VInt before any internal SGDK stuff are proceed.
 - added SYS_isNTSC() and SYS_isPAL() methods for easy system determination.
 - minor fix on SP register initialization (preserve value set in vector table)
 - added SYS_hardReset() to force hard reset.
* TOOLS:
 - added new LZ4W compression (very fast unpacking but average compression level)
 - removed RLE, RLE_MAP and UFTC compression (LZ4W performs better in almost all cases).
 - removed UnpackEx(..) method (useless now).
 - added setRandomSeed(u16 seed) to initialize randomizer.
* VDP:
 - renamed WPLAN / WINDOW / VDP_WINDOW --> VDP_PLAN_WINDOW
 - renamed APLAN --> VDP_PLAN_A
 - renamed BPLAN --> VDP_PLAN_B
 - renamed SLIST / VDP_SPRITE_LIST --> VDP_SPRITE_TABLE
 - renamed HSCRL / VDP_SCROLL_H --> VDP_HSCROLL_TABLE
 - added planWidth / planHeight to replace VDP_getPlanWidth() / VDP_getPlanHeigth() for faster internal SGDK calculations.
 - added windowWidth / windowHeight for faster internal SGDK calculations.
 - added VDP_setWindowHPos(..) and VDP_setWindowVPos(..) methods to set window positions.
 - fixed a bug with VDP_setBPlanAddress(..) method.
* VDP BG/TILE:
 - replaced VDP_PLAN_A / VDP_PLAN_B constants by PLAN_A / PLAN_B in some methods.
 - some methods now support PLAN_WINDOW parameter.
 - added VDP_clearTextAreaBG(..), VDP_clearTextLineBG(..), VDP_clearTextArea(..) methods.
 - modified VDP_drawTextBG(..) method.
 - others minors changes.
* VDP SPRITE: major rewrite of 'vdp_sprite' unit (require project modifications) !
 - replaced SpriteDef structure by VDPSprite structure (fit better hardware structure).
 - added dynamic allocation of hardware sprite:
   VDP_allocateSprites(..), VDP_releaseSprites(..), VDP_getAvailableSprites()
 - VDP_updateSprites(..) can now use DMA queue.
 - many others changes.
* XGM driver:
 - better handling of main BUS contention with DMA
 - added methods to improve BUS contention when using PSG sound in music.
 - fixed PCM play status when PCM is used from XGM music.
 - minor fix in driver code for better music frame sync.
 - better pause/resume support.
 - music sync is now handled on 68000 side for more flexibility (adjustable tempo).
* Z80:
 - modified writeYM macros
* memory usage optimizations.

SAMPLE:
* Bench: added new sample for general test and benchmarking.
* Sound:
 - minors changes about Z80 load information for XGM driver.
* Sprite:
 - added basic enemies (no collision yet)
 - added basic SFX
 - updated to last SGDK
 - some refactoring


SGDK 1.12 (March 2015)
----------------------

COMPILER
* XGMTool:
 - minors changes and fixes. 

LIBRARY
* SPRITE:
 - added VDP_resetSpritesDirect() method. 
* SOUND:
 - minor fix to XGM driver (PCM in music was wrong in some case). 


SGDK 1.11 (December 2014)
-------------------------

* CONTROLLER:
 - fixed small issue in joystick code (Chilly Willy). 
* SOUND:
 - added Z80 CPU load information in XGM driver (experimental).
* Fixed small issue in joystick code (Chilly Willy).


SGDK 1.10 (December 2014)
-------------------------

COMPILER
* XGMTool:
 - removed DAC enabled command (automatically handle by the XGM driver).
 - added DAC enabled state (XGM driver uses it when no PCM are playing).
 - now uses the VGM 1.60 'stream id' information to allow multi PCM channel for XGM conversion.
   Note that each channel has its own priority as VGM music does not contains PCM priority information.

LIBRARY
* VDP:
 - VDP_fade(..) method now automatically disables interrupts if needed.
* SPRITE:
 - minor optimization in the Sprite Engine to quickly discard disabled sprites (visibility forced to off).
* SOUND:
 - added automatic DAC enabled control (XGM driver).
 - fixed issue with music pause operation on real hardware (XGM driver).
 - minor tweak to reduce a bit the size of the Z80 drivers.
 - removed Z80_DRIVER_4PCM which is useless (use Z80_DRIVER_4PCM_ENV driver instead).
* CONTROLLER:
 - added Sega Phaser support (Chilly Willy).
* SYSTEM:
 - disable library debug info.
* SAMPLE:
 - updated 'Joy' sample to add Phaser test (Chilly Willy).
 - updated 'Sound' sample to remove Z80_DRIVER_4PCM test and add an example of the MVS driver PCM SFX.


SGDK 1.01 (November 2014)
-------------------------

COMPILER
* fixed bugs in XGMTool.
* removed linear interpolation when converting WAV file.
* others minors changes.


SGDK 1.00 (November 2014)
-------------------------

COMPILER
* Rescomp:
 - added support to XGM resource in rescomp.
* added xgmtool to convert VGM into XGM and compile XGM file.
* removed Genitile tool sources.
* updated wavtoraw to support sample interpolation.

LIBRARY
* VDP:
 - added VDP_setScanMode(u16 mode) method to change the interlaced mode.
 - added VDP_interruptFade() method to interrupt async palette fading.
* SPRITE:
 - fixed a bug with SPR_init(..) method when using same definition, sometime timer could be not reseted and then animation is not working anymore.
 - fixed declaration of VDP_setSprite(..) / VDP_setSpriteDirect(..) / VDP_setSpritePosition(..) methods.
* SOUND:
 - Z80 memory is cleared before loading a custom driver.
 - added Z80_read(..) and Z80_write(..) methods for simple Z80 RAM read/write operations.
 - added XGM driver methods:
   u8 SND_isPlaying_XGM();
   void SND_startPlay_XGM(const u8 *song);
   void SND_stopPlay_XGM();
   void SND_resumePlay_XGM();
   u8 SND_isPlayingPCM_XGM(const u16 channel_mask);
   void SND_setPCM_XGM(const u8 id, const u8 *sample, const u32 len);
   void SND_setPCMFast_XGM(const u8 id, const u8 *sample, const u32 len);
   void SND_startPlayPCM_XGM(const u8 id, const u8 priority, const u16 channel);
   void SND_stopPlayPCM_XGM(const u16 channel);
* MATH:
 - minor fix in min/max defines.
* CONTROLLER:
 - fixed declaration of JOY_readJoypadX/Y(..) methods (they should return s16 and not u16)
* SYSTEM:
 - sega.s and rom_head.c files are now copied into the 'src/boot' project folder so they can easily be customized per project.
* SAMPLE:
 - modified sound sample to add XGM driver example.
* some fixes in the doxygen documentation.
* others changes and improvements.


SGDK 0.96d (june 2014)
----------------------

LIBRARY
* removed direct VRam Map data unpacking as it was buggy.


SGDK 0.96c (june 2014)
----------------------

COMPILER
* Rescomp:
 - fixed some issues on compression.
 - now accept string to define which compression to use (AUTO, APLIB, RLE) in resource definition. 

LIBRARY
* minors changes to Doxygen documentation. 


SGDK 0.96 (may 2014)
--------------------

COMPILER
* Rescomp:
 - improved BMP image support.
 - byte data are now output in word format to avoid the GCC bug (compilation with "-g" flag fails when byte data is encountered).
 - fixed issue on empty sprite animation detection.
 - fixed path separator issue on old windows system.
 - fixed compilation issues on unix system.
* Makefile:
 - Added "release" and "debug" target to makefile (default is "release").
  "debug" target allow you to use GDB interactive debugger through emulator supporting it.
* added appack tool sources code (compatibility for linux system).
* some cleanup in bintos tool (removed the useless sizealign command done by sizebnd tool).

LIBRARY
* VDP:
 - fixed palette fading methods where the last frame colors weren't always correct.
 - VDP_drawImage(..) and VDP_drawBitmap(..) now use dynamic VRAM tile index so they does not erase anymore the previous drawn image.
   'curTileInd' variable which contains the VRAM tile index where next tile will be uploaded is public.
* TILE:
 - fixed somes bugs in the tile cache engine.
 - removed the MEM_free(..) call from the VInt callback (tile cache engine).
   So we don't need anymore to disable interrupts at each memory allocation operation for safety ;)
* SPRITE:
 - fixed somes bugs in the sprite engine. 
 - added SPR_setAlwaysVisible(..) and SPR_setNeverVisible(..) to force (not) visibility on sprite (sprite engine)
* BITMAP:
 - minor performance improvement on the BMP_drawPolygon(..) method.
* SOUND:
 - fixed YM2612 write methods (can have issue on MD2 system).
* DMA:
 - fixed VRam Copy DMA.
* MATH:
- fixed 2D projection calculation in M3D_project_xxx(..) methods.
  Now the camera distance is correctly taken in account for the final projection (adding a minor impact on performance).
* SYSTEM:
 - fixed soft reset issues.
* TIMER:
 - fixed a minor issue with getTime(..) method.
 - waitSubTick(..) is now more accurate when called from V-Interrupt code.
* SAMPLE:
 - minors changes and improvements on the Sonic Sprite sample.
 - minors changes to 3D cube flat sample.
* DEBUG:
 - added some KDebug log methods (KLog, KLog_Uxx, KLog_Sxx...)
* added strcmp(..) method.
* others changes and improvements.


SGDK 0.95 (feb 2014)
--------------------

COMPILER
* Major change on resource compilation:
  A new resource compiler tool (rescomp) is used to compile all resource files.
  It support many type of resources as BMP, PNG, WAV, PCM, VGM, TFM..
  Read the rescomp.txt file to have more informations about it and look in the 'sound' and 'sprite' sample example.
  You can also convert your old project by using the "rescomp -convert" command on the project folder to convert.
- added appack tool.
- minor fix in wavtoraw tool.

LIBRARY
* added GFX compression support (see tools.h file for doxygen documentation).
* VDP:
 - added VDP_getBackgroundColor() and VDP_setBackgroundColor(..) methods.
 - added VDP_get/setTextPlan(), VDP_get/setTextPalette() and VDP_get/setTextPriority() methods to change text drawing properties.
 - added VDP_drawBitmap(..) and VDP_drawBitmapEx(..) methods to draw Bitmap resource.
 - added VDP_drawImage(..) and VDP_drawImageEx(..) methods to draw Image resources.
 - added VDP_loadTileSet(..) method to load TileSet resource.
 - added VDP_setMap(..) and VDP_setMapEx(..) methods to load Map resources.
 - lot of refactoring in the setTileMap methods...
 - some tweaks.
* TILE:
 - added new tile cache engine for easier tile allocation in VRAM.
   See the tile_cache.h file for doxygen documentation.
* SPRITE:
 - added sprite engine for easier sprite manipulation.
   See the sprite_eng.h file for doxygen documentation and the included "sprite" sample for an example.
* SOUND:
 - added TFC_isPlaying() command for the 68k TFM driver.
 - fixed issue with consecutive play command on TFM Z80 driver.
 - Improved VGM driver (thanks to kubilus1 and sigflup).
   You can now pause, resume music and even better play PCM SFX !
 - now clear Z80 memory on driver loading to avoid any problems with var initialization.
* DMA:
  - minor change to VDP_doDMAEx(..) method, be careful if you use this method, the last parameter changed.
* JOY:
 - added JOY_waitPressTime() and JOY_waitPressBtnTime() methods.
 - JOY_waitPressBtn() and JOY_waitPress() now return button pressed info.
* MEMORY:
 - fixed a minor buf with dynamic memory allocation.
* SYSTEM:
 - added SYS_disableInts() and SYS_enableInts() methods.
 - added SYS_isInInterrupt() to detect if we are in an interrupt callback.
 - added SYS_getAndSetInterruptMaskLevel(..) method to "atomically" get and set interrupt mask level.
 - added SYS_die(..) for fatal error handling.
* library font is now included as an image.
* added some logs for easier debugging (KDebug message).
* many others smalls improvements.
* lot of refactoring.


SGDK 0.94 (feb 2013):
---------------------

* Major rewrite of the Bitmap engine:
 - Fixed 256x160 resolution.
 - Removed all specifics flags as it now always use deferred flip operation with extended blank.
 - Simpler and easier to use.
 - Better performance (20 FPS in NTSC, 25 FPS in PAL).
 - Backface culling now directly handled in the BMP_drawPolygon(..) method.
 - Removed useless FF BMP engine (too complex, incomplete..)
 - Many others changes you will discover :)
* Added "Bitmap" structure for better bitmap handling.
  SGDK automatically convert 16 colors bitmap images to "Bitmap".
* Major rewrite of Maths3D engine:
  - Added many 3D related structures (as matrix, transform..).
  - More flexibility.
  - Improved performance (not much).
* added DMA capability to VDP_setHorizontalScrollxxx(..) / VDP_setVerticalScrollTile(..) functions.
* Added VDP_doDMAEx(..) so we can specify if we modify the VRam step.
* Refactored palette functions.
* Fixed QSort function.
* Removed useless VRAM table (eat rom space for minor speed boost).
* Minors fixes/tweaks in memset and memcpy functions.
* Updated WavToRaw tool (now support any output rate).
* Added Genitile 1.7 sources.
* Minors tweaks on makefile.
* Fixed a minor issue in rom_head (Thanks Chilly Willy).
* Updated demo samples.
* Others minors changes or improvements.
````

## File: readme.md
````markdown
# SGDK 2.11 (April 2025)
#### Copyright 2025 Stephane Dallongeville
Github: https://github.com/Stephane-D/SGDK<br>
Patreon: https://www.patreon.com/SGDK

**SGDK** is a free development kit allowing development of software in **C language** for the **Sega Mega Drive**.
It contains the development library itself (with the code sources) and some custom tools used to compile resources.
SGDK uses the GCC compiler (m68k-elf target) and the libgcc to generate a ROM image. Binaries (GCC 13.2) are provided for Windows OS for convenience, but you need to install it by yourself for other operating systems (more infos about it in *HELP AND SUPPORT* section).
Note that SGDK also requires Java (custom tools require it), so you need to have Java installed on your system.

SGDK library and custom tools are distributed under the MIT license (see [license.txt](license.txt) file).
GCC compiler and libgcc are under GNU license (GPL3) and any software build from it (as the SGDK library) is under the GCC runtime library exception license (see [COPYING.RUNTIME](COPYING.RUNTIME) file)
 
## GETTING STARTED

First, you need to know that SGDK uses the C language (assembly is also possible, but not necessary) so it's highly recommended to be familiar with C programming before trying to develop with SGDK. Learning C language at same time as learning 'Sega Mega Drive' programming is definitely too difficult and you will end up getting nowhere. It's also important to have, at least, a basic knowledge of the Sega Mega Drive hardware (specifically the video system). 

### MEGA DRIVE TECHNICAL INFO REFERENCES

* And-0 - Awesome Mega Drive Development references:<br>
https://github.com/And-0/awesome-megadrive
* Raster Scroll - Sega Mega Drive Graphics guide:<br>
https://rasterscroll.com/mdgraphics/
* Mega Cat Studios - Sega Mega Drive graphics guide:<br>
https://megacatstudios.com/blogs/retro-development/sega-genesis-mega-drive-vdp-graphics-guide-v1-2a-03-14-17
* Sik's Blog dedicated to MD assembly programming but explain a lot of stuff (and in a nice way) about the Sega Mega Drive hardware:<br>
https://plutiedev.com
* A nice article from Rodrigo Copetti explaining the Mega Drive architecture:<br>
https://www.copetti.org/projects/consoles/mega-drive-genesis
* Genesis Software Manual which contains absolutely everything you need to know about the Sega Mega Drive:<br>
https://segaretro.org/images/a/a2/Genesis_Software_Manual.pdf

### INSTALLATION AND DOCUMENTATION 

Then when you feel ready you can go further and finally install SGDK :)

You can find installation instructions and tutorials about how use SGDK on the wiki:<br>
https://github.com/Stephane-D/SGDK/wiki

SGDK also comes with an online doxygen documentation (generated from .h header files) which provides descriptions for all SGDK structures and functions:<br>
http://stephane-d.github.io/SGDK/

It's important to know that SGDK heavily relies on _resources_ which are compiled through the _rescomp_ tool. You should read the [rescomp.txt](https://raw.githubusercontent.com/Stephane-D/SGDK/master/bin/rescomp.txt) file to understand **which kind of resources you can use and how to declare them**. Then you can look at the *'sample'* folder from SGDK and, in particular, the [sonic sample](https://github.com/Stephane-D/SGDK/tree/master/sample/game/sonic) which is a good showcase of SGDK usage in general (functions and resources).

### OTHERS TUTORIALS

You can find a lot of tutorials online about SGDK but be careful, some are outdated and/or they contains errors.<br>
I really recommend starting from the [wiki](https://github.com/Stephane-D/SGDK/wiki) which provides the basics to start.<br>
Then if you need more _visual_ and complete tutorials you can give either of these a try:<br>
MD programming tutorials from Ohsat:<br>
https://www.ohsat.com/tutorial/mdmisc/creating-graphics-for-md/<br>
Very complete and visual tutorials from Danibus (spanish only):<br>
https://danibus.wordpress.com/<br>
Youtube tutorial series from Pigsy:<br>
https://www.youtube.com/watch?v=BnGqc5OTTY4&list=PL1xqkpO_SvY2_rSwHTBIBxXMqmek--GAb<br>

### USEFUL TOOLS

Here's a list of useful tools that will help you to create your assets:

#### Pixel art / Sprite

* [Asesprite](https://www.aseprite.org/)
* [Graphics gale](https://graphicsgale.com/)
* [Pro Motion](https://www.cosmigo.com/)
* [Grafx2](http://grafx2.chez.com/)
* [Palette quantizer](https://rilden.github.io/tiledpalettequant/): powerful online tool to convert your image within the Sega Mega Drive palette constraints

#### Map / Level

* [16Tile](https://allone-works.itch.io/16tile): SGDK specific tile map editor
* [PNGPalPrio4SGDK](https://github.com/Rahzelk/pngpalprio4sgdk): SGDK specific PNG image editor letting you to setup the tile palette index and priority information easily
* [Tiled](https://www.mapeditor.org/): Tile map editor, supported by SGDK resource compiler tool
* [LDTK](https://deepnight.net/tools/ldtk-2d-level-editor/): New but powerful tile map editor (support Tiled formats as well)
* [Ogmo](https://ogmo-editor-3.github.io/): Another nice and simple tile map editor
* [Palette merger](https://github.com/bolon667/paletteMergerForSGDK): A tool allowing to easily change tile palette index and priority

#### Music / sound

* [Deflemask](https://www.deflemask.com/): music tracker supporting several system as the Sega Mega Drive
* [Furnace](https://github.com/tildearrow/furnace): new music tracker supporting many systems
* [Wavosaur](https://www.wavosaur.com/): wave sound editor (windows only)
* [Audacity](https://www.audacityteam.org/): wave sound editor (all OS)

#### Alternates sound drivers

SGDK sound drivers may not fit your needs so it's important to know that some alternatives exist:
* [MDSDRV](https://github.com/superctr/MDSDRV): powerful sound driver support FM & PSG SFX, pitchable PCM playback...
* [Echo](https://github.com/sikthehedgehog/Echo): another sound driver supporting FM & PSG SFX and working 100% on Z80 CPU.
* [MiniMusic](https://github.com/sikthehedgehog/minimusic): a tiny Z80 sound driver for Mega Drive that runs entirely off Z80 RAM
and doesn't access the cartridge slot.
 
 
## HELP AND SUPPORT

If you need help or support with SGDK, you can join the SGDK Discord server:<br>
https://discord.gg/xmnBWQS

You can also go to the Spritesmind forum which is dedicated to Sega Mega Drive development and has a specific section for SGDK:<br>
http://gendev.spritesmind.net/forum/
 
### LINUX / MACOSX

There are several solutions to get SGDK working on Linux, here's a list of some of them:<br>

* Very simple script from Vojtěch Salajka allowing you to use SGDK from Wine easily:<br>
https://github.com/Franticware/SGDK_wine

* The SGDK Helper from tlsa allows you to use SGDK both from a native or a container environment:<br>
https://github.com/tlsa/sgdk-helper

* Another great alternative is to use the complete _MarsDev_ environment developed by Andy Grind:<br>
https://github.com/andwn/marsdev
It supports all OSes, provides SGDK compatibility as well as 32X support, so be sure to check it out !

* MacOSX users also have access to this specific solution from Sonic3D (probably a bit outdated though):<br>
https://github.com/SONIC3D/gendev-macos

### DOCKER

SGDK doesn't provide anymore any integrated Docker solution as the Doragasu docker solution seems better, faster and simpler to use (native linux compiler):<br>
https://gitlab.com/doragasu/docker-sgdk
 
### VS CODE

Almost everyone now use VSCode editor and the nice extension made by zerasul called *Genesis-Code* makes SGDK integration even smoother:<br>
https://marketplace.visualstudio.com/items?itemName=zerasul.genesis-code
Just install and setup it as it's detailled on the extension page ('GDK/SGDK' and 'Gens' emulator paths mainly) and you're ready to use it :)<br>
You can type *>Genesis* in the command palette (Ctrl+Shift+P shortcut) to see all commands supported by the extension.<br>
SGDK also now integrates a VSCode template located in _project/template_ folder so you can just create a copy of this folder (for instance _project/test_) and open it in VSCode to start your own project.

### AWS

Yes you're reading correctly, thanks to Matthew Nimmo you can even now use SGDK from AWS !<br>
Here's the link to his blog which explain how to do that:<br>
https://community.aws/content/2hZInsYwWX8cYEAIFKquutVoYQj/how-to-build-retro-games-in-aws-using-sega-genesis-development-kit-sgdk

 
## SUPPORT SGDK

SGDK is free but you can support it on Patreon: https://www.patreon.com/SGDK

## THANKS

- Doragasu for the multi-tasking engine (based on Sik implementation), MegaWifi, flash save support, docker and others nice additions.
- PaCHoN for its great work on MegaWifi Addon.
- Sik for the multi-tasking base implementation and for all its unvaluable Plutidev Sega Mega Drive technical information source. 
- Chilly Willy for making almost all the JOY / controller support in SGDK (and the joy test sample ^^).
- Vladikcomper for the advanced error handler capable of displaying human readable backtrace informations :)
- Astrofra for the starfield donut sample and the revamped readme ;)
- Gunpog for making the 'cute' SGDK logo.
- Adrian Spoz for making the 'professional' SGDK logo.
- Vojtěch Salajka for the script allowing to use SGDK easily from Wine (Linux/Unix but may work on OSX too).
- Andy Grind for the MarsDev project allowing to use SGDK on any OS and also supporting 32X dev.
- Sonic3D for the GenDev OSX port of SGDK.
- Andreas Dietrich for the nice Wobbler & scaling effect samples.
- werton for the numerous fixes / contributions.
- clbr for various contributions.
- jgyllinsky for providing / improving build batches.
- nolddor for fixes / contributions.
- starling13 for fixes.
- davidgf for its contributions (improved assembly LTO optimization).
- ShiningBzh / Jeremy and Kentosama for their precious help in testing.
- all those I forgot and generally all people who contributed in any form to SGDK !

## SPECIAL THANKS

Of course I thank all my patreon for their continuous support but I want to dedicace a very special and warmfull thanks for generous donation from:

- Bitmap Bureau (Xeno Crisis team)
- Neofid Studios (Demons of Asteborg team)
- Dmitry (D I)
- Rajen Savjani

## POWERED BY THE SGDK!

These projects are known to be based on the SGDK _(non-exhaustive list)_:

![alt text](doc/img/game_tanzer.gif)
 
Tanzer by [Mega Cat Studios](https://megacatstudios.com/products/tanzer-sega-genesis)

![alt text](doc/img/demo_masiaka.gif)
 
MASIAKA by [Resistance](https://www.pouet.net/prod.php?which=71543)

![alt text](doc/img/game_xenocrisis.gif)
 
Xeno Crisis by the [Bitmap Bureau](https://www.bitmapbureau.com/)

![alt text](doc/img/game_doa.gif)
 
Demons of Asteborg [Neofid Studios](https://neofid-studios.com/)

### Random list of SGDK-powered games and demos

* [2048](https://github.com/atamurad/sega-2048) by atamurad
* [Abbaye des Morts (l')](https://playonretro.itch.io/labbaye-des-morts-megadrivegenesis-por-002) unofficial MD port by Moon-Watcher
* [Art of LeonBli (the)](https://www.pouet.net/prod.php?which=72272) by Resistance
* [Barbarian](https://www.youtube.com/watch?v=e8IIfNLXzAU) unofficial MD port by Z-Team
* [Demons of Asteborg](https://demonsofasteborg.com/) by Neofid Studio
* [Devwill Too MD](https://amaweks.itch.io/devwill-too-md) by Amaweks
* [Fatal Smarties](https://globalgamejam.org/2016/games/fatal-smarties) made for the GGJ 2016
* [Fullscreen NICCC 2000](https://www.pouet.net/prod.php?which=81136) by Resistance
* [IK+ Deluxe](https://www.youtube.com/watch?v=mcm0TRsOwuw) unofficial MD port by Z-Team
* [Irena](https://white-ninja.itch.io/irena-genesis-metal-fury) by White Ninja Studio
* [Omega Blast](https://nendo16.jimdofree.com/omega-blast/) by Nendo
* [Perlin & Pinpin](https://lizardrive.itch.io/perlin-pinpin-episode1) by Lizardrive
* [Right 2 Repair](https://supermegabyte.itch.io/right-2-repair) by Super Megabyte made for the GGJ 2020
* [Return to Genesis](https://www.youtube.com/watch?v=jjy0Iz_64dY) unofficial MD port by Z-Team
* [Road to Valhalla](https://www.pouet.net/prod.php?which=72961) by Bounty/Banana & Resistance
* [Spiral (the)](https://www.pouet.net/prod.php?which=82607) by Resistance
* [Tetris MD](https://github.com/NeroJin/TetrisMD) unofficial MD port by Nero Jin
* [Travel](https://www.pouet.net/prod.php?which=65975) by Resistance
* [Wacky Willy Weiner Sausage Surfer](https://globalgamejam.org/2017/games/wacky-willy-weiner-sausage-surfer) made for the GGJ 2017
````
