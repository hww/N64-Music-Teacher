#include <ultra64.h>
#include "math.h"

// Origin vector for emptying things
vec3_t		vec3_origin={0,0,0};

/*
 * Calculates an axial normalized vector rotated over 'angles'
 * This is used for placing the letters in the right place when
 * the whole scroller is rotating
 */
void AngleVectors (vec3_t angles, vec3_t out)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[1] * (M_PI*2 / 360);
	sr = sinf(angle);
	cr = cosf(angle);
	angle = angles[2] * (M_PI*2 / 360);
	sp = sinf(angle);
	cp = cosf(angle);

	out[0] = cp*cr;
	out[1] = sp*cr;
	out[2] = -sr;
}

