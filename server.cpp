#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>


typedef int(*operation)(int, int);  //Here we announce template, typedef e arvel function pointer vra//

int _add(int num1, int num2)
{
	return num1 + num2;
}

int _sub(int num1,int num2)
{
	return num1 - num2;
}

int _mul(int num1, int num2)
{
	return num1 * num2;
}

int _div(int num1, int num2)
{
	if(num2 != 0)
		return num1/num2;
	std::cerr << "num2 = 0";
	exit(EXIT_FAILURE);
}

int main()
{


    const int num = 100;
    const char* filename = "/m_sh_m";

    int fd = shm_open(filename, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        std::cerr << "Cannot create shared memory" << std::endl;
        return 1;
    }
    const int buff = 4096;

    if (ftruncate(fd, buff) == -1)
    {
        std::cerr << "Cannot resize shared memory" << std::endl;
        return 1;
    }

    int* adr = (int*) mmap(nullptr, buff, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (adr == MAP_FAILED)
    {
        std::cerr << "MMAP WAS FAILED";
        exit(errno);
    }
    operation op;
    const char *sem1_name = "/my_sem1";
    const char *sem2_name = "/my_sem2";
    sem_t* sem1 = sem_open(sem1_name, O_CREAT, 0666, 0);
    sem_t* sem2 = sem_open(sem2_name, O_CREAT, 0666, 0);
    if(sem1 == SEM_FAILED){
        std::cerr << "SEMAPHORE WAS FAILED:" << std::endl;
        return 1;
    }



    while(true) {
        sem_wait(sem1);
        if (adr[0] == 0) {
            adr[3] = _add(adr[1], adr[2]);
        } else if (adr[0] == 1) {
            adr[3] = _sub(adr[1], adr[2]);
        } else if (adr[0] == 2) {
            adr[3] = _mul(adr[1], adr[2]);
        } else if (adr[0] == 3) {
            adr[3] = _div(adr[1], adr[2]);
        } else {
            std::cerr << "adr[0] > 3 || adr[0] < 0";
            break;
        }

        sem_post(sem1);

    }

munmap(adr, buff);
shm_unlink(filename);
sem_close(sem1);
sem_unlink(sem1_name);
return 0;
}
