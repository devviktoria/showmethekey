#ifndef __SMTK_PRESSURE_AREA_H__
#define __SMTK_PRESSURE_AREA_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SMTK_TYPE_PRESSURE_AREA smtk_pressure_area_get_type()
G_DECLARE_FINAL_TYPE(
	SmtkPressureArea,
	smtk_pressure_area,
	SMTK,
	PRESSURE_AREA,
	GtkDrawingArea
)

GtkWidget *smtk_pressure_area_new(void);
void smtk_pressure_area_set_value(SmtkPressureArea *area, const double pressure);

G_END_DECLS

#endif

