/*
 * at45dbx.h
 *
 * Created: 05.10.2012 12:41:26
 *  Author: f-ruml
 */ 


#ifndef AT45DBX_H_
#define AT45DBX_H_

 #ifdef __cplusplus
 extern "C" {
	 #endif
	
	 //_____ D E F I N I T I O N S ______________________________________________
	
	
	 #define AT45DBX_500KB           19
	 #define AT45DBX_1MB             20
	 #define AT45DBX_2MB             21
	 #define AT45DBX_4MB             22
	 #define AT45DBX_8MB             23
	
	
	 #define AT45DBX_SECTOR_BITS     9
	
	 #define AT45DBX_SECTOR_SIZE     (1 << AT45DBX_SECTOR_BITS)
	
	
	 //_____ D E C L A R A T I O N S ____________________________________________
	
	
	
	 extern bool at45dbx_init(void);
	
	 extern bool at45dbx_mem_check(void);
	
	 extern bool at45dbx_read_sector_open(uint32_t sector);
	
	 extern bool at45dbx_read_byte_open(uint32_t ad);
	
	
	 extern void at45dbx_read_close(void);
	
	 extern bool at45dbx_write_sector_open(uint32_t sector);
	
	 extern bool at45dbx_write_byte_open(uint32_t ad);
	
	 extern void at45dbx_write_close(void);
	
	
	
	
	
	 extern uint8_t at45dbx_read_byte(void);
	
	 extern bool at45dbx_write_byte(uint8_t b);
	
	
	
	
	
	 extern bool at45dbx_read_multiple_sector(uint16_t nb_sector);
	
	 extern void at45dbx_read_multiple_sector_callback(const void *psector);
	
	 extern bool at45dbx_write_multiple_sector(uint16_t nb_sector);
	
	 extern void at45dbx_write_multiple_sector_callback(void *psector);
	
	
	
	
	
	 extern bool at45dbx_read_sector_to_ram(void *ram);
	
	 extern bool at45dbx_write_sector_from_ram(const void *ram);
	
	 extern void at45dbx_spi_init(void);
	 extern void at45dbx_spi_select_device(uint8_t mem_id);
	 extern void at45dbx_spi_deselect_device(uint8_t mem_id);
	 extern void at45dbx_spi_write_byte(uint8_t data);
	 extern void at45dbx_spi_read_byte(uint8_t *data);
	 extern void at45dbx_spi_read_packet(void const *data, size_t len);
	 extern void at45dbx_spi_write_packet(void const *data, size_t len);
	
	
	 #ifdef __cplusplus
 }
 #endif

#endif  // _AT45DBX_H_
