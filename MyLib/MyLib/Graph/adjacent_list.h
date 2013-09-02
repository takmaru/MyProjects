#pragma once
#include <vector>
#include <list>
#include <set>

namespace MyLib {
namespace Graph {

template <classname T>
class adjacent_list {
public:
	class node {
	public:
		typedef std::set<const node*> adjacents;
	public:
		explicit node(T data):m_data(data){};
		~node(){};
	public:
		void addAdjacent(const node* adjacent) {
			m_adjacents.insert(adjacent);
		}
		void removeAdjacent(const node* adjacent) {
			m_adjacents.erase(adjacent);
		}
	private:
		T m_data;
		adjacents m_adjacents;
	};

	void insert(const T data) {
		m_nodes.push_back(node(data));
	}
	void erase(const T& data) {
		m_nodes.erase(node(data));
	}
private:
	std::vector<node> m_nodes;
	std::list<T*> m_adjacentNodes;
};

template <class T, class Weighted = int >
class weighted_adjacent_list {
};

}
}
