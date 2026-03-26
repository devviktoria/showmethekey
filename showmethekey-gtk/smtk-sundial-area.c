#include <math.h>
#include <gtk/gtk.h>

#include "smtk-sundial-area.h"

struct _SmtkSundialArea {
	GtkDrawingArea parent_instance;

	double x;
	double y;
};

G_DEFINE_TYPE(SmtkSundialArea, smtk_sundial_area, GTK_TYPE_DRAWING_AREA)

static void
draw(GtkDrawingArea *drawing_area,
     cairo_t *cr,
     int width,
     int height,
     void *data)
{
	SmtkSundialArea *this = SMTK_SUNDIAL_AREA(drawing_area);

	double center_x = width / 2.0;
	double center_y = height / 2.0;
	double radius = MIN(width, height) / 2.0;

	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_arc(cr, center_x, center_y, radius, 0, 2 * M_PI);
	cairo_fill(cr);

	double target_x = this->x * radius;
	double target_y = -this->y * radius;

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_move_to(cr, center_x, center_y);
	cairo_rel_line_to(cr, target_x, target_y);
	cairo_set_line_width(cr, 4.0);
	cairo_stroke(cr);
}

static void smtk_sundial_area_class_init(SmtkSundialAreaClass *klass)
{
	GObjectClass *o_class = G_OBJECT_CLASS(klass);
}

static void smtk_sundial_area_init(SmtkSundialArea *this)
{
	this->x = 0.0;
	this->y = 0.0;
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(this), draw, this, NULL);
}

GtkWidget *smtk_sundial_area_new(void)
{
	SmtkSundialArea *this = g_object_new(
		SMTK_TYPE_SUNDIAL_AREA, "vexpand", true, "hexpand", true, NULL
	);
	return GTK_WIDGET(this);
}

void smtk_sundial_area_set_vector(
	SmtkSundialArea *area,
	const double x,
	const double y,
	const bool update_x,
	const bool update_y
)
{
	g_return_if_fail(SMTK_IS_SUNDIAL_AREA(area));
	if (update_x) {
		area->x = CLAMP(x, -1.0, 1.0);
	}
	if (update_y) {
		area->y = CLAMP(y, -1.0, 1.0);
	}

	gtk_widget_queue_draw(GTK_WIDGET(area));
}

