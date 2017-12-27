#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
void MotorOperate(int,int,int);
int main(){
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	int sock;
	char buf[64];
	int n;
	char space[] = " ";
	int speed;
	int angle;
	char *tok;
	int fd = wiringPiI2CSetup(0x08);
	sock0 = socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(10000);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock0,(struct sockaddr *)&addr,sizeof(addr));

	listen(sock0,5);
	
	len = sizeof(client);
	while(1){
		sock = accept(sock0, (struct sockaddr *)&client, &len);
		while((n = read(sock,buf,sizeof(buf)) > 0)){
			printf("%s\n",buf);
			tok = strtok(buf,space);
			tok = strtok(NULL,space);
			angle = strtol(tok,NULL,10);
			tok = strtok(NULL,space);
			tok = strtok(NULL,space);
			speed = strtol(tok,NULL,10);
			printf("ANGLE:%d SPEED:%d\n",angle,speed);
			MotorOperate(angle,speed,fd);
			for(int i=0;i<64;i++){
				buf[i] = 0;
			}
		}
		close(sock);
	}
	close(sock0);

	return 0;
}

void MotorOperate(int raw_angle,int raw_speed,int fd){
	int angle = 0;
	if(raw_angle >= 270 && raw_angle <= 360){
		angle = raw_angle-270;
	}else if(raw_angle >= 0 && raw_angle <= 90){
		angle = raw_angle+90;
	}else{
		angle = 90;
	}
	float motorA_angle =((float)angle)/180;
	float motorB_angle = (180-(float)angle)/180;
	float speed = 0;
	int direction = 0;
	unsigned char motorA_data = 0;
	unsigned char motorB_data = 0;
	int motorA_speed;
	int motorB_speed;
	if((raw_speed <= 512) && (raw_speed >= 0)){
		if(raw_speed > 256){
			speed = ((float)raw_speed-256)/256;
			direction = 0;
		}else if (raw_speed == 256){
			speed = 0;
			direction = 0;
		}else{
			speed = (256-(float)raw_speed)/256;
			direction = 1;
		}
	}else{
		speed = 0;
		direction = 0;
	}
	motorA_speed = (int)(speed*motorA_angle*64);
	motorB_speed = (int)(speed*motorB_angle*64);
	motorA_data = (direction<<6)+motorA_speed;
	motorB_data = 0x80 + (direction<<6) + motorB_speed;
	printf("Motor:1 Direction:%d Speed:%d Data:%d\n",direction,motorA_speed,motorA_data);
	printf("Motor:2 Direciton:%d Speed:%d Data:%d\n",direction,motorB_speed,motorB_data);
	wiringPiI2CWrite(fd,motorA_data);
	wiringPiI2CWrite(fd,motorB_data);
}
