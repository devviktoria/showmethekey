#pragma once

typedef enum _SmtkMouseButton {
	SMTK_MOUSE_BUTTON_LEFT,
	SMTK_MOUSE_BUTTON_RIGHT,
	SMTK_MOUSE_BUTTON_MIDDLE,
	SMTK_MOUSE_BUTTON_UNKNOWN
} SmtkMouseButton;

typedef enum _SmtkControllerEventType {
	SMTK_CONTROLLER_EVENT_BUTTON,
	SMTK_CONTROLLER_EVENT_AXIS
} SmtkControllerEventType;

void smtk_register_types(void);

