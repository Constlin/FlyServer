/*****************************
test code for signal event.

author: Andrew lin
*****************************/
void signal_cb()
{
	printf("singla_cb called.\n");
}

void main()
{
	fly_event sig_event;
	fly_core *core = fly_core_init();

	if (fly_event_set(SIGINT, signal_cb, &sig_event, FLY_EVENT_SIG, NULL, core, NULL) == -1) {
        printf("fly_event_set error.\n");
        return;
    }

    fly_event_add(&sig_event);

    fly_core_cycle(core);
    return;
}