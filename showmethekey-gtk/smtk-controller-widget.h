#ifndef __SMTK_CONTROLLER_WIDGET_H__
#define __SMTK_CONTROLLER_WIDGET_H__

#include <gtk/gtk.h>
#include "smtk-types.h"

G_BEGIN_DECLS

#define SMTK_TYPE_CONTROLLER_WIDGET smtk_controller_widget_get_type()
G_DECLARE_FINAL_TYPE(
	SmtkControllerWidget,
	smtk_controller_widget,
	SMTK,
	CONTROLLER_WIDGET,
	GtkGrid
)

GtkWidget *smtk_controller_widget_new(void);
// Main event entry point
void smtk_controller_widget_handle_event(
	SmtkControllerWidget *this,
	SmtkControllerEventType event_type,
	guint8 number,
	double value,
	gboolean pressed
);

G_END_DECLS

#endif

