#include "include/converter.h"

auto converter::convert_trips_to_json(const QString& file_name) -> QJsonDocument
{
	const auto trips = parse_trips_from_file(file_name);
	QJsonArray trips_json;

	for (const auto& trip : trips)
	{
		QJsonObject trip_object;
		trip_object["trip_id"] = trip.get_trip_id();
		trip_object["operator_code"] = trip.get_operator_code();
		trip_object["line_code"] = trip.get_line_code();

		QJsonArray passings_json;
		for (const auto& passing : trip.get_passings())
		{
			QJsonObject passing_object;
			passing_object["stop_code"] = passing.get_stop_code();
			passing_object["zone_code"] = passing.get_zone_code();
			passing_object["passing_time"] = passing.get_passing_time().toString("HH:mm:ss dd/MM/yyyy");

			passings_json.append(passing_object);
		}
		trip_object["passings"] = passings_json;
		trips_json.append(trip_object);
	}

	return QJsonDocument{ trips_json };
}

auto converter::convert_zones_to_json(const QString& file_name) -> QJsonDocument
{
	const auto zones = parse_zones_from_file(file_name);
	QJsonArray zones_json;

	for (const auto& zone : zones)
	{
		QJsonObject zone_object;
		zone_object["zone_name"] = zone.get_name();

		QJsonArray adjacent_zones;
		for (const auto& adjacent_zone : zone.get_adjacent_zones())
		{
			adjacent_zones.append(adjacent_zone.get_name());
		}
		zone_object["adjacent_zones"] = adjacent_zones;
		zones_json.append(zone_object);
	}

	return QJsonDocument{ zones_json };
}

auto converter::save_zones_to_json(const QJsonDocument& json_data, const QString& out_file_name) -> bool
{
	return write_json_to_file(json_data, out_file_name);
}

auto converter::load_zones_from_json(const QString& file_name) -> QVector<zone>
{
	QVector<zone> zones;
	QFile out_file{ file_name };

	if (!out_file.open(QIODevice::ReadOnly))
	{
		qWarning("[load_zones_from_json] could not open file for read");
		return QVector<zone>{};
	}

	const auto json_data = out_file.readAll();
	const auto loaded_json(QJsonDocument::fromJson(json_data));
	const auto json_array = loaded_json.array();

	for(auto&& it : json_array)
	{
		const auto it_object = it.toObject();
		if(it_object.contains("zone_name") && it_object.contains("adjacent_zones"))
		{
			if(it_object["zone_name"].isString() && it_object["adjacent_zones"].isArray())
			{
				const auto zone_name = it_object["zone_name"].toString();
				const auto adjacent_zones = it_object["adjacent_zones"].toArray();

				zone new_zone{zone_name};
				for(auto&& adjacent_zone_name : adjacent_zones)
				{
					new_zone.add_adjacent_zone(zone{adjacent_zone_name.toString()});
				}
				zones.append(new_zone);
			} else
			{
				qWarning("[load_zones_from_json] either zone_name is not a string or adjacent_zones is not an array");
				return QVector<zone>{};
			}
		} else
		{
			qWarning("[load_zones_from_json] object does not contains zone_name or adjacent_zones keys");
			return QVector<zone>{};
		}
	}

	return zones;
}

auto converter::convert_fares_to_json(const QString& file_name) -> QJsonDocument
{
	const auto fares = parse_fares_from_file(file_name);
	QJsonArray fares_json;

	for (const auto& fare : fares)
	{
		QJsonObject fare_object;
		fare_object["fare_type"] = static_cast<int>(fare.get_fare_type());
		fare_object["fare_name"] = fare.get_fare_name();
		fare_object["fare_ref"] = fare.get_fare_ref();
		fare_object["fare_zones"] = fare.get_fare_rings_count();
		fare_object["fare_duration"] = fare.get_fare_duration();
		fare_object["fare_price"] = fare.get_fare_price();

		fares_json.append(fare_object);
	}

	return QJsonDocument{ fares_json };
}

auto converter::save_fares_to_json(const QJsonDocument& json_data, const QString& out_file_name) -> bool
{
	return write_json_to_file(json_data, out_file_name);
}

auto converter::save_trips_to_json(const QJsonDocument& json_data, const QString& out_file_name) -> bool
{
	return write_json_to_file(json_data, out_file_name);
}

auto converter::is_fare_data_valid(const QJsonObject& fare_object) -> bool
{
	return fare_object.contains("fare_duration") &&
		fare_object.contains("fare_name") &&
		fare_object.contains("fare_price") &&
		fare_object.contains("fare_ref") &&
		fare_object.contains("fare_type") &&
		fare_object.contains("fare_zones") &&
		fare_object["fare_name"].isString() &&
		fare_object["fare_ref"].isString();
}

auto converter::is_trip_data_valid(const QJsonObject& trip_object) -> bool
{
	return trip_object.contains("trip_id") &&
		trip_object.contains("line_code") &&
		trip_object.contains("operator_code") &&
		trip_object.contains("passings") &&
		trip_object["trip_id"].isString() &&
		trip_object["line_code"].isString() &&
		trip_object["operator_code"].isString() &&
		trip_object["passings"].isArray();
}

auto converter::is_passing_data_valid(const QJsonObject& passing_object) -> bool
{
	return passing_object.contains("stop_code") &&
		passing_object.contains("zone_code") &&
		passing_object.contains("passing_time");
}

auto converter::load_fares_from_json(const QString& file_name) -> QVector<fare>
{
	QVector<fare> fares;
	QFile out_file{ file_name };

	if (!out_file.open(QIODevice::ReadOnly))
	{
		qWarning("[load_fares_from_json] could not open file for read");
		return QVector<fare>{};
	}

	const auto json_data = out_file.readAll();
	const auto loaded_json(QJsonDocument::fromJson(json_data));
	const auto json_array = loaded_json.array();

	auto required_keys = QStringList{ "fare_duration", "fare_name", "fare_price", "fare_ref", "fare_type", "fare_zones" };
	std::sort(required_keys.begin(), required_keys.end());

	for (auto&& it : json_array)
	{
		const auto fare_object = it.toObject();
		auto fare_keys = fare_object.keys();
		std::sort(fare_keys.begin(), fare_keys.end());

		if (fare_keys == required_keys)
		{
			if (is_fare_data_valid(fare_object))
			{
				const auto fare_duration = fare_object["fare_duration"].toInt();
				const auto fare_name = fare_object["fare_name"].toString();
				const auto fare_price = fare_object["fare_price"].toInt();
				const auto fare_ref = fare_object["fare_ref"].toString();
				const auto fare_type = static_cast<::fare_type>(fare_object["fare_type"].toInt());
				const auto fare_zones = static_cast<short>(fare_object["fare_zones"].toInt());

				fare_builder builder;
				auto new_fare = builder
					.set_fare_type(fare_type)
					.set_fare_name(fare_name)
					.set_fare_ref(fare_ref)
					.set_fare_duration(fare_duration)
					.set_fare_rings_count(fare_zones)
					.set_fare_price(fare_price).build();
				fares.append(std::move(new_fare));
			}
		}
	}

	return fares;
}

auto converter::load_trips_from_json(const QString& file_name) -> QVector<trip>
{
	QVector<trip> trips;
	QFile out_file{ file_name };

	if (!out_file.open(QIODevice::ReadOnly))
	{
		qWarning("[load_trips_from_json] could not open file for read");
		return QVector<trip>{};
	}

	const auto json_data = out_file.readAll();
	const auto loaded_json(QJsonDocument::fromJson(json_data));
	const auto json_array = loaded_json.array();

	for (auto&& it : json_array)
	{
		const auto trip_object = it.toObject();

		if (is_trip_data_valid(trip_object))
		{
			const auto trip_id = trip_object["trip_id"];
			const auto line_code = trip_object["line_code"];
			const auto operator_code = trip_object["operator_code"];
			const auto passings = trip_object["passings"];

			trip new_trip{ trip_id.toString(), operator_code.toString(), line_code.toString() };
			const auto passings_array = passings.toArray();
			for (auto&& passing_it : passings_array)
			{
				const auto passing_object = passing_it.toObject();

				if (is_passing_data_valid(passing_object))
				{
					auto passing_time = QDateTime::fromString(passing_object["passing_time"].toString(), "HH:mm:ss dd/MM/yyyy");
					if (passing_time.isValid())
					{
						passing_time.setTimeSpec(Qt::UTC);

						const auto stop_code = passing_object["stop_code"].toString();
						const auto zone_code = passing_object["zone_code"].toString();
						passing_builder builder;
						auto new_passing = builder
							.set_stop_code(stop_code)
							.set_zone_code(zone_code)
							.set_passing_time(passing_time)
							.set_is_validation_passing(new_trip.get_passings().empty())
							.build();
						new_trip.add_passing(std::move(new_passing));
					}
					else
					{
						qWarning("[load_trips_from_json] found invalid passing time: %s", qUtf8Printable(passing_object["passing_time"].toString()));
						return QVector<trip>{};
					}
				}
			}
			trips.append(std::move(new_trip));
		}
	}

	return trips;
}

auto converter::parse_trips_from_file(const QString& file_name) -> QVector<trip>
{
	QVector<trip> trips;

	QFile input(file_name);
	QFileInfo input_info(input);

	if (!input.exists())
	{
		qDebug("[convert_trips_to_json] file %s does not exists!", qUtf8Printable(input_info.absoluteFilePath()));
		return trips;
	}

	if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug("[convert_trips_to_json] file %s could not be opened!", qUtf8Printable(input_info.absoluteFilePath()));
		return trips;
	}

	auto on_trip_mode = false;
	QTextStream in(&input);
	trip current_trip;

	while (!in.atEnd())
	{
		const auto line = in.readLine();

		if (!on_trip_mode && line.contains(trip_delimiter))
		{
			qDebug("[convert_trips_to_json] found trip on line: \"%s\"", qUtf8Printable(line));
			const auto less_than_index = line.indexOf("<");
			const auto greater_than_index = line.indexOf(">");

			if (less_than_index == -1 || greater_than_index == -1)
			{
				if (less_than_index == -1)
				{
					qDebug("[convert_trips_to_json] less_than_index is -1");
				}

				if (greater_than_index == -1)
				{
					qDebug("[convert_trips_to_json] greater_than_index is -1");
				}

				on_trip_mode = false;
				continue;
			}

			const auto trip_parsed = line.midRef(less_than_index + 1, greater_than_index - less_than_index - 1).toString();
			const auto trip_data = trip_parsed.split(',');

			if (trip_data.size() == 3)
			{
				const auto operator_code = trip_data[0].trimmed();
				const auto line_code = trip_data[1].trimmed();
				const auto trip_id = trip_data[2].trimmed();

				trip new_trip{ trip_id, operator_code, line_code };
				current_trip = std::move(new_trip);

				qDebug("[convert_trips_to_json] Extracted trip information: operator %s, line_code %s, trip_id %s", qUtf8Printable(operator_code),
					qUtf8Printable(line_code), qUtf8Printable(trip_id));
				on_trip_mode = true;
			}
			else
			{
				qDebug("[convert_trips_to_json] %s contains invalid data", qUtf8Printable(trip_parsed));
				on_trip_mode = false;
			}
		}
		else if (on_trip_mode && line.contains(passings_delimiter))
		{
			qDebug("[convert_trips_to_json] found passings line: \"%s\"", qUtf8Printable(line));
			const auto open_parenthesis_char = line.indexOf('(');
			const auto close_parenthesis_char = line.indexOf(')');

			if (open_parenthesis_char == -1 || close_parenthesis_char == -1)
			{
				if (open_parenthesis_char == -1)
				{
					qDebug("[convert_trips_to_json] open_parenthesis_char is -1");
				}

				if (close_parenthesis_char == -1)
				{
					qDebug("[convert_trips_to_json] close_parenthesis_char is -1");
				}

				on_trip_mode = true;
				continue;
			}

			const auto date_parsed = line.midRef(open_parenthesis_char + 1, close_parenthesis_char - open_parenthesis_char - 1).toString();
			const auto trip_date = QDate::fromString(date_parsed, "yyyy-MM-dd");
			qDebug("[convert_trips_to_json] trip date is: %s", qUtf8Printable(trip_date.toString()));

			auto less_than_char = line.indexOf('<');
			auto greater_than_char = line.indexOf('>');

			while (less_than_char != -1 && greater_than_char != -1)
			{
				const auto passing_parsed = line.midRef(less_than_char + 1, greater_than_char - less_than_char - 1).toString();

				const auto passing_data = passing_parsed.split(' ');
				if (passing_data.size() == 3)
				{
					auto stop_code = passing_data[0];
					auto zone_code = passing_data[1];
					const auto time = passing_data[2];

					const auto passing_time = QTime::fromString(time, "HH:mm:ss");
					QDateTime passing_datetime(trip_date, passing_time);
					passing_datetime.setTimeSpec(Qt::UTC);

					passing new_passing{
						std::move(stop_code),
						std::move(zone_code),
						std::move(passing_datetime),
						current_trip.get_trip_id(),
						current_trip.get_passings().empty() };
					
					qDebug("[convert_trips_to_json] Extracted passing information: stop_code %s, zone_code %s, passing_time %s",
						qUtf8Printable(new_passing.get_stop_code()), qUtf8Printable(new_passing.get_zone_code()),
						qUtf8Printable(new_passing.get_passing_time().toString()));

					current_trip.add_passing(std::move(new_passing));
				}
				else
				{
					qDebug("[convert_trips_to_json] %s contains invalid data", qUtf8Printable(passing_parsed));
				}

				less_than_char = line.indexOf('<', greater_than_char + 1);
				greater_than_char = line.indexOf('>', greater_than_char + 1);
			}

			trips.append(std::move(current_trip));
			on_trip_mode = false;
		}
	}

	return trips;
}

auto converter::parse_fares_from_file(const QString& file_name) -> QVector<fare>
{
	QFile input(file_name);
	QFileInfo input_info(input);
	QVector<fare> fares;

	if (!input.exists())
	{
		qDebug("[parse_fares_from_file] file %s does not exists!", qUtf8Printable(input_info.absoluteFilePath()));
		return fares;
	}

	if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug("[parse_fares_from_file] file %s could not be opened!", qUtf8Printable(input_info.absoluteFilePath()));
		return fares;
	}

	const auto get_fare_duration = [](const QString& fare_string)
	{
		const auto duration = fare_string.mid(0, fare_string.size() - 1).toInt();
		const auto identifier = fare_string[fare_string.size() - 1];

		QHash<QString, int> multiplier = {
			{ "m", 1 },
			{ "h", 60 },
			{ "d", 60 * 24 },
			{ "M", 60 * 24 * 31 }
		};

		return duration * multiplier[identifier];
	};

	QTextStream in(&input);
	while (!in.atEnd())
	{
		const auto line = in.readLine();
		auto line_parts = line.splitRef(QStringLiteral(","));

		if (line_parts.size() == 6)
		{
			std::transform(line_parts.begin(), line_parts.end(), line_parts.begin(), [](const QStringRef& val)
			{
				return val.trimmed();
			});

			const auto fare_type = static_cast<::fare_type>(line_parts[0].toInt());
			const auto fare_name = line_parts[1].toString();
			const auto fare_ref = line_parts[2].toString();
			const auto fare_zones = line_parts[3].toShort();
			const auto fare_duration = get_fare_duration(line_parts[4].toString());
			const auto fare_price = static_cast<int>(line_parts[5].toFloat() * 100);

			const fare new_fare{ fare_type, fare_name, fare_ref, fare_zones, fare_duration, fare_price };
			fares.append(new_fare);
		}
	}

	return fares;
}

auto converter::parse_zones_from_file(const QString& file_name) -> QVector<zone>
{
	QFile input(file_name);
	QFileInfo input_info(input);
	QVector<zone> zones;

	if (!input.exists())
	{
		qDebug("[parse_zones_from_file] file %s does not exists!", qUtf8Printable(input_info.absoluteFilePath()));
		return zones;
	}

	if (!input.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug("[parse_zones_from_file] file %s could not be opened!", qUtf8Printable(input_info.absoluteFilePath()));
		return zones;
	}

	QTextStream in(&input);
	while (!in.atEnd())
	{
		const auto line = in.readLine();
		const auto parts = line.splitRef(QStringLiteral(","));

		if (parts.size() > 1 && parts[0].toString() != "Date" && parts[0].toString() != "Code")
		{
			zone new_zone{ parts[0].toString() };
			std::for_each(parts.begin() + 1, parts.end(), [&](const QStringRef& zone_name)
			{
				const zone adjacent_zone{ zone_name.toString() };
				new_zone.add_adjacent_zone(adjacent_zone);
			});

			zones.append(std::move(new_zone));
		}
	}

	return zones;
}

auto converter::write_json_to_file(const QJsonDocument& document, const QString& out_file_name)->bool
{
	QFile out_file{ out_file_name };
	if (!out_file.open(QIODevice::WriteOnly))
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	out_file.write(document.toJson());
	return true;
}
