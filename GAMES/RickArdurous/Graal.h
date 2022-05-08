#ifndef _GRAAL_H_
#define _GRAAL_H_

#include "PickUpItem.h"

class Graal : public PickUpItem
{
public:
	virtual bool Update(UpdateStep step);
};

#endif