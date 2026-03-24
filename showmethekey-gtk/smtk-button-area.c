#include <gtk/gtk.h>
#include <math.h>

#include "smtk-button-area.h"

struct _SmtkButtonArea {
	GtkDrawingArea parent_instance;

	gboolean pressed;
	SmtkButtonShape shape;

	double inactive_r, inactive_g, inactive_b;
	double active_r, active_g, active_b;
};

G_DEFINE_TYPE(SmtkButtonArea, smtk_button_area, GTK_TYPE_DRAWING_AREA)

static void draw_circle(cairo_t *cr, int width, int height)
{
	double radius = MIN(width, height) / 2.0;
	cairo_arc(cr, width / 2.0, height / 2.0, radius * 0.8, 0, 2 * M_PI);
	cairo_fill(cr);
}

static void draw_rounded_rect(cairo_t *cr, int width, int height)
{
	double radius = 10.0;

	cairo_new_path(cr);
	cairo_arc(cr, width - radius, radius, radius, -M_PI / 2, 0);
	cairo_arc(cr, width - radius, height - radius, radius, 0, M_PI / 2);
	cairo_arc(cr, radius, height - radius, radius, M_PI / 2, M_PI);
	cairo_arc(cr, radius, radius, radius, M_PI, 3 * M_PI / 2);
	cairo_close_path(cr);

	cairo_fill(cr);
}

static void draw_dpad(cairo_t *cr, int width, int height, SmtkButtonShape shape)
{
	double w = width;
	double h = height;

	// Ratio between rectangle and triangle
	double rect_ratio = 0.6;
	double rect_h = h * rect_ratio;
	double tri_h = h - rect_h;

	cairo_new_path(cr);

	switch (shape) {

	case SMTK_BUTTON_SHAPE_DPAD_UP:
		// Draw rectangle (bottom part)
		cairo_rectangle(cr, 0, tri_h, w, rect_h);

		// Draw triangle (top part)
		cairo_move_to(cr, 0, tri_h); // left base corner
		cairo_line_to(cr, w, tri_h); // right base corner
		cairo_line_to(cr, w / 2.0, 0); // top peak
		cairo_close_path(cr);
		break;

	case SMTK_BUTTON_SHAPE_DPAD_DOWN:
		// Draw rectangle (top part)
		cairo_rectangle(cr, 0, 0, w, rect_h);

		// Draw triangle (bottom part)
		cairo_move_to(cr, 0, rect_h); // left base corner
		cairo_line_to(cr, w, rect_h); // right base corner
		cairo_line_to(cr, w / 2.0, h); // bottom peak
		cairo_close_path(cr);
		break;

	case SMTK_BUTTON_SHAPE_DPAD_LEFT:
		// Draw rectangle (right part)
		cairo_rectangle(cr, tri_h, 0, rect_h, h);

		// Draw triangle (left part)
		cairo_move_to(cr, tri_h, 0); // top base corner
		cairo_line_to(cr, tri_h, h); // bottom base corner
		cairo_line_to(cr, 0, h / 2.0); // left peak
		cairo_close_path(cr);
		break;

	case SMTK_BUTTON_SHAPE_DPAD_RIGHT:
		// Draw rectangle (left part)
		cairo_rectangle(cr, 0, 0, rect_h, h);

		// Draw triangle (right part)
		cairo_move_to(cr, rect_h, 0); // top base corner
		cairo_line_to(cr, rect_h, h); // bottom base corner
		cairo_line_to(cr, w, h / 2.0); // right peak
		cairo_close_path(cr);
		break;

	default:
		return;
	}

	cairo_fill(cr);
}
static void
draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, void *data)
{
	SmtkButtonArea *this = SMTK_BUTTON_AREA(area);

	if (this->pressed)
		cairo_set_source_rgb(
			cr, this->active_r, this->active_g, this->active_b
		);
	else
		cairo_set_source_rgb(
			cr, this->inactive_r, this->inactive_g, this->inactive_b
		);

	switch (this->shape) {
	case SMTK_BUTTON_SHAPE_CIRCLE:
		draw_circle(cr, width, height);
		break;

	case SMTK_BUTTON_SHAPE_ROUNDED_RECT:
		draw_rounded_rect(cr, width, height);
		break;

	default:
		draw_dpad(cr, width, height, this->shape);
		break;
	}
}

static void smtk_button_area_init(SmtkButtonArea *this)
{
	this->pressed = FALSE;
	this->shape = SMTK_BUTTON_SHAPE_CIRCLE;

	this->inactive_r = 0.5;
	this->inactive_g = 0.5;
	this->inactive_b = 0.5;

	this->active_r = 1.0;
	this->active_g = 1.0;
	this->active_b = 1.0;

	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(this), draw, NULL, NULL);

	gtk_widget_set_size_request(GTK_WIDGET(this), 20, 20);
}

static void smtk_button_area_class_init(SmtkButtonAreaClass *klass)
{
}

GtkWidget *smtk_button_area_new(void)
{
	return g_object_new(
		SMTK_TYPE_BUTTON_AREA, "hexpand", TRUE, "vexpand", TRUE, NULL
	);
}

void smtk_button_area_set_pressed(SmtkButtonArea *this, gboolean pressed)
{
	g_return_if_fail(SMTK_IS_BUTTON_AREA(this));

	this->pressed = pressed;
	gtk_widget_queue_draw(GTK_WIDGET(this));
}

void smtk_button_area_set_shape(SmtkButtonArea *this, SmtkButtonShape shape)
{
	g_return_if_fail(SMTK_IS_BUTTON_AREA(this));

	this->shape = shape;
	gtk_widget_queue_draw(GTK_WIDGET(this));
}

void smtk_button_area_set_colors(
	SmtkButtonArea *this,
	double r_inactive,
	double g_inactive,
	double b_inactive,
	double r_active,
	double g_active,
	double b_active
)
{
	g_return_if_fail(SMTK_IS_BUTTON_AREA(this));

	this->inactive_r = r_inactive;
	this->inactive_g = g_inactive;
	this->inactive_b = b_inactive;

	this->active_r = r_active;
	this->active_g = g_active;
	this->active_b = b_active;

	gtk_widget_queue_draw(GTK_WIDGET(this));
}

