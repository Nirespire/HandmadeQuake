#include "quakedef.h"

double realtime = 0;
double oldrealtime = 0;
double host_frametime = 0;

extern void* BackBuffer;

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
	VID_Init();
}

void host_frame(float timestep) {
	if (!host_filterTime(timestep)) {
		return;
	}

	sys_sendKeyEvents();

	VID_Update();



	// update game
	// render scene
}

void host_shutdown(void) {
	VID_Shutdown();
}