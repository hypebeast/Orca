/**************************************************************************
 * \file vtol_object_manager.c
 *
 * \brief The VTOL object manager is responsible for 
 *
 * Created: 13.01.2013 19:15:12
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "orca.h"

// Macros
#define SET_BITS(var, shift, value, mask) var = (var & ~(mask << shift)) | (value << shift);


// Private variables
static VTOLObjectList_t vtolo_list;

/**************************************************************************
* \brief Initializes the VTOL object subsystem. Must be called during system
*        startup.
* \return 0		Success
* \return -1	Failure
**************************************************************************/
uint8_t vtol_init(void)
{
	// Done
	return 0;
}

/******************************************************************************
* \brief Registers an object with the given id.
*
* \param id			The id of the object which should registered.
* \param isSettings True, if the VTOL object is a settings object; otherwise
*                   false.
* \param initCb		Callback function that's called after object is registered.
*
* \return 0			Success
* \return -1		Error
* \return -2		Object already registered
******************************************************************************/
uint8_t vtol_obj_register(VTOLObjHandle obj, uint16_t id, VTOLObjInitializeCallback initCb)
{
	// Don't allow duplicate registrations
	if (vtol_obj_get_by_id(id))
		return -2;
		
	// Save the object address
	vtolo_list.vtolo_list[vtolo_list.index++] = obj;
	
	// TODO: Attempt to load settings object from flash
	
	// TODO: Execute callback
	 
	return 0;
}

/**************************************************************************
* \brief Returns the VTOL object for the given id.
*
* \param obj	VTOL object id.
**************************************************************************/
VTOLObjHandle vtol_obj_get_by_id(uint16_t id)
{
	// TODO
}

/**************************************************************************
* \brief Returns the id for the given VTOL object.
*
* \param obj	VTOL object handler.
**************************************************************************/
uint16_t vtol_object_get_id(VTOLObjHandle obj)
{
	// TODO
}

/**************************************************************************
* \brief Returns the id for the given VTOL object.
*
* \param obj	VTOL object handler.
**************************************************************************/
uint16_t vtol_obj_get_num_bytes(VTOLObjHandle obj)
{
	// TODO
}

/**************************************************************************
* \brief Creates an instance of the VTOL object. Used for creating a new
*        instance of multi instance data VTOLO.
*
* \param obj	VTOL object.
* \param iniCb	Callback function that's called after object is initialized.
**************************************************************************/
uint16_t vtol_create_instance(VTOLObjHandle obj, VTOLObjInitializeCallback initCb)
{
	// TODO: Currently only single instance data VTOLOs are supported!
}