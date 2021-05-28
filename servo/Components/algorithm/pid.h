#ifndef __PID_H
#define __PID_H

typedef struct pid *p_pid_t;

struct pid_param
{
  int p;
  int i;
  int d;
  int input_max_err;

  int max_out;
  int inte_limit;
};

struct pid
{
  struct pid_param param;
  
  unsigned char enable;

  unsigned short set;
  unsigned short get;

  long err;
  long last_err;

  long pout;
  long iout;
  long dout;
  long out;

  void (*f_param_init)(struct pid *pid,
                       int max_output,
                       int inte_limit,
                       int p,
                       int i,
                       int d);
  void (*f_pid_reset)(struct pid *pid, int p, int i, int d);
};

void pid_struct_init(
    struct pid *pid,
    int maxout,
    int intergral_limit,

    int kp,
    int ki,
    int kd);

int pid_calculate(struct pid *pid, int get, int set);

#endif