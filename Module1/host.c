#include "quakedef.h"

double realtime = 0;
double oldrealtime = 0;
double host_frametime = 0;

qboolean host_filterTime(float time) {

	realtime += time;

	if (realtime - oldrealtime < 1.0 / 72.0) {
		return false;
	}

	host_frametime = realtime - oldrealtime;
	oldrealtime = realtime;

	return true;
}

void host_init(void) {

}

void host_frame(float timestep) {
	if (!host_filterTime(timestep)) {
		return;
	}

	// update game
	// render scene
}

void host_shutdown(void) {

}