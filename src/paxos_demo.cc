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
}

}

int main() {
    using namespace paxoscpp;
}
