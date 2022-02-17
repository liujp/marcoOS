#ifndef __CORE_H
#define __CORE_H
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef signed char int8_t;
typedef signed int int16_t;
typedef void (*pvHandle)(void *);
enum status_t{
    STOP = 0x00,
    START = 0x01,
    READY = 0x02,
    PENDING = 0x04
};
typedef struct {  
  uint8_t status;
  uint16_t count;
  uint16_t interval;
  pvHandle handler;
} task_t; 

/*task related*/
#define SET_TASK_STOP(task) task.status = STOP
#define SET_TASK_START(task) task.status |= START
#define SET_TASK_READY(task) task.status |= READY;
#define IS_TASK_STOP(task) (task.status == STOP)
#define IS_TASK_START(task) ((task.status & START) != STOP)
#define IS_TASK_READY(task) ((task.status & READY) != STOP)
#define IS_TASK_SATRT_AND_READY(task) (((task.status & READY) != STOP) && ((task.status & START) != STOP))
#define CLEAR_TASK_READY(task) task.status &= ~READY

/*Set bit map interfaces for 8 bit value */
#define SET_BITMAP_BIT(bitmap, bit) bitmap |= (0x01 << bit)
#define CLEAR_BITMAP_BIT(bitmap, bit) bitmap &= ~(0x01 << bit)
#define SET_ALL_BITMAP(bitmap) bitmap = 0xff
#define CLEAR_ALL_BITMAP(bitmap) bitmap = 0x00
#define IS_BITMAP_SET_BIT(bitmap, bit) (((bitmap >> bit) & 0x01) == 1)
#define IS_BITMAP_CLEAR_BIT(bitmap, bit) (((bitmap >> bit) & 0x01) == 0)

/*task macros*/
#define TDName(name) TASK_##name,
#define FName(name) HANDLER_##name
/*get event id from event name*/
#define EName(name) EVENT_##name
/*run task function*/
#define TRun(name, value) FName(name)(value)
/*define a task function*/
#define TDef(name) void FName(name)(void* para)
/*get task id from task name*/
#define TName(name) TASK_##name

#define FDefine(name) void FName(name)(void* para);
#define TaskListInit(_) enum TaskType{ _(TDName) TASK_NUM }; _(FDefine)
#define EventListInit(_) enum EventType{ _(EName) }

#define ADDTASK(tasks, name, value) tasks[TName(name)].status = START; \
                                    tasks[TName(name)].count = 0; \
                                    tasks[TName(name)].interval = value; \
                                    tasks[TName(name)].handler = FName(name);

#define SCHEDULE(tasks,task_num) for(uint8_t i = 0; i < task_num; i++) { \
        if(IS_TASK_START(tasks[i])) { \
           if(!IS_TASK_READY(tasks[i])) { \
            tasks[i].count ++; \
            if(tasks[i].count >= tasks[i].interval) { \
                SET_TASK_READY(tasks[i]); \
                tasks[i].count = 0; \
            } \
           } \
        } \
    } 

#define EPOLL(tasks, task_num, value) for(uint8_t i = 0; i < task_num; i++) { \
        if(IS_TASK_SATRT_AND_READY(tasks[i])) { \
            tasks[i].handler(&value); \
            CLEAR_TASK_READY(tasks[i]); \
        } \
    }

#endif