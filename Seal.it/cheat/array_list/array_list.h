#pragma once
#include <vector>
#include <string>

class c_array_list {
private:
	std::vector < std::string > enabled_modules;
public:
	auto add_module( std::string module_name ) -> void;
	auto remove_module( std::string module_name ) -> void;
	auto get_enabled_modules( ) -> std::vector< std::string >;
	auto toggle_module( std::string module_name ) -> void;
};

inline c_array_list g_array_list;