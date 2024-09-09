#include "array_list.h"


auto c_array_list::add_module( std::string module_name ) -> void {
	this->enabled_modules.push_back( module_name );
}

auto c_array_list::remove_module( std::string module_name ) -> void {
	this->enabled_modules.erase( std::remove( this->enabled_modules.begin( ), this->enabled_modules.end( ), module_name ), this->enabled_modules.end( ) );
}

auto c_array_list::get_enabled_modules( ) -> std::vector< std::string > {
	return this->enabled_modules;
}

auto c_array_list::toggle_module( std::string module_name ) -> void {
	bool is_enabled = std::find( this->enabled_modules.begin( ), this->enabled_modules.end( ), module_name ) != this->enabled_modules.end( );
	is_enabled ? this->remove_module( module_name ) : this->add_module( module_name );
}