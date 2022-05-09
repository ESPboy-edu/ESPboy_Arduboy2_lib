#include "Ardulem.h"
#include "LemManager.h"
#include "Lem.h"
#include "HUD.h"
#include "MapManager.h"
#include "Input.h"
#include "MainMenu.h"
#include "LEDManager.h"
#include "MemoryPool.h"

namespace LemManager
{
	// This enum is a helper for the function that choose the best lemming under the cursor
	enum WalkerChoicePreference
	{
		DONT_CHOOSE = 0,
		CHOOSE_AFTER_OTHER,
		CHOOSE_BEFORE_OTHER,
	};
	
	// This struct is used for searching the best lem under the cursor
	struct LemChoice
	{
		signed char LemId;
		unsigned char Score;
	};
	
	// the array storing all the Lems
	Lem * LemArray = (Lem *)&MemoryPool::Pool;

	// The lem array is sorted, and we place the lem that needs counter at the beggining of the list
	unsigned char OutLemCount = 0; // the number of lem currently out and that should be managed in the LemArray
	unsigned char SpawnLemCount = 0; // the total number of lem that was spawned since the beginning of the game
	unsigned char InLemCount = 0; // this one is used to compute the score, but the end of the list is not sorted
	unsigned char GetInLemCount() { return InLemCount; }

	// after the last lem inside, we wait a little before going to the result page, for the sound to play
	const int16_t WAIT_TIME_AFTER_LAST_LEM_IN = 10;
	signed char TimeToGoToResultPage = -1;

	// for the lem that need to be stopped to do something (like the stairer that build a stair only for x turns, 
	// or the bomber that start their bye bye anim, after x seconds) we use another limited array to store their
	// remaining time. It's an array of bitfield, the bitfield contains a normalized frame count (generally inside a seconde)
	// and a number of remaining ticks.
	struct LemTimer
	{
		uint16_t	IsBombTimer		: 1; // (bool flag) Is this timer a bomb timer (bit set) or a stair timer (bit unset)
		uint16_t	LemId			: 5; // (0-31) the id of the lem in the LemArray array, knowing that the lem with timer will be place at the beginning of this array
		uint16_t	RemainingTick	: 4; // (0-15) the number of remaining tick for Bomb and stair
		uint16_t	TimeModulo		: 6; // (0-63) the value that should be tested when doing the modulo on the current frame counter
	};
	const int16_t MAX_LEM_TIMER_COUNT = 16; // if 8 stair + 8 bomb, then total can be 16.
	LemTimer LemTimerList[MAX_LEM_TIMER_COUNT];
	unsigned char LemTimerCount = 0; // the number of timer currently reccorded in the LemTimerList
	
	// the current lem targeted by the cursor
	signed char CurrentLemIndexUnderCursor;
	
	int16_t GetSpawnLemPercentage()	{ return ((int16_t)SpawnLemCount * 100) / MapManager::GetAvailableLemCount(); }
	int16_t GetInLemPercentage()	{ return ((int16_t)InLemCount * 100) / MapManager::GetAvailableLemCount(); }

	// private functions
	void UpdateInput();
	void UpdateLemUnderCursor();
	void CheckLemTimers();
	void MoveDeadLemToDeadPool();
	bool ChangeLemStateIfPossible(int16_t lemId, Lem::StateId newState);
	void ChooseBestLem(LemChoice & currentLem, signed char challengerLemId, bool dontPickFallers, bool dontPickBlocker, Lem::StateId preferedRole, bool preferDiggers, WalkerChoicePreference walkerChoicePreference, bool preferWalkerWithWallInFront);

	// lem array manipulation and sorting
	void SwapTwoTimers(unsigned char lemId1, unsigned char lemId2);
	void RemoveTimer(unsigned char timerId);
	void SwapTwoLems(unsigned char lemId1, unsigned char lemId2);
	void MoveLemToDeadList(unsigned char lemId);
	bool AddTimerToLem(unsigned char lemId, bool isBombTimer);
	bool ExtendStairTimerOfLem(unsigned char lemId);
	void RemoveTimerOfLem(unsigned char lemId, bool removeAllTimer);
}

void LemManager::Init()
{
	// reset the lem counters
	OutLemCount = 0;
	SpawnLemCount = 0;
	InLemCount = 0;
	LemTimerCount = 0;
	TimeToGoToResultPage = -1;
}

void LemManager::Update()
{
	// update the input of game play (move cursor, and click on lem).
	// Some inputs are directly handled in the HUD (like button to navigate in HUD)
	UpdateInput();

	// Search the lem under the cursor
	UpdateLemUnderCursor();
	
	if (MainMenu::IsCurrentGameStatePlaying())
	{
		// check if it is time to spawn a new lem (and if there're still to spawn)
		if (SpawnLemCount < MapManager::GetAvailableLemCount())
		{
			if (arduboy.everyXFrames(HUD::GetLemDropFrameRate()))
			{
				LemArray[OutLemCount++].Spawn();
				SpawnLemCount++;
			}
		}
		else
		{
			// check if all spawned and all dead
			if (OutLemCount == 0)
			{
				// check if we already set the end game time or check if we need to exit
				if (TimeToGoToResultPage == -1)
					TimeToGoToResultPage = arduboy.frameCount % WAIT_TIME_AFTER_LAST_LEM_IN;
				else if ((arduboy.frameCount % WAIT_TIME_AFTER_LAST_LEM_IN) == TimeToGoToResultPage)
					MainMenu::SetCurrentGameState(MainMenu::GameState::RESULT_PAGE);
			}
		}
		
		// then update each Lem
		for (int16_t i = 0; i < OutLemCount; ++i)
			if (LemArray[i].Update())
				RemoveTimerOfLem(i, false);
		
		// also override the state of the lem if they reach the end of their timer
		CheckLemTimers();
		
		// remove dead lems
		MoveDeadLemToDeadPool();
	}
}

void LemManager::UpdateInput()
{
	// early exit if button A is down, because that means we want to control the HUD
	if (Input::IsDown(A_BUTTON))
		return;
	
	//if pressing on the B button while HUD button is selected, and cursor over a lem, give order to the lem
	if (Input::IsJustPressed(B_BUTTON) && HUD::IsSelectedButtonValid() && CurrentLemIndexUnderCursor > -1)
	{
		unsigned char lemState = LemArray[CurrentLemIndexUnderCursor].GetCurrentState();
		// no order can be given to a crashing lem or a lem saying bye bye to explose
		if (lemState > Lem::StateId::BYE_BYE_BOOM)
		{
			bool hasChangeHappened = false;
			switch (HUD::GetSelectedButton())
			{
				case HUD::Button::LEM_WALK:
					if (hasChangeHappened = ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::WALK))
						MapManager::DecreaseWalkerCount();
					break;
					
				case HUD::Button::LEM_BLOCKER:
					if (hasChangeHappened = ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::BLOCKER))
						MapManager::DecreaseBlockerCount();
					break;
					
				case HUD::Button::LEM_BOMB:
					if (hasChangeHappened = AddTimerToLem(CurrentLemIndexUnderCursor, true))
						MapManager::DecreaseBomberCount();
					break;
					
				case HUD::Button::LEM_DIG_DIAG:
					if (hasChangeHappened = ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::DIG_DIAG))
						MapManager::DecreaseDiggerDiagonalCount();
					break;
					
				case HUD::Button::LEM_DIG_HORIZ:
					if (hasChangeHappened = ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::DIG_HORIZ))
						MapManager::DecreaseDiggerHorizontalCount();
					break;
					
				case HUD::Button::LEM_DIG_VERT:
					if (hasChangeHappened = ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::DIG_VERT))
						MapManager::DecreaseDiggerVerticalCount();
					break;
					
				case HUD::Button::LEM_STAIR:
					if (lemState < Lem::StateId::CLIMB)
					{
						// check if it is already a stairer that should be extended, or if we get a new one
						if (lemState == Lem::StateId::STAIR)
						{
							if (hasChangeHappened = ExtendStairTimerOfLem(CurrentLemIndexUnderCursor))
								MapManager::DecreaseStairerCount();
						}
						else
						{
							LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::STAIR);
							if (hasChangeHappened = AddTimerToLem(CurrentLemIndexUnderCursor, false))
								MapManager::DecreaseStairerCount();
						}
					}
					break;
					
				case HUD::Button::LEM_CLIMB:
					// promote the lem as climber, and decrease the count only he was not already a climber
					if (hasChangeHappened = LemArray[CurrentLemIndexUnderCursor].PromoteClimber())
						MapManager::DecreaseClimberCount();
					break;
					
				case HUD::Button::LEM_PARACHUTE:
					// promote the lem as parachuter, and decrease the count only he was not already a parachuter
					if (hasChangeHappened = LemArray[CurrentLemIndexUnderCursor].PromoteParachuter())
						MapManager::DecreaseParachuterCount();
					break;
			}

			// give visual feedback if we actually gave an order
			if (hasChangeHappened)
				LEDManager::StartLEDCommand(LEDManager::GAME, {0,0,1,5,0,60,0,1});
		}
	}
}

bool LemManager::ChangeLemStateIfPossible(int16_t lemId, Lem::StateId newState)
{
	// get the current state of the lem
	unsigned char currentState = LemArray[lemId].GetCurrentState();
	if ((currentState != newState) && (currentState < Lem::StateId::CLIMB))
	{
		// give the new state
		LemArray[lemId].SetCurrentState(newState);
		// and remove the timer, as the lem may be a stairer before
		RemoveTimerOfLem(lemId, false);
		// return true if we changed the state
		return true;
	}
	// return false as we didn't changed the state
	return false;
}

void LemManager::ChooseBestLem(LemChoice & currentLem, signed char challengerLemId, bool dontPickFallers, bool dontPickBlocker, Lem::StateId preferedRole, bool preferDiggers, WalkerChoicePreference walkerChoicePreference, bool preferWalkerWithWallInFront)
{
	unsigned char challengerLemState = LemArray[challengerLemId].GetCurrentState();
	
	// in any case, we don't choose a bomber (a bomber can never change role)
	// and if we should also ignore the lem falling, don't choose the challenger if he is falling
	// and similarly, ignore blockers if we need to
	// also we should not take a digger of the exact same type (because digger never stop so it's useless to give them the same order)
	if ((challengerLemState <= Lem::StateId::BYE_BYE_BOOM) ||
		(dontPickFallers && (challengerLemState >= Lem::StateId::START_FALL)) ||
		(dontPickBlocker && (challengerLemState == Lem::StateId::BLOCKER)) ||
		(preferDiggers && (challengerLemState == preferedRole)) )
		return;
	
	// check if the challenger has any role
	bool isChallengerAParachuter = LemArray[challengerLemId].IsAParachuter();
	bool isChallengerAClimber = LemArray[challengerLemId].IsAClimber();
	
	// now check the prefered role in priority (if not equals to DEAD, which means we don't have a prefered role)
	// if the challenger has the wanted role, return it
	if (	(preferedRole != Lem::StateId::DEAD) &&
			(	(preferedRole == challengerLemState) || 
				((preferedRole == Lem::StateId::PARACHUTE) && isChallengerAParachuter) ||
				((preferedRole == Lem::StateId::CLIMB) && isChallengerAClimber)
			)
		)
		currentLem = {challengerLemId, 0};
	
	// if the score is too good, no need for doing further check
	if (currentLem.Score <= 1)
		return;

	// now check if we need to select in priority the diggers
	// (we have already eraly exit if the challenger is the same type of digger than the prefered role, 
	// so we are sure to not pick the same role)
	if (preferDiggers && (challengerLemState >= Lem::StateId::DIG_DIAG) && (challengerLemState <= Lem::StateId::DIG_VERT))
		currentLem = {challengerLemId, 1};

	// if the score is too good, no need for doing further check
	if (currentLem.Score <= 2)
		return;

	bool hasChallengerARole = isChallengerAParachuter || isChallengerAClimber ||
							((challengerLemState >= Lem::StateId::BLOCKER) && (challengerLemState <= Lem::StateId::STAIR));
	
	// now check if we need to choose walker before the other roles
	if ((walkerChoicePreference == WalkerChoicePreference::CHOOSE_BEFORE_OTHER) && !hasChallengerARole)
	{
		// only climber check to request wall with a walker choice before, and for climber, we don't want to check stairs)
		if (!preferWalkerWithWallInFront || LemArray[challengerLemId].IsThereAWallInFrontOfYou(2, false))
			currentLem = {challengerLemId, 2};
		else
			currentLem = {challengerLemId, 5};
	}

	// if the score is too good, no need for doing further check
	if (currentLem.Score <= 3)
		return;
	
	// now check if the challenger has any role (not a simple walker)
	if (hasChallengerARole)
		currentLem = {challengerLemId, 3};

	// if the score is too good, no need for doing further check
	if (currentLem.Score <= 4)
		return;
	
	// finally check again the walker if we need to choose walker after the other roles
	if ((walkerChoicePreference == WalkerChoicePreference::CHOOSE_AFTER_OTHER) && !hasChallengerARole)
	{
		// only horizontal diggers check to request wall with a walker choice after, and for horizontal digger, we want to check stairs to dig through)
		if (!preferWalkerWithWallInFront || LemArray[challengerLemId].IsThereAWallInFrontOfYou(0, true))
			currentLem = {challengerLemId, 4};
	}
}

/*
 * This function try to find the best lem under the current cursor.
 * The function will choose in priority different type of lem depending on the HUD button selected:
 * - If the walker is selected: it will choose a blocker first, then any lem with a function, (not a bomber/walker)
 * - If the blocker is selected: it will choose any lem with a function (not a blocker/bomber), then a walker
 * - If the bomb action is selected, it will choose a Blocker in priority, then any lem with a function (not a bomber), then a walker
 * - If a digger is selected, if will choose a digger in priority (of different type), then any function (not a bomber/digger of same type), then a walker
 * - if a stair is selected, it will choose a stairer first, then any function (not a bomber), then a walker
 * - If the climber is selected, it will choose a parachuter first, then a walker, then any function (not a bomber)
 * - If the parachute is selected, it will choose a Climber first, then a walker, then any function (not a bomber)
 */
void LemManager::UpdateLemUnderCursor()
{
	// declare a local lem choice struct and init its score with a high value
	LemChoice currentLemChoice = {-1, 10 };
	
	// iterate on all lem to find the best candidate
	for (signed char i = 0; i < OutLemCount; ++i)
		if (LemArray[i].IsUnderCursorPosition())
		{
			// otherwise check if it is better one
			switch (HUD::GetSelectedButton())
			{
				case HUD::Button::LEM_WALK:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last
					ChooseBestLem(currentLemChoice, i, true, false, Lem::StateId::BLOCKER, false, WalkerChoicePreference::DONT_CHOOSE, false);
					break;
				case HUD::Button::LEM_BLOCKER:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last
					ChooseBestLem(currentLemChoice, i, true, true, Lem::StateId::DEAD, false, WalkerChoicePreference::CHOOSE_AFTER_OTHER, false);
					break;
				case HUD::Button::LEM_BOMB:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last
					ChooseBestLem(currentLemChoice, i, true, false, Lem::StateId::BLOCKER, false, WalkerChoicePreference::CHOOSE_AFTER_OTHER, false);
					break;
				case HUD::Button::LEM_DIG_DIAG:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last
					ChooseBestLem(currentLemChoice, i, true, false, Lem::StateId::DIG_DIAG, true, WalkerChoicePreference::CHOOSE_AFTER_OTHER, false);
					break;
				case HUD::Button::LEM_DIG_HORIZ:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last, better to have a wall in front
					ChooseBestLem(currentLemChoice, i, true, false, Lem::StateId::DIG_HORIZ, true, WalkerChoicePreference::CHOOSE_AFTER_OTHER, true);
					break;
				case HUD::Button::LEM_DIG_VERT:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last
					ChooseBestLem(currentLemChoice, i, true, false, Lem::StateId::DIG_VERT, true, WalkerChoicePreference::CHOOSE_AFTER_OTHER, false);
					break;
				case HUD::Button::LEM_STAIR:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last
					ChooseBestLem(currentLemChoice, i, true, false, Lem::StateId::STAIR, false, WalkerChoicePreference::CHOOSE_AFTER_OTHER, false);
					break;
				case HUD::Button::LEM_CLIMB:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last, better to have a wall in front
					ChooseBestLem(currentLemChoice, i, false, false, Lem::StateId::PARACHUTE, false, WalkerChoicePreference::CHOOSE_BEFORE_OTHER, true);
					break;
				case HUD::Button::LEM_PARACHUTE:
					// current, challenger, don't pick faller, don't pick blocker, prefered state, prefer diggers first, choose walker at last
					ChooseBestLem(currentLemChoice, i, false, false, Lem::StateId::CLIMB, false, WalkerChoicePreference::CHOOSE_BEFORE_OTHER, false);
					break;
			}
			
			// if we found the best lem stop the loop
			if (currentLemChoice.Score == 0)
				break;
		}
	
	// set the lem pointer with the one found in the current lem choice
	CurrentLemIndexUnderCursor = currentLemChoice.LemId;

	// Set the correct cursor depending if we found something or not
	// (don't care if we read data outside the LemArray if the current lem is -1, has the data will not be used)
	HUD::SetCursorShape(CurrentLemIndexUnderCursor != -1, LemArray[CurrentLemIndexUnderCursor].IsDirectionMirrored());
}

/*
 * Swap the two specified id inside the LemTimerList
 */
void LemManager::SwapTwoTimers(unsigned char timerId1, unsigned char timerId2)
{
	if (timerId1 != timerId2)
	{
		LemTimer swap;
		memccpy(&swap,                     &(LemTimerList[timerId1]), 1, sizeof(swap));
		memccpy(&(LemTimerList[timerId1]), &(LemTimerList[timerId2]), 1, sizeof(swap));
		memccpy(&(LemTimerList[timerId2]), &swap,                     1, sizeof(swap));
	}
}

/*
 * Remove the specified timer id from the list of timer
 */
void LemManager::RemoveTimer(unsigned char timerId)
{
	// swap the specified timer with the end, and decrease the timer count
	LemTimerCount--;
	if (LemTimerCount > 0)
		SwapTwoTimers(timerId, LemTimerCount);
}

/*
 * Swap the two specified lem in the lem array. This is used to reorder the lem array.
 */
void LemManager::SwapTwoLems(unsigned char lemId1, unsigned char lemId2)
{
	// does nothing if the two lems are the same
	if (lemId1 != lemId2)
	{
		Lem swap;
		memccpy(&swap,               &(LemArray[lemId1]), 1, sizeof(swap));
		memccpy(&(LemArray[lemId1]), &(LemArray[lemId2]), 1, sizeof(swap));
		memccpy(&(LemArray[lemId2]), &swap,               1, sizeof(swap));
	}
}

/*
 * Move one specific lem id inside the dead list. This will adjust the out counter as needed.
 */
void LemManager::MoveLemToDeadList(unsigned char lemId)
{
	// nothing to do if the lem is already dead
	if (lemId < OutLemCount)
	{
		OutLemCount--;
		if (OutLemCount > 0)
			SwapTwoLems(lemId, OutLemCount);
		
		// if the lem had a timer(s), we should remove them
		RemoveTimerOfLem(lemId, true);
	}
}

/*
 * This function goes through all the out lem, and check the state of them. If there state is DEAD,
 * it will move them in the dead pool to stop updating them
 */
void LemManager::MoveDeadLemToDeadPool()
{
	for (int16_t i = 0; i < OutLemCount; ++i)
		if (LemArray[i].GetCurrentState() == Lem::StateId::DEAD)
		{
			MoveLemToDeadList(i);
			i--;
		}
}

bool LemManager::AddTimerToLem(unsigned char lemId, bool isBombTimer)
{
	const int16_t LEM_ID_MAX_STORABLE_VALUE = 32; //We use 32 because LemTimer.LemId use 5 bits
	
	// check if the current lem id is sufficiently low so that we can save it's id in the timer instance
	// in such case, we don't need to swap his place to move it at the beginning of the LemArray list
	// but if the lem id is too big, we need to swap it first.
	if (lemId >= LEM_ID_MAX_STORABLE_VALUE)
	{
		// if the lem id is too far in the lem array, we need to swap it closer to the beggining,
		// and we are sure that it is not already inside the timer array (because it could not have been stored)
		// so we will search for a free space (a lem that doesn't have a timer) starting from the beggining
		int16_t newIndex = 0;
		for (int16_t i = 0; i < LEM_ID_MAX_STORABLE_VALUE; ++i)
		{
			bool isAnyTimerFound = false;
			for (int16_t j = 0; j < LemTimerCount; ++j)
				if (LemTimerList[j].LemId == i)
				{
					isAnyTimerFound = true;
					break;
				}
			// check if the current i is a good candidate (no timer found)
			if (!isAnyTimerFound)
			{
				newIndex = i;
				break;
			}
		}
		// now swap the lem with the new found index, and update the lem id
		SwapTwoLems(lemId, newIndex);
		lemId = newIndex;
	}
	else
	{
		// If the lem id is small, maybe this lem already has a timer, so we should check and early exit if it is the case
		// iterate on the timer list to check if the lem doesn't already have a timer of the speciified type
		// if we found one, return false, meaning no timer was added
		for (int16_t i = 0; i < LemTimerCount; ++i)
			if ((LemTimerList[i].LemId == lemId) && (LemTimerList[i].IsBombTimer == isBombTimer))
				return false;
	}
	
	// now insert the timer in the timer array
	LemTimerList[LemTimerCount].IsBombTimer = isBombTimer;
	LemTimerList[LemTimerCount].LemId = lemId;
	LemTimerList[LemTimerCount].RemainingTick = isBombTimer ? 4 : 10;
	LemTimerList[LemTimerCount].TimeModulo = arduboy.frameCount % HUD::NORMAL_SPEED_FPS;
	LemTimerCount++;
	
	// return true because the timer has been added.
	return true;
}

bool LemManager::ExtendStairTimerOfLem(unsigned char lemId)
{
	// normally the lemId should always be valid, but test if we are et the end of the stair
	for (int16_t i = 0; i < LemTimerCount; ++i)
		if ((LemTimerList[i].LemId == lemId) && !LemTimerList[i].IsBombTimer && (LemTimerList[i].RemainingTick < 4))
		{
			LemTimerList[i].RemainingTick += 10;
			LEDManager::ClearLEDCommand(LEDManager::LEM);
			return true;
		}
	return false;
}

void LemManager::RemoveTimerOfLem(unsigned char lemId, bool removeAllTimer)
{
	// iterate the timer list to find all the timers with the specified lemId
	for (int16_t i = 0; i < LemTimerCount; ++i)
		if ((LemTimerList[i].LemId == lemId) && (removeAllTimer || !LemTimerList[i].IsBombTimer))
		{
			// remove this timer from the list
			RemoveTimer(i);
			// and decrease also i to check again the new guys that took his place
			i--;
		}
}

/*
 * This function check all the timer and may update the lem status if the timer reach zero
 */
void LemManager::CheckLemTimers()
{
	// check the lem timer
	for (int16_t i = 0; i < LemTimerCount; ++i)
	{
		// get the lemId
		int16_t lemId = LemTimerList[i].LemId;
		int16_t remainingTicks = LemTimerList[i].RemainingTick;

		// draw the time abobe his head for the bombers
		if (remainingTicks < 4)
			LemArray[lemId].DrawTimerAboveHead(remainingTicks);

		int16_t currentFPS = HUD::NORMAL_SPEED_FPS;
		int16_t currentTimeModulo = LemTimerList[i].TimeModulo;
		if (HUD::IsGameSpeedUp())
		{
			currentFPS = HUD::FAST_SPEED_FPS;
			currentTimeModulo /= HUD::RATIO_BETWEEN_FAST_AND_NORMAL_FPS;
		}

		// check if it's time to tick the timer
		if ((arduboy.frameCount % currentFPS) == currentTimeModulo)
		{
			// decrease the tick and check if it reaches zero
			LemTimerList[i].RemainingTick--;
			if (LemTimerList[i].RemainingTick == 0)
			{
				// if it is a bomb timer, set the bomb anim, and the stair is finished, return to walk
				if (LemTimerList[i].IsBombTimer)
					LemArray[lemId].SetCurrentState(Lem::StateId::BYE_BYE_BOOM);
				else
					LemArray[lemId].SetCurrentState(Lem::StateId::WALK);
				
				// remove the timer for the current lem
				RemoveTimer(i);
				// and decrease also i to check again the new guys that took his place
				i--;
			}
			else if ((LemTimerList[i].RemainingTick == 3) && !LemTimerList[i].IsBombTimer)
			{
				// start a stair led count down
				LEDManager::StartLEDCommand(LEDManager::LEM, {0,1,0,4,(uint16_t)(currentFPS-4),65,1,3});
			}
		}
	}
}

/*
 * Tell if there's a lem at the specified position.
 */
bool LemManager::IsThereABlockerAt(unsigned char worldX, unsigned char worldY, bool isWalkingInMirror)
{
	for (int16_t i = 0; i < OutLemCount; ++i)
		if (LemArray[i].IsBlockingPosition(worldX, worldY, isWalkingInMirror))
			return true;
	return false;
}

/*
 * This function should be called when the player wants to abandon the current level
 */
void LemManager::KillAllLems()
{
	// stop all the timers
	LemTimerCount = 0;
	
	// set the spawn count to the max to avoid new spawn
	SpawnLemCount = MapManager::GetAvailableLemCount();
	
	// and set all lem in BYE_BYE_BOOM state
	for (int16_t i = 0; i < OutLemCount; ++i)
		LemArray[i].SetCurrentState(Lem::StateId::BYE_BYE_BOOM);

	// launch the count down led if we actually killed any lem
	if (OutLemCount > 0)
	{
		if (HUD::IsGameSpeedUp())
			LEDManager::StartLEDCommand(LEDManager::GAME, {1,0,0,2,1,60,-2,5});
		else
			LEDManager::StartLEDCommand(LEDManager::GAME, {1,0,0,10,5,60,-2,5});
	}
}

void LemManager::NotifyInHomeLem()
{
	InLemCount++;
	LEDManager::StartLEDCommand(LEDManager::GAME, {0,0,0,1,2,80,1,3});
}

void LemManager::Draw()
{
	// draw all the Lems
	for (int16_t i = 0; i < OutLemCount; ++i)
		LemArray[i].Draw();
}
