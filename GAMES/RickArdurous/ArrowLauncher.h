#ifndef _ARROW_LAUNCHER_H_
#define _ARROW_LAUNCHER_H_

#include "ArrowBullet.h"

class ArrowLauncher : public Item
{
public:
	ArrowLauncher();
	void Init(int startX, unsigned char startY, unsigned char flags, unsigned char detectionWidth);
	virtual bool Update(UpdateStep step);

private:
	static constexpr int LAUNCH_PERIOD = 180;
	static constexpr int CAN_LAUNCH_ARROW = 255;
	
	ArrowBullet Arrow;
	unsigned char LastLaunchTime = CAN_LAUNCH_ARROW;
	unsigned char DetectionWidth;
	
	static void CheckTrigererCallback(Item * me, Item * trigerer);
	void CheckTrigerer(bool isAlive, int trigererX, unsigned char trigererY);
};

#endif