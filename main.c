#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


// ͳ���ַ�Ƶ�ȵ���ʱ���
typedef struct {
	unsigned char uch;			// ��8bitsΪ��Ԫ���޷����ַ�
	unsigned long weight;		// ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ��
} TmpNode;

// �����������
typedef struct {
	unsigned char uch;				// ��8bitsΪ��Ԫ���޷����ַ�
	unsigned long weight;			// ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ��
	char *code;						// �ַ���Ӧ�Ĺ��������루��̬����洢�ռ䣩
	int parent, lchild, rchild;		// ����˫�׺����Һ���
} HufNode, *HufTree;

// ѡ����С�ʹ�С��������㣬����������������
void select(HufNode *huf_tree, unsigned int n, int *s1, int *s2)
{
	// ����С
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for(i = 0; i < n; ++i)           
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
		huf_tree[*s1].parent=1;   // ��Ǵ˽���ѱ�ѡ��

	// �Ҵ�С
	min=ULONG_MAX;
	for(i = 0; i < n; ++i)            
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
} 

// ������������
void CreateTree(HufNode *huf_tree, unsigned int char_kinds, unsigned int node_num)
{
	unsigned int i;
	int s1, s2;
	printf("\n��������Ϊ��\nparent rchild	lchild	weight\n");
	for(i = char_kinds; i < node_num; ++i)  
	{ 
		select(huf_tree, i, &s1, &s2);		// ѡ����С���������
		huf_tree[s1].parent = huf_tree[s2].parent = i; 
		huf_tree[i].lchild = s1; 
		huf_tree[i].rchild = s2; 
		huf_tree[i].weight = huf_tree[s1].weight + huf_tree[s2].weight; 
	} 
	for(i = 0; i < node_num; ++i) 
		printf("%d	%d	%d	%d\n",huf_tree[i].parent,huf_tree[i].rchild,huf_tree[i].lchild,huf_tree[i].weight);
}

// ���ɹ���������,	��Ҷ�����������������
void HufCode(HufNode *huf_tree, unsigned char_kinds)
{
	unsigned int i;
	int cur, next, index;
	char *code_tmp = (char *)malloc(256*sizeof(char));		// �ݴ���룬���256��Ҷ�ӣ����볤�Ȳ�����255
	code_tmp[256-1] = '\0'; 
	printf("\n�����ַ������Ӧ�ı���Ϊ��\n");
	for(i = 0; i < char_kinds; ++i) 
	{
		index = 256-1;	// ������ʱ�ռ�������ʼ��

		for(cur = i, next = huf_tree[i].parent; next != 0; 
			cur = next, next = huf_tree[next].parent)  
			if(huf_tree[next].lchild == cur) 
				code_tmp[--index] = '0';	// ��0��
			else 
				code_tmp[--index] = '1';	// �ҡ�1��
		huf_tree[i].code = (char *)malloc((256-index)*sizeof(char));			// Ϊ��i���ַ����붯̬����洢�ռ� 
		strcpy(huf_tree[i].code, &code_tmp[index]);     // ���򱣴���뵽�������Ӧ����
		printf("%c:%s\n",(char)huf_tree[i].uch,huf_tree[i].code);
	} 
	
	free(code_tmp);		// �ͷű�����ʱ�ռ�
	
}

void HufCode1(HufNode *huf_tree,unsigned char_kinds)
{
	unsigned int p,i;
	int cdlen=0; 
	char *code_tmp = (char *)malloc(256*sizeof(char));		// �ݴ���룬���256��Ҷ�ӣ����볤�Ȳ�����255
	p=2*char_kinds-1;
	printf("\n��Ҷ�ӵ������롪���������������ַ������Ӧ�ı���Ϊ��\n");
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


// ѹ������
int compress(char *ifname, char *ofname)
{
	unsigned char ch[1295];
	unsigned int i, j,b=0;
	unsigned int char_kinds;		// �ַ�����
	unsigned char char_temp;		// �ݴ�8bits�ַ�
	unsigned long file_len = 0;
	FILE *infile, *outfile;
	TmpNode node_temp;
	unsigned int node_num;
	HufTree huf_tree;
	char code_buf[256] = "\0";		// ������뻺����
	unsigned int code_len;
	/*
	** ��̬����256����㣬�ݴ��ַ�Ƶ�ȣ�
	** ͳ�Ʋ������������������ͷ�
	*/
	TmpNode *tmp_nodes =(TmpNode *)malloc(256*sizeof(TmpNode));		

	// ��ʼ���ݴ���
	for(i = 0; i < 256; ++i)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].uch = (unsigned char)i;		// �����256���±���256���ַ���Ӧ
	}
	printf("�ļ�����Ϊ��\n");
	// �����ļ�����ȡ�ַ�Ƶ��
	infile = fopen(ifname, "r");
	if (infile == NULL)
		return -1;
		//��ʾ�ļ����� 
	for(b=0;b<1295;b++){
		fscanf(infile,"%c",&ch[b]);
		printf("%c",ch[b]);
		b++;
	}
	fclose(infile);

	infile = fopen(ifname, "rb");
	fread((char *)&char_temp, sizeof(unsigned char), 1, infile);		// ����һ���ַ�
	while(!feof(infile))
	{
		++tmp_nodes[char_temp].weight;		// ͳ���±��Ӧ�ַ���Ȩ�أ����������������ʿ���ͳ���ַ�Ƶ��
		++file_len;
		fread((char *)&char_temp, sizeof(unsigned char), 1, infile);		// ����һ���ַ�
	}
	fclose(infile);

	// ð�����򣬽�Ƶ��Ϊ��ķ�����޳�
	for(i = 0; i < 256-1; ++i)           
		for(j = i+1; j < 256; ++j)
			if(tmp_nodes[i].weight < tmp_nodes[j].weight)
			{
				node_temp = tmp_nodes[i];
				tmp_nodes[i] = tmp_nodes[j];
				tmp_nodes[j] = node_temp;
			}

	// ͳ��ʵ�ʵ��ַ����ࣨ���ִ�����Ϊ0��
	for(i = 0; i < 256; ++i)
		if(tmp_nodes[i].weight == 0) 
			break;
	char_kinds = i;

	if (char_kinds == 1)
	{
		outfile = fopen(ofname, "wb");					// ��ѹ�������ɵ��ļ�
		fwrite((char *)&char_kinds, sizeof(unsigned int), 1, outfile);		// д���ַ�����
		fwrite((char *)&tmp_nodes[0].uch, sizeof(unsigned char), 1, outfile);		// д��Ψһ���ַ�
		fwrite((char *)&tmp_nodes[0].weight, sizeof(unsigned long), 1, outfile);		// д���ַ�Ƶ�ȣ�Ҳ�����ļ�����
		free(tmp_nodes);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_kinds - 1;		// �����ַ������������㽨������������������ 
		huf_tree = (HufNode *)malloc(node_num*sizeof(HufNode));		// ��̬������������������     

		// ��ʼ��ǰchar_kinds�����
		for(i = 0; i < char_kinds; ++i) 
		{ 
			// ���ݴ�����ַ���Ƶ�ȿ����������
			huf_tree[i].uch = tmp_nodes[i].uch; 
			huf_tree[i].weight = tmp_nodes[i].weight;
			huf_tree[i].parent = 0; 
		}	
		free(tmp_nodes); // �ͷ��ַ�Ƶ��ͳ�Ƶ��ݴ���

		// ��ʼ����node_num-char_kins�����
		for(; i < node_num; ++i) 
			huf_tree[i].parent = 0; 

		CreateTree(huf_tree, char_kinds, node_num);		// ������������

		HufCode(huf_tree, char_kinds);		// ���ɹ���������

		// д���ַ�����ӦȨ�أ�����ѹʱ�ؽ���������
		outfile = fopen(ofname, "wb");					// ��ѹ�������ɵ��ļ�
		fwrite((char *)&char_kinds, sizeof(unsigned int), 1, outfile);		// д���ַ�����
		for(i = 0; i < char_kinds; ++i)
		{
			fwrite((char *)&huf_tree[i].uch, sizeof(unsigned char), 1, outfile);			// д���ַ��������򣬶�����˳�򲻱䣩
			fwrite((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, outfile);		// д���ַ���ӦȨ��
		}

		// �������ַ���Ȩ����Ϣ����д���ļ����Ⱥ��ַ�����
		fwrite((char *)&file_len, sizeof(unsigned long), 1, outfile);		// д���ļ�����
		infile = fopen(ifname, "rb") ;		// �Զ�������ʽ�򿪴�ѹ�����ļ�
		fread((char *)&char_temp, sizeof(unsigned char), 1, infile);     // ÿ�ζ�ȡ8bits
		printf("\n����ƥ�䣬����λ�ʹ��룬ֱ���������̱���仯Ϊ��\n");
		while(!feof(infile))
		{
			// ƥ���ַ���Ӧ����
			for(i = 0; i < char_kinds; ++i)
				if(char_temp == huf_tree[i].uch)
					strcat(code_buf, huf_tree[i].code);
			printf("%s\n",code_buf);	//���������Եı��� 

			// ��8λ��һ���ֽڳ��ȣ�Ϊ����Ԫ 
			while(strlen(code_buf) >= 8)
			{
				char_temp = '\0';		// ����ַ��ݴ�ռ䣬��Ϊ�ݴ��ַ���Ӧ����
				for(i = 0; i < 8; ++i)
				{
					char_temp <<= 1;		// ����һλ��Ϊ��һ��bit�ڳ�λ��
					if(code_buf[i] == '1')
						char_temp |= 1;		// ������Ϊ"1"��ͨ���������������ӵ��ֽڵ����λ
				}
				fwrite((char *)&char_temp, sizeof(unsigned char), 1, outfile);		// ���ֽڶ�Ӧ��������ļ�
				strcpy(code_buf, code_buf+8);		// ���뻺��ȥ���Ѵ����ǰ��λ
			}
			fread((char *)&char_temp, sizeof(unsigned char), 1, infile);     // ÿ�ζ�ȡ8bits
		}
		// ���������8bits����
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
			char_temp <<= 8-code_len;       // �������ֶδ�β���Ƶ��ֽڵĸ�λ
			fwrite((char *)&char_temp, sizeof(unsigned char), 1, outfile);       // �������һ���ֽ�
		}

		// �ر��ļ�
		fclose(infile);
		fclose(outfile);

		// �ͷ��ڴ�
		for(i = 0; i < char_kinds; ++i)
			free(huf_tree[i].code);
		free(huf_tree);
	}
}//compress



// ��ѹ����
int extract(char *ifname, char *ofname)
{
	unsigned char ch[1295];
	unsigned int i,b=0;
	unsigned long file_len;
	unsigned long writen_len = 0;		// �����ļ�д�볤��
	FILE *infile, *outfile;
	unsigned int char_kinds;		// �洢�ַ�����
	unsigned int node_num;
	HufTree huf_tree;
	unsigned char code_temp;		// �ݴ�8bits����
	unsigned int root;		// ������ڵ���������ƥ�����ʹ��

	infile = fopen(ifname, "rb");		// �Զ����Ʒ�ʽ��ѹ���ļ�
	// �ж������ļ��Ƿ����
	if (infile == NULL)
		return -1;

	// ��ȡѹ���ļ�ǰ�˵��ַ�����Ӧ���룬�����ؽ���������
	fread((char *)&char_kinds, sizeof(unsigned int), 1, infile);     // ��ȡ�ַ�������
	if (char_kinds == 1)
	{
		fread((char *)&code_temp, sizeof(unsigned char), 1, infile);     // ��ȡΨһ���ַ�
		fread((char *)&file_len, sizeof(unsigned long), 1, infile);     // ��ȡ�ļ�����
		outfile = fopen(ofname, "wb");					// ��ѹ�������ɵ��ļ�
		while (file_len--)
			fwrite((char *)&code_temp, sizeof(unsigned char), 1, outfile);	
		fclose(infile);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_kinds - 1;		// �����ַ������������㽨������������������ 
		huf_tree = (HufNode *)malloc(node_num*sizeof(HufNode));		// ��̬��������������ռ�
		// ��ȡ�ַ�����ӦȨ�أ�������������ڵ�
		for(i = 0; i < char_kinds; ++i)     
		{
			fread((char *)&huf_tree[i].uch, sizeof(unsigned char), 1, infile);		// �����ַ�
			fread((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, infile);	// �����ַ���ӦȨ��
			huf_tree[i].parent = 0;
		}
		// ��ʼ����node_num-char_kins������parent
		for(; i < node_num; ++i) 
			huf_tree[i].parent = 0;

		CreateTree(huf_tree, char_kinds, node_num);		// �ؽ�������������ѹ��ʱ��һ�£�

		// �����ַ���Ȩ����Ϣ�������Ŷ�ȡ�ļ����Ⱥͱ��룬���н���
		fread((char *)&file_len, sizeof(unsigned long), 1, infile);	// �����ļ�����
		outfile = fopen(ofname, "wb");		// ��ѹ�������ɵ��ļ�
		root = node_num-1;
		while(1)
		{
			fread((char *)&code_temp, sizeof(unsigned char), 1, infile);		// ��ȡһ���ַ����ȵı���

			// �����ȡ��һ���ַ����ȵı��루ͨ��Ϊ8λ��
			for(i = 0; i < 8; ++i)
			{
				// �ɸ�����ֱ��Ҷ�ڵ�����ƥ������Ӧ�ַ�
				if(code_temp & 128)
					root = huf_tree[root].rchild;
				else
					root = huf_tree[root].lchild;

				if(root < char_kinds)
				{
					fwrite((char *)&huf_tree[root].uch, sizeof(unsigned char), 1, outfile);
					++writen_len;
					if (writen_len == file_len) break;		// �����ļ����ȣ������ڲ�ѭ��
					root = node_num-1;        // ��λΪ��������ƥ����һ���ַ�
				}
				code_temp <<= 1;		// �����뻺�����һλ�Ƶ����λ����ƥ��
			}
			if (writen_len == file_len) break;		// �����ļ����ȣ��������ѭ��
		}
		fclose(outfile);
		printf("\n��ѹ���ļ�������Ϊ��\n"); 
		outfile = fopen(ofname, "r");
	// �ж������ļ��Ƿ����
		if (outfile == NULL)
		return -1;
		//��ʾ�ļ����� 
		for(b=0;b<1295;b++){
			fscanf(outfile,"%c",&ch[b]);
			printf("%c",ch[b]);
			b++;
		}
		
		// �ر��ļ�
		fclose(infile);
		fclose(outfile);

		// �ͷ��ڴ�
		free(huf_tree);
	}
}

int main()
{
	
	system("color 8f");
	while(1)
	{
		
		int opt, flag  = 0;		// ÿ�ν���ѭ����Ҫ��ʼ��flagΪ0
		char ifname[256], ofname[256],buffer[256],of1name[256];		// ������������ļ���
		strcpy(ifname,"D:\������.txt");
		strcpy(ofname,"D:\ѹ���ļ�.txt");
		strcpy(of1name,"D:\������һ.txt");
		printf("��ѡ����Ҫ���еĲ���:\n 1: ѹ��\n 2: ��ѹ\n 3: �˳�\n");
		scanf("%d", &opt);
		if (opt == 3)
			break;
	
		switch(opt)
		{
		case 1:	
				fflush(stdin);		// ��ձ�׼����������ֹ����gets������ȡ�ļ���
				printf("Ҫѹ���ļ�Ϊ��������.txt\n");
	
				printf("ѹ������ļ�Ϊ��ѹ���ļ�.txt\n");
				
					flag = compress(ifname, ofname);	// ѹ��������ֵ�����ж��Ƿ��ļ���������	
	
				break;		
		case 2:

		
				printf("��ѹ���ļ�Ϊ��ѹ���ļ�.txt\n");
				
				printf("��ѹ����ļ�Ϊ��������һ.txt\n");

				flag = extract(ofname, of1name);	
			break;
				
		}
		if (flag == -1)
			printf("���ִ���\n");		// �����־Ϊ��-1���������ļ�������
		else
			printf("�������!\n");	
	}

	return 0;
}

