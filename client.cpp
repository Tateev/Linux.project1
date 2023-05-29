#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstring>
#include <string>


int main(){


    const char* shm_name = "/m_sh_m";
    const int buff = 4096;
    const int shm_size = buff * sizeof(int);

    int fd = shm_open(shm_name, O_RDWR, 0);
    if(fd == -1){
        std::cerr << "Can not open shared memory: " << std::strerror(errno) << std::endl;

    close(fd);
}

    const char *sem1_name = "/my_sem1";
    const char *sem2_name = "/my_sem2";
        sem_t* sem1 = sem_open(sem1_name, O_CREAT, 0666, 0);
        sem_t* sem2 = sem_open(sem2_name, O_CREAT, 0666, 0);
            if(sem1 == SEM_FAILED){
                std::cerr << "Cannot create semaphore:" << std::endl;
                return 1;
            }

	int* adr = (int*) mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (adr == MAP_FAILED)
        {
                std::cerr << "MMAP FAILED";
                exit(errno);

        close(fd);
        }

	int functionID;
	std::cout << "Please, enter the function identifier (0: add, 1: sub, 2: mul, 3: div): " << std::endl;
	std::cin >> functionID;

	int a, b;
	std::cout << "Enter the arguments: " << std::endl;
	std::cin >> a >> b;

	adr[0] = functionID;
	adr[1] = a;
	adr[2] = b;

	sem_post(sem1);

	sem_wait(sem1);
	int res = adr[3];

	std::cout << "The result is: " << res << std::endl;

	munmap(adr, shm_size);
        sem_close(sem1);

	return 0;
}
