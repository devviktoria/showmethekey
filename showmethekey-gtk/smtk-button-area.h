#ifndef __SMTK_BUTTON_AREA_H__
#define __SMTK_BUTTON_AREA_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SMTK_TYPE_BUTTON_AREA smtk_button_area_get_type()
G_DECLARE_FINAL_TYPE(
	SmtkButtonArea,
	smtk_button_area,
	SMTK,
	BUTTON_AREA,
	GtkDrawingArea
)

typedef enum {
	SMTK_BUTTON_SHAPE_CIRCLE,
	SMTK_BUTTON_SHAPE_ROUNDED_RECT,
	SMTK_BUTTON_SHAPE_DPAD_UP,
	SMTK_BUTTON_SHAPE_DPAD_DOWN,
	SMTK_BUTTON_SHAPE_DPAD_LEFT,
	SMTK_BUTTON_SHAPE_DPAD_RIGHT
} SmtkButtonShape;

GtkWidget *smtk_button_area_new(void);

void smtk_button_area_set_pressed(SmtkButtonArea *area, gboolean pressed);

void smtk_button_area_set_shape(SmtkButtonArea *area, SmtkButtonShape shape);

void smtk_button_area_set_colors(
	SmtkButtonArea *area,
	double r_inactive,
	double g_inactive,
	double b_inactive,
	double r_active,
	double g_active,
	double b_active
);

G_END_DECLS

#endif

