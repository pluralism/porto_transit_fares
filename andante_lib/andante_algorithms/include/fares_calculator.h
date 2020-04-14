#pragma once
#include <chrono>
#include "trip.h"
#include "andante_manager.h"
#include "andante_pass.h"
#include "fare_ticket.h"
#include "steiner.h"
#include "fare_solution.h"
#include "andante_occasional.h"

class __declspec(dllexport) fares_calculator
{
public:
	explicit fares_calculator(QVector<trip>& trips);
	auto calculate_tickets() -> fare_solution;
	auto initialize_data() -> void;
	auto calculate_passes() -> void;
	int last_ticket_index = -1;
private:
	auto calculate_occasional_tickets(QVector<QVector<QVector<passing>>>& passings) -> fare_solution;
	auto group_trips_by_time_range(const QVector<trip>& trips, int break_duration) const -> QVector<QVector<trip>>;
    auto build_group_solution(const QVector<QVector<passing>> &grouped_passings,
                              const QVector<int> &parent_ticket, QVector<andante_occasional> &group_tickets) const -> fare_solution;

	QVector<trip> trips_;
	QVector<zone> zones_;
	QVector<fare> ring_time_fares_;
	QVector<fare> pass_fares_;

	QVector<QVector<QVector<passing>>> grouped_passings_;
	QHash<QString, QSet<QString>> trips_by_zone_;

	fare_solution best_solution_;
	fare_solution occasional_solution_;
};
