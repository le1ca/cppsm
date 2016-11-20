#include <iostream>
#include "StateMachine.hpp"

using namespace std;
using namespace cppsm;

// enum specifying the FSM's states
enum MyState {
    INIT,
    SENT_HELLO,
    SENT_AUTH,
    READY
};

// enum specifying the FSM's alphabet / events
enum MyEvent {
    EPSILON = 0,
    TIMEOUT,
    RECV_OK
};

// Stream operator overload to pretty-print the states
ostream& operator<<(ostream& os, MyState s){
    switch(s){
        case INIT:          os << "INIT";       break;
        case SENT_HELLO:    os << "SENT_HELLO"; break;
        case SENT_AUTH:     os << "SENT_AUTH";  break;
        case READY:         os << "READY";      break;
        default: os.setstate(ios_base::failbit);
    }
    return os;
}

// Stream operator overload to pretty-print the events
ostream& operator<<(ostream& os, MyEvent s){
    switch(s){
        case EPSILON:   os << "EPSILON"; break;
        case TIMEOUT:   os << "TIMEOUT"; break;
        case RECV_OK:   os << "RECV_OK"; break;
        default: os.setstate(ios_base::failbit);
    }
    return os;
}

// Callback which sends a HELLO message
void doSendHello(){
    cout << "Sending hello..." << endl;
}

// Callback which sends an AUTH message
void doSendAuth(){
    cout << "Sending auth..." << endl;
}

int main(int argc, char **argv){

    // Start state: INIT
    auto mysm = StateMachine<MyState, MyEvent>(INIT);
    
    // Use the EPSILON event to trigger the transition from INIT -> SENT_HELLO,
    // and call the doSendHello function.
    mysm.AddTransition(INIT,        SENT_HELLO, EPSILON,    doSendHello);
    
    // When we receive the OK message after we sent a HELLO, then send the AUTH
    // and move to the SENT_AUTH state.
    mysm.AddTransition(SENT_HELLO,  SENT_AUTH,  RECV_OK,    doSendAuth );
    
    // When we receive the OK message after we sent an AUTH, move to READY state
    // without a callback.
    mysm.AddTransition(SENT_AUTH,   READY,      RECV_OK                );
    
    // When we TIMEOUT from the SENT_HELLO state, self-loop and resend HELLO.
    mysm.AddTransition(SENT_HELLO,              TIMEOUT,    doSendHello);
    
    // When we TIMEOUT from the SENT_AUTH state, resend HELLO and go back to
    // SENT_HELLO.
    mysm.AddTransition(SENT_AUTH,   SENT_HELLO, TIMEOUT,    doSendHello);
    
    // Examples of sending events:
    mysm.SendEvent(EPSILON); // send EPSILON to trigger sending HELLO
    mysm.SendEvent(RECV_OK); // receive OK for the HELLO message
    mysm.SendEvent(EPSILON); // EPSILON does nothing while in WAIT_AUTH
    mysm.SendEvent(TIMEOUT); // timeout while waiting for OK from AUTH
    mysm.SendEvent(TIMEOUT); // timeout while waiting for OK from HELLO
    mysm.SendEvent(RECV_OK); // receive OK, go to SENT_AUTH
    mysm.SendEvent(RECV_OK); // receive OK, go to READY
    
    cout << "Current state: " << mysm.GetState() << endl;
    
    return 0;
}
