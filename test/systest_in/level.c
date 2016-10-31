//#include <stdio.h>
//int main() {
  int i = 1;printf("%d ", __LINE__);
  for (int i = 2; printf("%d ", __LINE__), i; i--) {
    if (printf("%d ", __LINE__), i == 1) {
      printf("%d ", __LINE__);break;
    }
    if (printf("%d ", __LINE__), i == 2) {
      i = 0;printf("%d ", __LINE__);
    }
    int i = 3;printf("%d ", __LINE__);
    if (printf("%d ", __LINE__), i == 3) {
      i++;printf("%d ", __LINE__);break;
    }
  }
  
  if (printf("%d ", __LINE__), i == 1) {
	  i = -1;printf("%d ", __LINE__);
}
  if (printf("%d ", __LINE__), i == 0) {
	  i = -0;printf("%d ", __LINE__);
}
  if (printf("%d ", __LINE__), i == 3) {
	  i = -3;printf("%d ", __LINE__);
}
//}
