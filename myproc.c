#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>

#include <linux/sched.h>
#include <linux/utsname.h>
struct task_struct *task;

#define MAX_PROC_SIZE 100
static char proc_data[MAX_PROC_SIZE];
char *endp;
int ipid=0;

static struct proc_dir_entry *proc_write_entry;

int read_proc(char *buf,char **start,off_t offset,int count,int *eof,void *data )
{

  struct list_head *list;
  int len=0;
  int num_of_children = 0;
  for_each_process( task ){
      if(task->pid == ipid){
        list_for_each(list, &task->children){ ++num_of_children; }
        len = sprintf(buf,"\n%d\n%lX~%lX\n%lX~%lX\n%lX~%lX\n%s\n%d\n",
                      task->parent->pid,
                      task->mm->start_code, task->mm->end_code,
                      task->mm->start_data, task->mm->end_data,
                      task->mm->start_brk,  task->mm->brk,
                      utsname()->release,
                      num_of_children);
      }
   }

    return len;
}

int write_proc(struct file *file,const char *buf,int count,void *data )
{

if(count > MAX_PROC_SIZE)
    count = MAX_PROC_SIZE;
if(copy_from_user(proc_data, buf, count))
      return -EFAULT;
    ipid=simple_strtol(proc_data,&endp,10);
return count;
}

void create_new_proc_entry()
{
    proc_write_entry = create_proc_entry("myproc",0666,(void *)NULL);
if(!proc_write_entry)
      {
    printk(KERN_INFO "Error creating proc entry");
    return;
    }
proc_write_entry->read_proc = (void *)read_proc ;
proc_write_entry->write_proc = (void *)write_proc;
printk(KERN_INFO "proc initialized");
}

int proc_init (void) {
    create_new_proc_entry();
    return 0;
}

void proc_cleanup(void) {
    printk(KERN_INFO " Inside cleanup_module\n");
    remove_proc_entry("myproc",NULL);
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
