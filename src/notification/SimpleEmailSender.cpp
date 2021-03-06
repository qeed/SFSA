#include "SimpleEmailSender.h"
#include "../base64.h"

SimpleEmailSender::SimpleEmailSender()
{
	bzero(hostname);
	bzero(login);
	bzero(password);
	bzero(from);
	bzero(to);
	bzero(subject);
	bzero(data);
	memset(&servaddr, 0, sizeof(servaddr));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
}

void SimpleEmailSender::setMyEmail(char *myemail)
{
	strcpy(myEmail, myemail);
}

void SimpleEmailSender::sendPacket(int sockfd, char *buff)
{
	char recvline[2048];
	bzero(recvline);

	std::cout<<"SEND: "<<buff<<std::endl;
	send(sockfd, buff, strlen(buff), 0);
	recv(sockfd, recvline, sizeof(recvline), 0);
	std::cout<<"RECV: "<<recvline<<std::endl;
}

void SimpleEmailSender::setHost(char *hostname)
{
	strcpy(this->hostname, hostname);
	host = gethostbyname(hostname);
	memcpy((char*)&(servaddr.sin_addr), host->h_addr, host->h_length);
}

void SimpleEmailSender::setPort(int port)
{
	this->port = port;
	servaddr.sin_port = htons(port);
}

void SimpleEmailSender::setLogin(char *login)
{
	strcpy(this->login, login);
	char *base64;
	base64_encode(login, &base64);
	strcpy(this->base64Login, base64);
}

void SimpleEmailSender::setPassword(char *password)
{
	strcpy(this->password, password);
	char *base64;
	base64_encode(password, &base64);
	strcpy(this->base64Password, base64);
}

void SimpleEmailSender::setFrom(char *from)
{
	strcpy(this->from, from);
}

void SimpleEmailSender::setTo(char *to)
{
	strcpy(this->to, to);
}

void SimpleEmailSender::setSubject(char *subject)
{
	strcpy(this->subject, subject);
}

void SimpleEmailSender::setData(char *data)
{
	strcpy(this->data, data);
}

int SimpleEmailSender::Send()
{
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		throw "Connect error";
		return -1;
	}
	if(isCharStringEmpty(hostname))
		throw (char const*)"Hostname is empty";
	if(isCharStringEmpty(login))
		throw (char const*)"Login is empty";
	if(isCharStringEmpty(password))
		throw (char const*)"Password is empty";
	if(isCharStringEmpty(from))
		throw (char const*)"From is empty";
	if(isCharStringEmpty(to))
		throw (char const*)"'To' is empty";

	char buff[256];
	bzero(buff);

	recv(sockfd, buff, sizeof(buff), 0);
	bzero(buff);

	strcpy(buff, "EHLO ");
	strcat(buff, hostname);
	strcat(buff, "\r\n");

	sendPacket(sockfd, buff);
	sendPacket(sockfd, (char*)"AUTH LOGIN\r\n");

	bzero(buff);
	strcpy(buff, base64Login);
	strcat(buff, "\r\n");
	sendPacket(sockfd, buff);

	bzero(buff);
	strcpy(buff, base64Password);
	strcat(buff, "\r\n");
	sendPacket(sockfd, buff	);

	// shit-code...

	bzero(buff);
	strcpy(buff, (char*)"MAIL FROM:<");
	strcat(buff, myEmail);
	strcat(buff, (char*)">\r\n");
	sendPacket(sockfd, buff);

	bzero(buff);
	strcpy(buff, "RCPT TO:<");
	strcat(buff, to);
	strcat(buff, ">\r\n");
	sendPacket(sockfd, buff);
	sendPacket(sockfd, (char*)"DATA\r\n");

	bzero(buff);
	strcpy(buff, (char*)"From:");
	strcat(buff, from);
	strcat(buff, (char*)"<");
	strcat(buff, myEmail);
	strcat(buff, (char*)">\r\n");

	strcat(buff, (char*)"TO:");
	strcat(buff, to);
	strcat(buff, (char*)"\r\nSubject:");
	strcat(buff, subject);
	strcat(buff, (char*)"\r\n\r\n");
	strcat(buff, data);
	strcat(buff, (char*)"\r\n.\r\n");
	sendPacket(sockfd, buff);

	sendPacket(sockfd, (char*)"QUIT\r\n");

	return 0;
}