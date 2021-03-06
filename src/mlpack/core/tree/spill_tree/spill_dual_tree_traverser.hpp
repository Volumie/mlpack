/**
 * @file spill_dual_tree_traverser.hpp
 * @author Ryan Curtin
 * @author Marcos Pividori
 *
 * Defines the SpillDualTreeTraverser for the SpillTree tree type.  This is a
 * nested class of SpillTree which traverses two trees in a depth-first
 * manner with a given set of rules which indicate the branches which can be
 * pruned and the order in which to recurse.
 * The Defeatist template parameter determines if the traversers must do
 * defeatist search on overlapping nodes.
 */
#ifndef MLPACK_CORE_TREE_SPILL_TREE_SPILL_DUAL_TREE_TRAVERSER_HPP
#define MLPACK_CORE_TREE_SPILL_TREE_SPILL_DUAL_TREE_TRAVERSER_HPP

#include <mlpack/core.hpp>

#include "spill_tree.hpp"

namespace mlpack {
namespace tree {

template<typename MetricType,
         typename StatisticType,
         typename MatType,
         template<typename HyperplaneMetricType> class HyperplaneType,
         template<typename SplitMetricType, typename SplitMatType>
             class SplitType>
template<typename RuleType, bool Defeatist>
class SpillTree<MetricType, StatisticType, MatType, HyperplaneType, SplitType>::
    SpillDualTreeTraverser
{
 public:
  /**
   * Instantiate the dual-tree traverser with the given rule set.
   */
  SpillDualTreeTraverser(RuleType& rule);

  /**
   * Traverse the two trees.  This does not reset the number of prunes.
   *
   * @param queryNode The query node to be traversed.
   * @param referenceNode The reference node to be traversed.
   * @param score The score of the current node combination.
   */
  void Traverse(SpillTree& queryNode,
                SpillTree& referenceNode);

  //! Get the number of prunes.
  size_t NumPrunes() const { return numPrunes; }
  //! Modify the number of prunes.
  size_t& NumPrunes() { return numPrunes; }

  //! Get the number of visited combinations.
  size_t NumVisited() const { return numVisited; }
  //! Modify the number of visited combinations.
  size_t& NumVisited() { return numVisited; }

  //! Get the number of times a node combination was scored.
  size_t NumScores() const { return numScores; }
  //! Modify the number of times a node combination was scored.
  size_t& NumScores() { return numScores; }

  //! Get the number of times a base case was calculated.
  size_t NumBaseCases() const { return numBaseCases; }
  //! Modify the number of times a base case was calculated.
  size_t& NumBaseCases() { return numBaseCases; }

 private:
  //! Reference to the rules with which the trees will be traversed.
  RuleType& rule;

  //! The number of prunes.
  size_t numPrunes;

  //! The number of node combinations that have been visited during traversal.
  size_t numVisited;

  //! The number of times a node combination was scored.
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
#include "spill_dual_tree_traverser_impl.hpp"

#endif // MLPACK_CORE_TREE_SPILL_TREE_SPILL_DUAL_TREE_TRAVERSER_HPP

