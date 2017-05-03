int fly_bind_socket(const char *addr, int port)
{
	struct addrinfo *ai = NULL;
    int open = 1;

	if (addr == NULL || port < 0) {
		return -1;
	}

    ai = fly_make_addr(addr, port);

    if (ai == NULL) {
    	return -1;
    }

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_CP);

	if (fd == -1) {
		printf("[ERROR] create socket error.\n");
		return -1;
	}

	if (fly_set_nonblocking(fd) == -1) {
		fly_close_fd(fd);
		return -1;
	}

	if (fly_set_closeonexec(fd) == -1) {
		fly_close_fd(fd);
		return -1;
	}

    //after call close(fd), the socket fd will process TIME_WAIT and then closed.
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on)) == -1) {
    	fly_close_fd(fd);
		return -1;
    }

    if (bind(fd, ai->ai_addr, (socketlen_t)ai->ai_addrlen) == -1) {
    	printf("[ERROR] bin socket error.\n");
    	fly_close_fd(fd);
		return -1;
    }

    return fd;
}

int fly_accept_socket(int fd);

int fly_create_tcp_service_with_handle(const char *addr, int port)
{
	int fd = fly_bind_socket(addr, port);

	if (fd == -1) {
		printf("[ERROR] fly_bind_socket error.\n");
		return -1;
	}

    if (listen(fd, 128) == -1) {
    	printf("[ERROR] listen socket error.\n");
    	fly_close_fd(fd);
		return -1;
    }

    //todo: fork process to process the fd.
}

struct addrinfo *fly_make_addr(const char *addr, int port)
{
    struct addrinfo hints;
    struct addrinfo *ai = NULL;
    char *strport;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    fly_int_to_char(port, strport);

    if (getaddrinfo(addr, strport, &hints, &ai) != 0) {
    	return -1;
    }

    return ai;
}

int fly_int_to_char(int n, char *c)
{

}