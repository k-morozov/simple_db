#pragma once

#include "actor.h"
#include "types.h"

namespace actor {

class Runtime final {
public:
    Runtime() = default;
    ~Runtime() = default;

    ActorID register_actor(IActor* actor);
    void send();
    
private:
};

} // namespace actor