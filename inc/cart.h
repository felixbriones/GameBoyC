#include "gb.h"

// Memory map
#define ADDR_ENTRY_START  	0x0100
#define ADDR_ENTRY_END 	  	0x0103
#define ADDR_LOGO_START	  	0x0104
#define ADDR_LOGO_END	  	0x0133
#define ADDR_TITLE_START  	0x0134
#define ADDR_TITLE_END    	0x0143
#define ADDR_MANUFACTURER_START 0x013F
#define ADDR_MANUFACTURER_END   0x0142
#define ADDR_CGB_FLAG	  	0x0143 // Color Flag (For use with Gameboy Color)
#define ADDR_NEW_LICENSEE_START 0x0144
#define ADDR_NEW_LICENSEE_END   0x0145
#define ADDR_SGB_FLAG		0x0146 // Specifies if game supports SGB function
#define ADDR_CART_TYPE		0x0147 // Specifies hardware on cartridge
#define ADDR_ROM_SIZE		0x0148 // Specifies how much ROM is on cart 
#define ADDR_RAM_SIZE		0x0149 // Specifies how much RAM is on cart (if any)
#define ADDR_DESTINATION_CODE   0x014A // Specifies if cart is intended to be sold in Japan or elsewhere
#define ADDR_OLD_LICENSEE_START 0x014B // Used in older (Pre-SGB) carts to specify publisher
#define ADDR_ROM_VERSION_NUMBER 0x014C // Used to specify version number of the game
#define ADDR_HEADER_CHECKSUM    0x014D // Contains checksum computed from 0x0134 - 0x014C. Boot ROM will lock if checksum fails

// Function prototypes
void cartLoadRom(gameBoy_t* gb, const char* gameRom);
