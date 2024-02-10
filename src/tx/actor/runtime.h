#pragma once

#include <queue>
#include <vector>
#include <unordered_map>

#include <tx/actor/actor.h>
#include <tx/actor/msg/message.h>
#include <tx/actor/msg/sent_message.h>
#include <tx/actor/types.h>

namespace sdb::actor {

class Runtime final {
public:
    Runtime() = default;
    ~Runtime() = default;

    void register_actor(IActor* actor);
    void send(Message&& msg);
    
private:
	Timestamp timestamp_{0};

	std::vector<IActor*> actors_;
	std::unordered_map<ActorID, Messages> actor_messages_;
	std::priority_queue<SentMessage, std::vector<SentMessage>, DeliveredLast> messages_queue_;

};

} // namespace sdb::actor