//#include <stdio.h>
//int main() {
int a, b, c;printf("%d ", __LINE__);

a = b = c = 0xFf;printf("%d ", __LINE__);

a++;b=a;printf("%d ", __LINE__);
c = a+++b++;printf("%d ", __LINE__);

for (int i = c; printf("%d ", __LINE__), i > c - a - b; i--, a++, b--) {	
	printf(/*asdasd*/"", i);printf("%d ", __LINE__);
}

//}
