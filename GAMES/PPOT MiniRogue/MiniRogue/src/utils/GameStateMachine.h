#pragma once

template< typename ContextType, typename StateIdType >
class GameState;

template< typename ContextType, typename StateIdType >
class GameStateMachine {

	public:
		using Context = ContextType;
		using StateId = StateIdType;
		using State = GameState<Context, StateId>;
		
	protected:
		Context context;
		StateId savedCurrentState;
		StateId currentState;

	public:
		virtual ~GameStateMachine(void) = default;
		Context & getContext(void) { return this->context; }
		const Context & getContext(void) const { return this->context; }
		void changeState(StateId stateId) { this->currentState = stateId; }
		
};