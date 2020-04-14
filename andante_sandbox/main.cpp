#include "converter.h"
#include "andante_manager.h"
#include "fares_calculator.h"

auto main() -> int
{
	// This is required in order to convert "trips.txt" format to a more readable "trips.json"
	converter::get_instance().save_trips_to_json(converter::get_instance().convert_trips_to_json("trips.txt"), "trips.json");

	const auto zones = converter::get_instance().load_zones_from_json("zones.json");
	const auto fares = converter::get_instance().load_fares_from_json("fares.json");

	for (auto&& zone : zones)
	{
		andante_manager::get_instance().add_zone(zone);
	}

	for (auto&& fare : fares)
	{
		andante_manager::get_instance().add_fare(fare);
	}

	andante_manager::get_instance().build_andante_graph();
	auto trips = converter::get_instance().load_trips_from_json("trips.json");

	fares_calculator calculator{ trips };
	calculator.initialize_data();
	const auto solution = calculator.calculate_tickets();
	return 0;
}
