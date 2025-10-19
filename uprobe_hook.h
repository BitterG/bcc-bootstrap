struct event {
    unsigned int pid;
};

struct event_open
{
      unsigned int pid;
      char pathname[128];
};
