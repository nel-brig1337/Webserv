#ifndef MAINCLIENT_HPP
#define MAINCLIENT_HPP

#include "WSNetworking.hpp"

class MainClient {

  private:
	const vector<ConfigServerParser *> servers;
	RequestParser					  *request_parser;
	ConfigServerParser				  *config_server_parser;
	Response						  *Res;
	Cgi								  *cgi;
	bool							   send_receive_status;
	string							   msg_status;
	int								   port, client_socket, status, phase;
	int								   php_status;
	bool							   write_header, write_body, write_status;
	bool							   file_open;
	HeaderBodyReader				  *header_body_reader;
	bool							   cgi_status;
	int								   cgi_counter;
	bool							   is_cgi;
	bool							   access;
	bool							   alloc;
	std::vector<std::string>		   files_to_remove;

  private:
	std::map<std::string, std::string> content_type;
	std::map<std::string, std::string> extention;
	map<string, string>				   type_mime;
	std::streampos					   position;
	string							   header;
	int								   location;
	std::string						   redirection;
	std::string						   new_url;
	std::string						   serve_file;
	std::string						   body_file;
	int								   start_php;
	std::string						   upload_path;

  private:
	// Copy constructor and assignation operator
	MainClient();
	MainClient(const MainClient &);
	MainClient &operator=(const MainClient &);

  public:
	// Getters
	const map<string, string> &get_request() const;
	const string			  &get_request(string key);
	const bool				  &get_send_receive_status() const;
	const int				  &get_phase() const;
	const string			  &get_body_file_name() const;
	const int				  &get_client_socket() const;
	const int				  &get_location() const;
	ConfigServerParser		  *get_config_server() const;
	std::string				   get_content_type(std::string extention);
	const map<string, string> &get_mime_type() const;
	const string			  &get_mime_type(const string &type) const;
	Cgi						  *get_cgi();
	bool					   get_cgi_status();
	// Setters
	void set_send_receive_status(bool send_receive_status);
	void set_location(int location);
	void set_header(std::string header);
	void reset_body_file_name(std::string new_name);
	void set_files_to_remove(std::string file);

	// Constructors and destructor
	MainClient(int client_socket, const vector<ConfigServerParser *> servers, int port, int idx_server);
	~MainClient();

	// Methods
	int			GetClientSocket();
	void		set_header_for_errors_and_redirection(const char *what);
	void		set_redirection(std::string &redirection);
	std::string get_new_url();
	void		set_extention_map();
	void		set_start_php(int start);
	std::string get_extention(std::string content);
	std::string get_upload_path();
	int			get_cgi_counter();

	std::string get_serve_file();
	std::string write_into_file(DIR *directory, std::string root);
	int			convert_to_int(const std::string &str);
	void		set_serve_file(std::string file_to_serve);
	void		send_to_socket();
	void		set_content_type_map();
	void		check_upload_path();
	void		set_write_status(bool status);
	void		set_cgi_status(bool status);
	void		start(string task);
	void		set_is_cgi(bool status);
	bool		get_access();
	void		set_access(bool status);
	std::string	generate_random_name();
	void	set_new_url(std::string new_url);
	int			get_location();

  private:
	// Methods
	void start_handle(string task);
	void handle_read();
	void handle_write();

	// Tools for matching socket with server of config file
	int	 get_right_config_server_parser_from_name_sever(string name_server);
	void match_right_server();

	void		set_header_for_errors_and_redirection();
	int			match_location();
	void		is_method_allowed_in_location();
	void		check_files_error();
	int			check_for_root_directory();
	void		throw_accurate_redirection();
	void		remove_files();
};

#endif	// MAINCLIENT_HPP