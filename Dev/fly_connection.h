struct fly_connection {
	//the sockef fd that this fly_connection assicuated.
    int fd;

    //the read event about this fly_connection.
    struct fly_event *read;

    //the write event about this fly_connection.
    struct fly_event *write;

    //the buffer used for read this connection's internet data.
    struct fly_buf *read_buf;

    //the buffer used for write data to this connection's internet.
    struct fly_buf *write_buf;
};