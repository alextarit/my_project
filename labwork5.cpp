#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>



int main(){
	// дескрипторы дочерних процессов
	pid_t pid1, pid2, pid3;
	
	// дескриптор канала
	int fd[2];
	
	pipe(fd);
	
	// создаём дочерний процесс для записи первого сообщения
	pid1 = fork();
	if (pid1 == 0){
		char message[] = "Сообщение первого процесса  \n";
		for (int i = 0; i < 5; i++){
			message[strlen(message) - 2] = ('0' + i);
			
			// закрываем дескриптор чтения
			close(fd[0]);
			
			// посылаем сообщение в канал
			write(fd[1], message, sizeof(message));
			
		}
		exit(0);
	}
	
	else {	
		// создаём дочерний процесс для записи второго сообщения
		pid2 = fork();
		if (pid2 == 0){
			char message[] = "Сообщение второго процесса  \n";
			for (int i = 0; i < 5; i++){
				message[strlen(message) - 2] = ('0' + i);			
				
				// закрываем дескриптор чтения
				close(fd[0]);
				
				// посылаем сообщение в канал
				write(fd[1], message, sizeof(message));
				
			}
			exit(0);
			
		}
		else{
			// создаём дочерний процесс для чтения сообщений
			pid3 = fork();
			
			if (pid3 == 0){
				// число прочитанных байтов
				int number;
				
				// буффер очередного сообщения
				char buffer[80];
				
				// считываем 10 сообщений из каналов
				for (int i = 0; i < 10; i++){
					
					// закрываем дескриптор записи
					close(fd[1]);
					
					// устанавливаем ноль в число прочитанных символов
					number = 0;
					
					// считываем первый символ в буффер
					read(fd[0], buffer + number, 1);
					
					// считываем в строку данные, пока не встретил символ "\0"
					while (buffer[number] != '\0'){
						// увеличиваем на единицу счёт
						number++;
						
						// читаем данные из канала
						read(fd[0], buffer + number, 1);
					}
					// записываем на стандратный вывод данные прочитанные из канала
					write(1, buffer, number);
				}
				
				exit(0);
				
			} 
			else {
				// ожидаем завершение процессов
				waitpid(pid1, NULL, 0);
				waitpid(pid2, NULL, 0);
				waitpid(pid3, NULL, 0);
				exit(0);
			}
		}
	}
	
	return 0;
}
