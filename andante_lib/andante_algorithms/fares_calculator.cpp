#include "include/fares_calculator.h"

fares_calculator::fares_calculator(QVector<trip> &trips) : trips_{ trips } { }

auto fares_calculator::calculate_tickets() -> fare_solution
{
	const auto start = std::chrono::steady_clock::now();
	occasional_solution_ = calculate_occasional_tickets(grouped_passings_);
	calculate_passes();
	const auto end = std::chrono::steady_clock::now();
	qDebug("[calculate_tickets] took %lld ms to execute", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());;
	return best_solution_;
}

auto fares_calculator::calculate_passes() -> void
{
	QSet<QSet<QString>> visited_combinations{};
	auto passes_by_zones_count = QHash<int, fare>{};

	const auto all_zones_size = andante_manager::get_instance().get_zones().size();
	auto graph = QVector<QVector<int>>(all_zones_size, QVector<int>(all_zones_size, INT_MAX));
	auto visited_zones = trips_by_zone_.keys();

	std::for_each(pass_fares_.begin(), pass_fares_.end(), [&passes_by_zones_count](const auto& fare)
	{
		passes_by_zones_count.insert(fare.get_fare_rings_count(), fare);
	});

	for (const auto& zone : andante_manager::get_instance().get_zones())
	{
		for (const auto& adjacent_zone : zone.get_adjacent_zones())
		{
			auto edge_cost = 0;
			const auto zone_name = zone.get_name();
			const auto adjacent_zone_name = adjacent_zone.get_name();

			if (trips_by_zone_.contains(zone_name))
			{
				edge_cost += trips_by_zone_[zone_name].count();
			}

			if (trips_by_zone_.contains(adjacent_zone_name))
			{
				edge_cost += trips_by_zone_[adjacent_zone_name].count();
			}

			const auto zone_mapping_code = andante_manager::get_instance().get_zone_mapping_code(zone_name);
			const auto adjacent_zone_mapping_code = andante_manager::get_instance().get_zone_mapping_code(adjacent_zone_name);
			graph[zone_mapping_code][adjacent_zone_mapping_code] = 0x04CB2F - edge_cost;
		}
	}

	steiner steiner;
	steiner.set_graph(graph);

	const auto max_combination_size = pass_fares_.last().get_fare_rings_count();
	for (auto combination_size = 2; combination_size <= max_combination_size; combination_size++)
	{
		if (passes_by_zones_count[combination_size].get_fare_price() > occasional_solution_.get_price())
		{
			break;
		}

		auto z = 0;
		for (auto combination = (1 << combination_size) - 1; combination < 1 << visited_zones.size(); z = combination | (combination - 1), combination = (z + 1) | (((~z & -~z) - 1) >> (__clzsi2(combination) + 1)))
		{
			auto combination_zones = QVector<int>{};
			auto combination_copy = combination;
			auto current_index = visited_zones.size() - 1;

			while (combination_copy)
			{
				if (combination_copy & 1)
				{
					combination_zones.append(andante_manager::get_instance().get_zone_mapping_code(visited_zones[current_index]));
				}
				current_index--;
				combination_copy >>= 1;
			}

			auto steiner_result = steiner.calculate(combination_zones);

			QSet<QString> pass_zones;
			for (const auto& edge : steiner_result)
			{
				pass_zones.insert(andante_manager::get_instance().get_zone_by_mapping_code(edge.first));
				pass_zones.insert(andante_manager::get_instance().get_zone_by_mapping_code(edge.second));
			}

			if(visited_combinations.contains(pass_zones))
			{
				continue;
			}
			visited_combinations.insert(pass_zones);

			// Check if it is even worth to calculate the occasional solution
			const auto pass_zone_count = std::max(2, pass_zones.size());
			if (occasional_solution_.get_price() < passes_by_zones_count[pass_zone_count].get_fare_price())
			{
				continue;
			}

			auto grouped_passings_copy = grouped_passings_;
			andante_pass pass_ticket{};
			pass_ticket.set_fare(passes_by_zones_count[pass_zone_count]);

			for(auto j = 0; j < grouped_passings_copy.count(); j++)
			{
				auto trips_group = grouped_passings_copy[j];

				for (auto i = 0; i < trips_group.count(); i++)
				{
					auto can_remove_first_passing{ true };
					auto can_remove_last_passing{ true };

					if (pass_zones.contains(trips_group[i].first().get_zone_code()))
					{
						if (i > 0)
						{
							const auto previous_first_passing = trips_group[i - 1].first();
							if (!pass_zones.contains(previous_first_passing.get_zone_code()) && previous_first_passing.get_trip_id() == trips_group[i].first().get_trip_id())
							{
								can_remove_first_passing = false;
							}
						}

						if (i != trips_group.count() - 1)
						{
							const auto next_first_passing = trips_group[i + 1].first();
							if (!pass_zones.contains(next_first_passing.get_zone_code()) && next_first_passing.get_trip_id() == trips_group[i].first().get_trip_id())
							{
								can_remove_last_passing = false;
							}
						}

						if (!can_remove_first_passing && can_remove_last_passing)
						{
							auto passings_group = trips_group[i];
							passings_group[0].set_is_validation_passing(true);
							pass_ticket.add_passings(passings_group);

							trips_group[i].erase(trips_group[i].begin() + 1, trips_group[i].end());
						}

						if (can_remove_first_passing && !can_remove_last_passing)
						{
							pass_ticket.add_passings(trips_group[i]);

							trips_group[i].erase(trips_group[i].begin(), trips_group[i].end() - 1);
							trips_group[i].last().set_is_validation_passing(true);
						}

						if (!can_remove_first_passing && !can_remove_last_passing)
						{
							auto passings_group = trips_group[i];
							passings_group[0].set_is_validation_passing(true);
							pass_ticket.add_passings(passings_group);

							auto trips_group_copy = trips_group[i];
							trips_group.erase(trips_group.begin() + i);
							trips_group.insert(i, QVector<passing>{trips_group_copy.first()});
							trips_group.insert(i + 1, QVector<passing>{trips_group_copy.last()});
							trips_group[i + 1].first().set_is_validation_passing(true);
						}

						if (can_remove_first_passing && can_remove_last_passing)
						{
							pass_ticket.add_passings(trips_group[i]);
							trips_group.erase(trips_group.begin() + i);
							i--;
						}
					}
				}

				if (trips_group.empty())
				{
					grouped_passings_copy.erase(std::next(grouped_passings_copy.begin(), j));
				}
			}

			auto occasional_tickets = fare_solution{};
			if(!grouped_passings_copy.empty())
			{
				occasional_tickets = calculate_occasional_tickets(grouped_passings_copy);
			}

			const auto final_price = occasional_tickets.get_price() + pass_ticket.get_fare().get_fare_price();

			auto solution_tickets(occasional_tickets.get_tickets());
			solution_tickets.append(std::move(pass_ticket));

			if (final_price < occasional_solution_.get_price())
			{
				best_solution_ = fare_solution{ final_price, solution_tickets };
			} else
			{
				best_solution_ = occasional_solution_;
			}
		}
	}
}

auto fares_calculator::calculate_occasional_tickets(QVector<QVector<QVector<passing>>>& grouped_passings) -> fare_solution
{
	int price{};
	QVector<fare_ticket> tickets{};

	auto total_price = 0.f;
	for (const auto& passings_group : grouped_passings)
	{
		auto zone_mapping_code{ 0 };
		QVector<QSet<int>> rings_memoization{ passings_group.size() };

		QVector<int> parent_ticket(passings_group.size() + 1, last_ticket_index);
		QVector<andante_occasional> group_tickets(parent_ticket.size());

		QVector<int> dp(passings_group.size() + 1, INT_MAX);
		dp[0] = 0;

		auto last_validation_time = QDateTime{};

		for (auto i = 0; i < passings_group.size(); i++)
		{
			if (passings_group[i].first().is_validation_passing())
			{
				last_validation_time = passings_group[i].first().get_passing_time();
			}

			zone_mapping_code = andante_manager::get_instance().get_zone_mapping_code(passings_group[i].first().get_zone_code());
			rings_memoization[i].insert(zone_mapping_code);

			auto last_ticket = 0;
			for (auto j = 0; j <= i; j++)
			{
				rings_memoization[i - j].insert(zone_mapping_code);

				auto validation_zone = passings_group[i - j].first().get_zone_code();
				auto validation_time = passings_group[i - j].first().get_passing_time();
				if (i - j > 0)
				{
					if (!passings_group[i - j].first().is_validation_passing())
					{
						auto last_passing_previous_group = passings_group[i - j - 1].last();

						validation_zone = last_passing_previous_group.get_zone_code();
						validation_time = last_passing_previous_group.get_passing_time();
					}
				}

				const auto rings_count = andante_manager::get_instance().get_rings_count(validation_zone, rings_memoization[i - j]);
				auto fare_index = rings_count - 2;

				while (fare_index < ring_time_fares_.size())
				{
					if (validation_time.secsTo(last_validation_time) <= ring_time_fares_[fare_index].get_fare_duration() * 60 &&
						ring_time_fares_[fare_index].get_fare_rings_count() >= rings_count)
					{
						break;
					}
					fare_index++;
				}

				if (fare_index < ring_time_fares_.size())
				{
					const auto start_index = i - j;
					const auto price = std::min(dp[i + 1], ring_time_fares_[fare_index].get_fare_price() + dp[i - j]);

					if (price < dp[i + 1])
					{
						dp[i + 1] = price;
						last_ticket = fare_index;

						for (auto k = start_index; k <= i; k++)
						{
							parent_ticket[k] = start_index;
						}
					}
				}
			}
			group_tickets[i] = andante_occasional(ring_time_fares_[last_ticket], passings_group[0].first());
		}
		total_price += dp[passings_group.size()];

		const auto ocas_solution = build_group_solution(passings_group, parent_ticket, group_tickets);
		tickets.append(ocas_solution.get_tickets());
		price += ocas_solution.get_price();
	}

	return fare_solution{ price, tickets };
}

auto fares_calculator::build_group_solution(const QVector<QVector<passing>> &grouped_passings,
	const QVector<int> &parent_ticket, QVector<andante_occasional> &group_tickets) const -> fare_solution {
	QVector<int> ticket_indexes;
	QVector<passing> ticket_passings;
	QHash<int, QVector<passing>> passings_by_parent_ticket;

	auto last_seen_ticket_index{ parent_ticket[0] };

	for (auto i = 0; i < parent_ticket.size(); i++)
	{
		if (parent_ticket[i] == last_ticket_index)
		{
			ticket_indexes.append(i - 1);
			if (!grouped_passings[parent_ticket[i - 1]].first().is_validation_passing())
			{
				group_tickets[i - 1].add_passing(grouped_passings[parent_ticket[i - 1] - 1].last());
			}
			group_tickets[i - 1].add_passings(ticket_passings);
			ticket_passings.clear();
			break;
		}

		if (parent_ticket[i] != last_seen_ticket_index)
		{
			ticket_indexes.append(i - 1);
			if (!grouped_passings[parent_ticket[i - 1]].first().is_validation_passing())
			{
				group_tickets[i - 1].add_passing(grouped_passings[parent_ticket[i - 1] - 1].last());
			}
			group_tickets[i - 1].add_passings(ticket_passings);
			ticket_passings.clear();

			last_seen_ticket_index = parent_ticket[i];
		}

		ticket_passings.append(grouped_passings[i]);
	}

	int price{};
	QVector<fare_ticket> tickets{};
	for (const auto& ticket_index : ticket_indexes)
	{
		price += group_tickets[ticket_index].get_fare().get_fare_price();
		tickets.append(group_tickets[ticket_index]);
	}

	return fare_solution{ price, tickets };
}

auto fares_calculator::group_trips_by_time_range(const QVector<trip> &trips, const int break_duration) const -> QVector<QVector<trip>>
{
	auto grouped_trips = QVector<QVector<trip>>{};
	auto current_trips_group = QVector<trip>{};

	for (const auto& trip : trips)
	{
		if (current_trips_group.isEmpty())
		{
			current_trips_group.append(trip);
		}
		else
		{
			if (current_trips_group.last().get_validation_time().secsTo(trip.get_validation_time()) > break_duration * 60)
			{
				grouped_trips.append(std::move(current_trips_group));
				current_trips_group = { trip };
			}
			else
			{
				current_trips_group.append(trip);
			}
		}
	}

	if (!current_trips_group.isEmpty())
	{
		grouped_trips.append(std::move(current_trips_group));
	}

	return grouped_trips;
}

auto fares_calculator::initialize_data() -> void
{
	std::sort(trips_.begin(), trips_.end(), [](const trip& t1, const trip& t2)
	{
		return t1.get_validation_time() < t2.get_validation_time();
	});

	ring_time_fares_.append(andante_manager::get_instance().get_filtered_fares(fare_type::ring_time));
	ring_time_fares_.append(andante_manager::get_instance().get_filtered_fares(fare_type::ring24));
	ring_time_fares_.append(andante_manager::get_instance().get_filtered_fares(fare_type::tour_time));
	std::sort(ring_time_fares_.begin(), ring_time_fares_.end());

	pass_fares_.append(andante_manager::get_instance().get_filtered_fares(fare_type::zone_group));
	std::sort(pass_fares_.begin(), pass_fares_.end());

	const auto break_duration = andante_manager::get_instance().get_filtered_fares(fare_type::tour_time).last().get_fare_duration();
	const auto grouped_trips = group_trips_by_time_range(trips_, break_duration);

	for (const auto& trips_group : grouped_trips)
	{
		auto grouped_passings = QVector<QVector<passing>>{};
		auto current_passings_group = QVector<passing>{};
		auto last_zone = QString{};

		for (const auto& trip : trips_group)
		{
			for (const auto& passing : trip.get_passings())
			{
				if (!trips_by_zone_.contains(passing.get_zone_code()))
				{
					trips_by_zone_.insert(passing.get_zone_code(), QSet<QString>{});
				}
				trips_by_zone_[passing.get_zone_code()].insert(trip.get_trip_id());

				if (last_zone.isEmpty())
				{
					last_zone = passing.get_zone_code();
					current_passings_group.append(passing);
				}
				else
				{
					if (passing.get_zone_code() == last_zone)
					{
						current_passings_group.append(passing);
					}
					else
					{
						grouped_passings.append(std::move(current_passings_group));
						last_zone = passing.get_zone_code();
						current_passings_group = { passing };
					}
				}
			}

			if (!current_passings_group.isEmpty())
			{
				grouped_passings.append(std::move(current_passings_group));
				last_zone = "";
			}
		}

		grouped_passings_.append(grouped_passings);
	}
}