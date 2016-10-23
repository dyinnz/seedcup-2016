/*******************************************************************************
 * Author: Dyinnz.HUST.UniqueStudio
 * Email:  ml_143@sina.com
 * Github: https://github.com/dyinnz
 * Date:   2016-08-25
 ******************************************************************************/

#include <iostream>
#include <algorithm>
#include <queue>

#include "simplelogger.h"
#include "finite_automaton.h"

using std::vector;
using std::string;
using std::list;
using std::set;
using std::pair;
using std::unordered_map;
using std::unordered_set;
using std::move;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;

extern simple_logger::BaseLogger logger;

/*----------------------------------------------------------------------------*/

namespace {

using namespace regular_expression;

/**
 * @brief   a helper class, convert NFA to DFA
 */
class DFAConverter {
 private:
  friend shared_ptr<DFA> regular_expression::ConvertNFAToDFA(const NFA *nfa);

  DFAConverter(const NFA *nfa) : nfa_(nfa) {}

  const NFANode *GetNFANode(int number) const {
    return nfa_->GetNode(number);
  }

  void ConversionPreamble();

  const NumberSet &EpsilonClosure(const NFANode *u);

  NFAEdge::CharMasks GetEdgeCharMasks(const NumberSet &num_set);

  NumberSet GetAdjacentSet(const NumberSet &curr_set, char c);

  DFANode *ConstructDFADiagram();

  std::vector<DFANode *> CollectEndNodes();

  std::vector<DFANode *> CollectAllNodes();

  std::shared_ptr<DFA> Convert();

 private:
  std::unordered_map<NumberSet, DFANode *, NumberSet::Hasher> set_to_dfa_node_;
  std::vector<NumberSet> e_closures_;
  const NFA *nfa_;
};

void DFAConverter::ConversionPreamble() {
  e_closures_.resize(nfa_->size());
  // set_to_dfa_node_.reserve(nfa_->size());
}

const NumberSet &DFAConverter::EpsilonClosure(const NFANode *u) {
  NumberSet &s = e_closures_[u->number()];
  if (!s.empty()) {
    return s;
  }

  // construct epsilon closure
  s.insert(u->number());
  for (NFAEdge *edge : u->edges()) {
    if (edge->IsEpsilon()) {
      NFANode *v = edge->next_node();

      if (!s.contains(v->number())) {
        auto v_s = EpsilonClosure(v);
        s.insert(v_s);
      }
    }
  }
  return s;
}

NFAEdge::CharMasks DFAConverter::GetEdgeCharMasks(const NumberSet &num_set) {
  NFAEdge::CharMasks char_masks;
  for (int num : num_set) {
    for (NFAEdge *edge : GetNFANode(num)->edges()) {
      char_masks |= edge->char_masks();
    }
  }
  return char_masks;
}

NumberSet DFAConverter::GetAdjacentSet(const NumberSet &curr_set, char c) {
  NumberSet adjacent_set;
  for (int num : curr_set) {
    for (NFAEdge *edge : GetNFANode(num)->edges()) {
      if (edge->test(c)) {
        auto e_closure = EpsilonClosure(edge->next_node());
        adjacent_set.insert(e_closure);
      }
    }
  }
  return adjacent_set;
}

DFANode *DFAConverter::ConstructDFADiagram() {
  auto start_dfa_node = new DFANode(Node::kStart);

  NumberSet start_set = EpsilonClosure(nfa_->start());
  set_to_dfa_node_.insert({start_set, start_dfa_node});

  std::queue<NumberSet> q;
  q.push(start_set);

  while (!q.empty()) {
    NumberSet curr_set = q.front();
    DFANode *dfa_curr = set_to_dfa_node_[curr_set];

    /*
    logger.debug("current set {}", to_string(curr_set));
     */

    NFAEdge::CharMasks chars = GetEdgeCharMasks(curr_set);
    for (char c = 0; c < CHAR_MAX; ++c) {
      if (chars.test(c)) {

        NumberSet adjacent_set = GetAdjacentSet(curr_set, c);

        auto iter = set_to_dfa_node_.find(adjacent_set);
        if (set_to_dfa_node_.end() == iter) {
          iter = set_to_dfa_node_.insert(
              {adjacent_set, new DFANode(Node::kNormal)}).first;
          q.push(adjacent_set);
        }
        DFANode *dfa_adjacent = iter->second;

        /*
        logger.debug("char {}: adjacent set {}, node: {}", c,
                     to_string(adjacent_set), dfa_adjacent);
                     */

        dfa_curr->AddEdge(c, dfa_adjacent);
      }
    }

    q.pop();
  }

  return start_dfa_node;
}

vector<DFANode *> DFAConverter::CollectEndNodes() {
  // collect END nodes
  vector<DFANode *> ends;
  for (auto p : set_to_dfa_node_) {
    NumberSet num_set = p.first;
    DFANode *dfa_node = p.second;

    for (int num : num_set) {
      const NFANode *nfa_node = GetNFANode(num);
      if (nfa_node->IsEnd()) {
        if (!dfa_node->IsEnd()) {
          // first END NFANode in set
          dfa_node->AttachState(DFANode::kEnd);
          dfa_node->set_priority(nfa_node->priority());
          ends.push_back(dfa_node);

        } else {
          // set the priority with higher priority
          if (nfa_node->priority() < dfa_node->priority()) {
            dfa_node->set_priority(nfa_node->priority());
          }
        }
      } // end of nfa_node->IsEnd()
    }
  }
  return ends;
}

vector<DFANode *> DFAConverter::CollectAllNodes() {
  // collect all nodes and number them
  vector<DFANode *> nodes;
  for (auto p : set_to_dfa_node_) {
    nodes.push_back(p.second);
  }
  return nodes;
}

shared_ptr<DFA> DFAConverter::Convert() {

  ConversionPreamble();

  DFANode *start = ConstructDFADiagram();

  auto ends = CollectEndNodes();
  auto nodes = CollectAllNodes();

  return make_shared<DFA>(start, std::move(ends), std::move(nodes));
}


/*----------------------------------------------------------------------------*/

/**
 * @brief   a help class, minimize the DFA
 */
class DFAOptimizer {
 private:
  friend shared_ptr<DFA>
  regular_expression::MinimizeDFA(const shared_ptr<DFA> normal);

  DFAOptimizer(const shared_ptr<DFA> normal) : normal_(normal),
                                               num_to_set_(normal->size()) {}

  const DFANode *GetNormalNode(int number) {
    return normal_->GetNode(number);
  }

  void InitPartition();

  void BuildPartitionMap();

  std::unordered_set<char> GetSetEdgeChars(const NumberSet &s);

  bool PartSetByChar(std::list<NumberSet> &parted_curr_set,
                     const NumberSet &curr_set, char c);

  void TryPartEachSet();

  shared_ptr<DFA> ConstructFromSets();

  shared_ptr<DFA> Minimize();

 private:
  const shared_ptr<DFA> normal_{nullptr};
  std::list<NumberSet> partition_;
  std::vector<NumberSet *> num_to_set_;
};

// implement
void DFAOptimizer::InitPartition() {
  unordered_map<int, NumberSet> part_map;

  auto create_insert = [&](int priority, int number) {
    auto iter = part_map.find(priority);
    if (part_map.end() == iter) {
      iter = part_map.emplace(priority, NumberSet()).first;
    }
    iter->second.insert(number);
  };

  for (size_t i = 0; i < normal_->size(); ++i) {
    auto *node = normal_->GetNode(i);
    create_insert(node->priority(), node->number());
  }

  for (auto &p : part_map) {
    partition_.push_back(p.second);
  }
}

void DFAOptimizer::BuildPartitionMap() {
  for (NumberSet &s : partition_) {
    for (int num : s) {
      num_to_set_[num] = &s;
    }
  }
}

unordered_set<char> DFAOptimizer::GetSetEdgeChars(const NumberSet &s) {
  unordered_set<char> chars;
  for (int num : s) {
    for (auto p : GetNormalNode(num)->edges()) {
      chars.insert(p.first);
    }
  }
  return chars;
}

bool DFAOptimizer::PartSetByChar(list<NumberSet> &parted_sets,
                                 const NumberSet &curr_set, char c) {
  // logger.debug("part set: {} by {}", to_string(curr_set), c);

  unordered_map<NumberSet *, NumberSet> old_to_new;
  for (int u_num : curr_set) {
    const DFANode *u = GetNormalNode(u_num);
    const DFANode *v = u->GetNextNode(c);

    if (v) {
      old_to_new[num_to_set_[v->number()]].insert(u_num);
    } else {
      old_to_new[nullptr].insert(u_num);
    }
  }

  bool is_parted = false;
  for (auto p : old_to_new) {
    if (p.second != curr_set) {
      is_parted = true;
      PartSetByChar(parted_sets, p.second, c);
    }
  }

  if (is_parted) {
    auto iter = std::unique(parted_sets.begin(), parted_sets.end());
    parted_sets.erase(iter, parted_sets.end());

  } else {
    parted_sets.push_back(curr_set);
  }
  return is_parted;
}

void DFAOptimizer::TryPartEachSet() {
  size_t last_size = 0;
  list<NumberSet> new_partition;
  do {
    // logger.debug("new loop");

    last_size = partition_.size();
    new_partition.clear();

    BuildPartitionMap();

    for (NumberSet &curr_set : partition_) {

      // logger.debug("current set: {}", to_string(curr_set));

      auto chars = GetSetEdgeChars(curr_set);
      bool is_parted = false;

      for (char c : chars) {
        list<NumberSet> parted_sets;
        is_parted = PartSetByChar(parted_sets, curr_set, c);

        if (is_parted) {
          new_partition.splice(new_partition.begin(), parted_sets);
          break;
        }
      }

      if (!is_parted) {
        new_partition.push_back(curr_set);
      }
    }

    std::swap(new_partition, partition_);
  } while (last_size < partition_.size());

  /*
  logger.debug("end of partition");
  for (auto &s : partition_) {
    logger.debug("final parted set: {}", to_string(s));
  }
   */
}

shared_ptr<DFA> DFAOptimizer::ConstructFromSets() {
  std::vector<DFANode *> normal_to_min(normal_->size());

  DFANode *start{nullptr};
  vector<DFANode *> ends;
  vector<DFANode *> nodes;

  // collect
  for (NumberSet &s : partition_) {
    auto *min_node = new DFANode(Node::kNormal);
    nodes.push_back(min_node);

    for (int old_num : s) {
      const DFANode *normal_node = GetNormalNode(old_num);
      normal_to_min[old_num] = min_node;

      if (normal_node->IsStart()) {
        min_node->AttachState(Node::kStart);
        start = min_node;
      }

      if (normal_node->IsEnd()) {
        if (!min_node->IsEnd()) {
          // first END DFANode in set
          min_node->AttachState(Node::kEnd);
          min_node->set_priority(normal_node->priority());
          ends.push_back(min_node);
        } else {
          // set the priority with higher priority
          if (normal_node->priority() < min_node->priority()) {
            min_node->set_priority(normal_node->priority());
          }
        }
      }

    } // end of inner for
  } // end of outer for

  // add edges
  for (NumberSet &s : partition_) {
    DFANode *min_u = normal_to_min[*s.begin()];

    for (int num : s) {
      for (auto p : GetNormalNode(num)->edges()) {
        char c = p.first;
        DFANode *min_v = normal_to_min[p.second->number()];
        min_u->AddEdge(c, min_v);
      }
    }
  }

  return make_shared<DFA>(start, move(ends), move(nodes));
}

shared_ptr<DFA> DFAOptimizer::Minimize() {

  InitPartition();
  if (partition_.size() <= 1) {
    // need not minimizing
    return normal_;
  }

  TryPartEachSet();

  auto minimum = ConstructFromSets();
  return minimum;
}

} // end of anonymous namespace


/*----------------------------------------------------------------------------*/

namespace regular_expression {

/**
 * class NFAEdge
 */

NFAEdge::NFAEdge(const std::string &s) {
  for (char c : s) {
    set(c);
  }
}

string to_string(const NFAEdge &edge) {
  string s{"--"};
  for (char c = 0; c < CHAR_MAX; ++c) {
    if (edge.test(c)) s += c;
  }
  s += "--";
  return s;
}


/*----------------------------------------------------------------------------*/
/**
 * class Node
 */

void Node::AttachState(State state) {
  if ((kStart == state_ && kEnd == state)
      || (kStart == state && kEnd == state_)) {
    state_ = kStartEnd;

  } else if (kStartEnd == state_ && (kStart == state || kEnd == state)) {
    // do nothing
  } else {
    state_ = state;
  }
}

string to_string(const Node &node) {
  string s{"("};
  s += std::to_string(node.number());
  switch (node.state()) {
    case Node::kStart:s += ":start";
      break;
    case Node::kEnd:s += ":end";
      break;
    case Node::kStartEnd:s += ":start/end";
      break;
    case Node::kNormal:s += ":normal";
      break;
    default:break;
  }
  if (node.IsEnd()) {
    s += ':';
    s += std::to_string(node.priority());
  }
  s += ')';
  return s;
}


/*----------------------------------------------------------------------------*/
/**
 * class NFAComponent
 */

NFANode *NFAComponent::SetNewStart(NFANode *start) {
  assert(start->IsStart());
  NFANode *old_start = start_;
  old_start->AttachState(Node::kNormal);
  start_ = start;
  return old_start;
}

NFANode *NFAComponent::SetNewEnd(NFANode *end) {
  assert(end->IsEnd());
  NFANode *old_end = end_;
  old_end->AttachState(Node::kNormal);
  end_ = end;
  return old_end;
}

NFANode *NFAComponent::RemoveStart() {
  NFANode *old_start = start_;
  start_ = nullptr;
  return old_start;
}

NFANode *NFAComponent::RemoveEnd() {
  NFANode *old_end = end_;
  end_ = nullptr;
  return old_end;
}


/*----------------------------------------------------------------------------*/
/**
 * class NFAComponentManager
 */

NFAComponent *
NFAManager::Concatenate(NFAComponent *lhs, NFAComponent *rhs) {
  // merge the end of lhs and the start of rhs
  NFANode *rhs_start = rhs->RemoveStart();
  lhs->end()->FetchEdges(rhs_start);
  // node_manager().Destroy(rhs_start);

  lhs->SetNewEnd(rhs->RemoveEnd());

  // Destroy(rhs);

  return lhs;
}

NFAComponent *NFAManager::Union(NFAComponent *lhs, NFAComponent *rhs) {
  NFANode *rhs_start = rhs->RemoveStart();
  lhs->start()->FetchEdges(rhs_start);
  // node_manager().Destroy(rhs_start);

  auto *new_end = CreateNode(Node::kEnd);
  lhs->end()->AddEdge(CreateEdge(), new_end);
  rhs->end()->AddEdge(CreateEdge(), new_end);
  lhs->SetNewEnd(new_end);
  rhs->end()->AttachState(Node::kNormal);

  // Destroy(rhs);

  return lhs;
}

NFAComponent *NFAManager::KleenStar(NFAComponent *nfa) {
  NFANode *old_start = nfa->SetNewStart(CreateNode(Node::kStart));
  NFANode *old_end = nfa->SetNewEnd(CreateNode(Node::kEnd));

  old_end->AddEdge(CreateEdge(), old_start);

  nfa->start()->AddEdge(CreateEdge(), old_start);
  old_start->AddEdge(CreateEdge(), nfa->end());
  return nfa;
}

NFAComponent *NFAManager::Optional(NFAComponent *nfa) {
  nfa->start()->AddEdge(CreateEdge(), nfa->end());
  return nfa;
}

NFAComponent *NFAManager::LeastOne(NFAComponent *nfa) {
  NFANode *old_start = nfa->SetNewStart(CreateNode(Node::kStart));
  NFANode *old_end = nfa->SetNewEnd(CreateNode(Node::kEnd));

  nfa->start()->AddEdge(CreateEdge(), old_start);
  old_end->AddEdge(CreateEdge(), nfa->end());

  old_end->AddEdge(CreateEdge(), old_start);
  return nfa;
}

NFAComponent *NFAManager::UnionWithMultiEnd(NFAComponent *lhs,
                                            NFAComponent *rhs) {
  assert(Node::kUnsetInt != lhs->end()->priority());
  assert(Node::kUnsetInt != rhs->end()->priority());

  NFANode *rhs_start = rhs->RemoveStart();
  lhs->start()->FetchEdges(rhs_start);
  // node_manager().Destroy(rhs_start);
  // edge_manager().Destroy(rhs);

  return lhs;
}

NFA *NFAManager::BuildNFA(NFAComponent *comp) {
  return Create(comp->start());
}


/*----------------------------------------------------------------------------*/
/**
 * class NFA
 */

NFA::NFA(NFANode *start) : start_(start) {
  unordered_set<NFANode *> visits;
  CollectNodes(start, visits);
}

void NFA::CollectNodes(NFANode *u, std::unordered_set<NFANode *> &visits) {
  u->set_number(nodes_.size());
  nodes_.push_back(u);
  visits.insert(u);

  for (NFAEdge *edge : u->edges()) {
    NFANode *v = edge->next_node();
    if (visits.end() == visits.find(v)) {
      CollectNodes(v, visits);
    }
  }
}

const char *
NFA::MatchDFS(NFANode *curr, const char *beg, const char *end) const {
  logger.debug("{}(): {}", __func__, to_string(*curr));

  if (curr->IsEnd()) {
    // recurse base, arrive END state
    return beg;
  }

  for (NFAEdge *edge : curr->edges()) {
    const char *match_pos = nullptr;

    if (edge->IsEpsilon()) {
      // epsilon move
      match_pos = MatchDFS(edge->next_node(), beg, end);

    } else if (edge->test(*beg) && beg < end) {
      // match
      match_pos = MatchDFS(edge->next_node(), beg + 1, end);

    } else {
      // skip
      continue;
    }

    if (match_pos == end) {
      return match_pos;
    }
  }

  return nullptr;
}

bool NFA::Match(const char *beg, const char *end) const {
  logger.debug("{}(): {}", __func__, beg);
  return (MatchDFS(start_, beg, end) == end);
}

const char *
NFA::SearchDFS(NFANode *curr, const char *beg, const char *end) const {
  // TODO
  return nullptr;
}

const char *NFA::Search(const char *begin, const char *end) const {
  // TODO
  return nullptr;
}

// for debug
static void PrintNFARecur(const NFANode *u, vector<bool> &visit) {
  visit[u->number()] = true;

  for (NFAEdge *edge : u->edges()) {
    NFANode *v = edge->next_node();

    logger.log("{}{}{}", to_string(*u), to_string(*edge), to_string(*v));

    if (!visit[v->number()]) {
      PrintNFARecur(v, visit);
    }
  }
}

void PrintNFA(const NFA *nfa) {
  std::vector<bool> visit(nfa->size());
  PrintNFARecur(nfa->start(), visit);
}


/*----------------------------------------------------------------------------*/
/**
 * class NumberSet
 */

size_t NumberSet::Hasher::operator()(const NumberSet &num_set) const {
  size_t value = 0;
  for (int num : num_set.set()) {
    value |= num % sizeof(size_t);
  }
  return value;
}

std::string to_string(const NumberSet &num_set) {
  std::string str{"{"};
  for (int num : num_set.set()) {
    str += std::to_string(num);
  }
  str += '}';
  return str;
}

// for debug
string to_string(const set<int> &num_set) {
  string str{"{"};
  for (int num : num_set) {
    str += std::to_string(num);
  }
  str += '}';
  return str;
}

/*----------------------------------------------------------------------------*/
/**
 * class DFA
 */

void DFA::NumberNode() {
  for (size_t i = 0; i < nodes_.size(); ++i) {
    nodes_[i]->set_number(i);
  }
}

bool DFA::Match(const char *beg, const char *end) const {
  const char *s = beg;
  const DFANode *curr_node = start_;

  logger.debug("{}", to_string(*curr_node));
  while (s != end) {
    const DFANode *next_node = curr_node->GetNextNode(*s);

    if (next_node) {
      curr_node = next_node;
      s += 1;
      logger.debug("{}", to_string(*curr_node));

    } else {
      return false;
    }
  }

  return curr_node->IsEnd();
}

bool DFA::Match(const std::string &s) const {
  return Match(s.c_str(), s.c_str() + s.length());
}

const char *DFA::Search(const char *begin, const char *end) const {
  // TODO
  return nullptr;
}

size_t DFA::Search(const std::string &s) const {
  return -1;
}

static void PrintDFARecur(const DFANode *u, std::vector<bool> &visit) {
  visit[u->number()] = true;

  for (auto p : u->edges()) {
    DFANode *v = p.second;
    logger.log("{}--{}--{}", to_string(*u), p.first, to_string(*v));

    if (!visit[v->number()]) {
      PrintDFARecur(v, visit);
    }
  }
}

void PrintDFA(const DFA *dfa) {
  vector<bool> visit(dfa->size());
  PrintDFARecur(dfa->start(), visit);
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<DFA> MinimizeDFA(const shared_ptr<DFA> normal) {
  return DFAOptimizer(normal).Minimize();
}

std::shared_ptr<DFA> ConvertNFAToDFA(const NFA *nfa) {
  return DFAConverter(nfa).Convert();
}

} // end of namespace regular_expression
