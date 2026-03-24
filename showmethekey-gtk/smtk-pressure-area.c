#include <math.h>
#include <gtk/gtk.h>

#include "smtk-pressure-area.h"

struct _SmtkPressureArea {
	GtkDrawingArea parent_instance;

	double pressure;
};

G_DEFINE_TYPE(SmtkPressureArea, smtk_pressure_area, GTK_TYPE_DRAWING_AREA)

static void
draw(GtkDrawingArea *drawing_area,
     cairo_t *cr,
     int width,
     int height,
     void *data)
{
	SmtkPressureArea *this = SMTK_PRESSURE_AREA(drawing_area);

	/* background */
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	double filled_width = width * this->pressure;
	/* pressure */
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, filled_width, height);
	cairo_fill(cr);
}

static void smtk_pressure_area_class_init(SmtkPressureAreaClass *klass)
{
	GObjectClass *o_class = G_OBJECT_CLASS(klass);
}

static void smtk_pressure_area_init(SmtkPressureArea *this)
{
	this->pressure = 0.0;
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(this), draw, this, NULL);
}

GtkWidget *smtk_pressure_area_new(void)
{
	SmtkPressureArea *this = g_object_new(
		SMTK_TYPE_PRESSURE_AREA, "vexpand", true, "hexpand", true, NULL
	);
	return GTK_WIDGET(this);
}

void smtk_pressure_area_set_value(SmtkPressureArea *this, const double pressure)
{
	g_return_if_fail(SMTK_IS_PRESSURE_AREA(this));

	this->pressure = pressure;

	gtk_widget_queue_draw(GTK_WIDGET(this));
}

