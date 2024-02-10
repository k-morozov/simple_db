#pragma once

#include "types.h"

namespace sdb::tx {

class IActor {
public:
    virtual ~IActor() = default;

    virtual ActorID get_actor_id() const = 0;
    virtual void send_on_tick(/*time*/Messages&& msgs) = 0;
};

} // namespace sdb::tx