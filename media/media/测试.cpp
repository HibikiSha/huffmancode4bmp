# include <stdio.h>
# include <string.h>
# include <stdlib.h>
struct SNode {
	long int freq; // Ƶ��
	int depth; // ��ȣ����볤��
	char gray;
	struct SNode * pPar; // ����� 
	struct SNode * pLeft; // ����
	struct SNode * pRight; // �ҽ��
	char * code; // �ֽڱ���

};
struct SNode nodes[256]; // ���ֽڱ���
struct SNode tree[520];
struct SNode* head = 0; // ������ʱ���У�ָ���һλ��ָ��
struct SNode* temp = 0; // ��ʱָ�룬���ڽ���ʱ��ָ��ǰ���ߵ��Ľ��
int m_iCodeNum; // �Ѿ�����ĸ���
int m_iRead;
long int imcodenum = 0;
//////////////�Խ��������г�ʼ��///////////////////
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
///////////ͳ��ÿ���Ҷ�ֵ��Ƶ��////////////////////
void freqadd(unsigned char n)
{
	nodes[n].freq++;//nodes��n��Ԫ�ش洢��Ϊ�Ҷ�ֵΪn��Ӧ�Ľ��
}
/////////��ÿ���Ҷ�ֵ������Ƶ���������ʱ�����У���С����//////////////
void addtolist(struct SNode* pNode)
{
	if (head == NULL)  				// ����Ϊ�գ�		

	{
		head = pNode;// ֱ�Ӽ���ͷ��
	}
	else
	{
		if (pNode->freq < head->freq)//���¼���Ľ���Ƶ��С��head����Ƶ��ʱ�����½��ŵ�����
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
//////////////////���ɱ���//////////////////////////////
void createcode(struct SNode * snode) {
	struct	SNode* p = snode;
	int post = 0;
	if (snode->depth == 1) {//��ʱ������ֻ��һ��Ԫ��
		snode->code = (char *)malloc(sizeof(char));
		snode->code[0] = '0';
	}
	else {
		snode->code = (char *)malloc(sizeof(char)*snode->depth);//Ϊ����������洢�ռ�
		post = snode->depth - 1;
		snode->code[post] = '\0';
		post--;
		while (p->pPar != NULL) {
			if (p == p->pPar->pLeft) // �ý��Ϊ�丸�ڵ������
			{
				snode->code[post] = '0';
			}
			else
			{
				snode->code[post] = '1';//�ý��Ϊ�丸�ڵ���Һ���
			}
			post--;
			p = p->pPar;
		}
	}
}
/////////// ���ӽ����ӱ��볤��//////////////////////////
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
/////////////////��������huffman��/////////////////
void createcodetree(struct SNode *ptree, int len) {
	int offset, i = 0;

	while (head != NULL && head->pPar != NULL)
	{
		// ȡ��ǰ����Ƶ����С���
		struct SNode *p1 = head;
		struct SNode *p2 = head->pPar;
		head = p2->pPar;


		offset = len + i;
		ptree[offset].freq = p1->freq + p2->freq; // Ƶ��Ϊ����֮��
		ptree[offset].pLeft = p1;
		ptree[offset].pRight = p2;
		p1->pPar = &ptree[offset];
		p2->pPar = &ptree[offset];
		AddChildLen(&ptree[offset]);//��������ÿ���������
		addtolist(&ptree[offset]);//���²����Ľ�㣬�ŵ���ʱ������
		i++;
	}
	// ��������ɣ����ɱ���
	for (i = 0; i < 256; i++)
	{
		if (nodes[i].freq > 0)
		{
			createcode(&nodes[i]);
		}
	}


}
/////////////////////////////////���ر�����������Ԫ�ĳ��Ⱥ�////////////////////////////
int getallcodelen() {
	int alllen = 0, i = 0;
	for (; i < 256; i++) {
		alllen = alllen + nodes[i].depth - 1;
	}
	return alllen;
}
////////////////////�����еĽ��ı���ŵ�codebuf������/////////////////////////
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
////////////////////////////����//////////////////////////////////
void huffmancode() {
	struct SNode *p;//�м�ָ��
	unsigned char b[1024];//��������
	char * buf, *test;
	int n = 0, len = 0, j = 0;
	int i = 0;
	int codelens = 0;//���б�����ܳ��ȡ�
	FILE * fpin, *fpout;
	init(nodes, 256);//��ÿһ������ʼ��
	init(tree, 511);
	//for(i=0;i<256;i++){
		//printf("gray is %d\n",nodes[i].gray);
	//}
	if ((fpin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
	}//��ͼ���ļ�
	fseek(fpin, 54, 0);//ǰ54�ֽڵ��ļ�Ϊ�ļ�ͷ��ͼ����Ϣ��Ҫ����
	len = fread(b, sizeof(char), 1024, fpin);//ѭ������ͼ������ݲ���
	while (len > 0)
	{

		for (i = 0; i < len; i++)
		{
			freqadd(b[i]);//ͳ�ƸûҶ�ֵ��Ƶ��
		}
		len = fread(b, sizeof(char), 1024, fpin);
	}
	///���ݻҶ�ֵ��Ƶ����С�����������///////
	for (i = 0, len = 0; i < 256; i++) {//������������ʱ���У�����С��������
		if (nodes[i].freq > 0) {
			addtolist(&nodes[i]);
			nodes[i].depth++;
			// printf("%d\n",nodes[i].freq);
			len++;
			tree[i] = nodes[i];
		}

	}
	createcodetree(tree, len);//����huffman���������б���

	codelens = getallcodelen();//�õ�����Ԫ�ر�����ܵı��볤��
	//printf("%d\n",codelens);
	buf = getcode(codelens);//�����еı����ݴ浽buf������
	printf("-----------------------------ÿ���Ҷ�ֵ����Ӧ�ı���---------------------------\n");
	for (n = 0; n < 256; n++) {
		printf("%d:%s\n", n, nodes[n].code);
	}
	printf("\n\n");
	printf("ͼ��������ڽ�����.....\n\n");

	if ((fpout = fopen("huffman.txt", "wb+")) == NULL) {
		printf("file open error");
		exit(1);
	}
	fseek(fpin, 54, 0);
	len = fread(b, sizeof(char), 1024, fpin);//ѭ������ͼ������ݲ���
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
	printf("ͼ��������\n\n");
	fclose(fpin);//�ر��ļ�ָ��
	fclose(fpout);

}
////////////////////����////////////////////////
void decode() {
	FILE * hufin, *hufout, *imgin;
	struct SNode *p;
	int i = 0, n = 0, num = 0;
	char b;
	char debuf[507132];
	//debuf=(char *)malloc(sizeof(char)*(len+1));//����������ڴ�
	//debuf[len]='\0'; 
	if ((imgin = fopen("lena.bmp", "rb")) == NULL) {
		printf("file open error\n");
		exit(1);
	}

	if ((hufout = fopen("decode.bmp", "wb+")) == NULL) {//�򿪴洢���������ļ�
		printf("file open error");
		exit(1);
	}

	if ((hufin = fopen("huffman.txt", "rt+")) == NULL) {//�򿪴洢���������ļ�
		printf("huffman.txt open error\n");
		exit(1);
	}
	fread(debuf, sizeof(char), 54, imgin);//����ԭʼͼ����ļ�ͷ��ͼ����Ϣ�ŵ�b�У���Ϊ�ļ�ͷû�н��б��������������ֱ�ӿ���
	fwrite(debuf, sizeof(char), 54, hufout);//���ļ�ͷ��ͼ����Ϣ�ŵ��������ļ���
	//n=len+1;
	fgets(debuf, 507132, hufin);  //���ļ��ж������еı������У�ֻ��n-1���ڣ����Զ�ʹb��n��Ϊ��\0��
	p = head;                //ÿ�ν��붼Ҫ�����ĸ��ڵ㿪ʼ
	for (i = 0; debuf[i] != '\0'; i++) {//��ʼ����
		switch (debuf[i]) {
		case '0': {
			if (p->pLeft != NULL) {//�������Ϊ0�������Ӳ�Ϊ�գ���ôpָ��p�����ӡ�
				p = p->pLeft;
				break;
			}
			else {              //�������Ϊ�գ���˽��ΪҶ�ӽ�㣬ȡ����Ҷ�ֵ��Ϊ������
				b = p->gray;
				fwrite(&b, sizeof(char), 1, hufout);

				p = head;
				i = i - 1;//��ʱiָ����һ����Ԫ�Ŀ�ʼλ�ã�����forѭ������һ��i++���Դ�ʱ��Ҫ��i��һ
				break;
			}

		}
		case '1': {
			if (p->pRight != NULL) {//�������Ϊ1�����Һ��Ӻ��Ӳ�Ϊ�գ���ôpָ��p���Һ��ӡ�
				p = p->pRight;
				break;
			}

			else {//����Һ��Ӻ���Ϊ�գ���˽��ΪҶ�ӽ�㣬ȡ����Ҷ�ֵ��Ϊ������
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
	b = p->gray;//�����switch���δ��ͳ�����һ���������������ڴ�ͳ��һ��
	fwrite(&b, sizeof(char), 1, hufout);
}


int main() {
	char ans = 0;
	huffmancode();
	printf("��Ҫ����ô��y(����)/n(������)\n");
	scanf("%c", &ans);
	switch (ans) {
	case 'y':printf("�������ڽ�����......\n\n");
		decode();//����
		printf("��������ɣ��뵽Դ�ļ�����Ŀ¼�鿴���\n\n");
		break;
	default:break;
	}
	return 0;

}














