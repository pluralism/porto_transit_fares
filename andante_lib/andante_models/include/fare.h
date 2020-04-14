#pragma once
#include <QString>
#include <utility>

enum class fare_type
{
	ring_time = 1,
	ring24 = 2,
	tour_time = 3,
	zone_group = 4,
	eletrico = 5,
	funicular = 6
};

class __declspec(dllexport) fare
{
public:
	fare() = default;
	fare(const fare_type fare_type, QString fare_name, QString fare_ref, const short fare_rings_count, const int fare_duration, const int fare_price)
		: fare_type_{ fare_type }, fare_name_{std::move(fare_name)}, fare_ref_{ std::move(fare_ref) }, fare_rings_count_{ fare_rings_count }, fare_duration_{ fare_duration }, fare_price_{fare_price} { }
	fare(const fare&) = default;
	auto operator=(const fare&)->fare& = default;
	fare(fare&& other) noexcept
	{
		std::swap(fare_type_, other.fare_type_);
		std::swap(fare_name_, other.fare_name_);
		std::swap(fare_ref_, other.fare_ref_);
		std::swap(fare_rings_count_, other.fare_rings_count_);
		std::swap(fare_duration_, other.fare_duration_);
		std::swap(fare_price_, other.fare_price_);
	}
	fare& operator=(fare&& other) noexcept
	{
		std::swap(fare_type_, other.fare_type_);
		std::swap(fare_name_, other.fare_name_);
		std::swap(fare_ref_, other.fare_ref_);
		std::swap(fare_rings_count_, other.fare_rings_count_);
		std::swap(fare_duration_, other.fare_duration_);
		std::swap(fare_price_, other.fare_price_);
		return *this;
	}
	~fare() = default;
	bool operator<(const fare& other) const noexcept { return fare_price_ < other.get_fare_price(); }
	auto get_fare_type() const -> fare_type { return fare_type_; }
	auto get_fare_name() const -> QString { return fare_name_; }
	auto get_fare_ref() const -> QString { return fare_ref_; }
	auto get_fare_rings_count() const -> short { return fare_rings_count_; }
	auto get_fare_duration() const -> int { return fare_duration_; }
	auto get_fare_price() const -> int { return fare_price_; }
private:	
	fare_type fare_type_;
	QString fare_name_;
	QString fare_ref_;
	short fare_rings_count_;
	int fare_duration_;
	int fare_price_;
};

class fare_builder
{
public:
	fare_builder& set_fare_type(const fare_type fare_type)
	{
		fare_type_ = fare_type;
		return *this;
	}

	fare_builder& set_fare_name(const QString& fare_name)
	{
		fare_name_ = fare_name;
		return *this;
	}

	fare_builder& set_fare_ref(const QString& fare_ref)
	{
		fare_ref_ = fare_ref;
		return *this;
	}

	fare_builder& set_fare_rings_count(const short fare_rings_count)
	{
		fare_rings_count_ = fare_rings_count;
		return *this;
	}

	fare_builder& set_fare_duration(const int fare_duration)
	{
		fare_duration_ = fare_duration;
		return *this;
	}

	fare_builder& set_fare_price(const int fare_price)
	{
		fare_price_ = fare_price;
		return *this;
	}

	fare build() const
	{
		return fare{ fare_type_ , fare_name_, fare_ref_, fare_rings_count_, fare_duration_, fare_price_ };
	}
private:
	fare_type fare_type_;
	QString fare_name_;
	QString fare_ref_;
	short fare_rings_count_;
	int fare_duration_;
	int fare_price_;
};