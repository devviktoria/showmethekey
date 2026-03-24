#include <gtk/gtk.h>
#include <adwaita.h>

#include "smtk-controller-widget.h"
#include "smtk-button-area.h"
#include "smtk-pressure-area.h"
#include "smtk-sundial-area.h"

struct _SmtkControllerWidget {
	GtkGrid parent_instance;

	/* template children */
	SmtkPressureArea *left_trigger;
	SmtkPressureArea *right_trigger;

	SmtkButtonArea *left_bumper;
	SmtkButtonArea *middle_button_1;
	SmtkButtonArea *middle_button_2;
	SmtkButtonArea *middle_button_3;
	SmtkButtonArea *right_bumper;

	SmtkButtonArea *dpad_up;
	SmtkButtonArea *dpad_left;
	SmtkButtonArea *dpad_right;
	SmtkButtonArea *dpad_down;

	SmtkSundialArea *left_stick;
	SmtkSundialArea *right_stick;

	SmtkButtonArea *button_y;
	SmtkButtonArea *button_x;
	SmtkButtonArea *button_a;
	SmtkButtonArea *button_b;
};

G_DEFINE_TYPE(SmtkControllerWidget, smtk_controller_widget, GTK_TYPE_GRID)

static void setup_buttons(SmtkControllerWidget *this)
{
	/* bumpers → rounded rect */
	smtk_button_area_set_shape(
		this->left_bumper, SMTK_BUTTON_SHAPE_ROUNDED_RECT
	);
	smtk_button_area_set_shape(
		this->middle_button_1, SMTK_BUTTON_SHAPE_ROUNDED_RECT
	);
	smtk_button_area_set_shape(
		this->middle_button_2, SMTK_BUTTON_SHAPE_ROUNDED_RECT
	);
	smtk_button_area_set_shape(
		this->middle_button_3, SMTK_BUTTON_SHAPE_ROUNDED_RECT
	);
	smtk_button_area_set_shape(
		this->right_bumper, SMTK_BUTTON_SHAPE_ROUNDED_RECT
	);

	/* dpad */
	smtk_button_area_set_shape(this->dpad_up, SMTK_BUTTON_SHAPE_DPAD_UP);
	smtk_button_area_set_shape(this->dpad_down, SMTK_BUTTON_SHAPE_DPAD_DOWN);
	smtk_button_area_set_shape(this->dpad_left, SMTK_BUTTON_SHAPE_DPAD_LEFT);
	smtk_button_area_set_shape(
		this->dpad_right, SMTK_BUTTON_SHAPE_DPAD_RIGHT
	);

	/* ABXY colors */
	smtk_button_area_set_shape(this->button_y, SMTK_BUTTON_SHAPE_CIRCLE);
	smtk_button_area_set_colors(
		this->button_y, 0.66, 0.33, 0.0, 1.0, 0.66, 0.0
	);

	smtk_button_area_set_shape(this->button_x, SMTK_BUTTON_SHAPE_CIRCLE);
	smtk_button_area_set_colors(
		this->button_x, 0.0, 0.33, 1.0, 0.0, 0.66, 1.0
	);

	smtk_button_area_set_shape(this->button_a, SMTK_BUTTON_SHAPE_CIRCLE);
	smtk_button_area_set_colors(
		this->button_a, 0.0, 0.33, 0.0, 0.0, 0.66, 0.0
	);

	smtk_button_area_set_shape(this->button_b, SMTK_BUTTON_SHAPE_CIRCLE);
	smtk_button_area_set_colors(
		this->button_b, 0.66, 0.0, 0.0, 1.0, 0.0, 0.0
	);
}

static void smtk_controller_widget_class_init(SmtkControllerWidgetClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	gtk_widget_class_set_template_from_resource(
		widget_class,
		"/one/alynx/showmethekey/smtk-controller-widget.ui"
	);

	/* bind children */
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, left_trigger
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, right_trigger
	);

	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, left_bumper
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, middle_button_1
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, middle_button_2
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, middle_button_3
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, right_bumper
	);

	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, dpad_up
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, dpad_left
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, dpad_right
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, dpad_down
	);

	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, left_stick
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, right_stick
	);

	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, button_y
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, button_x
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, button_a
	);
	gtk_widget_class_bind_template_child(
		widget_class, SmtkControllerWidget, button_b
	);
}

static void smtk_controller_widget_init(SmtkControllerWidget *this)
{
	gtk_widget_init_template(GTK_WIDGET(this));

	setup_buttons(this);
}

GtkWidget *smtk_controller_widget_new(void)
{
	return g_object_new(SMTK_TYPE_CONTROLLER_WIDGET, NULL);
}

static void smtk_controller_widget_handle_axis_event(
	SmtkControllerWidget *this,
	guint8 number,
	double value
)
{
	switch (number) {

	// LEFT STICK X
	case 0:
		smtk_sundial_area_set_vector(
			this->left_stick, value, 0.0, true, false
		);
		break;

	// LEFT STICK Y
	case 1:
		smtk_sundial_area_set_vector(
			this->left_stick, 0.0, -value, false, true
		);
		break;

	// LEFT TRIGGER
	case 2:
		smtk_pressure_area_set_value(this->left_trigger, value);
		break;

	// RIGHT STICK X
	case 3:
		smtk_sundial_area_set_vector(
			this->right_stick, value, 0.0, true, false
		);
		break;

	// RIGHT STICK Y
	case 4:
		smtk_sundial_area_set_vector(
			this->right_stick, 0.0, -value, false, true
		);
		break;

	// RIGHT TRIGGER
	case 5:
		smtk_pressure_area_set_value(this->right_trigger, value);
		break;

	// DPAD X → gombként kezeljük
	case 6:
		smtk_button_area_set_pressed(this->dpad_right, value > 0.5);
		smtk_button_area_set_pressed(this->dpad_left, value < -0.5);
		break;

	// DPAD Y → gombként kezeljük
	case 7:
		smtk_button_area_set_pressed(this->dpad_down, value > 0.5);
		smtk_button_area_set_pressed(this->dpad_up, value < -0.5);
		break;

	default:
		break;
	}
}

static void smtk_controller_widget_handle_button_event(
	SmtkControllerWidget *this,
	guint8 number,
	gboolean pressed
)
{
	switch (number) {

	case 0: // A
		smtk_button_area_set_pressed(this->button_a, pressed);
		break;

	case 1: // B
		smtk_button_area_set_pressed(this->button_b, pressed);
		break;

	case 2: // X
		smtk_button_area_set_pressed(this->button_x, pressed);
		break;

	case 3: // Y
		smtk_button_area_set_pressed(this->button_y, pressed);
		break;

	case 4: // Right bumper
		smtk_button_area_set_pressed(this->right_bumper, pressed);
		break;

	case 5: // Left bumper
		smtk_button_area_set_pressed(this->left_bumper, pressed);
		break;

	case 6:
		smtk_button_area_set_pressed(this->middle_button_1, pressed);
		break;

	case 7:
		smtk_button_area_set_pressed(this->middle_button_3, pressed);
		break;

	case 8:
		smtk_button_area_set_pressed(this->middle_button_2, pressed);
		break;

	default:
		break;
	}
}

void smtk_controller_widget_handle_event(
	SmtkControllerWidget *this,
	SmtkControllerEventType event_type,
	guint8 number,
	double value,
	gboolean pressed
)
{
	g_return_if_fail(this != NULL);

	switch (event_type) {
	case SMTK_CONTROLLER_EVENT_AXIS:
		smtk_controller_widget_handle_axis_event(this, number, value);
		break;

	case SMTK_CONTROLLER_EVENT_BUTTON:
		smtk_controller_widget_handle_button_event(
			this, number, pressed
		);
		break;

	default:
		break;
	}
}

