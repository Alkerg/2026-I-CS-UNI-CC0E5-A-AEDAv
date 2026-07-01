#ifndef __TRAITS_H__
#define __TRAITS_H__
#include <functional> // para less y greater
#include "../types.h"
template <typename _Node, typename _Comp>
struct BaseTrait{
    using Node       = _Node;
    using value_type = typename _Node::value_type;
    using Comp       = _Comp;
};

template <typename _Node>
struct AscendingTrait : public BaseTrait<_Node, less<typename _Node::value_type>>{
};
template <typename _Node>
struct DescendingTrait : public BaseTrait<_Node, greater<typename _Node::value_type>>{
};

template <typename _value_type, typename _Comp, typename _objIdType>
struct BTreeTrait{
    using value_type = _value_type;
    using Comp       = _Comp;
    using objIdType  = _objIdType;

};

template <typename _value_type, typename _Comp = greater<_value_type>, typename _objIdType = T1>
struct BTreeDescendingTrait: public BTreeTrait<_value_type, _Comp, _objIdType>{
};

template <typename _value_type, typename _Comp = less<_value_type>, typename _objIdType = T1>
struct BTreeAscendingTrait: public BTreeTrait<_value_type, _Comp, _objIdType>{
};

#endif // __TRAITS_H__