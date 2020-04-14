#include "include/andante_manager.h"

auto andante_manager::add_fare(const fare &fare) -> void
{
	fares_.append(fare);
}

auto andante_manager::add_zone(const zone &zone) -> void
{
	zones_.append(zone);
	
	const auto mappings_size = zones_mappings_.size();
	zones_mappings_.insert(zone.get_name(), mappings_size);
	zones_mappings_reverse_.insert(mappings_size, zone.get_name());
}

auto andante_manager::get_fares() const -> QVector<fare>
{
	return fares_;
}

auto andante_manager::get_zones() const -> QVector<zone>
{
	return zones_;
}

auto andante_manager::build_andante_graph() -> void
{
	zones_distances_ = QVector<QVector<int>>(zones_.size(), QVector<int>(zones_.size()));

	for (const auto& zone : zones_)
	{
		graph_.add_zone(zone);
	}

	for (const auto& zone : zones_)
	{
		for (const auto& adjacent_zone : zone.get_adjacent_zones())
		{
			graph_.add_edge(zone, adjacent_zone);
		}
	}

	for (const auto& zone : zones_)
	{
		const auto dists = graph_.run_dijkstra(zone);
		for (const auto& graph_zone : dists.keys())
		{
			zones_distances_[zones_mappings_[zone.get_name()]][zones_mappings_[graph_.get_zone(graph_zone).get_name()]] = dists.value(graph_zone) + 1;
		}
	}
}

auto andante_manager::get_rings_count(const QString& validation_zone, const QSet<int>& zones) const -> int
{
	auto rings_count{ 2 };

	for (const auto& zone_code : zones)
	{
		rings_count = std::max(rings_count, zones_distances_[zones_mappings_[validation_zone]][zone_code]);
	}

	return rings_count;
}

auto andante_manager::get_zone_mapping_code(const QString& zone) const -> int
{
	return zones_mappings_[zone];
}

auto andante_manager::get_zone_by_mapping_code(const int code) const -> QString
{
	return zones_mappings_reverse_[code];
}

auto andante_manager::get_filtered_fares(const fare_type &type) -> QVector<fare>
{
	auto filtered_fares = QVector<fare>{};

	std::copy_if(fares_.begin(), fares_.end(), std::back_inserter(filtered_fares), [&type](const fare& fare)
	{
		return fare.get_fare_type() == type;
	});

	return filtered_fares;
}
