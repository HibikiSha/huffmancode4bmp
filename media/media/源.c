# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <math.h>
#pragma warning(disable:4996)
struct SNode {
	long int freq; // 频率
	int depth; // 深度，编码长度
	unsigned char gray;
	struct SNode * pPar; 
	struct SNode * pLeft;
	struct SNode * pRight; 
	char * code; // 字节编码

};
struct SNode nodes[256]; // 各字节编码
struct SNode tree[520];
struct SNode* head = 0; // 编码临时队列，指向第一位的指针
struct SNode* temp = 0; // 临时指针，用于解码时，指向当前已走到的结点
int m_iCodeNum; // 已经编码的个数
int m_iRead;
long int imcodenum = 0;
double efficiency;
double avglen;
int totallens;
//////////////初始化函数///////////////////
void init(struct SNode*p, int n) {
	int i;
	for (i = 0; i < n; i++) {
		p->code = "";
		p->freq = 0;
		p->pLeft = NULL;
		p->pPar = NULL;
		p->pRight = NULL;
		p->depth = 0;
		p->gray = i;
		p = p + 1;
	}
}
///////////统计每个灰度值的频数////////////////////
void freqadd(unsigned char n)
{
	nodes[n].freq++;
}
//////////////计算平均编码长度及编码效率///////////
void geteffavg(){
	int sumfreq=0;
	avglen = 0;
	double xinyuanshang = 0;
	for(int i = 0; i<256;i++){
		sumfreq = sumfreq + nodes[i].freq;
	}
	for(int i = 0; i < 256;i++){
		avglen += (nodes[i].depth-1) * (double)nodes[i].freq / (double)sumfreq;
			}
	efficiency = avglen/8;
	printf("编码的平均码长为%lf\n",avglen);
	printf("编码效率为%lf\n",efficiency);
	
}
/////////灰度值升序创建临时链表//////////////
void addtolist(struct SNode* pNode)
{
	if (head == NULL)  				// 队列为空，		

	{
		head = pNode;// 直接加入头部
	}
	else
	{
		if (pNode->freq < head->freq)//当新加入=head结点的频数时，将新结点放到队首
		{
			pNode->pPar = head;
			head = pNode;
		}
		else
		{
			struct	SNode* p = head;
			while ((p->pPar != 0) && (p->pPar->freq < pNode->freq))
			{
				p = p->pPar;
			}
			pNode->pPar = p->pPar;
			p->pPar = pNode;
		}
	}
}
//////////////////生成编码//////////////////////////////
void createcode(struct SNode * snode) {
	struct	SNode* p = snode;
	int post = 0;
	if (snode->depth == 1) {//临时队列中只有一个元素
		snode->code = (char *)malloc(sizeof(char));
		snode->code[0] = '0';
	}
	else {
		snode->code = (char *)malloc(sizeof(char)*snode->depth);//为编码结果分配存储空间
		post = snode->depth - 1;
		snode->code[post] = '\0';
		post--;
		while (p->pPar != NULL) {
			if (p == p->pPar->pLeft) // 该结点为其父节点的左孩子
			{
				snode->code[post] = '0';
			}
			else
			{
				snode->code[post] = '1';//该结点为其父节点的右孩子
			}
			post--;
			p = p->pPar;
		}
	}
}
/////////// 增加结点儿子节点的编码长度//////////////////////////
void AddChildLen(struct SNode* pNode)
{
	pNode->depth++;
	if (pNode->pLeft != NULL)
	{
		AddChildLen(pNode->pLeft);
	}
	if (pNode->pRight != NULL)
	{
		AddChildLen(pNode->pRight);
	}
}
/////////////////构建编码huffman树/////////////////
void createcodetree(struct SNode *ptree, int len) {
	int offset, i = 0;

	while (head != NULL && head->pPar != NULL)
	{
		// 取出前两个频数最小结点
		struct SNode *p1 = head;
		struct SNode *p2 = head->pPar;
		head = p2->pPar;
		offset = len + i;
		ptree[offset].freq = p1->freq + p2->freq; // 频率为两者之和
		ptree[offset].pLeft = p1;
		ptree[offset].pRight = p2;
		p1->pPar = &ptree[offset];
		p2->pPar = &ptree[offset];
		AddChildLen(&ptree[offset]);//调整树中每个结点的深度
		addtolist(&ptree[offset]);//将新产生的结点，放到临时队列中
		i++;
	}
	// 构建树完成，生成编码
	for (i = 0; i < 256; i++)
	{
		if (nodes[i].freq > 0)
		{
			createcode(&nodes[i]);
		}
	}


}
/////////////////////////////////返回编码后的所有码元的长度和////////////////////////////
int getallcodelen() {
	int alllen = 0, i = 0;
	for (; i < 256; i++) {
		alllen = alllen + nodes[i].depth - 1;
	}
	return alllen;
}
////////////////////将所有的结点的编码放到codebuf缓冲中/////////////////////////
char * getcode(int len) {
	char *codebuf;
	int i, offset = 0;
	codebuf = (char *)malloc(sizeof(char)*(len + 1));
	codebuf[len] = '\0';
	for (i = 0; i < 256; i++) {
		
			strcpy(codebuf + offset, nodes[i].code);
			offset += (nodes[i].depth - 1);
		
	}

	return codebuf;
}
////////////////////////////编码//////////////////////////////////
void huffmancode() {
	struct SNode *p;//中间指针
	unsigned char b[512];//做缓冲用
	char * buf, *test;
	int n = 0, len = 0, j = 0;
	int i = 0;
	int codelens = 0;//所有编码的总长度。
	int count = 0;
	FILE * fpin, *fpout;
	init(nodes, 256);//将每一个结点初始化
	init(tree, 511);
	fpin = fopen("lena.bmp", "rb");
	fseek(fpin, 1078, 0);//跳过前54字节
	len = fread(b, sizeof(char), 512, fpin);//循环读出图像的数据部分
	while (len > 0)
	{
		for (i = 0; i < len; i++)
		{
			freqadd(b[i]);//统计该灰度值的频数
		}
		len = fread(b, sizeof(char), 512, fpin);
		count++;
	}
	printf("读取次数为%d\n", count);
	for (i = 0, len = 0; i < 256; i++) {//将个结点加入临时队列，并从小到大排序
		if (nodes[i].freq > 0) {
			addtolist(&nodes[i]);
			nodes[i].depth++;
			len++;
			tree[i] = nodes[i];
		}

	}
	createcodetree(tree, len);//建立huffman树，并进行编码
	codelens = getallcodelen();//得到所有元素编码后总的编码长度
	buf = getcode(codelens);//将所有的编码暂存到buf缓冲里
	printf("每个灰度值所对应的编码如下所示\n");
	for (n = 0; n < 256; n++) {
		printf("%d:%s\n", n, nodes[n].code);
	}
	printf("\n\n");

	fpout = fopen("huffman.txt", "wb+");
	count = 0;
	fseek(fpin, 1078, 0);
	len = fread(b, sizeof(char), 512, fpin);//循环读出图像的数据部分
	while (len > 0) {
		for (i = 0; i < len; i++) {
			n = b[i];
			for (j = 0; j < (nodes[n].depth - 1); j++) {

				fwrite(&nodes[n].code[j], sizeof(char), 1, fpout);
			}
		}
		len = fread(b, sizeof(char), 512, fpin);

	}
	fclose(fpin);//关闭文件指针
	fclose(fpout);

}
////////////////////解码////////////////////////
void decode() {
	FILE * hufin, *hufout, *imgin;
	struct SNode *p;
	int i = 0, n = 0, num = 0;
	char b;
	char debuf[1968288];

	imgin = fopen("lena.bmp", "rb");
	hufout = fopen("decode.bmp", "wb+");
	hufin = fopen("huffman.txt", "rt+");
	fread(debuf, sizeof(char), 1078, imgin);//从源文件获取文件头
	fwrite(debuf, sizeof(char), 1078, hufout);//写入文件头
	hufin = fopen("huffman.txt", "rt+");
	fgets(debuf, 1968288, hufin);  //读出所有的编码序列
	p = head;                
	for (i = 0; debuf[i] != '\0'; i++) {//开始解码
		switch (debuf[i]) {
		case '0': {
			if (p->pLeft != NULL) {//编码为0并且左孩子不为空，指向左孩子。
				p = p->pLeft;
				break;
			}
			else {              //左孩子为空，为叶子结点，取出其灰度值
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);

				p = head;
				i = i - 1;//因为有i++
				break;
			}

		}
		case '1': {
			if (p->pRight != NULL) {//编码为1并且右孩子不为空，指向右孩子
				p = p->pRight;
				break;
			}

			else {//右孩子为空，为叶子结点，取出其灰度值
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);
				p = head;
				i = i - 1;
				break;
			}
		}
		default: break;
		}
	}
	b = p->gray;//获得最后一个编码结果
	fwrite(&b, sizeof(char), 1, hufout);
}


void main() {
	char ans = 0;
	huffmancode();
	geteffavg();
	printf("输入d开始解码");
	scanf("%c", &ans);
	switch (ans) {
	case 'd':
		decode();
		printf("解码完成");
		break;
	default:break;
	}


}














