#ifndef __SMTK_CONTROLLER_EMITTER__
#define __SMTK_CONTROLLER_EMITTER__

#include <glib-object.h>

G_BEGIN_DECLS

#define SMTK_TYPE_CONTROLLER_EMITTER smtk_controller_emitter_get_type()

G_DECLARE_FINAL_TYPE(
	SmtkControllerEmitter,
	smtk_controller_emitter,
	SMTK,
	CONTROLLER_EMITTER,
	GObject
)

// Event API
SmtkControllerEmitter *smtk_controller_emitter_new(void);

void smtk_controller_emitter_start_async(SmtkControllerEmitter *this);
void smtk_controller_emitter_stop_async(SmtkControllerEmitter *this);

G_END_DECLS

#endif

