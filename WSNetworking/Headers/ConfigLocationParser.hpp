#ifndef CONFIGLOCATIONPARSER_HPP
#define CONFIGLOCATIONPARSER_HPP

#include "WSNetworking.hpp"

class ConfigLocationParser {

  private:
	// Attributes
	string				config_location;
	string				location;
	bool				location_status;
	bool				autoindex;
	bool				autoindex_status;
	string				root;
	bool				root_status;
	vector<string>		index;
	bool				index_status;
	string				return_;
	bool				return_status;
	string				file_body;
	bool				file_body_status;
	vector<string>		methods;
	bool				methods_status;
	map<string, string> cgi_ext_path;
	bool				cgi_ext_path_status;

  public:
	// Getters
	const string			  &get_location() const;
	const bool				  &get_autoindex() const;
	const string			  &get_root() const;
	const vector<string>	  &get_index() const;
	const string			  &get_index(int i) const;
	const string			  &get_return() const;
	const string			  &get_file_body() const;
	const vector<string>	  &get_methods() const;
	const string			  &get_methods(int i) const;
	const map<string, string> &get_cgi_ext_path() const;
	const string			  &get_cgi_ext_path(string key) const;

	// Constructors and copy constructor and copy assignment operator and destructor
	ConfigLocationParser(string config_location);
	~ConfigLocationParser();

  private:
	// Tools
	int			   checkType(string str);
	int			   stringToInt(string str);
	vector<string> split_methods(const string &str);
	vector<string> stringToMethods(string host);

	// Setters
	void set_location(string location, size_t pos);
	void set_autoindex(string autoindex, size_t pos);
	void set_root(string root, size_t pos);
	void set_index(string index, size_t pos);
	void set_return(string return_, size_t pos);
	void set_file_body(string file_body, size_t pos);
	void set_methods(string methods, size_t pos);
	void set_cgi_ext_path(string cgi_ext_path, size_t pos);

	// Methods
	void parse_config_location();
	void check_status();
};

std::ostream &operator<<(std::ostream &os, const ConfigLocationParser &clp);

#endif // CONFIGLOCATIONPARSER_HPP