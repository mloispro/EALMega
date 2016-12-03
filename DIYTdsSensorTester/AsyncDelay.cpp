#include "AsyncDelay.h"
void AsyncDelay::Start(delay_callback f) {
    if (f == NULL) {
        return;
    }
    delayStartCallback = f;
}
void AsyncDelay::Stop(delay_callback f) {
    if (f == NULL) {
        return;
    }
    delayStopCallback = f;
}


