#pragma once

#include "ColormapRenderer.h"



class GlyphRenderer : public ColormapRenderer
{
public:

enum SCALING_MODE
{
	SCALE_NORMALIZE = 0,
	SCALE_SCALE
};	

				GlyphRenderer(): scaling_mode(SCALE_NORMALIZE),scaling_factor(1) {}


void			setScalingMode(SCALING_MODE s)
				{ scaling_mode = s; }

void			setScalingFactor(float s)
				{ scaling_factor = s; }

float			getScalingFactor()
				{ return scaling_factor; }

void			changeScalingMode()
				{ scaling_mode = (scaling_mode==SCALE_NORMALIZE)? SCALE_SCALE : SCALE_NORMALIZE; }

void			draw(Grid&);

protected:

SCALING_MODE	scaling_mode;

float			scaling_factor;

private:

void			drawCones(Grid& g);
};



						

