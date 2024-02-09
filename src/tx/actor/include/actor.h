#pragma once

#include "types.h"

namespace actor {

class IActor {
public:
    virtual ~IActor() = default;

    virtual int get_actor_id() const = 0;
    virtual void send_on_tick(/*time and msgs*/) = 0;
};

} // namespace actor