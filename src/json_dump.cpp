/*
* Copyright (c) YANDEX LLC. All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3.0 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.
*/

#include <handystats/json_dump.hpp>

#include "json/gauge_json_writer.hpp"
#include "json/counter_json_writer.hpp"
#include "json/timer_json_writer.hpp"
#include "json/attribute_json_writer.hpp"

namespace handystats { namespace json {

template<typename Allocator>
void fill(
		rapidjson::Value& dump, Allocator& allocator,
		const std::map<std::string, handystats::metrics::metric_variant>& metrics_map
	)
{
	dump.SetObject();

	for (auto metric_iter = metrics_map.cbegin(); metric_iter != metrics_map.cend(); ++metric_iter) {
		rapidjson::Value metric_value;
		switch (metric_iter->second.which()) {
			case metrics::metric_index::GAUGE:
				json::write_to_json_value(&boost::get<metrics::gauge>(metric_iter->second), &metric_value, allocator);
				break;
			case metrics::metric_index::COUNTER:
				json::write_to_json_value(&boost::get<metrics::counter>(metric_iter->second), &metric_value, allocator);
				break;
			case metrics::metric_index::TIMER:
				json::write_to_json_value(&boost::get<metrics::timer>(metric_iter->second), &metric_value, allocator);
				break;
			case metrics::metric_index::ATTRIBUTE:
				json::write_to_json_value(&boost::get<metrics::attribute>(metric_iter->second), &metric_value, allocator);
				break;
		}

		dump.AddMember(rapidjson::Value(metric_iter->first.c_str(), allocator),
		               rapidjson::Value(metric_value, allocator),
		               allocator);
	}
}

std::string to_string(const std::map<std::string, handystats::metrics::metric_variant>& metrics_map) {
	typedef rapidjson::MemoryPoolAllocator<> allocator_type;

	rapidjson::Value dump;
	allocator_type allocator;
	fill(dump, allocator, metrics_map);

	rapidjson::GenericStringBuffer<rapidjson::UTF8<>, allocator_type> buffer(&allocator);
	rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF8<>, allocator_type>> writer(buffer);
	dump.Accept(writer);

	return std::string(buffer.GetString(), buffer.GetSize());
}

}} // namespace handystats::json

std::string HANDY_JSON_DUMP() {
	return handystats::json::to_string(*HANDY_METRICS_DUMP());
}

