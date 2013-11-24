 #include <stdio.h>
 #include <sys/ioctl.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>


 /* Define ioctl commands */
 #define IOCTL_PATCH_TABLE 0x00000001
 #define IOCTL_FIX_TABLE   0x00000004


int main(void)
{
   int device = open("/dev/interceptor", O_RDWR);
   ioctl(device, IOCTL_PATCH_TABLE);
   sleep(5);
   ioctl(device, IOCTL_FIX_TABLE);
   close(device);
   return 0;
}
