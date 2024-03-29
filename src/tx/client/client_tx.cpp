//
// Created by focus on 2/15/24.
//

#include "client_tx.h"

#include <common/log/log.h>
#include <tx/msg/message.h>

#include <cassert>
#include <stdexcept>
#include <unordered_map>

namespace sdb::tx::client {

    std::ostream &operator<<(std::ostream &stream, const ClientTXState &state) {
        switch (state) {
            case ClientTXState::NOT_STARTED:
                stream << "NOT_STARTED";
                break;
            case ClientTXState::START_SENT:
                stream << "START_SENT";
                break;
            case ClientTXState::OPEN:
                stream << "OPEN";
                break;
            case ClientTXState::COMMIT_SENT:
                stream << "COMMIT_SENT";
                break;
            case ClientTXState::COMMITTED:
                stream << "COMMITTED";
                break;
            case ClientTXState::ROLLED_BACK_BY_SERVER:
                stream << "ROLLED_BACK_BY_SERVER";
                break;
        }
        return stream;
    }

    void progress_state(ClientTXState *state) {
        assert(state);

        switch (*state) {
            case ClientTXState::NOT_STARTED:
                LOG_DEBUG << "[ClientTx::tick]"
                          << " change state from NOT_STARTED to START_SENT";
                *state = ClientTXState::START_SENT;
                break;
            case ClientTXState::START_SENT:
                LOG_DEBUG << "[ClientTx::tick]"
                          << " change state from START_SENT to OPEN";
                *state = ClientTXState::OPEN;
                break;
            case ClientTXState::OPEN:
                LOG_DEBUG << "[ClientTx::tick]"
                          << " change state from OPEN to COMMIT_SENT";
                *state = ClientTXState::COMMIT_SENT;
                break;
            case ClientTXState::COMMIT_SENT:
                LOG_DEBUG << "[ClientTx::tick]"
                          << " change state from COMMIT_SENT to COMMITTED";
                *state = ClientTXState::COMMITTED;
                break;
            case ClientTXState::COMMITTED:
                // @TODO think
                LOG_DEBUG << "[ClientTx::tick] current state is COMMITTED already. skip.";
                break;
            case ClientTXState::ROLLED_BACK_BY_SERVER:
                LOG_DEBUG << "[ClientTx::tick]"
                          << " change state from COMMIT_SENT to ROLLED_BACK_BY_SERVER.";
                *state = ClientTXState::COMMITTED;
                break;
        }
    }

    std::ostream &operator<<(std::ostream &stream, const ClientTx &self) {
        stream << "[ClientTx]"
               << "[actor_id=" << self.actor_id_ << "]" << self.spec_
               << "[state=" << self.state_ << "]"
               << "[coordinator=" << self.coordinator_actor_id_ << "]";

        return stream;
    }

    ClientTx::ClientTx(ActorID actor_id, const TxSpec &spec,
                       const Discovery *discovery, Retrier *retrier)
            : actor_id_(actor_id),
              spec_(spec),
              discovery_(discovery),
              retrier_(retrier) {
        LOG_SELF_DEBUG << "created.";
    }

    void ClientTx::tick(const Timestamp ts, const Messages &msgs,
                        Messages *scheduled_msgs) {
        LOG_SELF_DEBUG << "[ts=" << ts << "] call tick";

        Messages external_msgs;
        std::unordered_map<ActorID, Messages> reply_messages_per_actor;

        for (const auto &msg: msgs) {
            switch (msg.type) {
                case msg::MessageType::MSG_START_ACK:
                case msg::MessageType::MSG_PUT_REPLY:
                    LOG_SELF_DEBUG << "got reply msg from server: " << msg;
                    reply_messages_per_actor[msg.source].push_back(msg);
                    break;
                default:
                    LOG_SELF_DEBUG << "got new msg from server: " << msg;
                    external_msgs.push_back(msg);
                    break;
            }
        }

        for (auto &[actor_id, participant]: participants_) {
            participant->process_incoming(ts, reply_messages_per_actor[actor_id]);
        }

        LOG_DEBUG << "[ClientTx::tick][state=" << state_ << "][tx=" << txid_
                  << "][ts=" << ts << "]";

        switch (state_) {
            case ClientTXState::NOT_STARTED: {
                assert(msgs.empty());

                // we started work with tx after earliest_start_ts in tx spec.
                if (ts >= spec_.earliest_start_ts) {
                    LOG_DEBUG << "[ClientTx::tick] ts=" << ts
                              << " is greater/equal than earliest_start_ts="
                              << spec_.earliest_start_ts
                              << ", schedule_start_msg configure coordinator.";

                    configure_coordinator(ts);
                    progress_state(&state_);
                }
                break;
            }
            case ClientTXState::START_SENT: {
                process_replies_start_sent(msgs);

                assert(participants_.at(coordinator_actor_id_));
                if (participants_[coordinator_actor_id_]->is_open()) {
                    configure_read_ts();
                    progress_state(&state_);
                }
                break;
            }
            case ClientTXState::OPEN: {
                process_replies_open(external_msgs);

                if (next_put_ < spec_.puts.size() &&
                    ts >= spec_.puts[next_put_].earliest_ts) {
                    LOG_DEBUG << "[ClientTx::tick] ts=" << ts << ", next_put=" << next_put_
                              << ", from put_spec earliest_ts="
                              << spec_.puts[next_put_].earliest_ts;

                    const auto put_spec = spec_.puts[next_put_];
                    const auto key = put_spec.key;
                    const ActorID actor_id = discovery_->get_by_key(key);

                    // @todo maybe_init_participant - for another participant
                    participants_[actor_id]->add_put(key, put_spec.value);

                    next_put_++;
                }

                const auto completed_puts = completed_requests();
                if (completed_puts == spec_.puts.size() &&
                    ts >= spec_.earliest_commit_ts) {
                    LOG_DEBUG << "[ClientTx::tick][ts=" << ts
                              << "] all puts done. From spec earliest_commit_ts="
                              << spec_.earliest_commit_ts;

                    if (spec_.action == TxSpec::Action::COMMIT) {
                        LOG_DEBUG << "[ClientTx::tick]"
                                  << "[tx " << txid_ << "] send COMMIT";
                        // create commit msgs for another participant except coordinator
                        // send msg
                        auto commit_msg =
                                msg::CreateMsgCommit(actor_id_, coordinator_actor_id_, txid_);

                        retrier_->schedule(ts, commit_msg);

                        progress_state(&state_);
                    }
                }

                break;
            }

            case ClientTXState::COMMIT_SENT:
                process_replies_commit_sent(msgs);
                break;
            case ClientTXState::COMMITTED:
                LOG_DEBUG << "[ClientTx::tick] ignore COMMITTED.";
                break;
            case ClientTXState::ROLLED_BACK_BY_SERVER:
                LOG_DEBUG << "[ClientTx::tick] ignore ROLLED_BACK_BY_SERVER.";
                break;
        }

        for (auto &[_, participant]: participants_) {
            participant->schedule_requests(ts);
        }

        retrier_->get_scheduled_msgs(ts, scheduled_msgs);

        LOG_DEBUG << "[ClientTx::tick][state=" << state_ << "][tx=" << txid_
                  << "][ts=" << ts << "] scheduled " << scheduled_msgs->size()
                  << " msgs.";
    }

    void ClientTx::configure_coordinator(const Timestamp ts) {
        coordinator_actor_id_ = discovery_->get_random();
        LOG_DEBUG << "[ClientTx::configure_coordinator] set coordinator_actor_id="
                  << coordinator_actor_id_;

        participants_[coordinator_actor_id_] = std::make_unique<TxParticipant>(
                get_actor_id(), coordinator_actor_id_, retrier_);

        participants_[coordinator_actor_id_]->schedule_start_msg(ts);

        txid_ = participants_[coordinator_actor_id_]->txid();

        LOG_DEBUG << "[ClientTx::configure_coordinator] finished for txid=" << txid_
                  << ", start msg scheduled.";
    }

    void ClientTx::configure_read_ts() {
        read_ts_ = participants_[coordinator_actor_id_]->read_ts();
        LOG_DEBUG << "[ClientTx::configure_read_ts][state=" << state_ << "]"
                  << " setup read_ts=" << read_ts_
                  << ". We will send get/put requests in the next tick.";
    }

    void ClientTx::process_replies_start_sent(const Messages &msgs) {
        // handled MSG_ROLLED_BACK_BY_SERVER
    }

    void ClientTx::process_replies_open(const Messages &msgs) {
        // handled MSG_ROLLED_BACK_BY_SERVER
    }

    void ClientTx::process_replies_commit_sent(const Messages &msgs) {
        for (const auto &msg: msgs) {
            LOG_DEBUG << "[ClientTx::process_replies_commit_sent] got " << msg;

            switch (msg.type) {
                case msg::MessageType::MSG_ROLLED_BACK_BY_SERVER:
                    state_ = ClientTXState::ROLLED_BACK_BY_SERVER;
                    conflict_txid_ = msg.payload.get<msg::MsgRolledBackByServerPayload>().conflict_txid;

                    LOG_DEBUG << "[ClientTx::tick]"
                              << " change state from COMMIT_SENT to ROLLED_BACK_BY_SERVER, conflict_txid=" << conflict_txid_;
                    break;

                case msg::MessageType::MSG_COMMIT_ACK: {
                    commit_ts_ = msg.payload.get<msg::MsgCommitAckPayload>().commit_ts;

                    progress_state(&state_);
                    break;
                }
                default:
                    throw std::logic_error("process_replies_commit_sent: broken case.");
            }
        }
    }

    size_t ClientTx::completed_requests() {
        size_t puts_completed_requests{0};
        for (auto &[actor_id, participant]: participants_) {
            puts_completed_requests += participant->completed_puts();
        }
        return puts_completed_requests;
    }

    ClientTx::ExportResult ClientTx::export_results() const {
        ClientTx::ExportResult exp;

        exp.state = state_;
        exp.txid = txid_;
        exp.conflict_txid = conflict_txid_;

        exp.read_ts = read_ts_;
        exp.commit_ts = commit_ts_;

        for (const auto &[actor_id, p]: participants_) {
            p->export_results(&exp.puts);
        }

        return exp;
    }

}  // namespace sdb::tx::client