#ifndef __SMTK_MOUSE_AREA_H__
#define __SMTK_MOUSE_AREA_H__

#include <gtk/gtk.h>
#include "smtk-types.h"

G_BEGIN_DECLS

#define SMTK_TYPE_MOUSE_AREA smtk_mouse_area_get_type()
G_DECLARE_FINAL_TYPE(
	SmtkMouseArea,
	smtk_mouse_area,
	SMTK,
	MOUSE_AREA,
	GtkDrawingArea
)

GtkWidget *smtk_mouse_area_new(void);
void smtk_mouse_area_handle_event(
	SmtkMouseArea *area,
	const SmtkMouseButton button,
	const gboolean pressed
);

G_END_DECLS

#endif

