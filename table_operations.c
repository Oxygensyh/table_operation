#include "../include/table_operations.h"
#include "../include/jhash.h"
#include <sys/time.h>


/******************************五元组精确匹配********************************/


unsigned int get_tuple_hash(five_tuple_info tuple,unsigned int length)
{
    unsigned int a[3];
    a[0] = tuple.dst_ip;
    a[1] = tuple.src_ip;
    a[2] = (tuple.dst_port<<16)|tuple.src_ip;

    return jhash2(a,3,VOICE_HASH_GOLDEN_INTERER)&(length-1);
}

table_operations init_index_table(unsigned int tablesize)
{
    table_operations tops;
    tops.tablesize = tablesize;
    tops.idle_id_count = tablesize;

    htitem **ht = (htitem **)calloc(tablesize,sizeof(htitem*));
    for(int i =0;i<tablesize;i++)
    {
        ht[i]=(htitem *)malloc(sizeof(htitem));
        memset(ht[i],0,sizeof(htitem));
    }
    tops.index_table = ht;

    LinkedList head = NULL;
    head = tailInsert(&head,tablesize);
    tops.idle_id_table = head;

    return tops;
}

int search_index_table(table_operations tablename, five_tuple_info md)
{
    unsigned int index = get_tuple_hash(md,tablename.tablesize);
    htitem *tmp = tablename.index_table[index];
    while (tmp->next)
    {
        tmp = tmp->next;
        if(cmp_tuples(&md,&(tmp->md)))
        {
            //tmp->entry_info.hit_count++;
            return tmp->id;
        }    
    }
    return -1;
}


int cmp_tuples(five_tuple_info *key1,five_tuple_info *key2)
{
    u8 *m1 = (u8 *)key1;
	u8 *m2 = (u8 *)key2;
	u8 diffs = 0;
	int i = 0;

	while(i<13 && diffs == 0)
	{
		diffs |= (m1[i] ^ m2[i]);	
		i++;
	}
	return diffs == 0;
}

int insert_new_entry(table_operations *tablename, five_tuple_info md)
{
    int pos;
    if(tablename->idle_id_count == 0)
    {
        printf("FULL!\n");
        return -1;
    }
    else
    {
        //find pos to insert
        LinkedList p = tablename->idle_id_table->next->next;
        pos = p->data;
        //write in index table
        // index_table_entry temp;

        // struct timeval start;
        // gettimeofday(&start,NULL);
        // temp.time = start;
        // temp.valid = 1;
        // temp.hit_count = 0;

        //update idle id count
        tablename->idle_id_count --;

        //update idle id table
        deleteK(tablename->idle_id_table,1);

        unsigned int index = get_tuple_hash(md,tablename->tablesize);//获取哈希值即桶的位置
        //printf("hashvalue: %d pos: %d\n",index,pos);

        htitem *tmp = tablename->index_table[index];

        
        while (tmp->next)
        {
            tmp = tmp->next;
        }
        tmp->next=(htitem *)malloc(sizeof(htitem));
        tmp->next->md = md;
        tmp->next->id = pos;
        //tmp->next->entry_info = temp;
        tmp->next->next = NULL;

        return pos;    
    }  
}

int insert_new_entry_to_pos(table_operations *tablename, five_tuple_info md,int pos)
{
    if(tablename->idle_id_count == 0)
    {
        printf("FULL!\n");
        return -1;
    }
    else
    {
        //find pos to insert
        int x_pos = findX(tablename->idle_id_table,pos);
        if(x_pos == -1)
        {
            printf("This Pos is INAVALIDABLE!\n");
            return -1;
        }
        else
        {
            deleteK(tablename->idle_id_table,x_pos);
        }
        
        //write in index table
        // index_table_entry temp;

        // struct timeval start;
        // gettimeofday(&start,NULL);
        // temp.time = start;
        // temp.valid = 1;
        // temp.hit_count = 0;

        //update idle id count
        //tablename->idle_id_count --;

        //update idle id table
        //deleteK(tablename->idle_id_table,1);

        unsigned int index = get_tuple_hash(md,tablename->tablesize);//获取哈希值即桶的位置
        printf("hashvalue: %d pos: %d\n",index,pos);

        htitem *tmp = tablename->index_table[index];

        
        while (tmp->next)
        {
            tmp = tmp->next;
        }
        tmp->next=(htitem *)malloc(sizeof(htitem));
        tmp->next->md = md;
        tmp->next->id = pos;
        //tmp->next->entry_info = temp;
        tmp->next->next = NULL;

        return pos;    
    }  
}

int delete_entry(table_operations *tablename, five_tuple_info md)
{
    int tablesize = tablename->tablesize;
    unsigned int index = get_tuple_hash(md,tablesize);
    htitem *tmp = tablename->index_table[index];
    while (tmp->next)
    {
        tmp = tmp->next;
        if(cmp_tuples(&md,&(tmp->md)))
        {
            int id = tmp->id;
            //tmp->entry_info.valid = 0;
            tablename->idle_id_count++;
            add(tablename->idle_id_table,tablesize-1,id);
            return 0;
        }      
    }
    printf("NO Entry!\n");
    return -1;
}

int delete_all_entry(table_operations *tablename)
{
    int size = tablename->tablesize;
    htitem *tmp,*p;
    for(int i = 0;i<size;i++)
    {
        tmp = tablename->index_table[i]->next;
        while (tmp)
        {
            p = tmp->next;          
            add(tablename->idle_id_table,size-1,tmp->id);
            free(tmp);
            tablename->idle_id_count ++;
            tmp = p;
        } 
        tablename->index_table[i]->next = NULL;
    }
    return 0;
}

unsigned int get_idle_entry_num(table_operations tablename)
{
    return tablename.idle_id_count;
}

int get_table_entry_num(table_operations *tablename)
{
    return (tablename->tablesize)-(tablename->idle_id_count);
}


void print_entry_info(table_operations tablename)
{
    int size = tablename.tablesize;
    htitem *tmp;
    for(int i = 0;i<size;i++)
    {
        tmp = tablename.index_table[i];
        printf("Indextable[%d]\n",i);
        while (tmp->next)
        {
            tmp = tmp->next;
            five_tuple_info md = tmp->md;
            //struct timeval time = tmp->entry_info.time;
            //int count = tmp->entry_info.hit_count;
            printf("==========================================================\n");
            //printf("Valid: %d\n",tmp->entry_info.valid);
            printf("Tuples Value: %x %d %x %d %d\n",md.src_ip,md.src_port,md.dst_ip,md.dst_port,md.proto);
            printf("Index: %d\n",tmp->id);
            //printf("Time: %s",ctime((time_t *)&(time.tv_sec)));
            //printf("HitCounter: %d\nID:%d\n",count,tmp->id);
            printf("==========================================================\n");  
        } 
    }
}
    

void free_index_table(table_operations *tablename)
{
    int length = tablename->tablesize;
    for(int i = 0;i<length;i++)
    {
        free(tablename->index_table[i]);
    }
    free(tablename->index_table);
    free_list(tablename->idle_id_table);
}

/******************************带掩码精确匹配********************************/


table_operations_mask init_index_table_m(unsigned int tablesize)
{
    table_operations_mask tops;
    tops.tablesize = tablesize;
    tops.idle_id_count = tablesize;

    index_table_entry_mask *index_table = (index_table_entry_mask *)calloc(tablesize, sizeof(index_table_entry_mask));
    
    tops.index_table = index_table;

    LinkedList head = NULL;
    head = tailInsert(&head,tablesize);
    tops.idle_id_table = head;

    return tops;
}

int search_index_table_m(table_operations_mask tablename, five_tuple_info md)
{
    int tablesize = tablename.tablesize;
    for (int i = 0; i < tablesize; i++)
    {
        if(cmp_tuples_mask(&md,&(tablename.index_table[i].rule))&&tablename.index_table[i].valid)
        {
            return i;
        }
        else
        {
            continue;
        } 
    }
    return -1;
}

int cmp_tuples_mask(five_tuple_info *key,tuple_rule *rule)
{
    five_tuple_info *value = &(rule->value);
    five_tuple_info *mask = &(rule->mask);
    u8 *m1 = (u8 *)value;
	u8 *m2 = (u8 *)key;
	u8 *m3 = (u8 *)mask;
	u8 diffs = 0;
	int i = 0;//*cnt = sizeof(struct flow)/4;

	while(i<13 && diffs == 0)
	{
		diffs |= (m1[i] ^ m2[i])&m3[i];	
		i++;
	}
	return diffs == 0;
}

int insert_new_entry_m(table_operations_mask *tablename,tuple_rule rule)
{
    if(tablename->idle_id_count == 0)
    {
        printf("FULL!\n");
        return 0;
    }
    else
    {
        //find pos to insert
        LinkedList p = tablename->idle_id_table->next->next;
        int pos = p->data;
        //printf("____%d\n",pos);
        //write in index table
        index_table_entry_mask temp;

        temp.rule = rule;
        struct timeval start;
        gettimeofday(&start,NULL);

        temp.valid = 1;
        temp.index = pos;

        tablename->index_table[pos] = temp;

        //update idle id count
        tablename->idle_id_count --;

        //update idle id table
        deleteK(tablename->idle_id_table,1);

        return pos;
    }   
}
//插入新的表项掩码规则至指定位置
int insert_new_entry_to_pos_m(table_operations_mask *tablename,tuple_rule rule,int pos)
{
    if(tablename->idle_id_count == 0)
    {
        printf("FULL!\n");
        return 0;
    }
    else
    {
        //find pos to insert
        int x_pos = findX(tablename->idle_id_table,pos);
        if(x_pos == -1)
        {
            printf("This Pos is INAVALIDABLE!\n");
            return -1;
        }
        else
        {
            deleteK(tablename->idle_id_table,x_pos);
        }
        //LinkedList p = tablename->idle_id_table->next->next;
        //int pos = p->data;
        //printf("____%d\n",pos);
        //write in index table
        index_table_entry_mask temp;

        temp.rule = rule;
        struct timeval start;
        gettimeofday(&start,NULL);

        temp.valid = 1;
        temp.index = pos;
        tablename->index_table[pos] = temp;

        //update idle id count
        tablename->idle_id_count --;

        //update idle id table
        //deleteK(tablename->idle_id_table,1);

        return pos;
    }   
}

int delete_entry_m(table_operations_mask *tablename,tuple_rule rule)
{
    int tablesize = tablename->tablesize;
    int id = 0;
    for (int i = 0; i < tablesize + 1; i++)
    {
        if(i == tablesize)
        {
            id = -1;
        }
        else
        {
            if(cmp_tuples(&(tablename->index_table[i].rule.mask),&(rule.mask))&&cmp_tuples(&(tablename->index_table[i].rule.value),&(rule.value)))
            {
                id = i;
                break;
            }
            else
            {
                continue;
            }
        }  
    }
    //printf("delete pos: %d\n",id);
    if(id == -1)
    {
        printf("No entry!");
        return -1;
    }
    else
    {
        tablename->index_table[id].valid = 0;
        // idle 操作
        tablename->idle_id_count++;
        add(tablename->idle_id_table,tablesize-1,id);
        return 0;
    }
}

unsigned int get_idle_entry_num_m(table_operations_mask tablename)
{
    return tablename.idle_id_count;
}

int get_table_entry_num_m(table_operations_mask *tablename)
{
    return (tablename->tablesize)-(tablename->idle_id_count);
}

void print_entry_info_m(table_operations_mask tablename,int entry_id)
{
    index_table_entry_mask temp = tablename.index_table[entry_id];
    five_tuple_info md = temp.rule.value;
    five_tuple_info md1 = temp.rule.mask;

    printf("==========================================================\n");
    printf("Valid: %d\n",temp.valid);
    printf("Tuples Value: %x %d %x %d %d\n",md.src_ip,md.src_port,md.dst_ip,md.dst_port,md.proto);
    printf("Tuples Mask: %x %d %x %d %d\n",md1.src_ip,md1.src_port,md1.dst_ip,md1.dst_port,md1.proto);
    //printf("Time: %s",ctime((time_t *)&(time.tv_sec)));
    printf("==========================================================\n");
    //print(tablename.idle_id_table);
}

int delete_all_entry_m(table_operations_mask *tablename)
{
    int size = tablename->tablesize;

    for(int i = 0;i<size;i++)
    {
        index_table_entry_mask *tmp = &(tablename->index_table[i]);
        if(tmp->valid == 1)
        {
            tmp->valid = 0;
            add(tablename->idle_id_table,size-1,tmp->index);
            tablename->idle_id_count++;
        }
    }
    return 0;
}

void free_index_table_m(table_operations_mask *tablename)
{
    int length = tablename->tablesize;
    for(int i = 0;i<length;i++)
    {
        free(&(tablename->index_table[i]));
    }
    free_list(tablename->idle_id_table);
}

/******************************链表相关操作********************************/

//头插法 
LinkedList headInsert(LinkedList *L,unsigned int tablesize){
	LinkedList p,s;
	(*L) = s = (LinkedList)malloc(sizeof(LNode));
	s->next = NULL;
	int num = 0;
	while(num < tablesize){
		p = (LinkedList)malloc(sizeof(LNode));
		p->data = num;
		p->next = s->next;
		s->next = p;
        num ++;
	}
	return s;
}

//尾插法
LinkedList tailInsert(LinkedList *L,unsigned int tablesize){
	LinkedList p,s;
	int num = 0;
    int count = -1;
	(*L) = s = (LinkedList)malloc(sizeof(LNode)); 
	s->next = NULL;
	while(num < tablesize+1){
		p = (LinkedList)malloc(sizeof(LNode));
		p->data = count;
		p->next = NULL;
		s->next = p;
		s = p;
		num ++;
        count++;
	}
	return (*L);
} 

//给第k给结点之后增加一个值x
void  add(LinkedList L, int k, int x){
	int num;
	LinkedList p,s;
	p = L->next;
	for(int i=1; i<k; i++){
		p = p->next;
	} 
	s = (LinkedList)malloc(sizeof(LNode));
	s->data = x;
	s->next = p->next;
	p->next = s;
}

//删除第k个结点
void deleteK(LinkedList L, int k){
	LinkedList p,q;
	p = L->next;
	for(int i=1; i<k-1; i++){
		p = p->next;
	}
	q = p->next;
	p->next = q->next;
	free(q);
} 

//更改第k个结点的值为x
void update(LinkedList L, int k, int x){
	LinkedList p = L->next;
	for(int i=1; i<k; i++){
		p = p->next;
	}
	p->data = x;
} 

//查询第k个结点的值 
int getK(LinkedList L, int k){
	LinkedList p = L->next;
	for(int i=1; i<k; i++){
		p = p->next;
	} 
	return p->data;
}

//输出链表所有值 
void print(LinkedList L){
	LinkedList p = L->next;
	while(p){
		printf("%d\t", p->data);
		p = p->next;
	}
	printf("\n");
}

int findX(LinkedList L,int x)
{
	LinkedList p = L->next;
	int pos = 0;
	while(p->next)
	{
		if(p->data == x)
		{
			return pos;
		}
		pos ++;
		p = p->next;
	}
	return -1;
}

void free_list(LinkedList L)
{
	LinkedList p,q;
	p = L->next;

	while(p)
	{
        q = p->next;
        free(p);
        p = q;
	}
	free(L);
}

