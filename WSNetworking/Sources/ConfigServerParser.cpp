#include "ConfigServerParser.hpp"

// Getters
const int &ConfigServerParser::get_port() const {
	return this->port;
}

const int &ConfigServerParser::get_host(int i) const {
	return this->host_v[i];
}

const string &ConfigServerParser::get_host() const {
	return this->host_s;
}

const string &ConfigServerParser::get_server_name() const {
	return this->server_name;
}

const size_t &ConfigServerParser::get_client_max_body_size() const {
	return this->client_max_body_size;
}

const map<int, string> &ConfigServerParser::get_error_page() const {
	return this->error_page;
}

const vector<ConfigLocationParser *> &ConfigServerParser::get_config_location_parser() const {
	return this->config_location_parser;
}

// Constructors and copy constructor and copy assignment operator and destructor
ConfigServerParser::ConfigServerParser(string config_server) {
	this->port_status					= false;
	this->host_status					= false;
	this->server_name_status			= false;
	this->client_max_body_size_status	= false;
	this->error_page_status				= false;
	this->config_location_parser_status = false;

	this->parse_config_server(config_server);
}

ConfigServerParser::~ConfigServerParser() {
	for (size_t i = 0; i < this->config_location_parser.size(); i++) {
		delete this->config_location_parser[i];
	}
}

// Tools
int ConfigServerParser::checkType(string str) {
	int	 i		= 0;
	int	 len	= str.length();
	bool point	= false;
	int	 num	= 0;
	int	 decLen = 0;
	int	 type	= -1; // {1: int} {2: float} {3: double}

	if (str[i] == '-' || str[i] == '+')
		i++;
	if (str[i] == '\0')
		return type;
	while (i < len) {
		if (isdigit(str[i]))
			num++;
		else if (str[i] == '.' || str[i] == ',') {
			if (point || i == len - 1)
				break;
			point = true;
			i++;
			while (i < len) {
				if (isdigit(str[i])) {
					decLen++;
				} else if (str[i] == 'f' && i == len - 1)
					break;
				else
					break;
				i++;
			}
			break;
		} else
			break;
		i++;
	}
	if (decLen > 0) {
		if (str[len - 1] == 'f' && i == len - 1)
			type = 2;
		else if (i == len)
			type = 3;
	} else if (num > 0 && i == len) {
		type = 1;
	}
	return type;
}

int ConfigServerParser::stringToInt(string str) {
	int	 i	  = 0;
	int	 len  = str.length();
	int	 sign = 1;
	long num  = 0;

	if (str[i] == '-') {
		sign = -1;
		i++;
	} else if (str[i] == '+')
		i++;
	if (str[i] == '\0')
		throw std::runtime_error(str_red("Bad Input : " + str));
	while (i < len) {
		if (isdigit(str[i]))
			num = num * 10 + (str[i] - '0');
		else
			throw std::runtime_error(str_red("Bad Input : " + str));
		i++;
	}
	return (static_cast<int>(num * sign));
}

vector<int> ConfigServerParser::split_ip_address(const string &str) {
	vector<int>		  vect_nbr;
	std::stringstream ss_ip_address(str);
	string			  str_nbr;

	while (std::getline(ss_ip_address, str_nbr, '.')) {
		if (checkType(str_nbr) == 1 && str_nbr.length() <= 3)
			try {
				vect_nbr.push_back(stringToInt(str_nbr));
			} catch (...) {
				throw std::runtime_error(str_red("Host Bad Input : " + str));
			}
		else
			throw std::runtime_error(str_red("Host Bad Input : " + str));
	}
	if (vect_nbr.size() != 4)
		throw std::runtime_error(str_red("Host Bad Input : " + str));

	return vect_nbr;
}

vector<int> ConfigServerParser::stringToHost(string host) {
	vector<int> ip_address;

	ip_address = split_ip_address(host);
	if ((ip_address[0] < 1 || 255 < ip_address[0]) || (ip_address[1] < 0 || 255 < ip_address[1]) || (ip_address[2] < 0 || 255 < ip_address[2]) || (ip_address[3] < 0 || 255 < ip_address[3]))
		throw std::runtime_error(str_red("Host Bad Input : " + host));

	return ip_address;
}

// Setters
void ConfigServerParser::set_port(string port) {
	if (this->port_status == true || port.empty() == true || checkType(port) != 1) {
		throw std::runtime_error(str_red("Port Error : " + port));
	}
	this->port		  = this->stringToInt(port);
	this->port_status = true;
}

// parse host on format "127.0.0.1"
void ConfigServerParser::set_host(string host) {
	if (this->host_status == true || host.empty() == true) {
		throw std::runtime_error(str_red("Host Error : " + host));
	}

	this->host_v	  = stringToHost(host);
	this->host_s	  = host;
	this->host_status = true;
}

void ConfigServerParser::set_server_name(string server_name) {
	if (this->server_name_status == true || server_name.empty() == true) {
		throw std::runtime_error(str_red("Server Name Error : " + server_name));
	}

	this->server_name		 = server_name;
	this->server_name_status = true;
}

void ConfigServerParser::set_client_max_body_size(string client_max_body_size) {
	if (this->client_max_body_size_status == true || client_max_body_size.empty() == true || checkType(client_max_body_size) != 1) {
		throw std::runtime_error(str_red("Client Max Body Size Error : " + client_max_body_size));
	}

	this->client_max_body_size		  = stringToInt(client_max_body_size);
	this->client_max_body_size_status = true;
}

void ConfigServerParser::set_error_page(string error_page) {
	int	   status_code;
	string error_page_input = error_page;
	string status_code_str;
	string error_page_path;

	if (error_page.empty() == true) {
		throw std::runtime_error(str_red("Error Page Error : " + error_page_input));
	}

	status_code_str = error_page.substr(0, error_page.find(" "));
	if (status_code_str.empty() == true || checkType(status_code_str) != 1) {
		throw std::runtime_error(str_red("Error Page Error : " + error_page_input));
	}
	error_page.erase(0, error_page.find(" ") + 1);

	if (error_page.find(" ") != string::npos && error_page[error_page.find(" ") + 1] != '\0') {
		throw std::runtime_error(str_red("Error Page Error : " + error_page_input));
	}

	status_code		= stringToInt(status_code_str);
	error_page_path = error_page.substr(0, error_page.size());
	if (status_code < 400 || 599 < status_code || error_page_path.empty() == true) {
		throw std::runtime_error(str_red("Error Page Error : " + error_page_input));
	}

	this->error_page[status_code] = error_page_path;
	this->error_page_status		  = true;
}

void ConfigServerParser::set_config_location_parser(string config_location) {
	if (config_location.empty() == true) {
		throw std::runtime_error(str_red("Config Location Error : " + config_location));
	}
	this->config_location_parser.push_back(new ConfigLocationParser(config_location));
	this->config_location_parser_status = true;
}

// Methods
int ConfigServerParser::get_start_end_location(string location, size_t pos) {
	int bracket = 0;

	while (location[pos]) {
		if (location[pos] == '}')
			bracket--;

		if (bracket == -1)
			break;

		pos++;
	}
	if (location[pos] == '\0')
		throw std::runtime_error(str_red("Missing closing bracket in location : " + location));

	return pos;
}

int ConfigServerParser::split_config_location(string &location) {
	string content;
	size_t location_size = 0;
	size_t pos			 = 0;
	string delimiter	 = "location";

	if ((pos = location.find(delimiter)) != string::npos) {
		location_size = this->get_start_end_location(location, pos);
		content		  = location.substr(0, location_size);
		this->set_config_location_parser(content);
	}
	return location_size + 1;
}

void ConfigServerParser::parse_config_server(string config_server) {
	size_t pos = 0;
	string line;

	while ((pos = config_server.find("\n")) != string::npos) {
		line = config_server.substr(0, pos - 1);

		if (line.find("listen") != string::npos)
			this->set_port(line.substr(line.find(" ") + 1));

		else if (line.find("server_name") != string::npos)
			this->set_server_name(line.substr(line.find(" ") + 1));

		else if (line.find("host") != string::npos)
			this->set_host(line.substr(line.find(" ") + 1));

		else if (line.find("client_max_body_size") != string::npos)
			this->set_client_max_body_size(line.substr(line.find(" ") + 1));

		else if (line.find("error_page") != string::npos)
			this->set_error_page(line.substr(line.find(" ") + 1));

		else if (line.find("location") != string::npos)
			pos = this->split_config_location(config_server);

		// else
		// 	throw std::runtime_error(str_red("Error : unknown directive"));

		config_server.erase(0, pos + 1);
	}
	this->check_status();
}

void ConfigServerParser::check_status() {
	if (!this->port_status)
		throw std::runtime_error(str_red("Error: port is missing"));
	if (!this->host_status)
		throw std::runtime_error(str_red("Error: host is missing"));
	if (!this->server_name_status)
		throw std::runtime_error(str_red("Error: server_name is missing"));
	if (!this->client_max_body_size_status)
		throw std::runtime_error(str_red("Error: client_max_body_size is missing"));
	if (!this->error_page_status)
		throw std::runtime_error(str_red("Error: error_page is missing"));
	if (!this->config_location_parser_status)
		throw std::runtime_error(str_red("Error: config_location_parser is missing"));
}

std::ostream &operator<<(std::ostream &out, const ConfigServerParser &config_server_parser) {
	print_line("Parsed Server");

	out << "port: " << config_server_parser.get_port() << endl;
	out << "host: " << config_server_parser.get_host() << endl;
	out << "server_name: " << config_server_parser.get_server_name() << endl;
	out << "client_max_body_size: " << config_server_parser.get_client_max_body_size() << endl;
	for (std::map<int, string>::const_iterator it = config_server_parser.get_error_page().begin(); it != config_server_parser.get_error_page().end(); ++it) {
		out << "error_page: " << it->first << " " << it->second << endl;
	}
	for (std::vector<ConfigLocationParser *>::const_iterator it = config_server_parser.get_config_location_parser().begin(); it != config_server_parser.get_config_location_parser().end(); ++it) {
		out << **it;
	}
	return out;
}