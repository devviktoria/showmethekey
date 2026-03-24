#ifndef __SMTK_TABLET_AREA_H__
#define __SMTK_TABLET_AREA_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SMTK_TYPE_TABLET_AREA smtk_tablet_area_get_type()
G_DECLARE_FINAL_TYPE(
	SmtkTabletArea,
	smtk_tablet_area,
	SMTK,
	TABLET_AREA,
	GtkFrame
)

GtkWidget *smtk_tablet_area_new(void);
void smtk_tablet_area_handle_event(
	SmtkTabletArea *area,
	const double pressure,
	const double tilt_x,
	const double tilt_y
);

G_END_DECLS

#endif

