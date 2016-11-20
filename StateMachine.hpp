#ifndef cppsm_h
#define cppsm_h

#include <type_traits>
#include <sstream>
#include <utility>
#include <string>
#include <map>

namespace cppsm {

/** Class for runtime exceptions in StateMachine.
 */
class StateMachineException : public std::exception {

    public:
    
        StateMachineException()
        : m_what ("StateMachineException")
        {}
    
        StateMachineException(const char *what)
        : m_what (what)
        {}
        
        virtual const char *what() const throw()
        {
            return m_what.c_str();
        }
        
    private:
    
        std::string m_what;
        
};

/** Function pointer to state transition callback.
 */
typedef void (*StateTransitionCallback)();

/** Class for a StateMachine with state set S and event set (alphabet) E.
 */
template <
    typename S,
    typename E,
    typename std::enable_if<std::is_enum<S>::value>::type* = nullptr,
    typename std::enable_if<std::is_enum<E>::value>::type* = nullptr
>
class StateMachine {

    public:
    
        /** Constructor: specifies the initial state of the StateMachine.
         */
        StateMachine(S initialState)
        : m_currentState(initialState)
        {}
        
        /** Returns the current state of the StateMachine.
         */
        S GetState() const
        {
            return m_currentState;
        }
        
        /** Adds a transition to the StateMachine. Raises a StateMachineException
         *  if a transition is already defined for this (fromState, onEvent) pair.
         *
         *  Params:
         *  - fromState:    the origin state of this transition
         *  - toState:      the destination state of this transition
         *  - onEvent:      the event which triggers this transition
         *  - withCallback: an optional callback function which should be called
         *                  when handling the event
         */
        void AddTransition(S fromState, S toState, E onEvent, StateTransitionCallback withCallback = nullptr)
        {
            StateTransitionEvent  event (fromState, onEvent);
            StateTransitionAction action(toState, withCallback);
            
            if(m_transitionMap.count(event)){
                std::stringstream ss;
                ss << "Duplicate transition from state " << fromState << " on event " << onEvent;
                StateMachineException ex (ss.str().c_str());
                throw ex;
            }
            
            m_transitionMap[event] = action;
        }
        
        /** Adds a loop-transition to the StateMachine. Raises a
         *  StateMachineException if a transition is already defined for
         *  this (fromState, onEvent) pair.
         *
         *  Params:
         *  - fromState:    the origin/destination state of this transition
         *  - onEvent:      the event which triggers this transition
         *  - withCallback: an optional callback function which should be called
         *                  when handling the event
         */
        void AddTransition(S fromState, E onEvent, StateTransitionCallback withCallback = nullptr)
        {
            AddTransition(fromState, fromState, onEvent, withCallback);
        }
        
        /** Triggers an event in the StateMachine. If no transition is specified
         *  for this event when in the current state, it is silently ignored.
         */
        void SendEvent(E event)
        {
            StateTransitionEvent eventPair(m_currentState, event);
            if(m_transitionMap.count(eventPair)){
                StateTransitionAction a = m_transitionMap[eventPair];
                if(a.second != nullptr){
                    a.second();
                }
                m_currentState = a.first;
            }
        }

    private:
    
        typedef std::pair<S,  StateTransitionCallback>
        StateTransitionAction;
        
        typedef std::pair<S, E>
        StateTransitionEvent;
    
        S m_currentState;
        
        std::map<StateTransitionEvent, StateTransitionAction> m_transitionMap;

};

}

#endif
