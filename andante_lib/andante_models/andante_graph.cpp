#include "include/andante_graph.h"

template <typename T>
auto min_heap<T>::insert_key(const T& element, const int dist) -> void
{
	const min_heap_node<T> new_node{ element, dist };
	elements_.append(new_node);
	elements_mapping_.insert(element, new_node);
}

template <typename T>
auto min_heap<T>::is_empty() const -> bool
{
	return elements_.isEmpty();
}

template <typename T>
auto min_heap<T>::heapify(int index) -> void
{
	const auto left = (index << 1) + 1;
	const auto right = (index << 1) + 2;
	auto smallest{ index };

	if (left < elements_.size() && elements_[left].get_dist() < elements_[smallest].get_dist())
	{
		smallest = left;
	}

	if (right < elements_.size() && elements_[right].get_dist() < elements_[smallest].get_dist())
	{
		smallest = right;
	}

	if (smallest != index)
	{
		std::iter_swap(elements_.begin() + index, elements_.begin() + smallest);
		heapify(smallest);
	}
}

template <typename T>
auto min_heap_node<T>::set_dist(const int new_dist) -> void
{
	dist_ = new_dist;
}

template <typename T>
auto min_heap<T>::extract_min() -> min_heap_node<T>
{
	const auto min = elements_.first();
	const auto last_element_key = elements_.last().get_key();
	std::swap(elements_.first(), elements_.last());
	elements_.pop_back();
	heapify(0);

	return min;
}

template <typename T>
auto min_heap<T>::decrease_key(const min_heap_node<T>& node, const int dist) -> void
{
	auto current_index = get_node_position(node);
	elements_[current_index].set_dist(dist);

	while (current_index && elements_[current_index].get_dist() < elements_[(current_index - 1) / 2].get_dist())
	{
		std::iter_swap(elements_.begin() + current_index, elements_.begin() + (current_index - 1) / 2);
		current_index = (current_index - 1) / 2;
	}
}

template <typename T>
auto min_heap<T>::is_in_min_heap(const T& node) const -> bool
{
	return elements_mapping_.contains(node);
}

template <typename T>
auto min_heap<T>::get_node_with_key(const T& node) -> min_heap_node<T>
{
	return elements_mapping_[node];
}

template <typename T>
auto min_heap<T>::get_node_position(const min_heap_node<T>& node) -> int
{
	const auto it = std::find_if(elements_.begin(), elements_.end(), [&node](const min_heap_node<T>& elem)
	{
		return elem.get_key() == node.get_key();
	});

	if (it != elements_.end())
	{
		return static_cast<int>(std::distance(elements_.begin(), it));
	}

	return -1;
}

template <typename T>
auto min_heap<T>::build_min_heap() -> void
{
	for (auto i = elements_.size() / 2 - 1; i >= 0; i--)
	{
		heapify(i);
	}
}

template <typename T>
auto min_heap<T>::get_elements() const -> QVector<min_heap_node<T>>
{
	return elements_;
}

auto andante_graph::add_zone(const zone& node) -> void
{
	const auto node_name = node.get_name();
	nodes_.insert(node_name);
	mappings_.insert(node_name, nodes_.size());
	reverse_mappings_.insert(nodes_.size(), node);
	adjacency_.insert(mappings_[node_name], QSet<int>{});
}

auto andante_graph::add_edge(const zone& s, const zone& d) -> void
{
	const auto s_name = s.get_name();
	const auto d_name = d.get_name();

	adjacency_[mappings_[s_name]].insert(mappings_[d_name]);
	adjacency_[mappings_[d_name]].insert(mappings_[s_name]);
}

auto andante_graph::run_dijkstra(const zone& src) const -> QHash<int, int>
{
	min_heap<int> heap;
	const auto nodes = adjacency_.keys();
	QHash<int, int> dist;

	std::for_each(nodes.begin(), nodes.end(), [&heap, &dist](const int& node_val)
	{
		heap.insert_key(node_val, INT_MAX);
		dist.insert(node_val, INT_MAX);
	});

	heap.decrease_key(heap.get_node_with_key(mappings_.value(src.get_name())), 0);
	dist[mappings_.value(src.get_name())] = 0;
	heap.build_min_heap();

	while (!heap.is_empty())
	{
		const auto min_node = heap.extract_min();
		const auto adjacent_nodes = adjacency_.value(min_node.get_key());

		for (const auto& adjacent_node_key : adjacent_nodes)
		{
			const auto adjacent_node = heap.get_node_with_key(adjacent_node_key);

			if (heap.is_in_min_heap(adjacent_node_key) && min_node.get_dist() != INT_MAX &&
				1 + dist[min_node.get_key()] < dist[adjacent_node_key])
			{
				dist[adjacent_node_key] = 1 + dist[min_node.get_key()];
				heap.decrease_key(adjacent_node, dist[adjacent_node_key]);
			}
		}
	}

	return dist;
}

auto andante_graph::get_zone(const int zone_id) const -> zone
{
	return reverse_mappings_.value(zone_id);
}
