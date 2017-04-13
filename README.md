# FlyServer
just for fun and practice, still coding.

A asynchronous non-blocking server which was drived by events,and also support multi-threaded.

Just support linux.

the reason for choose multi-threaded + epoll is that the service is process many I/O events which hold CPU not so long.



timeline:
1).March 24 2017 
upload fly_event.c/.h fly_queue.c/.h, only support simple I/O event using linux's epoll. need to compile and debug!

2).April 5 2017
move all .c .h filed to the dir /Dev, developing in this dir temporarily.

3).April 13 2017
finish implod timeout event and still need more detailed test.
