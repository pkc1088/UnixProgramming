#ifndef __CHAT_SHARE_MEMORY_H__
#define __CHAT_SHARE_MEMORY_H__
typedef struct chatInfo{
    char userlist[3][10];   //������� 
    int user_no;         //���� �ε����� ����   
    char messages[10][60]; //�޼��� ����
    int message_index;      //�޼��� �ε����� ����
} CHAT_INFO;

#endif//__CHAT_SHARE_MEMORY_H__
