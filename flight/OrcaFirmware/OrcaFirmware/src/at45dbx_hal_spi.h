/*
 * at45dbx_hal_spi.h
 *
 * Created: 05.10.2012 13:11:58
 *  Author: f-ruml
 */ 


#ifndef AT45DBX_HAL_SPI_H_
#define AT45DBX_HAL_SPI_H_


#include "user_board.h"

 #ifdef __cplusplus
 extern "C" {
	 #endif
	
	
	
	 // Sanity check to enable connection with only one SPI service
	 #if defined(AT45DBX_USES_SPI_MASTER_SERVICE) &&  defined(AT45DBX_USES_USART_SPI_SERVICE)
	     #error only one SPI service should be selected
	 #endif
	 // At least one SPI service should be connnectec...
	 #if !defined(AT45DBX_USES_SPI_MASTER_SERVICE) &&  !defined(AT45DBX_USES_USART_SPI_SERVICE)
	     #error No SPI service selected
	 #endif
	
	 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
	     #include "spi_master.h"
	     // Create as many spi_device as required...
	     #if (AT45DBX_MEM_CNT>0)
	         struct spi_device AT45DBX_DEVICE1 = {
		             .id = AT45DBX_CS
	         };
	     #endif
	     #if (AT45DBX_MEM_CNT>1)
	         struct spi_device AT45DBX_DEVICE2 = {
		             .id = AT45DBX_CS2
	         };
	     #endif
	     #if (AT45DBX_MEM_CNT>2)
	         struct spi_device AT45DBX_DEVICE3 = {
		             .id = AT45DBX_CS3
	         };
	     #endif
	     #if (AT45DBX_MEM_CNT>3)
	         struct spi_device AT45DBX_DEVICE4 = {
		             .id = AT45DBX_CS4
	         };
	     #endif
	
	 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
	     #include "usart_spi.h"
	     // Create as many usart_spi_device as required...
	     #if (AT45DBX_MEM_CNT>0)
	         struct usart_spi_device AT45DBX_DEVICE1 = {
		             .id = AT45DBX_CS
	         };
	     #endif
	     #if (AT45DBX_MEM_CNT>1)
	         struct usart_spi_device AT45DBX_DEVICE2 = {
		             .id = AT45DBX_CS2
	         };
	     #endif
	     #if (AT45DBX_MEM_CNT>2)
	         struct usart_spi_device AT45DBX_DEVICE3 = {
		             .id = AT45DBX_CS3
	         };
	     #endif
	     #if (AT45DBX_MEM_CNT>3)
	         struct usart_spi_device AT45DBX_DEVICE4 = {
		             .id = AT45DBX_CS4
	         };
	     #endif
	 #endif
	
	 inline void at45dbx_spi_init(void)
	 {
		 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
		     spi_master_init(AT45DBX_SPI_MODULE);
		     #if (AT45DBX_MEM_CNT==1)
		         spi_master_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE1,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		     #if (AT45DBX_MEM_CNT>1)
		         spi_master_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE2,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		     #if (AT45DBX_MEM_CNT>2)
		         spi_master_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE3,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		     #if (AT45DBX_MEM_CNT>3)
		         spi_master_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE4,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		     spi_enable(AT45DBX_SPI_MODULE);
		
		 // Implementation with USART in SPI mode service
		 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
		     usart_spi_init(AT45DBX_SPI_MODULE);
		     #if (AT45DBX_MEM_CNT==1)
		         usart_spi_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE1,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		     #if (AT45DBX_MEM_CNT>1)
		         usart_spi_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE2,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		     #if (AT45DBX_MEM_CNT>2)
		         usart_spi_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE3,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		     #if (AT45DBX_MEM_CNT>3)
		         usart_spi_setup_device(AT45DBX_SPI_MODULE,&AT45DBX_DEVICE4,SPI_MODE_0,AT45DBX_SPI_MASTER_SPEED,0);
		     #endif
		 #endif
	 }
	
	 inline void at45dbx_spi_select_device(uint8_t mem_id)
	 {
		 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
		     #if (AT45DBX_MEM_CNT==1)
		     spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
		     #else
		     switch(mem_id) {
			     case 1:
			         spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
			         break;
			
			     case 2:
			         spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE2);
			         break;
			
			     case 3:
			         spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE3);
			         break;
			
			     case 4:
			         spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE4);
			         break;
			
			     default:
			         /* unhandled_case(id); */
			         return;
		     }
		     #endif
		
		 // Implementation with USART in SPI mode service
		 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
		     #if (AT45DBX_MEM_CNT==1)
		     usart_spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
		     #else
		     switch(mem_id) {
			     case 1:
			         usart_spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
			         break;
			
			     case 2:
			         usart_spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE2);
			         break;
			
			     case 3:
			         usart_spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE3);
			         break;
			
			     case 4:
			         usart_spi_select_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE4);
			         break;
			
			     default:
			         /* unhandled_case(id); */
			         return;
		     }
		 #endif
		
		 #endif
	 }
	
	 inline void at45dbx_spi_deselect_device(uint8_t mem_id)
	 {
		 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
		     #if (AT45DBX_MEM_CNT==1)
		     spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
		     #else
		     switch(mem_id) {
			     case 1:
			         spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
			         break;
			
			     case 2:
			         spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE2);
			         break;
			
			     case 3:
			         spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE3);
			         break;
			
			     case 4:
			         spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE4);
			         break;
			
			     default:
			         /* unhandled_case(id); */
			         return;
		     }
		     #endif
		
		 // Implementation with USART in SPI mode service
		 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
		     #if (AT45DBX_MEM_CNT==1)
		     usart_spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
		     #else
		     switch(mem_id) {
			     case 1:
			         usart_spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE1);
			         break;
			
			     case 2:
			         usart_spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE2);
			         break;
			
			     case 3:
			         usart_spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE3);
			         break;
			
			     case 4:
			         usart_spi_deselect_device(AT45DBX_SPI_MODULE, &AT45DBX_DEVICE4);
			         break;
			
			     default:
			         /* unhandled_case(id); */
			         return;
		     }
		 #endif
		 #endif
	 }
	
	 inline void at45dbx_spi_write_byte(uint8_t data)
	 {
		 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
		     spi_write_packet(AT45DBX_SPI_MODULE, &data, 1);
		 // Implementation with USART in SPI mode service
		 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
		     usart_spi_write_packet(AT45DBX_SPI_MODULE, &data, 1);
		 #endif
	 }
	
	 inline void at45dbx_spi_read_byte(uint8_t *data)
	 {
		 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
		     spi_read_packet(AT45DBX_SPI_MODULE, data, 1);
		
		 // Implementation with USART in SPI mode service
		 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
		     usart_spi_read_packet(AT45DBX_SPI_MODULE, data, 1);
		 #endif
	 }
	
	
	 inline void at45dbx_spi_read_packet(void const *data, size_t len)
	 {
		 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
		     spi_read_packet(AT45DBX_SPI_MODULE, (uint8_t*)data, len);
		
		 // Implementation with USART in SPI mode service
		 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
		     usart_spi_read_packet(AT45DBX_SPI_MODULE, (uint8_t*)data, len);
		 #endif
	 }
	
	 inline void at45dbx_spi_write_packet(void const *data, size_t len)
	 {
		 #if defined( AT45DBX_USES_SPI_MASTER_SERVICE)
		     spi_write_packet(AT45DBX_SPI_MODULE, (uint8_t*)data, len);
		
		 // Implementation with USART in SPI mode service
		 #elif defined(AT45DBX_USES_USART_SPI_SERVICE)
		     usart_spi_write_packet(AT45DBX_SPI_MODULE, (uint8_t*)data, len);
		 #endif
	 }
	
	
	 #ifdef __cplusplus
 }
 #endif

#endif /* AT45DBX_HAL_SPI_H_ */