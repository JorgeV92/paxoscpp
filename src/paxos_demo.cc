#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


namespace paxoscpp {

struct ProposalNumber {
    int64_t round = 0;
    int proposer_id = 0;

    friend bool operator<(const ProposalNumber& lhs, const ProposalNumber& rhs) {
        if (lhs.round != rhs.round) {
            return lhs.round < rhs.round;
        }
        return lhs.proposer_id < rhs.proposer_id;
    }
    friend bool operator==(const ProposalNumber& lhs, const ProposalNumber& rhs) {
        return lhs.round == rhs.round && lhs.proposer_id == rhs.proposer_id;
    }
    friend bool operator!=(const ProposalNumber& lhs, const ProposalNumber& rhs) {
        return !(lhs == rhs);
    }
};

std::ostream& operator<<(std::ostream& os, const ProposalNumber& proposal) {
    os << "(" << proposal.round << "," << proposal.proposer_id << ")";
    return os;
}

struct AcceptedValue {
    ProposalNumber proposal;
    std::string value;
};

struct PrepareRequest {
    ProposalNumber proposal;
};

struct PromiseReply {
    bool promised = false;
    ProposalNumber promised_proposal;
    std::optional<AcceptedValue> last_accepted;
};

struct AcceptRequest {
    ProposalNumber proposal;
    std::string value;
};

struct AcceptReply {
    bool accepted = false;
    ProposalNumber promised_proposal;
};

class Acceptor {
public:
    explicit Acceptor(int id) : id_(id) {   }

    PromiseReply OnePrepare(const PrepareRequest& request) {
        if (promised_proposal_ < request.proposal || promised_proposal_ == request.proposal) {
            promised_proposal_ = request.proposal;
            return PromiseReply({true, promised_proposal_, accepted_value_});
        } 
        return PromiseReply({false, promised_proposal_, accepted_value_});
    }

    AcceptReply OneAccept(const AcceptRequest& request) {
        if (promised_proposal_ < request.proposal || promised_proposal_ == request.proposal) {
            promised_proposal_ = request.proposal;
            return AcceptReply{true, promised_proposal_};
        } 
        return AcceptReply{false, promised_proposal_};
    }

    int id() const { return id_; }

    ProposalNumber promised_proposal() const { return promised_proposal_; }

    std::optional<AcceptedValue> accepted_value() const { return accepted_value_; }

private:
    int id_ = 0;
    ProposalNumber promised_proposal_{};
    std::optional<AcceptedValue> accepted_value_;
};

class Learner {
public:
    explicit Learner(int id) : id_(id) {}

    void ObserveAccepted(int acceptor_id, const ProposalNumber& proposal, const std::string& value) {
        observations_[acceptor_id] = AcceptedValue{proposal, value};
    }

    std::optional<std::string> ChosenValue(int quorum_size) const {
        std::map<std::pair<int64_t, int>, std::map<std::string, int>> counts;
        for (const auto& [acceptor_id, accepted] : observations_) {
            (void)acceptor_id;
            const auto key = std::make_pair(accepted.proposal.round,
                                            accepted.proposal.proposer_id);
            counts[key][accepted.value] += 1;
        }

        for (const auto& [proposal_key, value_counts] : counts) {
            (void)proposal_key;
            for (const auto& [value, count] : value_counts) {
                if (count >= quorum_size) {
                    return value;
                }
            }
        }
        return std::nullopt;
    }

    int id() const { return id_; }
private:
    int id_ = 0;
    std::map<int, AcceptedValue> observations_;
};

} // namespace paxoscpp

int main() {
    using namespace paxoscpp;
}
