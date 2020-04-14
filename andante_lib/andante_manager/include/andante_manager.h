#pragma once
#include "fare.h"
#include "andante_graph.h"

class __declspec(dllexport) andante_manager
{
public:
	static andante_manager& get_instance()
	{
		static andante_manager instance;
		return instance;
	}
	andante_manager(const andante_manager&) = delete;
	auto operator=(const andante_manager&) -> andante_manager& = delete;

	auto add_fare(const fare&) -> void;
	auto add_zone(const zone&) -> void;

	auto get_fares() const -> QVector<fare>;
	auto get_zones() const -> QVector<zone>;

	auto get_rings_count(const QString& validation_zone, const QSet<int>& zones) const -> int;
	auto get_zone_mapping_code(const QString& zone) const -> int;
	auto get_zone_by_mapping_code(int code) const -> QString;
	auto get_filtered_fares(const fare_type& type)->QVector<fare>;

	auto build_andante_graph() -> void;
private:
	andante_manager() = default;

	QVector<fare> fares_;
	QVector<zone> zones_;
	QHash<QString, int> zones_mappings_;
	QHash<int, QString> zones_mappings_reverse_;
	QVector<QVector<int>> zones_distances_;
	andante_graph graph_;
};