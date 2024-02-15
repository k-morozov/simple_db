#pragma once

#include <memory>
#include <queue>
#include <vector>
#include <unordered_map>

#include <common/random/generator.h>
#include <tx/actor.h>
#include <tx/msg/message.h>
#include <tx/msg/sent_message.h>
#include <tx/types.h>
#include <tx/clock/clock.h>

namespace sdb::tx {

class Runtime;
using RuntimePtr = std::shared_ptr<Runtime>;

class Runtime final {
public:
    Runtime() = default;
    ~Runtime() = default;

    void register_actor(IActor* actor);
    void send(msg::Message msg);

	void run(int ticks = 10000);
    
private:
	Timestamp timestamp_{0};
	Clock clock_;

	GeneratorRandom gen_{};

	std::vector<IActor*> actors_;
	std::unordered_map<ActorID, Messages> destination_actor_messages_;
	std::priority_queue<msg::SentMessage, std::vector<msg::SentMessage>, msg::DeliveredLast> messages_queue_;

	void clear_destination_actor_messages_();
	void fill_destination_actor_messages();
	void send_to_actors();
};

} // namespace sdb::tx