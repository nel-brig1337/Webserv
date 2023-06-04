#include "RequestParser.hpp"

// Getters
const string &RequestParser::get_head() const { return this->head; }

const map<string, string> &RequestParser::get_request() const { return this->request; }

const string &RequestParser::get_request(string key) { return this->request[key]; }

// Setters
void RequestParser::set_head(string &head) { this->head = head; }
// Constructors and destructor
RequestParser::RequestParser() : parse_status(false) {}

RequestParser::~RequestParser() {}

// Methods
void RequestParser::run_parse(string &head) {
	if (this->parse_status == true)
		return;
	this->parse_status = true;
	this->request.clear();
	this->head.clear();
	this->set_head(head);
	this->parse_head();
}

void RequestParser::parse_head() {
	this->is_head_valid();
	this->parse_first_line();
	this->is_first_line_valid();
	this->parse_rest_lines();
}

void RequestParser::is_head_valid() {
	cout << endl;
	if (head.length() == 0)
		cout << C_RED << "head.length() : " << head.length();
	else
		cout << C_GREEN << "head.length() : " << head.length();
	cout << C_RES << endl << endl;

	if (head.empty())
		throw Error::BadRequest400();
}

void RequestParser::is_first_line_valid() {
	string allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxy"
						   "z0123456789-._~:/?#[]@!$&'()*+,;=%";
	if (this->request.size() != 3)
		throw Error::NotImplemented501();

	if (this->request["Request-URI"].find_first_not_of(allowed_chars) != string::npos)
		throw Error::BadRequest400();

	if (this->request["Request-URI"].length() > 2048)
		throw Error::RequestURITooLong414();
}

void RequestParser::parse_first_line() {
	string line;
	string key;
	string value;
	size_t pos;

	if ((pos = this->head.find("\r")) != string::npos) {
		line = this->head.substr(0, pos + 2);
		this->head.erase(0, pos + 2);
		if (line.empty())
			throw Error::BadRequest400();

		if ((pos = line.find(" ")) != string::npos) {
			key				   = "Request-Type";
			value			   = line.substr(0, pos);
			this->request[key] = value;
			line.erase(0, pos + 1);
		}
		if ((pos = line.find(" ")) != string::npos) {
			key				   = "Request-URI";
			value			   = line.substr(0, pos);
			this->request[key] = value;
			line.erase(0, pos + 1);
		}
		if ((pos = line.find("\r")) != string::npos) {
			key				   = "Protocol-Version";
			value			   = line.substr(0, pos);
			this->request[key] = value;
		}
	}
}

void RequestParser::parse_rest_lines() {
	string line;
	string key;
	string value;
	size_t pos;

	while ((pos = this->head.find("\r")) != string::npos) {
		line = this->head.substr(0, pos);
		this->head.erase(0, pos + 2);
		if (line.empty()) {
			break;
		}
		if ((pos = line.find(": ")) != string::npos) {
			key				   = line.substr(0, pos);
			value			   = line.substr(pos + 2);
			this->request[key] = value;
		}
	}
	// parse the last line
	if (!this->head.empty()) {
		line = this->head;
		if ((pos = line.find(": ")) != string::npos) {
			key				   = line.substr(0, pos);
			value			   = line.substr(pos + 2);
			this->request[key] = value;
		}
	}
}

// Operators <<
std::ostream &operator<<(std::ostream &out, const RequestParser &requestParser) {
	out << "RequestParser {" << endl;
	out << "	request: {" << endl;
	for (map<string, string>::const_iterator it = requestParser.get_request().begin();
		 it != requestParser.get_request().end(); it++) {
		cout << "		" << it->first << " : " << it->second << endl;
	}
	out << "	}" << endl;
	out << "}" << endl;
	return out;
}

void	RequestParser::reset_request_uri(std::string &str)
{
	this->request["Request-URI"] = str;
}