/**
 * @file dual_tree_traverser.hpp
 * @author Ryan Curtin
 *
 * Define the dual-tree traverser for the Octree.
 */
#ifndef MLPACK_CORE_TREE_OCTREE_DUAL_TREE_TRAVERSER_HPP
#define MLPACK_CORE_TREE_OCTREE_DUAL_TREE_TRAVERSER_HPP

#include <mlpack/core.hpp>
#include "octree.hpp"

namespace mlpack {
namespace tree {

template<typename MetricType,
         typename StatisticType,
         typename MatType>
template<typename RuleType>
class Octree<MetricType, StatisticType, MatType>::DualTreeTraverser
{
 public:
  /**
   * Instantiate the given dual-tree traverser with the given rule set.
   */
  DualTreeTraverser(RuleType& rule);

  /**
   * Traverse the two trees.  This does not reset the statistics of the
   * traversals (it just adds to them).
   */
  void Traverse(Octree& queryNode, Octree& referenceNode);

  //! Get the number of pruned nodes.
  size_t NumPrunes() const { return numPrunes; }
  //! Modify the number of pruned nodes (i.e. to reset it).
  size_t& NumPrunes() { return numPrunes; }

  //! Get the number of visited node combinations.
  size_t NumVisited() const { return numVisited; }
  //! Modify the number of visited node combinations.
  size_t& NumVistied() { return numVisited; }

  //! Get the number of times a node was scored.
  size_t NumScores() const { return numScores; }
  //! Modify the number of times a node was scored.
  size_t& NumScores() { return numScores; }

  //! Get the number of times a base case was computed.
  size_t NumBaseCases() const { return numBaseCases; }
  //! Modify the number of times a base case was computed.
  size_t& NumBaseCases() { return numBaseCases; }

 private:
  //! The rule type to use.
  RuleType& rule;

  //! The number of prunes.
  size_t numPrunes;
  //! The number of visited node combinations.
  size_t numVisited;
  //! The number of times a node was scored.
  size_t numScores;
  //! The number of times a base case was calculated.
  size_t numBaseCases;

  //! Traversal information, held in the class so that it isn't continually
  //! being reallocated.
  typename RuleType::TraversalInfoType traversalInfo;
};

} // namespace tree
} // namespace mlpack

// Include implementation.
#include "dual_tree_traverser_impl.hpp"

#endif
