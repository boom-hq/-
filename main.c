#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


// 统计字符频度的临时结点
typedef struct {
	unsigned char uch;			// 以8bits为单元的无符号字符
	unsigned long weight;		// 每类（以二进制编码区分）字符出现频度
} TmpNode;

// 哈夫曼树结点
typedef struct {
	unsigned char uch;				// 以8bits为单元的无符号字符
	unsigned long weight;			// 每类（以二进制编码区分）字符出现频度
	char *code;						// 字符对应的哈夫曼编码（动态分配存储空间）
	int parent, lchild, rchild;		// 定义双亲和左右孩子
} HufNode, *HufTree;

// 选择最小和次小的两个结点，建立哈夫曼树调用
void select(HufNode *huf_tree, unsigned int n, int *s1, int *s2)
{
	// 找最小
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for(i = 0; i < n; ++i)           
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
		huf_tree[*s1].parent=1;   // 标记此结点已被选中

	// 找次小
	min=ULONG_MAX;
	for(i = 0; i < n; ++i)            
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
} 

// 建立哈夫曼树
void CreateTree(HufNode *huf_tree, unsigned int char_kinds, unsigned int node_num)
{
	unsigned int i;
	int s1, s2;
	printf("\n哈夫曼树为：\nparent rchild	lchild	weight\n");
	for(i = char_kinds; i < node_num; ++i)  
	{ 
		select(huf_tree, i, &s1, &s2);		// 选择最小的两个结点
		huf_tree[s1].parent = huf_tree[s2].parent = i; 
		huf_tree[i].lchild = s1; 
		huf_tree[i].rchild = s2; 
		huf_tree[i].weight = huf_tree[s1].weight + huf_tree[s2].weight; 
	} 
	for(i = 0; i < node_num; ++i) 
		printf("%d	%d	%d	%d\n",huf_tree[i].parent,huf_tree[i].rchild,huf_tree[i].lchild,huf_tree[i].weight);
}

// 生成哈夫曼编码,	从叶子向根反向遍历求编码
void HufCode(HufNode *huf_tree, unsigned char_kinds)
{
	unsigned int i;
	int cur, next, index;
	char *code_tmp = (char *)malloc(256*sizeof(char));		// 暂存编码，最多256个叶子，编码长度不超多255
	code_tmp[256-1] = '\0'; 
	printf("\n各个字符极其对应的编码为：\n");
	for(i = 0; i < char_kinds; ++i) 
	{
		index = 256-1;	// 编码临时空间索引初始化

		for(cur = i, next = huf_tree[i].parent; next != 0; 
			cur = next, next = huf_tree[next].parent)  
			if(huf_tree[next].lchild == cur) 
				code_tmp[--index] = '0';	// 左‘0’
			else 
				code_tmp[--index] = '1';	// 右‘1’
		huf_tree[i].code = (char *)malloc((256-index)*sizeof(char));			// 为第i个字符编码动态分配存储空间 
		strcpy(huf_tree[i].code, &code_tmp[index]);     // 正向保存编码到树结点相应域中
		printf("%c:%s\n",(char)huf_tree[i].uch,huf_tree[i].code);
	} 
	
	free(code_tmp);		// 释放编码临时空间
	
}

void HufCode1(HufNode *huf_tree,unsigned char_kinds)
{
	unsigned int p,i;
	int cdlen=0; 
	char *code_tmp = (char *)malloc(256*sizeof(char));		// 暂存编码，最多256个叶子，编码长度不超多255
	p=2*char_kinds-1;
	printf("\n从叶子到根编码——————各个字符极其对应的编码为：\n");
	for(i=0 ; i<p ; ++i)
	{
		huf_tree[i].weight = 0;
		printf("%d	",huf_tree[i].parent);
		}
	p=p-1;
	while(p>=0&&p<(2*char_kinds-1))
	{
		if(huf_tree[p].weight == 0)
		{
			huf_tree[p].weight = 1;
			if(huf_tree[p].lchild !=0){
				p=huf_tree[p].lchild;
				code_tmp[cdlen++] = '0';
			}
			else if(huf_tree[p].rchild == 0){
				huf_tree[p].code = (char *)malloc((cdlen+1)*sizeof(char));	
				code_tmp[cdlen]= '\0';
				strcpy(huf_tree[i].code,code_tmp);
				printf("%c:%s\n",(char)huf_tree[i].uch,huf_tree[i].code);
				printf("\n*****************\n");
			}
		}
		else if(huf_tree[p].weight == 1){
			huf_tree[p].weight = 2;
			if(huf_tree[p].rchild != 0){
				p = huf_tree[p].rchild;
				code_tmp[cdlen++] = '1';
			}
			else{
				huf_tree[p].weight = 0; 
				p = huf_tree[p].parent;
				--cdlen; 
			}
		}
	}
	free(code_tmp);
	
} 


// 压缩函数
int compress(char *ifname, char *ofname)
{
	unsigned char ch[1295];
	unsigned int i, j,b=0;
	unsigned int char_kinds;		// 字符种类
	unsigned char char_temp;		// 暂存8bits字符
	unsigned long file_len = 0;
	FILE *infile, *outfile;
	TmpNode node_temp;
	unsigned int node_num;
	HufTree huf_tree;
	char code_buf[256] = "\0";		// 待存编码缓冲区
	unsigned int code_len;
	/*
	** 动态分配256个结点，暂存字符频度，
	** 统计并拷贝到树结点后立即释放
	*/
	TmpNode *tmp_nodes =(TmpNode *)malloc(256*sizeof(TmpNode));		

	// 初始化暂存结点
	for(i = 0; i < 256; ++i)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].uch = (unsigned char)i;		// 数组的256个下标与256种字符对应
	}
	printf("文件内容为：\n");
	// 遍历文件，获取字符频度
	infile = fopen(ifname, "r");
	if (infile == NULL)
		return -1;
		//显示文件内容 
	for(b=0;b<1295;b++){
		fscanf(infile,"%c",&ch[b]);
		printf("%c",ch[b]);
		b++;
	}
	fclose(infile);

	infile = fopen(ifname, "rb");
	fread((char *)&char_temp, sizeof(unsigned char), 1, infile);		// 读入一个字符
	while(!feof(infile))
	{
		++tmp_nodes[char_temp].weight;		// 统计下标对应字符的权重，利用数组的随机访问快速统计字符频度
		++file_len;
		fread((char *)&char_temp, sizeof(unsigned char), 1, infile);		// 读入一个字符
	}
	fclose(infile);

	// 冒泡排序，将频度为零的放最后，剔除
	for(i = 0; i < 256-1; ++i)           
		for(j = i+1; j < 256; ++j)
			if(tmp_nodes[i].weight < tmp_nodes[j].weight)
			{
				node_temp = tmp_nodes[i];
				tmp_nodes[i] = tmp_nodes[j];
				tmp_nodes[j] = node_temp;
			}

	// 统计实际的字符种类（出现次数不为0）
	for(i = 0; i < 256; ++i)
		if(tmp_nodes[i].weight == 0) 
			break;
	char_kinds = i;

	if (char_kinds == 1)
	{
		outfile = fopen(ofname, "wb");					// 打开压缩后将生成的文件
		fwrite((char *)&char_kinds, sizeof(unsigned int), 1, outfile);		// 写入字符种类
		fwrite((char *)&tmp_nodes[0].uch, sizeof(unsigned char), 1, outfile);		// 写入唯一的字符
		fwrite((char *)&tmp_nodes[0].weight, sizeof(unsigned long), 1, outfile);		// 写入字符频度，也就是文件长度
		free(tmp_nodes);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_kinds - 1;		// 根据字符种类数，计算建立哈夫曼树所需结点数 
		huf_tree = (HufNode *)malloc(node_num*sizeof(HufNode));		// 动态建立哈夫曼树所需结点     

		// 初始化前char_kinds个结点
		for(i = 0; i < char_kinds; ++i) 
		{ 
			// 将暂存结点的字符和频度拷贝到树结点
			huf_tree[i].uch = tmp_nodes[i].uch; 
			huf_tree[i].weight = tmp_nodes[i].weight;
			huf_tree[i].parent = 0; 
		}	
		free(tmp_nodes); // 释放字符频度统计的暂存区

		// 初始化后node_num-char_kins个结点
		for(; i < node_num; ++i) 
			huf_tree[i].parent = 0; 

		CreateTree(huf_tree, char_kinds, node_num);		// 创建哈夫曼树

		HufCode(huf_tree, char_kinds);		// 生成哈夫曼编码

		// 写入字符和相应权重，供解压时重建哈夫曼树
		outfile = fopen(ofname, "wb");					// 打开压缩后将生成的文件
		fwrite((char *)&char_kinds, sizeof(unsigned int), 1, outfile);		// 写入字符种类
		for(i = 0; i < char_kinds; ++i)
		{
			fwrite((char *)&huf_tree[i].uch, sizeof(unsigned char), 1, outfile);			// 写入字符（已排序，读出后顺序不变）
			fwrite((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, outfile);		// 写入字符对应权重
		}

		// 紧接着字符和权重信息后面写入文件长度和字符编码
		fwrite((char *)&file_len, sizeof(unsigned long), 1, outfile);		// 写入文件长度
		infile = fopen(ifname, "rb") ;		// 以二进制形式打开待压缩的文件
		fread((char *)&char_temp, sizeof(unsigned char), 1, infile);     // 每次读取8bits
		printf("\n依次匹配，满八位就存入，直至最后，其过程编码变化为：\n");
		while(!feof(infile))
		{
			// 匹配字符对应编码
			for(i = 0; i < char_kinds; ++i)
				if(char_temp == huf_tree[i].uch)
					strcat(code_buf, huf_tree[i].code);
			printf("%s\n",code_buf);	//输出存进电脑的编码 

			// 以8位（一个字节长度）为处理单元 
			while(strlen(code_buf) >= 8)
			{
				char_temp = '\0';		// 清空字符暂存空间，改为暂存字符对应编码
				for(i = 0; i < 8; ++i)
				{
					char_temp <<= 1;		// 左移一位，为下一个bit腾出位置
					if(code_buf[i] == '1')
						char_temp |= 1;		// 当编码为"1"，通过或操作符将其添加到字节的最低位
				}
				fwrite((char *)&char_temp, sizeof(unsigned char), 1, outfile);		// 将字节对应编码存入文件
				strcpy(code_buf, code_buf+8);		// 编码缓存去除已处理的前八位
			}
			fread((char *)&char_temp, sizeof(unsigned char), 1, infile);     // 每次读取8bits
		}
		// 处理最后不足8bits编码
		code_len = strlen(code_buf);
		if(code_len > 0)
		{
			char_temp = '\0';		
			for(i = 0; i < code_len; ++i)
			{
				char_temp <<= 1;		
				if(code_buf[i] == '1')
					char_temp |= 1;
			}
			char_temp <<= 8-code_len;       // 将编码字段从尾部移到字节的高位
			fwrite((char *)&char_temp, sizeof(unsigned char), 1, outfile);       // 存入最后一个字节
		}

		// 关闭文件
		fclose(infile);
		fclose(outfile);

		// 释放内存
		for(i = 0; i < char_kinds; ++i)
			free(huf_tree[i].code);
		free(huf_tree);
	}
}//compress



// 解压函数
int extract(char *ifname, char *ofname)
{
	unsigned char ch[1295];
	unsigned int i,b=0;
	unsigned long file_len;
	unsigned long writen_len = 0;		// 控制文件写入长度
	FILE *infile, *outfile;
	unsigned int char_kinds;		// 存储字符种类
	unsigned int node_num;
	HufTree huf_tree;
	unsigned char code_temp;		// 暂存8bits编码
	unsigned int root;		// 保存根节点索引，供匹配编码使用

	infile = fopen(ifname, "rb");		// 以二进制方式打开压缩文件
	// 判断输入文件是否存在
	if (infile == NULL)
		return -1;

	// 读取压缩文件前端的字符及对应编码，用于重建哈夫曼树
	fread((char *)&char_kinds, sizeof(unsigned int), 1, infile);     // 读取字符种类数
	if (char_kinds == 1)
	{
		fread((char *)&code_temp, sizeof(unsigned char), 1, infile);     // 读取唯一的字符
		fread((char *)&file_len, sizeof(unsigned long), 1, infile);     // 读取文件长度
		outfile = fopen(ofname, "wb");					// 打开压缩后将生成的文件
		while (file_len--)
			fwrite((char *)&code_temp, sizeof(unsigned char), 1, outfile);	
		fclose(infile);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_kinds - 1;		// 根据字符种类数，计算建立哈夫曼树所需结点数 
		huf_tree = (HufNode *)malloc(node_num*sizeof(HufNode));		// 动态分配哈夫曼树结点空间
		// 读取字符及对应权重，存入哈夫曼树节点
		for(i = 0; i < char_kinds; ++i)     
		{
			fread((char *)&huf_tree[i].uch, sizeof(unsigned char), 1, infile);		// 读入字符
			fread((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, infile);	// 读入字符对应权重
			huf_tree[i].parent = 0;
		}
		// 初始化后node_num-char_kins个结点的parent
		for(; i < node_num; ++i) 
			huf_tree[i].parent = 0;

		CreateTree(huf_tree, char_kinds, node_num);		// 重建哈夫曼树（与压缩时的一致）

		// 读完字符和权重信息，紧接着读取文件长度和编码，进行解码
		fread((char *)&file_len, sizeof(unsigned long), 1, infile);	// 读入文件长度
		outfile = fopen(ofname, "wb");		// 打开压缩后将生成的文件
		root = node_num-1;
		while(1)
		{
			fread((char *)&code_temp, sizeof(unsigned char), 1, infile);		// 读取一个字符长度的编码

			// 处理读取的一个字符长度的编码（通常为8位）
			for(i = 0; i < 8; ++i)
			{
				// 由根向下直至叶节点正向匹配编码对应字符
				if(code_temp & 128)
					root = huf_tree[root].rchild;
				else
					root = huf_tree[root].lchild;

				if(root < char_kinds)
				{
					fwrite((char *)&huf_tree[root].uch, sizeof(unsigned char), 1, outfile);
					++writen_len;
					if (writen_len == file_len) break;		// 控制文件长度，跳出内层循环
					root = node_num-1;        // 复位为根索引，匹配下一个字符
				}
				code_temp <<= 1;		// 将编码缓存的下一位移到最高位，供匹配
			}
			if (writen_len == file_len) break;		// 控制文件长度，跳出外层循环
		}
		fclose(outfile);
		printf("\n解压后文件的内容为：\n"); 
		outfile = fopen(ofname, "r");
	// 判断输入文件是否存在
		if (outfile == NULL)
		return -1;
		//显示文件内容 
		for(b=0;b<1295;b++){
			fscanf(outfile,"%c",&ch[b]);
			printf("%c",ch[b]);
			b++;
		}
		
		// 关闭文件
		fclose(infile);
		fclose(outfile);

		// 释放内存
		free(huf_tree);
	}
}

int main()
{
	
	system("color 8f");
	while(1)
	{
		
		int opt, flag  = 0;		// 每次进入循环都要初始化flag为0
		char ifname[256], ofname[256],buffer[256],of1name[256];		// 保存输入输出文件名
		strcpy(ifname,"D:\哈夫曼.txt");
		strcpy(ofname,"D:\压缩文件.txt");
		strcpy(of1name,"D:\哈夫曼一.txt");
		printf("请选择你要进行的操作:\n 1: 压缩\n 2: 解压\n 3: 退出\n");
		scanf("%d", &opt);
		if (opt == 3)
			break;
	
		switch(opt)
		{
		case 1:	
				fflush(stdin);		// 清空标准输入流，防止干扰gets函数读取文件名
				printf("要压缩文件为：哈夫曼.txt\n");
	
				printf("压缩后的文件为：压缩文件.txt\n");
				
					flag = compress(ifname, ofname);	// 压缩，返回值用于判断是否文件名不存在	
	
				break;		
		case 2:

		
				printf("解压的文件为：压缩文件.txt\n");
				
				printf("解压后的文件为：哈夫曼一.txt\n");

				flag = extract(ofname, of1name);	
			break;
				
		}
		if (flag == -1)
			printf("出现错误！\n");		// 如果标志为‘-1’则输入文件不存在
		else
			printf("操作完成!\n");	
	}

	return 0;
}

