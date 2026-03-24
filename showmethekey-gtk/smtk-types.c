#include "smtk-button-area.h"
#include "smtk-pressure-area.h"
#include "smtk-sundial-area.h"

void smtk_register_types(void)
{
	smtk_button_area_get_type();
	smtk_pressure_area_get_type();
	smtk_sundial_area_get_type();
}

