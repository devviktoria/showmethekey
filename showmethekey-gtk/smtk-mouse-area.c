#include <gtk/gtk.h>

#include "smtk-mouse-area.h"
#include "smtk-types.h"

struct _SmtkMouseArea {
	GtkDrawingArea parent_instance;

	gboolean buttons[SMTK_MOUSE_BUTTON_UNKNOWN];
};
G_DEFINE_TYPE(SmtkMouseArea, smtk_mouse_area, GTK_TYPE_DRAWING_AREA)

static void
draw(GtkDrawingArea *drawing_area,
     cairo_t *cr,
     int width,
     int height,
     void *data)
{
	SmtkMouseArea *this = SMTK_MOUSE_AREA(drawing_area);
	(void)data;

	double padding = 1.0;
	double content_width = MAX(1.0, width - 2.0 * padding);
	double content_height = MAX(1.0, height - 2.0 * padding);

	// Preserve the mouse silhouette aspect ratio while using as much of the
	// available frame content area as possible.
	double target_width = content_height * 0.9;
	if (target_width < content_width)
		content_width = target_width;
	else
		content_height = content_width / 0.9;

	double origin_x = (width - content_width) / 2.0;
	double origin_y = (height - content_height) / 2.0;

	double inner_x = origin_x + padding;
	double inner_y = origin_y + padding;

	double inner_width = MAX(1.0, content_width - 2.0 * padding);
	double inner_height = MAX(1.0, content_height - 2.0 * padding);

	double button_width = (inner_width - padding) / 2.0;
	double button_height = inner_height;

	double middle_width = button_width * 0.5;
	double middle_height = button_height * 0.5;

	double left_x = inner_x;
	double right_x = inner_x + button_width + padding;
	double top_y = inner_y;

	double middle_x = inner_x + (inner_width - middle_width) / 2.0;
	double middle_y = inner_y + (inner_height - middle_height) / 3.0;

	if (this->buttons[SMTK_MOUSE_BUTTON_LEFT])
		cairo_set_source_rgb(cr, 1, 1, 1);
	else
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_rectangle(cr, left_x, top_y, button_width, button_height);
	cairo_fill(cr);

	if (this->buttons[SMTK_MOUSE_BUTTON_RIGHT])
		cairo_set_source_rgb(cr, 1, 1, 1);
	else
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_rectangle(cr, right_x, top_y, button_width, button_height);
	cairo_fill(cr);

	cairo_save(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle(
		cr,
		middle_x - 3.0,
		middle_y - 3.0,
		middle_width + 6.0,
		middle_height + 6.0
	);
	cairo_fill(cr);
	cairo_restore(cr);

	if (this->buttons[SMTK_MOUSE_BUTTON_MIDDLE])
		cairo_set_source_rgb(cr, 1, 1, 1);
	else
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_rectangle(cr, middle_x, middle_y, middle_width, middle_height);
	cairo_fill(cr);
}

static void smtk_mouse_area_class_init(SmtkMouseAreaClass *klass)
{
	(void)klass;
}

static void smtk_mouse_area_init(SmtkMouseArea *this)
{
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(this), draw, NULL, NULL);
}

GtkWidget *smtk_mouse_area_new(void)
{
	SmtkMouseArea *this = g_object_new(
		SMTK_TYPE_MOUSE_AREA, "vexpand", true, "hexpand", true, NULL
	);
	return GTK_WIDGET(this);
}

void smtk_mouse_area_handle_event(
	SmtkMouseArea *this,
	const SmtkMouseButton button,
	const gboolean pressed
)
{
	g_return_if_fail(this != NULL);
	g_return_if_fail(button >= 0 && button < SMTK_MOUSE_BUTTON_UNKNOWN);

	g_debug("Adding mouse button: %u.", button);

	this->buttons[button] = pressed;
	gtk_widget_queue_draw(GTK_WIDGET(this));
}
