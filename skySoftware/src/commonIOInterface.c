#include "commonIOInterface.h"

int devWrite(sky_comDriver *dev, uint8_t *data, int len)
{
	return dev->write(dev, data, len);
}


int Write2dev(sky_comDriver *dev, uint8_t *data, int len)
{
	if (dev->fifo != 0)
		myFIFOWrite(dev->fifo, data, len);

	if (dev->single != 0)
		dev->single(dev, data, len);
	
	return 0;
}

