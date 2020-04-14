#pragma once
#include <QVector>
#include <utility>
#include "zone.h"

template <typename T>
class min_heap_node
{
public:
	explicit min_heap_node() = default;
	explicit min_heap_node(T key, const int dist) : key_{ std::move(key) }, dist_{ dist } {}
	auto get_key() const -> T { return key_; }
	auto get_dist() const -> int { return dist_; }
	auto set_dist(int new_dist) -> void;
private:
	T key_;
	int dist_;
};

template <typename T>
class min_heap
{
public:
	auto insert_key(const T& element, int dist) -> void;
	auto build_min_heap() -> void;
	auto get_elements() const-> QVector<min_heap_node<T>>;
	auto is_empty() const -> bool;
	auto heapify(int index) -> void;
	auto extract_min()->min_heap_node<T>;
	auto decrease_key(const min_heap_node<T>& node, int dist) -> void;
	auto is_in_min_heap(const T& node) const -> bool;
	auto get_node_with_key(const T& node) -> min_heap_node<T>;
	auto get_node_position(const min_heap_node<T>& node) -> int;
private:
	QVector<min_heap_node<T>> elements_;
	QHash<T, min_heap_node<T>> elements_mapping_;
};

class __declspec(dllexport) andante_graph
{
public:
	auto add_zone(const zone& node) -> void;
	auto add_edge(const zone& s, const zone& d) -> void;
	auto run_dijkstra(const zone& src) const -> QHash<int, int>;
	auto get_zone(int zone_id) const->zone;
private:
	QSet<QString> nodes_;
	QHash<QString, int> mappings_;
	QHash<int, zone> reverse_mappings_;
	QHash<int, QSet<int>> adjacency_;
};
