//
// Created by focus on 2/12/24.
//

#pragma once

#include <tx/actor.h>
#include <tx/runtime/runtime.h>

namespace sdb::tx {

class Server: public IActor {
public:
	~Server() override = default;
	Server(IActor actor_id, std::vector<int> key_intervals, Runtime runtime);

	ActorID get_actor_id() const override;

	void send_on_tick(Messages &&msgs) override;
private:

};

} // namespace sdb::tx

