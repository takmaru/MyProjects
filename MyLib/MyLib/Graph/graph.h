/*
　グラフ

	有向グラフ　　　　: directed_graph
	無向グラフ　　　　: graph
	重み付有向グラフ　: weighted_directed_graph
	重み付無向グラフ　: weighted_graph
*/

#pragma once

#include "adjacent_list.h"
#include "adjacent_matrix.h"

namespace MyLib {
namespace Graph {

template<class T, class Container = adjacent_list<T> >
class directed_graph {
};

template<class T, class Container = adjacent_list<T> >
class graph : public graph<T, Container> {
};

template<class T, class Container = weighted_adjacent_list<T>, class Weighted = int >
class weighted_directed_graph {
};

template<class T, class Container = weighted_adjacent_list<T>, class Weighted = int >
class weighted_graph : public weighted_directed_graph<T, Container, Weighted>{
};

}
}
