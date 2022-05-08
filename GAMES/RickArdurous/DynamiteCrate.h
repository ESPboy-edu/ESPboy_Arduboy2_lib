#ifndef _DYNAMITE_CRATE_H_
#define _DYNAMITE_CRATE_H_

#include "PickUpItem.h"

class DynamiteCrate : public PickUpItem
{
public:
	virtual bool Update(UpdateStep step);
};

#endif