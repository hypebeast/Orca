/**************************************************************************
 * \file vtol_object_manager.h
 *
 * \brief Header file for vtol_object_manager.c
 *
 * Created: 13.01.2013 19:15:12
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/


#ifndef VTOL_OBJECT_MANAGER_H_
#define VTOL_OBJECT_MANAGER_H_

#include <asf.h>

// Defines
#define VTOLOBJ_NUMBER_OF_VTOL_OBJECTS 100

/*
 * Shifts and masks used to read/write metadata flags.
 */
#define VTOLOBJ_ACCESS_SHIFT 0
#define VTOLOBJ_GCS_ACCESS_SHIFT 1
#define VTOLOBJ_TELEMETRY_ACKED_SHIFT 2
#define VTOLOBJ_GCS_TELEMETRY_ACKED_SHIFT 3
#define VTOLOBJ_TELEMETRY_UPDATE_MODE_SHIFT 4
#define VTOLOBJ_GCS_TELEMETRY_UPDATE_MODE_SHIFT 6
#define VTOLOBJ_UPDATE_MODE_MASK 0x3

/** VTOL object handle type definition. */
typedef void* VTOLObjHandle;

/***************************************************************************
 * \brief Object update mode, used by multiple modules (e.g. telemetry).
 **************************************************************************/
typedef enum {
	UPDATE_MODE_MANUAL = 0, /** Manually update object, by calling the update() function. */
	UPDATE_MODE_PERIODIC = 1, /** Automatically update object at periodic intervals */
	UPDATE_MODE_ONCHANGE = 2 /** Object is updated on change. */
} VTOLObjUpdateMode;

/***************************************************************************
 * \brief The metadata of a VTOL object.
 *
 *  Flags
 *  -----
 *  Bit 0:		access					Defines the access level for the local transactions (readonly=1 and readwrite=0).
 *  Bit 1:		gcsAccess				Defines the access level for the local GCS transactions (readonly=1 and readwrite=0). Not used in the FMU s/w.
 *  Bit 2:		telemetryAcked			Defines if an ack is required for the transaction of this object (1: acked, 0: not acked)
 *  Bit 3:		gcsTelemetryAcked		Defines if an ack is required for the transaction of this object (1: acked, 0: not acked)
 *  Bit 4-5:	telemetryUpdateMode		Update mode used by the telemetry module (VTOLObjUpdateMode)
 *  Bit 6-7:	gcsTelemetryUpdateMode	Update mode used by the GCS telemetry module (VTOLObjUpdateMode)
 *  Bit 8:		Not used!
 **************************************************************************/
typedef struct {
	uint8_t flags; /** Defines flags for update, access and logging modes */
	uint16_t telemetryUpdatePeriod; /** Update period used by the telemetry module */
	uint16_t gcsTelemetryUpdatePeriod; /** Update period used by the GCS */
	uint16_t loggingUpdatePeriod; /** Update period used by the logging module */
} __attribute__((packed)) VTOLObjMetaData;

/***************************************************************************
 * \brief Event types generated by the objects.
 **************************************************************************/
typedef enum {
	EV_NONE = 0x00, /** No event */
	EV_UNPACKED = 0x01, /** Object data updated by unpacking */
	EV_UPDATED = 0x02, /** Object data updated by changing the data structure */
	EV_UPDATED_MANUAL = 0x04, /** Object update event manually generated */
	EV_UPDATED_PERIODIC = 0x08, /** Object update from periodic event */
	EV_UPDATE_REQ = 0x10 /** Request to update object data */
} VTOLObjEventType;

/***************************************************************************
 * \brief Access types.
 **************************************************************************/
typedef enum {
	ACCESS_READWRITE = 0,
	ACCESS_READONLY = 1
} VTOLObjAccessType;

/***************************************************************************
 * \brief Event message. This struct is send each time an event is generated.
 **************************************************************************/
typedef struct {
	VTOLObjHandle obj; /** Object handle */
	uint16_t instId; /** Instance id */
	VTOLObjEventType event; /** Event type */
} VTOLObjEvent;

/***************************************************************************
 * \brief Event callback, this function is called when an event is invoked.
 **************************************************************************/
typedef void (*VTOLObjEventCallback)(VTOLObjEvent* ev);

/**************************************************************************
* \brief VTOL object base type.
**************************************************************************/
struct VTOLObjectBase {
	/* Describe the type of object that follows this header */
	struct VTOLOInfo {
		bool isMeta        : 1;
		bool isSingle      : 1;
		bool isSettings    : 1;
	} flags;
} __attribute__((packed));

/**************************************************************************
* \brief Augmented type for Meta VTOL object.
**************************************************************************/
struct VTOLObjectMeta {
	struct VTOLObjectBase base;
	VTOLObjMetaData instance;
} __attribute__((packed));

/**************************************************************************
* \brief Shared data structure for all data-carrying VTOLObjects.
**************************************************************************/
struct VTOLObjectData {
	struct VTOLObjectBase base; /** Base VTOL object header */
	uint16_t id; /** Object id */
	struct VTOLObjectMeta metaObj; /** Meta data */
	uint16_t instance_size; /** Instance size. Size of the object without the object header. */
} __attribute__((packed));

/**************************************************************************
* \brief Augmented type for single instance data VTOL objects.
* \note This type is not yet used!		
**************************************************************************/
struct VTOLObjectSingle {
	struct VTOLObjectData vtol_object;
	// If we would use dynamic memory allocation,
	// we would use this field for the data object.
	//uint8_t instance[];
} __attribute__((packed));

/**************************************************************************
* \brief VTOL object list data structure.
**************************************************************************/
typedef struct {
	VTOLObjHandle vtolo_list[VTOLOBJ_NUMBER_OF_VTOL_OBJECTS]; /** Pointers to the VTOL objects*/
	uint8_t index; /** Index of the last inserted object */
} VTOLObjectList_t;

/***************************************************************************
 * Callback used to initialize the object fields to their default values.
 **************************************************************************/
typedef void (*VTOLObjInitializeCallback)(VTOLObjHandle obj_handle);


// Function decelerations

uint8_t vtol_init(void);
uint8_t vtol_obj_register(VTOLObjHandle obj, uint16_t id, uint8_t isSingle,
			uint8_t isSettings, uint16_t numBytes, VTOLObjInitializeCallback initCb);
VTOLObjHandle vtol_obj_get_by_id(uint16_t id);
uint16_t vtol_obj_get_id(VTOLObjHandle obj);
uint16_t vtol_obj_get_num_bytes(VTOLObjHandle obj);
uint16_t vtol_create_instance(VTOLObjHandle obj, VTOLObjInitializeCallback initCb);
bool vtol_is_settings(VTOLObjHandle obj);
bool vtol_read_only(VTOLObjHandle obj);
bool vtol_is_meta_object(VTOLObjHandle obj);
bool vtol_is_meta_object(VTOLObjHandle obj);
bool vtol_get_ack_required(VTOLObjMetaData const *metadata);
VTOLObjAccessType vtol_get_access(const VTOLObjMetaData* metadata);
int8_t vtol_unpack(VTOLObjHandle obj, uint8_t instId, const uint8_t* dataIn);
int8_t vtol_pack(VTOLObjHandle obj, void* dataOut);
int8_t vtol_save(VTOLObjHandle obj, uint8_t instId);
int8_t vtol_load(VTOLObjHandle obj, uint8_t instId);
int8_t vtol_delete(VTOLObjHandle obj);
int8_t vtol_save_settings(void);
int8_t vtol_load_settings(void);
int8_t vtol_delete_settings(void);
int8_t vtol_set_data(VTOLObjHandle obj, const uint8_t* dataIn);
int8_t vtol_set_data_field(VTOLObjHandle obj, const void* dataIn, uint16_t offset, uint16_t size);
int8_t vtol_get_data(VTOLObjHandle obj, void* dataOut);
int8_t vtol_get_data_field(VTOLObjHandle obj, void* dataOut, uint16_t offset, uint16_t size);
VTOLObjAccessType vtol_get_access(const VTOLObjMetaData* metadata);
VTOLObjUpdateMode vtol_get_telemetry_update_mode(const VTOLObjMetaData* metadata);
void vtol_set_access(VTOLObjMetaData* dataOut, VTOLObjAccessType type);
void vtol_object_updated(VTOLObjHandle obj);
void vtol_get_metadata(VTOLObjHandle obj, struct VTOLObjectMeta* dataOut);
void vtol_set_metadata(VTOLObjHandle obj, const struct VTOLObjectMeta* dataIn);
void vtol_obj_iterate(void (*iterator)(VTOLObjHandle obj));


#endif /* VTOL_OBJECT_MANAGER_H_ */