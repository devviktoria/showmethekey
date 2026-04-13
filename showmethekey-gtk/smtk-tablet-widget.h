#ifndef __SMTK_TABLET_WIDGET_H__
#define __SMTK_TABLET_WIDGET_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SMTK_TYPE_TABLET_WIDGET smtk_tablet_widget_get_type()
G_DECLARE_FINAL_TYPE(
	SmtkTabletWidget,
	smtk_tablet_widget,
	SMTK,
	TABLET_WIDGET,
	GtkFrame
)

GtkWidget *smtk_tablet_widget_new(void);
void smtk_tablet_widget_handle_event(
	SmtkTabletWidget *widget,
	const double pressure,
	const double tilt_x,
	const double tilt_y
);

G_END_DECLS

#endif
