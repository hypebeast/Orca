/**************************************************************************
 * \file vtol_object_manager.c
 *
 * \brief The VTOL object manager is responsible for 
 *
 * Created: 13.01.2013 19:15:12
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include <string.h>
#include "orca.h"

// Macros
#define SET_BITS(var, shift, value, mask) var = (var & ~(mask << shift)) | (value << shift);


//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

static VTOLObjectList_t vtolo_list;

/* Pointer to the instance data of an VTOL object */
typedef void* ObjectInstanceHandle;


//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////

static ObjectInstanceHandle getObjectInstance(struct VTOLObjectData *obj);


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
* \param id			The object id.
* \param isSingle	True, if it's a single object.
* \param isSettings True, if the VTOL object is a settings object; otherwise
*                   false.
* \param numBytes	The number of bytes of object data.
* \param initCb		Callback function that's called after object is registered.
*
* \return 0			Success
* \return -1		Error
* \return -2		Object already registered
******************************************************************************/
uint8_t vtol_obj_register(VTOLObjHandle obj, uint16_t id, uint8_t isSingle,
			uint8_t isSettings, uint16_t numBytes, VTOLObjInitializeCallback initCb)
{
	// Don't allow duplicate registrations
	if (vtol_obj_get_by_id(id))
		return -2;
		
	// Save the object address
	vtolo_list.vtolo_list[vtolo_list.index++] = obj;
	
	// Initialize the common parts of the VTOLO
	struct VTOLObjectData *vtolo_data = (struct VTOLObjectData*)obj;
	vtolo_data->id = id;
	vtolo_data->instance_size = numBytes;
	struct VTOLObjectBase *vtolo_base = &(vtolo_data->base);
	memset(vtolo_base, 0, sizeof(*vtolo_base));
	vtolo_base->flags.isSingle = isSingle;
	vtolo_base->flags.isSettings = isSettings;
	
	// TODO: Attempt to load settings object from flash -> vtol_object_load()
	
	// Initialize object fields and metadata to default values
	if (initCb)
		initCb(obj);
	 
	return 0;
}

/**************************************************************************
* \brief Returns the VTOL object for the given id.
*
* \param obj	VTOL object id.
* \returns		The VTOL object handler, if the id was found. Otherwise NULL.
**************************************************************************/
VTOLObjHandle vtol_obj_get_by_id(uint16_t id)
{
	for (int i = 0; i < MAX_NUMBER_OF_VTOL_OBJECTS; i++)
	{
		struct VTOLObjectData* obj = (struct VTOLObjectData*)vtolo_list.vtolo_list[i];
		if (obj->id == id)
		{
			return vtolo_list.vtolo_list[i];
		}
	}
	
	return NULL;
}

/**************************************************************************
* \brief Returns the id for the given VTOL object.
*
* \param obj	VTOL object handler.
**************************************************************************/
uint16_t vtol_obj_get_id(VTOLObjHandle obj)
{
	struct VTOLObjectData* obj_data = (struct VTOLObjectData*)obj;
	return obj_data->id;
}

/**************************************************************************
* \brief Returns the number of bytes 
*
* \param obj	VTOL object handler.
* \returns		The number of bytes
**************************************************************************/
uint16_t vtol_obj_get_num_bytes(VTOLObjHandle obj)
{
	struct VTOLObjectData* obj_data = (struct VTOLObjectData*)obj;
	return obj_data->instance_size;
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
	// Not yet used! Currently only single instance data VTOLOs are supported!
}

/**************************************************************************
* \brief Returns if the VTOL object is a settings object.
*
* \param obj		The VTOL object handler.
* \returns TRUE		If the VTOL object a settings object.
* \returns FALSE	If the VTOL object is not a settings object.
**************************************************************************/
bool vtol_is_settings(VTOLObjHandle obj)
{
	struct VTOLObjectBase* obj_base = (struct VTOLObjectBase*) obj;
	return obj_base->flags.isSettings;
}

/**************************************************************************
* \brief Creates an instance of the VTOL object. Used for creating a new
*        instance of multi instance data VTOLO.
*
* \param obj	VTOL object.
* \param instId Instance ID. Not yet used!
* \param iniCb	Callback function that's called after object is initialized.
**************************************************************************/
uint16_t vtol_unpack(VTOLObjHandle obj, uint8_t instId, const uint8_t* dataIn)
{
	uint8_t ret = 0;
	
	if (vtol_is_meta_object(obj))
	{
		// TODO: Handle meta objects
	}
	else
	{
		// Cast handle to object
		struct VTOLObjectData* vtol = (struct VTOLObjectData*)obj;
		
		// TODO: Add support for multi instances
		
		// Get handle to object instance data
		ObjectInstanceHandle instance = getObjectInstance(vtol);
		
		// Set the data
		memcpy(instance, dataIn, vtol->instance_size);
	}
	
	// TODO: Fire event
	
	return ret;
}

/**************************************************************************
* \brief Pack an object to an byte array.
*
* \param obj		The VTOL object handle.
* \param dataOut	Data buffer (byte array).
* \return 0			Success
* \return -1		Error
**************************************************************************/
uint16_t vtol_pack(VTOLObjHandle obj, void* dataOut)
{
	// Cast handle to object
	struct VTOLObjectData* vtol_obj = (struct VTOLObjectData*)obj;
	
	// TODO: Add support for multi instances
	
	// Get handle to object instance data
	ObjectInstanceHandle instance = getObjectInstance(vtol_obj);
	
	// Pack data
	memcpy(dataOut, instance, vtol_obj->instance_size);
	
	return 0;
}

/**************************************************************************
* \brief Creates an instance of the VTOL object. Used for creating a new
*        instance of multi instance data VTOLO.
*
* \param obj	VTOL object.
* \param iniCb	Callback function that's called after object is initialized.
**************************************************************************/
void vtol_get_metadata(VTOLObjHandle obj, struct VTOLObjectMeta* dataOut)
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
void vtol_set_metadata(VTOLObjHandle obj, const struct VTOLObjectMeta* dataIn)
{
	
}

/**************************************************************************
* \brief Creates an instance of the VTOL object. Used for creating a new
*        instance of multi instance data VTOLO.
*
* \param obj	VTOL object.
* \param iniCb	Callback function that's called after object is initialized.
**************************************************************************/
uint16_t vtol_set_data(VTOLObjHandle obj, const uint8_t* dataIn)
{
	return 0;	
}

/**************************************************************************
* \brief Set the data of a specific object instance.
*
* \param obj	The VTOL object handle.
* \param dataIn	The input data.
* \param offset	The offset for the input data.
* \param size	Size of the data to write.
* \return 0		No error
* \return -1	Error
**************************************************************************/
uint16_t vtol_set_data_field(VTOLObjHandle obj, const void* dataIn, uint16_t offset, uint16_t size)
{
	struct VTOLObjectData *data;
	
	// Cast to object info
	data = (struct VTOLObjectData *)obj;
	
	// TODO: Check if it's a read only object
	
	// Set data
	memcpy(data + offset, dataIn, size);
	
	// TODO: Fire event
	
	return 0;
}

/**************************************************************************
* \brief Creates an instance of the VTOL object. Used for creating a new
*        instance of multi instance data VTOLO.
*
* \param obj	The VTOL object handle.
* \param iniCb	Callback function that's called after object is initialized.
**************************************************************************/
uint16_t vtol_get_data(VTOLObjHandle obj, void* dataOut)
{
	// TODO: I don't know if we need this function.
	return 0;
}

/**************************************************************************
* \brief Creates an instance of the VTOL object. Used for creating a new
*        instance of multi instance data VTOLO.
*
* \param obj	VTOL object.
* \param iniCb	Callback function that's called after object is initialized.
**************************************************************************/
uint16_t vtol_get_data_field(VTOLObjHandle obj, void* dataOut, uint16_t offset, uint16_t size)
{
	
}

/**************************************************************************
* \brief Checks if the object is a meta object.
*
* \param obj	VTOL object handle.
* \return True (1) if the object is a meta object; otherwise false (0).
**************************************************************************/
bool vtol_is_meta_object(VTOLObjHandle obj)
{
	// Get the common object header
	struct VTOLObjectBase* vtol_base = (struct VTOLObjectBase*)obj;
	return vtol_base->flags.isMeta;
}

/**************************************************************************
* \brief Save the data of the specified VTOL object to the file system (Flash).
*
* \param obj	VTOL object handle.
* \param instId	Instance ID. Not yet used!
* \return 0	if success; otherwise -1.
**************************************************************************/
uint16_t vtol_save(VTOLObjHandle obj, uint8_t instId)
{
	// TODO
	return 0;
}

/**************************************************************************
* \brief Load a VTOL object from the file system (Flash).
*
* \param obj	VTOL object handle.
* \param instId	Instance ID. Not yet used!
* \return 0	if success; otherwise -1.
**************************************************************************/
uint16_t vtol_load(VTOLObjHandle obj, uint8_t instId)
{
	// TODO
	return 0;
}

/**************************************************************************
* \brief Save all settings objects to the file system (Flash).
*
* \return 0	if success; otherwise -1.
**************************************************************************/
uint16_t vtol_save_settings(void)
{
	// TODO
	return 0;
}

/**************************************************************************
* \brief Load all settings objects from the file system (Flash).
*
* \return 0	if success; otherwise -1.
**************************************************************************/
uint16_t vtol_load_settings(void)
{
	// TODO
	return 0;
}

/**************************************************************************
* \brief Delete all settings objects from the file system (Flash).
*
* \return 0	if success; otherwise -1.
**************************************************************************/
uint16_t vtol_delete_settings(void)
{
	// TODO
	return 0;
}

/**************************************************************************
* \brief Returns a handle to the object data.
*
* \return Instance handle
**************************************************************************/
static ObjectInstanceHandle getObjectInstance(struct VTOLObjectData *obj)
{
	return ((ObjectInstanceHandle)(obj+sizeof(struct VTOLObjectData)));
}