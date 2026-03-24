#include <gtk/gtk.h>

#include "smtk-enum-types.h"
#include "smtk-mouse-area.h"
#include "smtk-types.h"

/*
struct key_data {
	char *string;
	int counter;
};

static void free_key_data(void *data)
{
	g_autofree struct key_data *key_data = data;
	if (key_data == NULL)
		return;
	g_clear_pointer(&key_data->string, g_free);
}*/

struct _SmtkMouseArea {
	GtkDrawingArea parent_instance;
	GSettings *settings;

	GSList *keys;
	// Use single list because we only prepend and iter.
	GMutex keys_mutex;

	PangoLayout *string_layout;
	PangoFontDescription *string_font;
	PangoLayout *counter_layout;
	PangoFontDescription *counter_font;

	gboolean buttons[SMTK_MOUSE_BUTTON_UNKNOWN];

	int width;
	int height;
	int string_height;
	int counter_height;
	double margin_ratio;
	int margin;
	int padding;
	int last_key_x;
	bool draw_border;

	GThread *timer_thread;
	bool timer_running;
	GTimer *timer;
	int timeout;
};
G_DEFINE_TYPE(SmtkMouseArea, smtk_mouse_area, GTK_TYPE_DRAWING_AREA)

enum { PROP_0, PROP_DRAW_BORDER, PROP_MARGIN_RATIO, PROP_TIMEOUT, N_PROPS };

static GParamSpec *props[N_PROPS] = { NULL };

static void set_property(
	GObject *o,
	unsigned int prop,
	const GValue *value,
	GParamSpec *pspec
)
{
	SmtkMouseArea *this = SMTK_MOUSE_AREA(o);

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
	SmtkMouseArea *this = SMTK_MOUSE_AREA(o);

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

static void
draw(GtkDrawingArea *drawing_area,
     cairo_t *cr,
     int width,
     int height,
     void *data)
{
	SmtkMouseArea *this = SMTK_MOUSE_AREA(drawing_area);

	// ---- Méretezés ----

	double content_height = height * 0.75;
	double content_width = content_height * 0.75;

	double border = 8.0; // keret vastagság
	double padding = 8.0; // belső távolság a gombok között

	// középre igazítás
	double origin_x = (width - content_width) / 2.0;
	double origin_y = (height - content_height) / 2.0;

	// ---- Külső keret ----

	cairo_set_source_rgb(cr, 1, 1, 1); // fehér
	cairo_set_line_width(cr, border);
	cairo_rectangle(cr, origin_x, origin_y, content_width, content_height);
	cairo_stroke(cr);

	// ---- Belső terület ----

	double inner_x = origin_x + border + padding;
	double inner_y = origin_y + border + padding;

	double inner_width = content_width - 2 * border - 2 * padding;
	double inner_height = content_height - 2 * border - 2 * padding;

	// ---- Gomb méretek ----

	double button_width = (inner_width - padding) / 2.0;
	double button_height = inner_height;

	double middle_width = button_width * 0.5;
	double middle_height = button_height * 0.5;

	double left_x = inner_x;
	double right_x = inner_x + button_width + padding;
	double top_y = inner_y;

	double middle_x = inner_x + (inner_width - middle_width) / 2.0;
	double middle_y = inner_y + (inner_height - middle_height) / 3.0;

	// ---- Bal gomb ----
	if (this->buttons[SMTK_MOUSE_BUTTON_LEFT])
		cairo_set_source_rgb(cr, 1, 1, 1);
	else
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);

	cairo_rectangle(cr, left_x, top_y, button_width, button_height);
	cairo_fill(cr);

	// ---- Jobb gomb ----
	if (this->buttons[SMTK_MOUSE_BUTTON_RIGHT])
		cairo_set_source_rgb(cr, 1, 1, 1);
	else
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);

	cairo_rectangle(cr, right_x, top_y, button_width, button_height);
	cairo_fill(cr);

	// Middle gomb “átlátszó” keret
	cairo_save(cr); // mentsük a state-et

	// Set composition mode to clear
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);

	// Rajzoljuk a téglalapot
	cairo_rectangle(
		cr,
		middle_x - 3.0,
		middle_y - 3.0,
		middle_width + 6.0,
		middle_height + 6.0
	);
	cairo_fill(cr);

	cairo_restore(cr); // visszaállítjuk a normál rajzolási módot
	// ---- Middle gomb ----
	if (this->buttons[SMTK_MOUSE_BUTTON_MIDDLE])
		cairo_set_source_rgb(cr, 1, 1, 1);
	else
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);

	cairo_rectangle(cr, middle_x, middle_y, middle_width, middle_height);
	cairo_fill(cr);

	// Middle gomb border
	/*cairo_set_source_rgb(cr, 0, 0, 0); // fehér keret
	cairo_set_line_width(cr, 3.0); // vékony vonal
	cairo_rectangle(cr, middle_x, middle_y, middle_width, middle_height);
	cairo_stroke(cr);*/
}

// true and false are C99 _Bool, but GLib expects gboolean, which is C99 int.
static int trigger_redraw(void *data)
{

	return 0;
}

static void *on_timeout(void *data)
{
	SmtkMouseArea *this = data;

	while (this->timer_running) {
		/*
		g_mutex_lock(&this->keys_mutex);
		int elapsed = g_timer_elapsed(this->timer, NULL) * 1000.0;
		g_mutex_unlock(&this->keys_mutex);

		if (this->timeout > 0 && elapsed > this->timeout) {
			g_debug("Timer triggered, clear keys.");
			g_mutex_lock(&this->keys_mutex);
			g_clear_slist(&this->keys, free_key_data);
			g_timer_start(this->timer);
			g_mutex_unlock(&this->keys_mutex);

			// Here is not UI thread so we need to kick an async
			// callback into GLib's main loop.
			g_timeout_add_full(
				G_PRIORITY_DEFAULT,
				0,
				trigger_redraw,
				g_object_ref(this),
				g_object_unref
			);
		}*/

		g_usleep(1000L);
	}

	return NULL;
}

static void constructed(GObject *o)
{
	SmtkMouseArea *this = SMTK_MOUSE_AREA(o);

	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(this), draw, NULL, NULL);

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

	G_OBJECT_CLASS(smtk_mouse_area_parent_class)->constructed(o);
}

static void dispose(GObject *o)
{
	SmtkMouseArea *this = SMTK_MOUSE_AREA(o);

	g_clear_object(&this->settings);

	g_clear_object(&this->string_layout);
	g_clear_pointer(&this->string_font, pango_font_description_free);

	g_clear_object(&this->counter_layout);
	g_clear_pointer(&this->counter_font, pango_font_description_free);

	if (this->timer_thread != NULL) {
		this->timer_running = false;
		g_thread_join(this->timer_thread);
		this->timer_thread = NULL;
	}

	g_clear_pointer(&this->timer, g_timer_destroy);

	G_OBJECT_CLASS(smtk_mouse_area_parent_class)->dispose(o);
}

static void finalize(GObject *o)
{
	SmtkMouseArea *this = SMTK_MOUSE_AREA(o);

	//g_clear_slist(&this->keys, free_key_data);

	G_OBJECT_CLASS(smtk_mouse_area_parent_class)->finalize(o);
}

static void smtk_mouse_area_class_init(SmtkMouseAreaClass *klass)
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

static void smtk_mouse_area_init(SmtkMouseArea *this)
{
	this->settings = NULL;
	//this->left_pressed = false;
	//this->middle_pressed = false;
	//this->right_pressed = false;
	this->width = 0;
	this->height = 0;

	this->string_layout = NULL;
	this->string_font = pango_font_description_new();
	pango_font_description_set_family(this->string_font, "monospace");
	this->string_height = 0;

	this->counter_layout = NULL;
	this->counter_font = pango_font_description_new();
	pango_font_description_set_family(this->counter_font, "monospace");
	this->counter_height = 0;

	this->keys = NULL;
	g_mutex_init(&this->keys_mutex);

	this->timer_running = true;
	this->timer = g_timer_new();

	// Just ignore timer thread error because it should work without timer.
	this->timer_thread = g_thread_try_new("timer", on_timeout, this, NULL);
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

	g_debug("Adding mouse button: %u.", button);

	this->buttons[button] = pressed;

	// Trigger re-draw because content changed.
	gtk_widget_queue_draw(GTK_WIDGET(this));
}

