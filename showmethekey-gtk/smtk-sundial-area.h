#ifndef __SMTK_SUNDIAL_AREA_H__
#define __SMTK_SUNDIAL_AREA_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SMTK_TYPE_SUNDIAL_AREA smtk_sundial_area_get_type()
G_DECLARE_FINAL_TYPE(
	SmtkSundialArea,
	smtk_sundial_area,
	SMTK,
	SUNDIAL_AREA,
	GtkDrawingArea
)

GtkWidget *smtk_sundial_area_new(void);
void smtk_sundial_area_set_vector(
	SmtkSundialArea *this,
	const double x,
	const double y,
	const bool update_x,
	const bool update_y
);

G_END_DECLS

#endif

