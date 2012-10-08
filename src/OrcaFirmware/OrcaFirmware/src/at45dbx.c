/*
 * at45dbx.c
 *
 * Created: 05.10.2012 12:32:05
 *  Author: f-ruml
 */ 

 //_____  I N C L U D E S ___________________________________________________


 #include "compiler.h"
 #include "at45dbx.h"
 #include "at45dbx_hal_spi.h"


 #if AT45DBX_MEM_CNT > 4
     #error AT45DBX_MEM_CNT must not exceed 4
 #endif


 //_____ D E F I N I T I O N S ______________________________________________


 #define AT45DBX_CMDA_RD_PAGE              0xD2        //!< Main Memory Page Read (Serial/8-bit Mode).
 #define AT45DBX_CMDA_RD_ARRAY_LEG         0xE8        //!< Continuous Array Read, Legacy Command (Serial/8-bit Mode).
 #define AT45DBX_CMDA_RD_ARRAY_LF_SM       0x03        //!< Continuous Array Read, Low-Frequency Mode (Serial Mode).
 #define AT45DBX_CMDA_RD_ARRAY_AF_SM       0x0B        //!< Continuous Array Read, Any-Frequency Mode (Serial Mode).
 #define AT45DBX_CMDA_RD_SECTOR_PROT_REG   0x32        //!< Read Sector Protection Register (Serial/8-bit Mode).
 #define AT45DBX_CMDA_RD_SECTOR_LKDN_REG   0x35        //!< Read Sector Lockdown Register (Serial/8-bit Mode).
 #define AT45DBX_CMDA_RD_SECURITY_REG      0x77        //!< Read Security Register (Serial/8-bit Mode).



 #define AT45DBX_CMDB_ER_PAGE              0x81        //!< Page Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_ER_BLOCK             0x50        //!< Block Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_ER_SECTOR            0x7C        //!< Sector Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_ER_CHIP              0xC794809A  //!< Chip Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_XFR_PAGE_TO_BUF1     0x53        //!< Main Memory Page to Buffer 1 Transfer (Serial/8-bit Mode).
 #define AT45DBX_CMDB_XFR_PAGE_TO_BUF2     0x55        //!< Main Memory Page to Buffer 2 Transfer (Serial/8-bit Mode).
 #define AT45DBX_CMDB_CMP_PAGE_TO_BUF1     0x60        //!< Main Memory Page to Buffer 1 Compare (Serial/8-bit Mode).
 #define AT45DBX_CMDB_CMP_PAGE_TO_BUF2     0x61        //!< Main Memory Page to Buffer 2 Compare (Serial/8-bit Mode).
 #define AT45DBX_CMDB_PR_BUF1_TO_PAGE_ER   0x83        //!< Buffer 1 to Main Memory Page Program with Built-in Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_PR_BUF2_TO_PAGE_ER   0x86        //!< Buffer 2 to Main Memory Page Program with Built-in Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_PR_BUF1_TO_PAGE      0x88        //!< Buffer 1 to Main Memory Page Program without Built-in Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_PR_BUF2_TO_PAGE      0x89        //!< Buffer 2 to Main Memory Page Program without Built-in Erase (Serial/8-bit Mode).
 #define AT45DBX_CMDB_PR_PAGE_TH_BUF1      0x82        //!< Main Memory Page Program through Buffer 1 (Serial/8-bit Mode).
 #define AT45DBX_CMDB_PR_PAGE_TH_BUF2      0x85        //!< Main Memory Page Program through Buffer 2 (Serial/8-bit Mode).
 #define AT45DBX_CMDB_RWR_PAGE_TH_BUF1     0x58        //!< Auto Page Rewrite through Buffer 1 (Serial/8-bit Mode).
 #define AT45DBX_CMDB_RWR_PAGE_TH_BUF2     0x59        //!< Auto Page Rewrite through Buffer 2 (Serial/8-bit Mode).



 #define AT45DBX_CMDC_RD_BUF1_LF_SM        0xD1        //!< Buffer 1 Read, Low-Frequency Mode (Serial Mode).
 #define AT45DBX_CMDC_RD_BUF2_LF_SM        0xD3        //!< Buffer 2 Read, Low-Frequency Mode (Serial Mode).
 #define AT45DBX_CMDC_RD_BUF1_AF_SM        0xD4        //!< Buffer 1 Read, Any-Frequency Mode (Serial Mode).
 #define AT45DBX_CMDC_RD_BUF2_AF_SM        0xD6        //!< Buffer 2 Read, Any-Frequency Mode (Serial Mode).
 #define AT45DBX_CMDC_RD_BUF1_AF_8M        0x54        //!< Buffer 1 Read, Any-Frequency Mode (8-bit Mode).
 #define AT45DBX_CMDC_RD_BUF2_AF_8M        0x56        //!< Buffer 2 Read, Any-Frequency Mode (8-bit Mode).
 #define AT45DBX_CMDC_WR_BUF1              0x84        //!< Buffer 1 Write (Serial/8-bit Mode).
 #define AT45DBX_CMDC_WR_BUF2              0x87        //!< Buffer 2 Write (Serial/8-bit Mode).
 #define AT45DBX_CMDC_RD_STATUS_REG        0xD7        //!< Status Register Read (Serial/8-bit Mode).
 #define AT45DBX_CMDC_RD_MNFCT_DEV_ID_SM   0x9F        //!< Manufacturer and Device ID Read (Serial Mode).



 #define AT45DBX_CMDD_EN_SECTOR_PROT       0x3D2A7FA9  //!< Enable Sector Protection (Serial/8-bit Mode).
 #define AT45DBX_CMDD_DIS_SECTOR_PROT      0x3D2A7F9A  //!< Disable Sector Protection (Serial/8-bit Mode).
 #define AT45DBX_CMDD_ER_SECTOR_PROT_REG   0x3D2A7FCF  //!< Erase Sector Protection Register (Serial/8-bit Mode).
 #define AT45DBX_CMDD_PR_SECTOR_PROT_REG   0x3D2A7FFC  //!< Program Sector Protection Register (Serial/8-bit Mode).
 #define AT45DBX_CMDD_LKDN_SECTOR          0x3D2A7F30  //!< Sector Lockdown (Serial/8-bit Mode).
 #define AT45DBX_CMDD_PR_SECURITY_REG      0x9B000000  //!< Program Security Register (Serial/8-bit Mode).
 #define AT45DBX_CMDD_PR_CONF_REG          0x3D2A80A6  //!< Program Configuration Register (Serial/8-bit Mode).
 #define AT45DBX_CMDD_DEEP_PWR_DN          0xB9        //!< Deep Power-down (Serial/8-bit Mode).
 #define AT45DBX_CMDD_RSM_DEEP_PWR_DN      0xAB        //!< Resume from Deep Power-down (Serial/8-bit Mode).




 #define AT45DBX_MSK_BUSY                  0x80        //!< Busy status bit-mask.
 #define AT45DBX_BUSY                      0x00        //!< Busy status value (0x00 when busy, 0x80 when ready).
 #define AT45DBX_MSK_DENSITY               0x3C        //!< Device density bit-mask.


 #if AT45DBX_MEM_SIZE==AT45DBX_500KB


 #define AT45DBX_DENSITY                   0x1C        //!< Device density value.
 #define AT45DBX_BYTE_ADDR_BITS            9           //!< Address bits for byte position within buffer.


 #elif AT45DBX_MEM_SIZE == AT45DBX_2MB


 #define AT45DBX_DENSITY                   0x2C        //!< Device density value.
 #define AT45DBX_BYTE_ADDR_BITS            10          //!< Address bits for byte position within buffer.


 #elif AT45DBX_MEM_SIZE == AT45DBX_4MB


 #define AT45DBX_DENSITY                   0x34        //!< Device density value.
 #define AT45DBX_BYTE_ADDR_BITS            10          //!< Address bits for byte position within buffer.


 #elif AT45DBX_MEM_SIZE == AT45DBX_8MB


 #define AT45DBX_DENSITY                   0x3C        //!< Device density value.
 #define AT45DBX_BYTE_ADDR_BITS            11          //!< Address bits for byte position within buffer.


 #else
     #error AT45DBX_MEM_SIZE is not defined to a supported value
 #endif

 #define AT45DBX_PAGE_ADDR_BITS            (AT45DBX_MEM_SIZE - AT45DBX_PAGE_BITS)

 #define AT45DBX_PAGE_BITS                 (AT45DBX_BYTE_ADDR_BITS - 1)

 #define AT45DBX_PAGE_SIZE                 (1 << AT45DBX_PAGE_BITS)

 #define AT45DBX_MSK_PTR_BYTE              ((1 << AT45DBX_PAGE_BITS) - 1)

 #define AT45DBX_MSK_PTR_PAGE              (((1 << AT45DBX_PAGE_ADDR_BITS) - 1) << AT45DBX_PAGE_BITS)

 #define AT45DBX_MSK_PTR_SECTOR            ((1 << AT45DBX_SECTOR_BITS) - 1)


 #define spi_write_dummy()                 spi_write(AT45DBX_SPI, 0xFF)


 static bool at45dbx_busy;

 static uint32_t at45dbx_gl_ptr_mem;

 static uint8_t sector_buf[AT45DBX_SECTOR_SIZE];



 bool at45dbx_init(void)
 {
	     at45dbx_spi_init();
	     // Memory ready by default.
	     at45dbx_busy = false;
	     return true;
 }


 static void at45dbx_chipselect_df(uint8_t memidx, bool bSelect)
 {
	     if (bSelect) {
		         // Select SPI chip.
		         at45dbx_spi_select_device(memidx);
	     }
	     else {
		      // Unselect SPI chip.
		         at45dbx_spi_deselect_device(memidx);
	     }
 }


 bool at45dbx_mem_check(void)
 {
	     uint8_t df;
	     uint8_t status = 0;
	
	     // DF memory check.
	     for (df = 0; df < AT45DBX_MEM_CNT; df++) {
		         // Select the DF memory to check.
		         at45dbx_chipselect_df(df, true);
		         // Send the Status Register Read command.
		         at45dbx_spi_write_byte(AT45DBX_CMDC_RD_STATUS_REG);
		         // Send a dummy byte to read the status register.
		             at45dbx_spi_read_byte(&status);
		         // Unselect the checked DF memory.
		         at45dbx_chipselect_df(df, false);
		         // Unexpected device density value.
		         if ((status & AT45DBX_MSK_DENSITY) < AT45DBX_DENSITY) return false;
	     }
	     return true;
 }


 static void at45dbx_wait_ready(void)
 {
	     uint8_t status;
	
	     // Select the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, true);
	
	     // Send the Status Register Read command.
	     at45dbx_spi_write_byte(AT45DBX_CMDC_RD_STATUS_REG);
	     // Read the status register until the DF is ready.
	     do {
		         // Send a dummy byte to read the status register.
		         at45dbx_spi_read_byte(&status);
	     } while ((status & AT45DBX_MSK_BUSY) == AT45DBX_BUSY);
	
	     // Unselect the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
 }

 bool at45dbx_read_sector_open(uint32_t sector)
 {
	     return at45dbx_read_byte_open(sector << AT45DBX_SECTOR_BITS);   //sector * AT45DBX_SECTOR_SIZE.
 }

 bool at45dbx_read_byte_open(uint32_t ad)
 {
	     uint32_t addr;
	
	     // Set the global memory pointer to a byte address.
	     at45dbx_gl_ptr_mem = ad;
	
	     // If the DF memory is busy, wait until it's ready.
	     if (at45dbx_busy) at45dbx_wait_ready();
	     at45dbx_busy = false;
	
	     // Select the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, true);
	
	     // Initiate a page read at a given sector.
	
	     // Send the Main Memory Page Read command.
	     at45dbx_spi_write_byte(AT45DBX_CMDA_RD_PAGE);
	
	     // Send the three address bytes, which comprise:
	     //  - (24 - (AT45DBX_PAGE_ADDR_BITS + AT45DBX_BYTE_ADDR_BITS)) reserved bits;
	     //  - then AT45DBX_PAGE_ADDR_BITS bits specifying the page in main memory to be read;
	     //  - then AT45DBX_BYTE_ADDR_BITS bits specifying the starting byte address within that page.
	     // NOTE: The bits of at45dbx_gl_ptr_mem above the AT45DBX_MEM_SIZE bits are useless for the local
	     // DF addressing. They are used for DF discrimination when there are several DFs.
	     addr = (Rd_bitfield(at45dbx_gl_ptr_mem, (uint32_t)AT45DBX_MSK_PTR_PAGE) << AT45DBX_BYTE_ADDR_BITS) |
	         Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_BYTE);
	
	     at45dbx_spi_write_byte(LSB2W(addr));
	     at45dbx_spi_write_byte(LSB1W(addr));
	     at45dbx_spi_write_byte(LSB0W(addr));
	
	     // Send 32 don't care clock cycles to initialize the read operation.
	     at45dbx_spi_write_byte(0x55);
	     at45dbx_spi_write_byte(0x55);
	     at45dbx_spi_write_byte(0x55);
	     at45dbx_spi_write_byte(0x55);
	     return true;
 }


 void at45dbx_read_close(void)
 {
	     // Unselect the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
	     // Memory ready.
	     at45dbx_busy = false;
 }

 bool at45dbx_write_sector_open(uint32_t sector)
 {
	     // Set the global memory pointer to a byte address.
	     return at45dbx_write_byte_open(sector << AT45DBX_SECTOR_BITS); // at45dbx_gl_ptr_mem = sector * AT45DBX_SECTOR_SIZE.
 }

 bool at45dbx_write_byte_open(uint32_t ad)
 {
	     volatile uint32_t addr;
	    at45dbx_gl_ptr_mem = ad;
	
	     // If the DF memory is busy, wait until it's ready.
	     if (at45dbx_busy) {
		         at45dbx_wait_ready();
	     }
	     at45dbx_busy = false;
	
	 #if AT45DBX_PAGE_SIZE > AT45DBX_SECTOR_SIZE
	     // Select the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, true);
	
	     // Transfer the content of the current page to buffer 1.
	
	     // Send the Main Memory Page to Buffer 1 Transfer command.
	     at45dbx_spi_write_byte(AT45DBX_CMDB_XFR_PAGE_TO_BUF1);
	
	     // Send the three address bytes, including:
	     //  - (24 - (AT45DBX_PAGE_ADDR_BITS + AT45DBX_BYTE_ADDR_BITS)) reserved bits;
	     //  - then AT45DBX_PAGE_ADDR_BITS bits specifying the page in main memory to be read;
	     //  - then AT45DBX_BYTE_ADDR_BITS don't care bits.
	     // NOTE: The bits of at45dbx_gl_ptr_mem above the AT45DBX_MEM_SIZE bits are useless for the local
	     // DF addressing. They are used for DF discrimination when there are several DFs.
	     addr = Rd_bitfield(at45dbx_gl_ptr_mem, (uint32_t)AT45DBX_MSK_PTR_PAGE) << AT45DBX_BYTE_ADDR_BITS;
	     at45dbx_spi_write_byte(LSB2W(addr));
	     at45dbx_spi_write_byte(LSB1W(addr));
	     at45dbx_spi_write_byte(LSB0W(addr));
	
	     // Unselect the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
	
	     // Wait for end of page transfer.
	     at45dbx_wait_ready();
	 #endif
	
	     // Select the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, true);
	
	     // Initiate a page write at a given sector.
	
	     // Send the Main Memory Page Program through Buffer 1 command.
	     at45dbx_spi_write_byte(AT45DBX_CMDB_PR_PAGE_TH_BUF1);
	
	     // Send the three address bytes, which comprise:
	     //  - (24 - (AT45DBX_PAGE_ADDR_BITS + AT45DBX_BYTE_ADDR_BITS)) reserved bits;
	     //  - then AT45DBX_PAGE_ADDR_BITS bits specifying the page in main memory to be written;
	     //  - then AT45DBX_BYTE_ADDR_BITS bits specifying the starting byte address within that page.
	     // NOTE: The bits of at45dbx_gl_ptr_mem above the AT45DBX_MEM_SIZE bits are useless for the local
	     // DF addressing. They are used for DF discrimination when there are several DFs.
	     addr = ((uint32_t)(Rd_bitfield(at45dbx_gl_ptr_mem, (uint32_t)AT45DBX_MSK_PTR_PAGE)) << AT45DBX_BYTE_ADDR_BITS) |
	             Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_BYTE);
	     at45dbx_spi_write_byte(LSB2W(addr));
	     at45dbx_spi_write_byte(LSB1W(addr));
	     at45dbx_spi_write_byte(LSB0W(addr));
	
	     return true;
 }


 void at45dbx_write_close(void)
 {
	     // While end of logical sector not reached, zero-fill remaining memory bytes.
	     while (Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_SECTOR)) {
		         at45dbx_spi_write_byte(0x00);
		         at45dbx_gl_ptr_mem++;
	     }
	
	     // Unselect the DF memory at45dbx_gl_ptr_mem points to.
	     at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
	
	     // Memory busy.
	     at45dbx_busy = true;
 }







 uint8_t at45dbx_read_byte(void)
 {
	     uint8_t data;
	
	     // Memory busy.
	     if (at45dbx_busy) {
		         // Being here, we know that we previously finished a page read.
		         // => We have to access the next page.
		
		         // Memory ready.
		         at45dbx_busy = false;
		
		     // Eventually select the next DF and open the next page.
		     // NOTE: at45dbx_read_open input parameter is a sector.
		         at45dbx_read_sector_open(at45dbx_gl_ptr_mem >> AT45DBX_SECTOR_BITS); // at45dbx_gl_ptr_mem / AT45DBX_SECTOR_SIZE.
	     }
	
	     at45dbx_spi_read_byte(&data);
	     at45dbx_gl_ptr_mem++;
	
	     // If end of page reached,
	     if (!Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_BYTE)) {
		         // unselect the DF memory at45dbx_gl_ptr_mem points to.
		         at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
		         // Memory busy.
		         at45dbx_busy = true;
	     }
	     return data;
 }


 bool at45dbx_write_byte(uint8_t b)
 {
	     // Memory busy.
	     if (at45dbx_busy) {
		         // Being here, we know that we previously launched a page programming.
		         // => We have to access the next page.
		         // Eventually select the next DF and open the next page.
		     // NOTE: at45dbx_write_open input parameter is a sector.
		         at45dbx_write_byte_open(at45dbx_gl_ptr_mem);
	     }
	
	     // Write the next data byte.
	     at45dbx_spi_write_byte(b);
	     at45dbx_gl_ptr_mem++;
	
	     // If end of page reached,
	     if (!Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_BYTE)) {
		         // unselect the DF memory at45dbx_gl_ptr_mem points to in order to program the page.
		         at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
		         // Memory busy.
		         at45dbx_busy = true;
	     }
	     return true;
 }







 bool at45dbx_read_multiple_sector(uint16_t nb_sector)
 {
	     while (nb_sector--) {
		         // Read the next sector.
		         at45dbx_read_sector_to_ram(sector_buf);
		         at45dbx_read_multiple_sector_callback(sector_buf);
	     }
	     return true;
 }


 bool at45dbx_write_multiple_sector(uint16_t nb_sector)
 {
	     while (nb_sector--) {
		         // Write the next sector.
		         at45dbx_write_multiple_sector_callback(sector_buf);
		         at45dbx_write_sector_from_ram(sector_buf);
	     }
	     return true;
 }





 bool at45dbx_read_sector_to_ram(void *ram)
 {
	     uint32_t end_addr=at45dbx_gl_ptr_mem+AT45DBX_SECTOR_SIZE;
	
	     if (at45dbx_busy) {
		         // Being here, we know that we previously finished a page read.
		         // => We have to access the next page.
		         at45dbx_busy = false;
		         // Eventually select the next DF and open the next page.
		         at45dbx_read_byte_open(at45dbx_gl_ptr_mem);
	     }
	
	     while(at45dbx_gl_ptr_mem<end_addr) {
		 #if (AT45DBX_PAGE_SIZE<AT45DBX_SECTOR_SIZE)
		         at45dbx_spi_read_packet(ram,AT45DBX_PAGE_SIZE);
		         at45dbx_gl_ptr_mem+=AT45DBX_PAGE_SIZE;
		         ram=(void*)((uint8_t*)ram+(uint32_t)AT45DBX_PAGE_SIZE);
		         at45dbx_read_close();
		         at45dbx_read_byte_open(at45dbx_gl_ptr_mem);
		 #else
		         at45dbx_spi_read_packet(ram,AT45DBX_SECTOR_SIZE);
		         at45dbx_gl_ptr_mem+=AT45DBX_SECTOR_SIZE;
		          // If end of page reached,
		         if (!Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_BYTE)) {
			             at45dbx_read_close();
			             at45dbx_read_byte_open(at45dbx_gl_ptr_mem);
		         }
		
		 #endif
	     }
	     return true;
 }


 bool at45dbx_write_sector_from_ram(const void *ram)
 {
	     uint32_t end_addr=at45dbx_gl_ptr_mem+AT45DBX_SECTOR_SIZE;
	     // Memory busy.
	     if (at45dbx_busy) {
		         //Being here, we know that we previously launched a page programming.
		         //=> We have to access the next page.
		
		         //Eventually select the next DF and open the next page.
		         // NOTE: at45dbx_write_open input parameter is a sector.
		         at45dbx_write_byte_open(at45dbx_gl_ptr_mem); // at45dbx_gl_ptr_mem / AT45DBX_SECTOR_SIZE.
	     }
	     while(at45dbx_gl_ptr_mem<end_addr) {
		 #if (AT45DBX_PAGE_SIZE<AT45DBX_SECTOR_SIZE)
		         at45dbx_spi_write_packet(ram,AT45DBX_PAGE_SIZE);
		         at45dbx_gl_ptr_mem+=AT45DBX_PAGE_SIZE;
		         ram=(void*)((uint8_t*)ram+(uint32_t)AT45DBX_PAGE_SIZE);
		          // If end of page reached,
		         if (!Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_BYTE)) {
			             // unselect the DF memory at45dbx_gl_ptr_mem points to in order to program the page.
			             at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
			             // Memory busy.
			             at45dbx_busy = true;
		         }
		         if(at45dbx_gl_ptr_mem<end_addr) {
			             at45dbx_write_byte_open(at45dbx_gl_ptr_mem);
		         }
		 #else
		         at45dbx_spi_write_packet(ram,AT45DBX_SECTOR_SIZE);
		         at45dbx_gl_ptr_mem+=AT45DBX_SECTOR_SIZE;
		         ram=(void*)((uint8_t*)ram+(uint32_t)AT45DBX_SECTOR_SIZE);
		          // If end of page reached,
		         if (!Rd_bitfield(at45dbx_gl_ptr_mem, AT45DBX_MSK_PTR_BYTE)) {
			             // unselect the DF memory at45dbx_gl_ptr_mem points to in order to program the page.
			             at45dbx_chipselect_df(at45dbx_gl_ptr_mem >> AT45DBX_MEM_SIZE, false);
			             // Memory busy.
			             at45dbx_busy = true;
		         }
		
		 #endif
	     }
	     return true;
 }
