//#include <stdio.h>
//int main() {
int a, b, c;printf("%d ", __LINE__);

a = b = c = 0;printf("%d ", __LINE__);

a++;b=a;printf("%d ", __LINE__);
c = a+++b++;printf("%d ", __LINE__);

for (int i = c; i > c - a - b; i--, a++, b--) {printf("%d ", __LINE__);
	printf(/*asdasd*/"i = %d\n", i);printf("%d ", __LINE__);
}

//}
