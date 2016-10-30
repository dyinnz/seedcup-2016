//#include <stdio.h>
//int main() {
int i = 0, j, k = 0; printf("%d ", __LINE__);

for (int x = 0, y = 0; i < 5; i++) { printf("%d ", __LINE__);
  {
    while (1) { printf("%d ", __LINE__);
      y = 1 + y; printf("%d ", __LINE__);
      printf("%d ", __LINE__); if (y < 3) {
        while (1) { printf("%d ", __LINE__);
          j = k++; printf("%d ", __LINE__);
          {
            printf("%d ", __LINE__); break;
          }
        }
      }
      else {
        printf("%d ", __LINE__); break;
      }
    }
    printf("%d ", __LINE__); if (i == 4) {
      printf("%d ", __LINE__);break;
    }
  }
}
//}
