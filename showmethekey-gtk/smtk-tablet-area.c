#include <gtk/gtk.h>

#include "smtk-enum-types.h"
#include "smtk-tablet-area.h"
#include "smtk-sundial-area.h"
#include "smtk-pressure-area.h"

struct _SmtkTabletArea {
	GtkFrame parent_instance;
	GSettings *settings;

	GtkWidget *box;
	GtkWidget *tilt_area;
	GtkWidget *pressure_area;

	bool draw_border;
	double margin_ratio;
	int timeout;
};

G_DEFINE_TYPE(SmtkTabletArea, smtk_tablet_area, GTK_TYPE_FRAME)

enum { PROP_0, PROP_DRAW_BORDER, PROP_MARGIN_RATIO, PROP_TIMEOUT, N_PROPS };

static GParamSpec *props[N_PROPS] = { NULL };

static void set_property(
	GObject *o,
	unsigned int prop,
	const GValue *value,
	GParamSpec *pspec
)
{
	SmtkTabletArea *this = SMTK_TABLET_AREA(o);

	switch (prop) {
	case PROP_DRAW_BORDER:
		this->draw_border = g_value_get_boolean(value);
		// Trigger re-draw because border changed.
		gtk_widget_queue_draw(GTK_WIDGET(this));
		break;
	case PROP_MARGIN_RATIO:
		this->margin_ratio = g_value_get_double(value);
		// Trigger re-draw because margin changed.
		gtk_widget_queue_draw(GTK_WIDGET(this));
		break;
	case PROP_TIMEOUT:
		this->timeout = g_value_get_int(value);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(o, prop, pspec);
		break;
	}
}

static void
get_property(GObject *o, unsigned int prop, GValue *value, GParamSpec *pspec)
{
	SmtkTabletArea *this = SMTK_TABLET_AREA(o);

	switch (prop) {
	case PROP_DRAW_BORDER:
		g_value_set_boolean(value, this->draw_border);
		break;
	case PROP_MARGIN_RATIO:
		g_value_set_double(value, this->margin_ratio);
		break;
	case PROP_TIMEOUT:
		g_value_set_int(value, this->timeout);
		break;
	default:
		/* We don't have any other property... */
		G_OBJECT_WARN_INVALID_PROPERTY_ID(o, prop, pspec);
		break;
	}
}

// true and false are C99 _Bool, but GLib expects gboolean, which is C99 int.
static int trigger_redraw(void *data)
{

	return 0;
}

static void constructed(GObject *o)
{
	SmtkTabletArea *this = SMTK_TABLET_AREA(o);

	this->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_margin_top(this->box, 6);
	gtk_widget_set_margin_bottom(this->box, 6);
	gtk_widget_set_margin_start(this->box, 6);
	gtk_widget_set_margin_end(this->box, 6);
	gtk_frame_set_child(GTK_FRAME(this), this->box);

	this->tilt_area = smtk_sundial_area_new();
	gtk_box_append(GTK_BOX(this->box), this->tilt_area);

	this->pressure_area = smtk_pressure_area_new();
	gtk_box_append(GTK_BOX(this->box), this->pressure_area);

	this->settings = g_settings_new("one.alynx.showmethekey");
	g_settings_bind(
		this->settings,
		"draw-border",
		this,
		"draw-border",
		G_SETTINGS_BIND_GET
	);
	g_settings_bind(
		this->settings,
		"margin-ratio",
		this,
		"margin-ratio",
		G_SETTINGS_BIND_GET
	);
	g_settings_bind(
		this->settings, "timeout", this, "timeout", G_SETTINGS_BIND_GET
	);

	G_OBJECT_CLASS(smtk_tablet_area_parent_class)->constructed(o);
}

static void dispose(GObject *o)
{
	SmtkTabletArea *this = SMTK_TABLET_AREA(o);

	g_clear_object(&this->settings);

	G_OBJECT_CLASS(smtk_tablet_area_parent_class)->dispose(o);
}

static void finalize(GObject *o)
{
	SmtkTabletArea *this = SMTK_TABLET_AREA(o);

	G_OBJECT_CLASS(smtk_tablet_area_parent_class)->finalize(o);
}

static void smtk_tablet_area_class_init(SmtkTabletAreaClass *klass)
{
	GObjectClass *o_class = G_OBJECT_CLASS(klass);

	o_class->constructed = constructed;

	o_class->set_property = set_property;
	o_class->get_property = get_property;

	o_class->dispose = dispose;
	o_class->finalize = finalize;

	props[PROP_DRAW_BORDER] = g_param_spec_boolean(
		"draw-border",
		"Draw Border",
		"Draw Key Border",
		true,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE
	);
	props[PROP_MARGIN_RATIO] = g_param_spec_double(
		"margin-ratio",
		"Margin Ratio",
		"Key Margin Ratio",
		0,
		10,
		0.4,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE
	);
	props[PROP_TIMEOUT] = g_param_spec_int(
		"timeout",
		"Text Timeout",
		"Text Timeout",
		0,
		30000,
		1000,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE
	);

	g_object_class_install_properties(o_class, N_PROPS, props);
}

static void smtk_tablet_area_init(SmtkTabletArea *this)
{
	this->settings = NULL;
}

GtkWidget *smtk_tablet_area_new(void)
{
	SmtkTabletArea *this = g_object_new(
		SMTK_TYPE_TABLET_AREA, "vexpand", true, "hexpand", true, NULL
	);
	return GTK_WIDGET(this);
}

void smtk_tablet_area_handle_event(
	SmtkTabletArea *this,
	const double pressure,
	const double tilt_x,
	const double tilt_y
)
{
	g_debug("Tablet: pressure: %f, tilt_x: %f, tilt_y: %f.",
		pressure,
		tilt_x,
		tilt_y);

	g_return_if_fail(this != NULL);

	double x = tilt_x / 90.0;
	double y = -tilt_y / 90.0;
	smtk_sundial_area_set_vector(
		SMTK_SUNDIAL_AREA(this->tilt_area),
		x,
		y,
		tilt_x > -100,
		tilt_y > -100
	);
	if (pressure > -0.1) {
		smtk_pressure_area_set_value(
			SMTK_PRESSURE_AREA(this->pressure_area), pressure
		);
	}
}

