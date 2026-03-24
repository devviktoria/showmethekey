#include "smtk-controller-emitter.h"

#include <fcntl.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <stdio.h>

#include "smtk-types.h"

struct _SmtkControllerEmitter {
	GObject parent_instance;

	GThread *poller;
	gboolean polling;

	int fd;
};

G_DEFINE_TYPE(SmtkControllerEmitter, smtk_controller_emitter, G_TYPE_OBJECT)

enum { SIG_CONTROLLER, N_SIGNALS };

static guint sigs[N_SIGNALS] = { 0 };

// ==============================
// UI THREAD DISPATCH
// ==============================

struct controller_idle_data {
	SmtkControllerEmitter *emitter;
	SmtkControllerEventType event_type;
	guint8 number; /* axis/button index */

	double value; /* axis */
	gboolean pressed; /* button */
};

static gboolean controller_idle(gpointer data)
{
	struct controller_idle_data *d = data;

	g_signal_emit_by_name(
		d->emitter,
		"controller",
		d->event_type,
		d->number,
		d->value,
		d->pressed
	);

	g_object_unref(d->emitter);
	g_free(d);

	return G_SOURCE_REMOVE;
}

static void trigger_controller_idle(
	SmtkControllerEmitter *this,
	SmtkControllerEventType event_type,
	guint8 number,
	double value,
	gboolean pressed
)
{
	struct controller_idle_data *d = g_malloc0(sizeof(*d));

	d->emitter = g_object_ref(this);
	d->event_type = event_type;
	d->number = number;
	d->value = value;
	d->pressed = pressed;

	g_idle_add_full(G_PRIORITY_DEFAULT, controller_idle, d, NULL);
}

// ==============================
// POLLING THREAD
// ==============================

static gpointer poll_joystick(gpointer data)
{
	SmtkControllerEmitter *this = data;

	struct js_event e;

	while (this->polling) {

		ssize_t n = read(this->fd, &e, sizeof(e));

		if (n < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				g_usleep(1000); // no data yet
				continue;
			} else {
				g_warning("Joystick read error");
				break;
			}
		}

		if (n != sizeof(e)) {
			continue;
		}

		guint8 type = e.type & ~JS_EVENT_INIT;

		if (type == JS_EVENT_AXIS) {
			// normalize: -32767..32767 → -1..1
			double v = (double)e.value / 32767.0;

			trigger_controller_idle(
				this,
				SMTK_CONTROLLER_EVENT_AXIS,
				e.number,
				v,
				FALSE
			);
		}

		if (type == JS_EVENT_BUTTON) {
			trigger_controller_idle(
				this,
				SMTK_CONTROLLER_EVENT_BUTTON,
				e.number,
				0.0,
				e.value ? TRUE : FALSE
			);
		}
	}

	return NULL;
}

// ==============================
// LIFECYCLE
// ==============================

static void smtk_controller_emitter_dispose(GObject *object)
{
	SmtkControllerEmitter *this = SMTK_CONTROLLER_EMITTER(object);

	smtk_controller_emitter_stop_async(this);

	G_OBJECT_CLASS(smtk_controller_emitter_parent_class)->dispose(object);
}

static void
smtk_controller_emitter_class_init(SmtkControllerEmitterClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = smtk_controller_emitter_dispose;

	sigs[SIG_CONTROLLER] = g_signal_new(
		"controller",
		SMTK_TYPE_CONTROLLER_EMITTER,
		G_SIGNAL_RUN_LAST,
		0,
		NULL,
		NULL,
		g_cclosure_marshal_generic,
		G_TYPE_NONE,
		4,
		G_TYPE_INT, // SmtkControllerEventType
		G_TYPE_UINT, // number
		G_TYPE_DOUBLE, // value (axis)
		G_TYPE_BOOLEAN // pressed (button)
	);
}

static void smtk_controller_emitter_init(SmtkControllerEmitter *this)
{
	this->poller = NULL;
	this->polling = FALSE;
	this->fd = -1;
}

// ==============================
// PUBLIC API
// ==============================

SmtkControllerEmitter *smtk_controller_emitter_new(void)
{
	return g_object_new(SMTK_TYPE_CONTROLLER_EMITTER, NULL);
}

void smtk_controller_emitter_start_async(SmtkControllerEmitter *this)
{
	g_debug("Calling smtk_controller_emitter_start_async().");
	g_return_if_fail(this != NULL);

	if (this->polling)
		return;

	this->fd = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);

	if (this->fd < 0) {
		g_warning("Failed to open joystick device.");
		return;
	}

	this->polling = TRUE;

	this->poller = g_thread_new("controller-poller", poll_joystick, this);
}

void smtk_controller_emitter_stop_async(SmtkControllerEmitter *this)
{
	g_debug("Calling smtk_controller_emitter_stop_async().");
	g_return_if_fail(this != NULL);

	if (!this->polling)
		return;
	this->polling = FALSE;

	if (this->fd >= 0) {
		close(this->fd);
		this->fd = -1;
	}

	if (this->poller) {
		g_thread_join(this->poller);
		this->poller = NULL;
	}
}

