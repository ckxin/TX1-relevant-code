#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <streambuf>
#include "pointer.hpp"


#define PORT 6000
#define BUFFSIZE 65536
#define FILE_NAME_MAX_SIZE 512

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <thread>
using namespace std;
using namespace cv;

//VideoCapture cap;
float ds;
char img[FILE_NAME_MAX_SIZE],imgname[FILE_NAME_MAX_SIZE];
//VideoCapture cap("rtsp://admin:12345678xin@172.168.1.64/h264/ch1/main/av_stream");
Mat frame;
Mat frame1,frame2,frame3,frame4,frame5;
int m=1,n=1;
bool flag,flag1;

int tcpclient(char *IP,char *path,char *name)//三个参数，第一个为为IP地址，第二个是要传送的文件路径，第三个是文件名称
{
    int clientfd;


   /* if(argc != 4)//当传入的实参不是三个时输出错误
    {
        fprintf(stderr,"Usage:./fileclient <IP_Address> <filename>\n");
        exit(1);
    }*/

    //Input the file name
    char filename[FILE_NAME_MAX_SIZE];//定义文件名称数组
    char imagename[FILE_NAME_MAX_SIZE];
    bzero(filename,FILE_NAME_MAX_SIZE);//将文件名数组置零
    bzero(imagename,FILE_NAME_MAX_SIZE);

    strcpy(filename,path);//将要发送的文件路径复制到filename中
    strcpy(imagename,name);
    struct sockaddr_in clientaddr;//创建客户端套接字
    bzero(&clientaddr,sizeof(clientaddr));

    clientaddr.sin_family=AF_INET;
    clientaddr.sin_addr.s_addr=htons(INADDR_ANY);
    clientaddr.sin_port=htons(0);

    clientfd=socket(AF_INET,SOCK_STREAM,0);//判断创建的套接字是否成功，成功返回文件描述符，失败返回-1

    if(clientfd<0)
    {
        perror("socket");
        exit(1);
    }
    //cout<<"the clientfd="<<clientfd<<endl;
    //bind
    if(bind(clientfd,(struct sockaddr*)&clientaddr,sizeof(clientaddr))<0)
    {
        perror("bind");
        exit(1);
    }

    struct sockaddr_in svraddr;//创建服务器套接字
    bzero(&svraddr,sizeof(svraddr));
    if(inet_aton(IP,&svraddr.sin_addr)==0)//将服务器字符串IP地址转换为网络序列IP地址，若转换失败则输出错误
    {
        perror("inet_aton");
        exit(1);
    }
    svraddr.sin_family=AF_INET;
    svraddr.sin_port=htons(PORT);

    socklen_t svraddrlen=sizeof(svraddr);
    if(connect(clientfd,(struct sockaddr*)&svraddr,svraddrlen)<0)//请求与服务器建立连接
    {
        perror("connect");
        exit(1);
    }

    //send file imformation
    char buff[BUFFSIZE];//创建buff数组(缓存区)

    char buff1[BUFFSIZE];

    int count;
    bzero(buff,BUFFSIZE);
    strncpy(buff,imagename,strlen(imagename)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(imagename));
    //将filename中内容复制到buff中，当filename长度大于FILE_NAME_MAX_SIZE长度时则复制FILE_NAME_MAX_SIZE个字节，否则复制filename长度个字节
    count=send(clientfd,buff,strlen(buff),0);//将buff中内容通过套接字协议发送，并返回发送字节数
    cout<<endl;
    cout<<"count="<<count<<endl;
    //sleep(0.8);
    bzero(buff1,BUFFSIZE);
    int n=recv(clientfd,buff1,BUFFSIZE,0);
    cout << "n=" << n << endl;
    while(1)
    {
    if(n!=0)
    {
    	break;
    }
    }
    if(count<0)
    {
        perror("Send file information");
        exit(1);
    }

    //read file
    FILE *fd=fopen(filename,"rb");
    if(fd==NULL)
    {
        printf("File :%s not found!\n",filename);
    }
    else
    {
        bzero(buff,BUFFSIZE);
        int file_block_length=0;
        while((file_block_length=fread(buff,sizeof(char),BUFFSIZE,fd))>0)
        {
            printf("file_block_length:%d\n",file_block_length);
            if(send(clientfd,buff,file_block_length,0)<0)
            {
                perror("Send");
                exit(1);
            }
            bzero(buff,BUFFSIZE);
        }
        fclose(fd);
        printf("Transfer file finished !\n");
    }

    close(clientfd);
    return 0;
}


void t2()
{
	VideoCapture cap("rtsp://admin:12345678xin@172.168.1.64/h264/ch1/main/av_stream");

	/*imshow("1",frame);
	waitKey(30);*/
	Mat diff1,diff2,diff3,diff4,diff5;

	for (;;)
		{

			flag=0;
			char photoname[FILE_NAME_MAX_SIZE];
			int photonum=0;
			sprintf(photoname, "%s%d%s", "/home/ubuntu/TX1/TX1main001/Photo/img", photonum, ".jpg");
			cap >> frame;
			imwrite(photoname,frame);
			photonum++;

			int sum1=0;
			int sum2=0;
			int sum3=0;
			int sum4=0;
			int sum5=0;
			bool OK = cap.grab();

			if (OK == false)
			{
			   cout << "cannot grab" << endl;
			}
			else
			{

			cap >> frame1;
			sleep(1/3*1000);
			cap >> frame2;
			sleep(1/3*1000);
			cap >> frame3;
			sleep(1/3*1000);
			cap >> frame4;
			sleep(1/3*1000);
			cap >> frame5;



			diff1=abs(frame2-frame1);
			diff2=abs(frame3-frame2);
			//diff3=abs(frame3-frame1);
			diff3=abs(frame4-frame3);
			diff4=abs(frame5-frame4);
			diff5=abs(frame5-frame1);

			for(int j=0;j<diff1.rows;j++)
			{
				for(int k=0;k<diff1.cols;k++)
			{
				Vec3b pixel1=(0,0,0);
				Vec3b pixel2=(0,0,0);
				Vec3b pixel3=(0,0,0);
				Vec3b pixel4=(0,0,0);
				Vec3b pixel5=(0,0,0);

				pixel1=diff1.at<Vec3b>(j,k);
				sum1=sum1+pixel1[0]+pixel1[1]+pixel1[2];

				pixel2=diff2.at<Vec3b>(j,k);
				sum2=sum2+pixel2[0]+pixel2[1]+pixel2[2];

				pixel3=diff3.at<Vec3b>(j,k);
				sum3=sum3+pixel3[0]+pixel3[1]+pixel3[2];

				pixel4=diff4.at<Vec3b>(j,k);
				sum4=sum4+pixel4[0]+pixel4[1]+pixel4[2];

				pixel5=diff5.at<Vec3b>(j,k);
				sum5=sum5+pixel5[0]+pixel5[1]+pixel5[2];

			}
			}


			if(sum1<2000000 && sum2<2000000 && sum3<2000000 && sum4<2000000 && sum5<3000000 )
			{
				flag=1;
				cout<<"grab succeed."<<endl;
				while(1)
				{
					if(flag1==1)
					{
						cout<<"cap the pic succeed"<<endl;
						break;
					}
				}


		    }


			}



		}

}


void t1()
{
	pointer detect;
	//float ds;
	Mat pic=imread(img);
	ds=detect.run0(pic);
	//ds=100;
	//cout << "ds=" << ds << endl;
}

int main(void)
{


	//VideoCapture cap("/home/xin/cuda-workspace/main/src/23681.mp4");

	ofstream oFile;
	oFile.open("/home/ubuntu/TX1/TX1main001/excel/indication.csv", ios::out | ios::trunc);    // 这样就很容易的输出一个需要的excel 文件
	oFile << "表计" << "," << "读数" <<endl;
	thread th1(t2);

	//Mat diff1,diff2,diff3,diff4,diff5;



	//char img[FILE_NAME_MAX_SIZE],imgname[FILE_NAME_MAX_SIZE];
	//char imgname[FILE_NAME_MAX_SIZE];
	char savePath[FILE_NAME_MAX_SIZE];


	for (;;)
	{

		/*Mat frame1=Mat::zeros(720,1280,CV_8UC3);
		Mat frame2=Mat::zeros(720,1280,CV_8UC3);
		Mat frame3=Mat::zeros(720,1280,CV_8UC3);
		Mat frame4=Mat::zeros(720,1280,CV_8UC3);
		Mat frame5=Mat::zeros(720,1280,CV_8UC3);

		Mat diff1=Mat::zeros(720,1280,CV_8UC3);
		Mat diff2=Mat::zeros(720,1280,CV_8UC3);
		Mat diff3=Mat::zeros(720,1280,CV_8UC3);
		Mat diff4=Mat::zeros(720,1280,CV_8UC3);
		Mat diff5=Mat::zeros(720,1280,CV_8UC3);*/

		/*cap >> frame;
		imshow("1",frame);
		waitKey(30);*/


		/*int sum1=0;
		int sum2=0;
		int sum3=0;
		int sum4=0;
		int sum5=0;


		cap >> frame1;
		sleep(1/3*1000);
		cap >> frame2;
		sleep(1/3*1000);
		cap >> frame3;
		sleep(1/3*1000);
		cap >> frame4;
		sleep(1/3*1000);
		cap >> frame5;


		diff1=abs(frame2-frame1);
		diff2=abs(frame3-frame2);
		//diff3=abs(frame3-frame1);
		diff3=abs(frame4-frame3);
		diff4=abs(frame5-frame4);
		diff5=abs(frame5-frame1);

		for(int j=0;j<diff1.rows;j++)
		{
			for(int k=0;k<diff1.cols;k++)
		{
			Vec3b pixel1=(0,0,0);
			Vec3b pixel2=(0,0,0);
			Vec3b pixel3=(0,0,0);
			Vec3b pixel4=(0,0,0);
			Vec3b pixel5=(0,0,0);

			pixel1=diff1.at<Vec3b>(j,k);
			sum1=sum1+pixel1[0]+pixel1[1]+pixel1[2];

			pixel2=diff2.at<Vec3b>(j,k);
			sum2=sum2+pixel2[0]+pixel2[1]+pixel2[2];

			pixel3=diff3.at<Vec3b>(j,k);
			sum3=sum3+pixel3[0]+pixel3[1]+pixel3[2];

			pixel4=diff4.at<Vec3b>(j,k);
			sum4=sum4+pixel4[0]+pixel4[1]+pixel4[2];

			pixel5=diff5.at<Vec3b>(j,k);
			sum5=sum5+pixel5[0]+pixel5[1]+pixel5[2];

		}
		}*/

		flag1=0;
		cout<<"main thread exe succeed."<<endl;
		if(flag==1 )
		{

			sprintf(img, "%s%d%s", "/home/ubuntu/TX1/TX1main001/Pictures/img", n, ".jpg");
			imwrite(img,frame5);
			flag1=1;
			cout<<"save succeed."<<endl;
			sprintf(imgname, "%s%d%s", "img", m, ".jpg");
			cout<<endl;
			cout<<img<<endl;
			cout<<endl;
			cout<<imgname<<endl;
			tcpclient("192.168.1.103",img,imgname);
			thread th1(t1);
			th1.join();

			//sleep(1);



			oFile << m<<"," << ds << endl;
			//oFile << m<<"," << m+200 << endl;
			bzero(img,sizeof(img));
			bzero(imgname,sizeof(imgname));
			sprintf(img, "%s", "/home/ubuntu/TX1/TX1main001/excel/indication.csv");
			sprintf(imgname, "%s", "indication.csv");
			tcpclient("192.168.1.103",img,imgname);
			//waitKey(2000);
			//for(int n1=0;n1<=10000;n1++);


			if(n==30)
			{
				for(int q=1;q<=n;q++)
					{
						

						bzero(savePath,sizeof(savePath));
						sprintf(savePath, "%s%d%s", "/home/ubuntu/TX1/TX1main001/Pictures/img", q, ".jpg");
						if(remove(savePath)==0)
						{
						   cout<<"删除成功"<<endl;
						}
						else
						{
						   cout<<"删除失败"<<endl;
						}

					}
				n=0;

			}

			bzero(img,sizeof(img));
			bzero(imgname,sizeof(imgname));
			m++;
			n++;

			/*sprintf(img, "%s%d%s", "/home/xin/cuda-workspace/main/img", m, ".png");
			imwrite(img,frame5);
			bzero(img,sizeof(img));*/

			/*cout<<"The difference="<<sum1<<"  "<<sum2<<"  "<<sum3<<"  "<<sum4<<"  "<<sum5<<" are little."<<endl;
			cout<<"So we think the pictures are same."<<endl;*/

		}

		/*else
		{
			//cout<<"The difference="<<sum1<<"  "<<sum2<<"  "<<sum3<<"  "<<sum4<<"  "<<sum5<<" are big."<<endl;

			//cout<<"So we think the pictures are different."<<endl;
			cout<<"We think the pictures are different."<<endl;


		}*/

		//if(m==200)
		//{break;}

	}

	oFile.close();
	return 0;


}
