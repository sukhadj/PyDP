// Provides bindings for algorithms

#include <string>

#include "../pydp_lib/casting.hpp"  // our caster helper library
#include "pybind11/complex.h"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "differential_privacy/algorithms/algorithm.h"
#include "differential_privacy/algorithms/bounded-algorithm.h"
#include "differential_privacy/algorithms/bounded-mean.h"
#include "differential_privacy/algorithms/bounded-sum.h"
#include "differential_privacy/algorithms/count.h"
#include "differential_privacy/algorithms/numerical-mechanisms.h"
#include "differential_privacy/algorithms/util.h"

using namespace std;

namespace py = pybind11;
using namespace py::literals;

namespace dp = differential_privacy;

// helper class for Algorithm, which is a templated class
// template <typename T>
// void declareAlgorithm(py::module& m, string const& suffix) {
//   py::class_<dp::Algorithm<T>> cls(m, ("Algorithm" + suffix).c_str());
//   cls.def(py::init<double>(), "epsilon"_a);
// }

// template <typename T, class Algorithm, class Builder>
// void declareAlgorithmBuilder(py::module& m, string const& suffix) {
//   py::class_<dp::AlgorithmBuilder<T, Algorithm, Builder>> cls(
//       m, ("AlgorithmBuilder" + suffix).c_str());
//   cls.def("build", &dp::AlgorithmBuilder<T, Algorithm, Builder>::Build);
//   cls.def("set_epsilon",&dp::AlgorithmBuilder<T, Algorithm, Builder>::SetEpsilon);
// }

// // Bounded Algorithm
// template <typename T, class Algorithm, class Builder>
// void declareBoundedAlgorithmBuilder(py::module& m, string const& suffix) {
//   py::class_<dp::BoundedAlgorithmBuilder<T, Algorithm, Builder>> cls(
//       m, ("BoundedAlgorithmBuilder" + suffix).c_str());
//   cls.def("set_lower",
//           &dp::BoundedAlgorithmBuilder<T, Algorithm, Builder>::SetLower);
//   cls.def("set_upper",
//           &dp::BoundedAlgorithmBuilder<T, Algorithm, Builder>::SetUpper);
//   cls.def("clear_bounds",
//           &dp::BoundedAlgorithmBuilder<T, Algorithm, Builder>::ClearBounds);
  
//   // this is an optional function
//   // todo: SetApproxBounds produces a "std::__cxx11::string =
//   // std::__cxx11::basic_string<char>" issue on compile which is usually due to
//   // different compilers. We need to fix this, potentially with a custom cast?
//   // cls.def("set_approx_bounds", &dp::BoundedAlgorithmBuilder<T, Algorithm,
//   // Builder>::SetApproxBounds);
  
//   // cls.def("build", &dp::BoundedAlgorithmBuilder<T, Algorithm, Builder>::Build);
// }

// Bounded Sum
template <typename T, typename std::enable_if<
                          std::is_integral<T>::value ||
                          std::is_floating_point<T>::value>::type* = nullptr>
void declareBoundedSum(py::module& m, string const& suffix) {
  py::class_<dp::BoundedSum<T>> cls(m, ("BoundedSum" + suffix).c_str());
  py::class_<typename dp::BoundedSum<T>::Builder> bld(cls, "Builder");
  bld.def(py::init<>());
}

template <typename T>
dp::Output list_iterator(py::list l){
  std::vector<T> vec;
  for (auto i : l){
    vec.push_back(i.cast<T>());
  }
  return &dp::BoundedMean<T>::Result(vec.begin(), vec.end());
}

template <typename T, typename Y>
dp::base::StatusOr<std::unique_ptr<Y>> entry_d(){
  return &dp::BoundedMean<T>::Builder::Build();
}

// Bounded Mean
template <typename T, typename std::enable_if<
                          std::is_integral<T>::value ||
                          std::is_floating_point<T>::value>::type* = nullptr>
void declareBoundedMean(py::module& m, string const& suffix) {
  py::class_<dp::BoundedMean<T>> cls(m, ("BoundedMean" + suffix).c_str());

  py::class_<typename dp::BoundedMean<T>::Builder> bld(cls, "Builder");
  bld.def(py::init<>());
  bld.def("set_epsilon", &dp::BoundedMean<T>::Builder::SetEpsilon);
  bld.def("set_lower", &dp::BoundedMean<T>::Builder::SetLower);
  bld.def("set_upper", &dp::BoundedMean<T>::Builder::SetUpper);
  bld.def("clear_bounds", &dp::BoundedMean<T>::Builder::ClearBounds);
  bld.def("build", &entry_d<T, dp::Algorithm<T>>); // this is breaking
  cls.def("add_entry", &dp::BoundedMean<T>::AddEntry);
  cls.def("result", &list_iterator<T>); // this is breaking
}

// Count
// todo: make these generators work. refer to the statusor implementation for
// inspiration
// template<typename T>
// void declareCount(py::module & m, string const & suffix) {
//     py::class_<dp::Count<T>> cls(m, ("Count" + suffix).c_str());
//     cls.def(py::init<double, std::unique_ptr<dp::LaplaceMechanism>>(),
//     "epsilon"_a, "mechanism"_a);
// }

// template<typename T>
// void declareCountBuilder(py::module & m, string const & suffix) {
//     py::class_<dp::Count<T>::Builder> cls(m, ("Builder" + suffix).c_str());
//     //cls.def("Build", &dp::Count<T>::Builder::Build);
// }

void init_algorithms(py::module& m) {
  // todo: this is the implementation of the generators we need to make work.
  // declareCount<double, std::unique_ptr<dp::LaplaceMechanism>>(m, "D");
  // declareCountBuilder<double>(m, "D");

  // from util
  m.def("xor_strings", &dp::XorStrings);
  m.def("default_epsilon", &dp::DefaultEpsilon);
  m.def("get_next_power_of_two", &dp::GetNextPowerOfTwo);
  m.def("qnorm", &dp::Qnorm);

  // declareAlgorithmBuilder<double, dp::BoundedSum,

  // declareBoundedSum<int>(m, "I");
  // declareBoundedAlgorithmBuilder<int, dp::BoundedSum<int>,
  //                                typename dp::BoundedSum<int>::Builder>(m, "I");

  declareBoundedMean<double>(m, "D");
  // declareBoundedAlgorithmBuilder<int,dp::BoundedMean<int>, typename
  // dp::BoundedMean<int>::Builder>(m,"II");
}
