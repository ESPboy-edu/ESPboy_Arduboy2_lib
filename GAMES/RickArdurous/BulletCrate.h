#ifndef _BULLET_CRATE_H_
#define _BULLET_CRATE_H_

#include "PickUpItem.h"

class BulletCrate : public PickUpItem
{
public:
	virtual bool Update(UpdateStep step);
};

#endif