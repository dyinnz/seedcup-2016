// int main() {
  // int __LINE__ = 0;
  int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;

  // only if
  if (a < b)
    printf("test empty printf");

  if (a < b) {
printf("%d\n", __LINE__);
}

  if (a < b) printf("%d\n", __LINE__);

  if (a < b) { printf("%d\n", __LINE__); }

  if (a < b) if (b < c)
      if (e < f) printf("%d\n", __LINE__);

  // with else
  if (a < b) {
  } else printf("nil");

  if (a < b) ;else printf("nil");
  if (a < b) ;else ;
  if (a < b)
    printf("%d\n", __LINE__);
        else
    if (c < d)
      printf("%d\n", __LINE__);
      else if (e < f)
  printf("%d\n", __LINE__);
  if (a < b)
    printf("%d\n", __LINE__);

  if (a < b);else if(0);else if(0);
  else {
    if (1) {
      if (0) ;
        else if (1) ;
      else {
        if (0) ; else if (1) ; else if (1) {
        }
      }
    }
  }
// }
