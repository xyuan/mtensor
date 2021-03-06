#pragma once

#include <matazure/algorithm.hpp>
#include <matazure/lambda_tensor.hpp>
#include <matazure/tensor.hpp>

#ifdef MATAZURE_CUDA
#include <matazure/cuda/tensor.hpp>
#endif

namespace matazure {
namespace view {

template <typename _Tensor, int_t _Rank>
struct broadcast_functor {
   private:
    _Tensor ts_;
    pointi<_Rank> shape_;

   public:
    broadcast_functor(_Tensor ts, pointi<_Rank> shape) : ts_(ts), shape_(shape) {
        static_assert(_Tensor::rank <= _Rank, "invalid broadcast shape");

        int_t shape_i = _Rank - 1;
        for (int_t ts_i = _Tensor::rank - 1; ts_i >= 0; --ts_i, --shape_i) {
            // only support travil broadcast
            if (ts_.shape(ts_i) == shape_[shape_i] || shape_[shape_i] == 1) continue;

            throw std::runtime_error("");
        }
    }

    MATAZURE_GENERAL auto operator()(pointi<_Rank> idx) const -> decltype(ts_(idx)) const {
        return access_imp(idx, make_integer_sequence<int_t, _Tensor::rank>{});
    }

    //    private:
    template <int_t... _Indices>
    MATAZURE_GENERAL auto access_imp(pointi<_Rank> idx, integer_sequence<int_t, _Indices...>) const
        -> decltype(ts_[0]) const {
        return ts_(idx[_Rank - _Tensor::rank + _Indices]...);
    }
};

template <typename _Tensor, int_t _Rank>
inline auto broadcast(_Tensor ts, pointi<_Rank> shape)
    -> decltype(make_lambda(shape, broadcast_functor<_Tensor, _Rank>(ts, shape),
                            typename _Tensor::runtime_type{},
                            typename layout_getter<typename _Tensor::layout_type, _Rank>::type{})) {
    return make_lambda(shape, broadcast_functor<_Tensor, _Rank>(ts, shape),
                       typename _Tensor::runtime_type{},
                       typename layout_getter<typename _Tensor::layout_type, _Rank>::type{});
}

}  // namespace view
}  // namespace matazure
