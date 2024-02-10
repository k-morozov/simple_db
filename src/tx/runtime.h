#pragma once

#include <queue>
#include <vector>
#include <unordered_map>

#include <tx/actor.h>
#include <tx/msg/message.h>
#include <tx/msg/sent_message.h>
#include <tx/types.h>
#include <tx/clock/clock.h>

namespace sdb::tx {

class Runtime final {
public:
    Runtime() = default;
    ~Runtime() = default;

    void register_actor(IActor* actor);
    void send(Message&& msg);

	void run(int ticks = 10000);
    
private:
	Timestamp timestamp_{0};
	Clock clock_;

	std::vector<IActor*> actors_;
	std::unordered_map<ActorID, Messages> actor_messages_;
	std::priority_queue<SentMessage, std::vector<SentMessage>, DeliveredLast> messages_queue_;

	void clear_actor_messages();
	void fill_actor_messages();
	void send_to_actors();
};

} // namespace sdb::tx