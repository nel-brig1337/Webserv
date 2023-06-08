#include "MainClient.hpp"
// Getters
const map<string, string> &MainClient::get_request() const { return request_parser->get_request(); }

const string &MainClient::get_request(string key) { return request_parser->get_request(key); }

const bool &MainClient::get_send_receive_status() const { return send_receive_status; }

const int &MainClient::get_phase() const { return phase; }

const string &MainClient::get_body_file() const { return body_file; }

const int &MainClient::get_client_socket() const { return (client_socket); }

const int &MainClient::get_location() const { return (location); }

ConfigServerParser *MainClient::get_config_server() const { return (config_server_parser); }

// Setters
void MainClient::set_send_receive_status(bool send_receive_status) {
	this->send_receive_status = send_receive_status;
}

void MainClient::set_location(int location) { this->location = location; }

// Constructors and destructor
MainClient::MainClient() { std::memset(buffer, 0, MAXLINE + 1); }

MainClient::MainClient(int client_socket, ConfigServerParser *config_server_parser)
	: config_server_parser(config_server_parser), request_parser(new RequestParser()),
	  send_receive_status(true), msg_status(Accurate::OK200().what()), client_socket(client_socket),
	  status(200), phase(READ_PHASE), head_status(false), body_status(false) {
	std::memset(buffer, 0, MAXLINE + 1);
}

MainClient::~MainClient() { delete request_parser; }

// Methods
void MainClient::start(string task) {
	if (task == "read" || task == "write")
		this->start_handle(task);
	else
		throw std::runtime_error("Unknown task");
}

void MainClient::start_handle(string task) {
	try {
		if (task == "read") {
			this->handle_read();
			this->phase = WRITE_PHASE;
		}

		else if (task == "write")
			this->handle_write();

	} catch (const std::exception &e) {

		if (string(e.what()).find("can't open file") != string::npos)
			throw std::runtime_error(string(e.what()));

		print_error(string(e.what()));

		if (string(e.what()) == "Still running")
			return;

		print_short_line("catch something");
		this->msg_status = e.what();
		set_header_for_errors_and_redirection();

		this->send_receive_status = false;
	}

	send(client_socket, this->header.c_str(), this->header.size(), 0);

	if (task == "write")
		this->send_receive_status = false;
}

void MainClient::header_reading() {
	int bytes;

	if (this->head_status)
		return;

	bytes = recv(this->client_socket, buffer, MAXLINE, 0);
	if (bytes == 0)
		return;
	if (bytes < 0)
		throw Error::BadRequest400();
	this->head.append(buffer, bytes);
	if (this->head.find("\r\n\r\n") != string::npos) {
		this->body		  = this->head.substr(this->head.find("\r\n\r\n") + 4);
		this->head		  = this->head.substr(0, this->head.find("\r\n\r\n") + 4);
		this->head_status = true;
		return;
	} else
		throw std::runtime_error("Still running");
}

string MainClient::generate_random_file_name() {
	std::stringstream ss;
	std::time_t		  now = std::time(0);

	// Seed the random number generator
	std::srand(static_cast<unsigned int>(std::time(0)));

	ss << "./tmp/body_" << std::hex << now << "_" << std::rand();
	return ss.str();
}

void MainClient::body_reading() {
	int		   n, bytes;
	static int count = 0;

	if (this->body_status)
		return;

	if (this->body_file.size() == 0) {
		this->body_file = generate_random_file_name();
		cout << "body file : " << this->body_file << endl;
	}

	// Open the file for writing
	std::ofstream outFile(this->body_file.c_str(), std::ios::app);
	if (!outFile)
		throw std::runtime_error(str_red("can't open file " + this->body_file));

	if (count == 0 && this->body.size() != 0) {
		// Write data to the file with flush
		count += this->body.size();
		outFile << this->body << std::flush;
		this->body.clear();
	}

	n = ConfigServerParser::stringToInt(this->request_parser->get_request("Content-Length"));
	if (n == 0)
		return;

	std::memset(buffer, 0, MAXLINE);
	bytes = recv(this->client_socket, buffer, MAXLINE, 0);
	if (bytes < 0)
		throw Error::BadRequest400();

	// Write data to the file
	outFile << buffer;
	count += bytes;

	// Close the file
	outFile.close();

	if (count == n || bytes == 0) {
		this->body_status = true;
		count			  = 0;
		return;
	} else {
		throw std::runtime_error("Still running");
	}
}

int MainClient::find_chunk_size0() {
	int				  i;
	std::stringstream ss;

	ss << this->body.substr(0, this->body.find("\r\n"));
	ss >> std::hex >> i;

	return i;
}

int MainClient::find_chunk_size1() {
	int				  bytes;
	std::stringstream ss;

	std::memset(buffer, 0, MAXLINE);

	for (int i = 0; i < 100; i++) {

		bytes = recv(this->client_socket, buffer, 1, 0);
		this->body.append(buffer, bytes);

		if (this->body[i] == '\r') {
			bytes = recv(this->client_socket, buffer, 1, 0);
			ss << this->body.substr(0, i);
			ss >> std::hex >> i;
			return i;
		}
	}
	return 0;
}

void MainClient::chunked_body_reading() {
	int		   n, bytes;
	static int count = 0;

	if (this->body_status)
		return;

	if (this->body_file.size() == 0) {
		this->body_file = generate_random_file_name();
		cout << "body file : " << this->body_file << endl;
	}

	// Open the file for writing
	std::ofstream outFile(this->body_file.c_str(), std::ios::app);
	if (!outFile)
		throw std::runtime_error(str_red("can't open file " + this->body_file));

	if (count == 0 && this->body.size() != 0) {
		n = find_chunk_size0();

		this->body = this->body.substr(this->body.find("\r\n") + 2);

		// Write data to the file with flush
		outFile << this->body << std::flush;

		this->body.clear();

	} else {
		n = find_chunk_size1();
		if (n == 0) {
			this->body_status = true;
			count			  = 0;
			return;
		}
	}

	char buffer0[n];
	std::memset(buffer0, 0, n);
	bytes = recv(this->client_socket, buffer0, n, 0);
	if (bytes < 0)
		throw Error::BadRequest400();

	// Write data to the file
	outFile << buffer0;
	count += bytes;

	// Close the file
	outFile.close();

	if (count == n || bytes == 0) {
		this->body_status = true;
		count			  = 0;
		return;
	} else {
		throw std::runtime_error("Still running");
	}
}

void MainClient::handle_read() {
	print_line("Client Request (read)");

	this->header_reading();
	this->request_parser->run_parse(this->head);

	if (this->request_parser->get_request("Request-Type") == "POST") {
		if (this->get_request("Content-Length").size() != 0)
			this->body_reading();
		else if (this->get_request("Transfer-Encoding") == "chunked")
			this->chunked_body_reading();
		else
			throw Error::BadRequest400();
	}

	this->location = this->check_and_change_request_uri();

	if (config_server_parser->get_config_location_parser()[this->location]->get_return().size()
		!= 0)
		throw Accurate::MovedPermanently301();
	is_method_allowed_in_location();

	// if (this->config_server_parser->get_config_location_parser()[locate]->get_autoindex() == 0)
	// 	throw Error::Forbidden403();
	// if (body.length() > this->config_server_parser->get_client_max_body_size())
	// 	throw Error::RequestEntityTooLarge413();
}

void MainClient::handle_write() {
	print_line("Server Response (write)");

	Response Response;
	if (this->request_parser->get_request("Request-Type") == "GET") {
		Response.Get(this);
		if (Response.GetContentType() == "cgi") {
			Cgi cgi(this, this->config_server_parser->get_config_location_parser());
			cgi.check_extention();
		}
	}
	if (this->request_parser->get_request("Request-Type") == "DELETE") {
		// DELETE
	}
}

void MainClient::is_method_allowed_in_location() {
	for (vector<ConfigLocationParser *>::const_iterator it
		 = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos
			|| this->get_request("Request-URI").find((*it)->get_root()) != string::npos) {
			for (size_t i = 0; i < (*it)->get_methods().size(); i++) {
				if ((*it)->get_methods(i) == this->get_request("Request-Type"))
					return;
			}
		}
	}
	throw Error::MethodNotAllowed405();
}

/*
int MainClient::get_matched_location_for_request_uri() {
	// get file name to compare with index
	int locate = 0;

	for (vector<ConfigLocationParser *>::const_iterator it
		 = config_server_parser->get_config_location_parser().begin();
		 it != config_server_parser->get_config_location_parser().end(); it++) {
		// if ((*it)->get_location().find("cgi") != string::npos) {
		// 	locate++;
		// 	continue;
		// }
		if (this->get_request("Request-URI") == (*it)->get_location()
			|| this->get_request("Request-URI") == (*it)->get_root())
			return locate;

		else if (this->get_request("Request-URI").find((*it)->get_location()) != string::npos)
			return locate;

		else if (this->get_request("Request-URI").find((*it)->get_root()) != string::npos)
			return locate;

		locate++;
	}

	// File is not a found
	throw Error::NotFound404();
}

int MainClient::check_and_change_request_uri() {
	std::string str = this->get_request("Request-URI");
	size_t		found;
	int			locate = this->get_matched_location_for_request_uri();

	found = str.find_last_of('/');
	str	  = str.substr(0, found);

	//! check condition of (locate == 0)
	if (str.size() != 0) {
		std::string new_url = this->get_request("Request-URI");
		new_url.replace(
			0, str.size(),
			this->config_server_parser->get_config_location_parser()[locate]->get_root());
		this->request_parser->set_request_uri(new_url);
		return locate;
	}

	//! check_if_uri_exist to serve it
	throw Error::NotFound404();
	return 0;
}
*/

int MainClient::check_and_change_request_uri() {
	std::string str = this->get_request("Request-URI");
	size_t		found;
	int			locate = 0;
	while (str.size() != 0) {
		locate = 0;
		for (vector<ConfigLocationParser *>::const_iterator itr
			 = config_server_parser->get_config_location_parser().begin();
			 itr != config_server_parser->get_config_location_parser().end(); itr++) {
			if ((*itr)->get_location() == str) {
				std::string new_url = this->get_request("Request-URI");
				new_url.replace(
					0, str.size(),
					this->config_server_parser->get_config_location_parser()[locate]->get_root());
				this->request_parser->set_request_uri(new_url);
				return (locate);
			}
			locate++;
		}
		found = str.find_last_of('/');
		str	  = str.substr(0, found);
	}
	//! check_if_uri_exist to serve it
	throw Error::NotFound404();
	return (-1);
}

void MainClient::set_header_for_errors_and_redirection() {
	std::stringstream ss(this->msg_status);
	ss >> this->status;
	if (this->status < 400)	 // redirection
	{
		this->header = "HTTP/1.1 ";
		this->header += this->msg_status;
		this->header += "\r\nContent-Length: 0\r\n";
		this->header += "Location: /";	// should use port and host or not ?
		this->header += this->config_server_parser->get_config_location_parser()[get_location()]
							->get_return();
		this->header += "\r\n\r\n";
		std::cout << "Header of redirection:\n" << this->header << std::endl;
	} else	// errors
	{
		Response Error;
		Error.SetError(this->msg_status);
		this->header = Error.GetHeader();
		std::cout << "Header of Error:\n" << this->header << std::endl;
		this->send_receive_status = false;
	}
}
