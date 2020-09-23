/**********************************update log**********************************
2020/09/08
    添加函数 删除整个索引表项 int delete_all_entry(table_operations *tablename);
    修改了带掩码部分的，删除了不必要的字段

2020/09/06 
    添加函数 int delete_entry_id(table_operations *tablename, unsigned int index);
    根据索引号删除某条表项,但是效率比較低
    可以通過id找到五元组，然后再调用五元组删除
2020/09/02 
    修改free函数，注意释放的方式

*******************************************************************************/
#ifndef TABLE_OPERATIONS_H
#define TABLE_OPERATIONS_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;



typedef struct node{
	int data;
	struct node *next;
}LNode,*LinkedList;

//匹配域（五元组）
typedef struct five_tuple_info{
	u32 src_ip;//源端ip地址
	u32 dst_ip;//目的端ip地址
	u16 src_port; //源端端口
	u16 dst_port; //目的端端口
	u8 proto; //IP协议类型
}five_tuple_info,match_domain;

/******************************五元组精确匹配********************************/
typedef struct index_table_entry
{
    struct timeval time;
    unsigned int hit_count;
    unsigned char valid;
}index_table_entry;

typedef struct htitem //hash节点的数据结构
{
    struct five_tuple_info md;
    unsigned int id;
	//struct index_table_entry entry_info;
	struct htitem *next;
}htitem;


typedef struct table_operations
{
    struct htitem **index_table; //索引哈希表
    struct node *idle_id_table;  //空闲ID记录表（链表）
    unsigned int idle_id_count;  //空闲ID数量
    unsigned int tablesize; //表项规模
}table_operations;

//初始化
table_operations init_index_table(unsigned int tablesize);
//按五元组查询，返回ID
int search_index_table(table_operations tablename, five_tuple_info md);
//比较两个五元组是否完全相等
int cmp_tuples(five_tuple_info *key1,five_tuple_info *key2);
//插入五元组至表项中
int insert_new_entry(table_operations *tablename, five_tuple_info md);
//插入五元组到指定位置
int insert_new_entry_to_pos(table_operations *tablename, five_tuple_info md,int pos);
//根据五元组删除表项
int delete_entry(table_operations *tablename, five_tuple_info md);
//根据索引号删除某条表项
int delete_all_entry(table_operations *tablename);
//获取空闲表项数量
unsigned int get_idle_entry_num(table_operations tablename);
//获取目前表中已经存取的数量
int get_table_entry_num (table_operations *tablename);
//打印某一表项的信息
void print_entry_info(table_operations tablename);
//释放资源
void free_index_table(table_operations *tablename);
//
unsigned int get_tuple_hash(five_tuple_info tuple,unsigned int length);

/******************************带掩码精确匹配********************************/
typedef struct tuple_rule
{
	five_tuple_info value;
	five_tuple_info mask;
}tuple_rule;

typedef struct index_table_entry_mask
{
    struct tuple_rule rule;
    int index;
    unsigned char valid;
}index_table_entry_mask;

typedef struct table_operations_mask
{
    struct index_table_entry_mask *index_table; //索引表
    struct node *idle_id_table;  //空闲ID记录表
    unsigned int idle_id_count;  //空闲ID数量
    unsigned int tablesize; //表项规模
}table_operations_mask;
//初始化
table_operations_mask init_index_table_m(unsigned int tablesize);
//按照五元组在表中查询，返回第一个复合掩码规则的ID
int search_index_table_m(table_operations_mask tablename, five_tuple_info md);
//比较是否符合带掩码
int cmp_tuples_mask(five_tuple_info *key,tuple_rule *rule);
//插入新的表项掩码规则
int insert_new_entry_m(table_operations_mask *tablename,tuple_rule rule);
//插入新的表项掩码规则至指定位置
int insert_new_entry_to_pos_m(table_operations_mask *tablename,tuple_rule rule,int pos);
//删除掩码规则
int delete_entry_m(table_operations_mask *tablename,tuple_rule rule);
//获取空闲表项的数量
unsigned int get_idle_entry_num_m(table_operations_mask tablename);
//获取当前表项的数量
int get_table_entry_num_m(table_operations_mask *tablename);
//打印某一表项的相关信息
void print_entry_info_m(table_operations_mask tablename,int entry_id);
//释放资源
void free_index_table_m(table_operations_mask *tablename);
int delete_all_entry_m(table_operations_mask *tablename);



/******************************链表相关操作********************************/
//头插法 
LinkedList headInsert(LinkedList *L,unsigned int tablesize);

//尾插法
LinkedList tailInsert(LinkedList *L,unsigned int tablesize);

//给第k给结点之后增加一个值x
void  add(LinkedList L, int k, int x);

//删除第k个结点
void deleteK(LinkedList L, int k);

//更改第k个结点的值为x
void update(LinkedList L, int k, int x); 

//查询第k个结点的值 
int getK(LinkedList L, int k);

//查找值为x的位置
int findX(LinkedList L,int x);

//输出链表所有值 
void print(LinkedList L);

void free_list(LinkedList L);

#endif