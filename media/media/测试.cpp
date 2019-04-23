# include <stdio.h>
# include <string.h>
# include <stdlib.h>
struct SNode {
	long int freq; // 频率
	int depth; // 深度，编码长度
	char gray;
	struct SNode * pPar; // 父结点 
	struct SNode * pLeft; // 左结点
	struct SNode * pRight; // 右结点
	char * code; // 字节编码

};
struct SNode nodes[256]; // 各字节编码
struct SNode tree[520];
struct SNode* head = 0; // 编码临时队列，指向第一位的指针
struct SNode* temp = 0; // 临时指针，用于解码时，指向当前已走到的结点
int m_iCodeNum; // 已经编码的个数
int m_iRead;
long int imcodenum = 0;
//////////////对结点数组进行初始化///////////////////
void init(struct SNode*p, int n) {
	int i;
	for (i = 0; i < n; i++) {
		p->code = NULL;
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
	nodes[n].freq++;//nodes第n个元素存储的为灰度值为n对应的结点
}
/////////将每个灰度值根据其频数插入大临时链表中，由小到大//////////////
void addtolist(struct SNode* pNode)
{
	if (head == NULL)  				// 队列为空，		

	{
		head = pNode;// 直接加入头部
	}
	else
	{
		if (pNode->freq < head->freq)//当新加入的结点的频数小于head结点的频数时，将新结点放到队首
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
/////////// 增加结点儿子编码长度//////////////////////////
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
	unsigned char b[1024];//做缓冲用
	char * buf, *test;
	int n = 0, len = 0, j = 0;
	int i = 0;
	int codelens = 0;//所有编码的总长度。
	FILE * fpin, *fpout;
	init(nodes, 256);//将每一个结点初始化
	init(tree, 511);
	//for(i=0;i<256;i++){
		//printf("gray is %d\n",nodes[i].gray);
	//}
	if ((fpin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
	}//打开图像文件
	fseek(fpin, 54, 0);//前54字节的文件为文件头和图像信息需要跳过
	len = fread(b, sizeof(char), 1024, fpin);//循环读出图像的数据部分
	while (len > 0)
	{

		for (i = 0; i < len; i++)
		{
			freqadd(b[i]);//统计该灰度值的频数
		}
		len = fread(b, sizeof(char), 1024, fpin);
	}
	///根据灰度值的频数从小到大进行排序///////
	for (i = 0, len = 0; i < 256; i++) {//将个结点加入临时队列，并从小到大排序
		if (nodes[i].freq > 0) {
			addtolist(&nodes[i]);
			nodes[i].depth++;
			// printf("%d\n",nodes[i].freq);
			len++;
			tree[i] = nodes[i];
		}

	}
	createcodetree(tree, len);//建立huffman树，并进行编码

	codelens = getallcodelen();//得到所有元素编码后总的编码长度
	//printf("%d\n",codelens);
	buf = getcode(codelens);//将所有的编码暂存到buf缓冲里
	printf("-----------------------------每个灰度值所对应的编码---------------------------\n");
	for (n = 0; n < 256; n++) {
		printf("%d:%s\n", n, nodes[n].code);
	}
	printf("\n\n");
	printf("图像编码正在进行中.....\n\n");

	if ((fpout = fopen("huffman.txt", "wb+")) == NULL) {
		printf("file open error");
		exit(1);
	}
	fseek(fpin, 54, 0);
	len = fread(b, sizeof(char), 1024, fpin);//循环读出图像的数据部分
	while (len > 0) {
		for (i = 0; i < len; i++) {
			n = b[i];
			for (j = 0; j < (nodes[n].depth - 1); j++) {

				fwrite(&nodes[n].code[j], sizeof(char), 1, fpout);
				//imcodenum++;
			}
		}
		len = fread(b, sizeof(char), 1024, fpin);
	}
	//printf("imcodenum=%ld\n",imcodenum);
	printf("图像编码完成\n\n");
	fclose(fpin);//关闭文件指针
	fclose(fpout);

}
////////////////////解码////////////////////////
void decode() {
	FILE * hufin, *hufout, *imgin;
	struct SNode *p;
	int i = 0, n = 0, num = 0;
	char b;
	char debuf[507132];
	//debuf=(char *)malloc(sizeof(char)*(len+1));//分配所需的内存
	//debuf[len]='\0'; 
	if ((imgin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
		exit(1);
	}

	if ((hufout = fopen("decode.bmp", "wb+")) == NULL) {//打开存储解码结果的文件
		printf("file open error");
		exit(1);
	}

	if ((hufin = fopen("huffman.txt", "rt+")) == NULL) {//打开存储编码结果的文件
		printf("huffman.txt open error\n");
		exit(1);
	}
	fread(debuf, sizeof(char), 54, imgin);//读出原始图像的文件头，图像信息放到b中，因为文件头没有进行编码所以无需解码直接拷贝
	fwrite(debuf, sizeof(char), 54, hufout);//将文件头，图像信息放到解码后的文件中
	//n=len+1;
	fgets(debuf, 507132, hufin);  //从文件中读出所有的编码序列，只读n-1个第，并自动使b【n】为‘\0’
	p = head;                //每次解码都要从树的根节点开始
	for (i = 0; debuf[i] != '\0'; i++) {//开始解码
		switch (debuf[i]) {
		case '0': {
			if (p->pLeft != NULL) {//如果编码为0并且左孩子不为空，那么p指向p的左孩子。
				p = p->pLeft;
				break;
			}
			else {              //如果左孩子为空，则此结点为叶子结点，取出其灰度值作为解码结果
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);

				p = head;
				i = i - 1;//此时i指向下一个码元的开始位置，但是for循环还有一个i++所以此时需要将i减一
				break;
			}

		}
		case '1': {
			if (p->pRight != NULL) {//如果编码为1并且右孩子孩子不为空，那么p指向p的右孩子。
				p = p->pRight;
				break;
			}

			else {//如果右孩子孩子为空，则此结点为叶子结点，取出其灰度值作为解码结果
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
	b = p->gray;//上面的switch语句未能统计左后一个解码结果，所以在此统计一下
	fwrite(&b, sizeof(char), 1, hufout);
}


int main() {
	char ans = 0;
	huffmancode();
	printf("需要解码么？y(解码)/n(不解码)\n");
	scanf("%c", &ans);
	switch (ans) {
	case 'y':printf("解码正在进行中......\n\n");
		decode();//解码
		printf("解码已完成，请到源文件所在目录查看结果\n\n");
		break;
	default:break;
	}
	return 0;

}














