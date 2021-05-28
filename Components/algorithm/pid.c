#include <stdlib.h>

#include "pid.h"

void abs_limit(long *a, int ABS_MAX)
{
  if (*a > ABS_MAX)
    *a = ABS_MAX;
  if (*a < -ABS_MAX)
    *a = -ABS_MAX;
}

static void pid_param_init(
    struct pid *pid,
    int maxout,
    int inte_limit,
    int kp,
    int ki,
    int kd)
{

  pid->param.inte_limit = inte_limit;
  pid->param.max_out = maxout;

  pid->param.p = kp;
  pid->param.i = ki;
  pid->param.d = kd;
  
  pid->enable = 0;
}

//static void pid_reset(struct pid *pid, short kp, short ki, short kd)
//{
//  pid->param.p = kp;
//  pid->param.i = ki;
//  pid->param.d = kd;
//
//  pid->pout = 0;
//  pid->iout = 0;
//  pid->dout = 0;
//  pid->out = 0;
//  
//  pid->enable = 0;
//}

int pid_calculate(struct pid *pid, int get, int set)
{
  pid->get = get;
  pid->set = set;
  pid->err = set - get;
  if ((pid->param.input_max_err != 0) && (abs(pid->err) > pid->param.input_max_err))
    return 0;

  pid->pout = pid->param.p * pid->err;
  pid->iout += pid->param.i * pid->err;
  pid->dout = pid->param.d * (pid->err - pid->last_err);

//  abs_limit(&(pid->iout), pid->param.inte_limit);
  pid->out = pid->pout + pid->iout + pid->dout;
  abs_limit(&(pid->out), pid->param.max_out);

  pid->last_err = pid->err;
  
  if (pid->enable == 0)
  {
    pid->out = 0;
  }
  
  return pid->out;
}

void pid_struct_init(
    struct pid *pid,
    int maxout,
    int inte_limit,

    int kp,
    int ki,
    int kd)
{
  pid->f_param_init = pid_param_init;
//  pid->f_pid_reset = pid_reset;

  pid->f_param_init(pid, maxout, inte_limit, kp, ki, kd);
//  pid->f_pid_reset(pid, kp, ki, kd);
}



