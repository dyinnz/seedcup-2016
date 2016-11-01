//#include <stdio.h>
//int main() {
  // 支持printf 返回值
  int hello = printf("hello %d\n", 1024); printf("%d ", __LINE__);

  // 支持十六进制和八进制
  int world = -0x10; printf("%d ", __LINE__);

  // 支持悬空的{}块
  {
    printf("hello + world: %d\n", hello + world); printf("%d ", __LINE__);
  }

  // 支持任意的表达式嵌套
  int b, c; 
  int a = b = c = 1; printf("%d ", __LINE__);
  int comlex_expr = (hello = world = 1 == 10 < -a++ * b - c, 999); printf("%d ", __LINE__);

  // 支持括号 和 括号嵌套
  int parentheses = ((3 + b) * (c > 9) , 999); printf("%d ", __LINE__);

  // 支持 if, while, do-while, for 的条件表达式中出现任意表达式
  if ( printf("%d ", __LINE__), 1, 2, 3) {
    for (int i = 0;  printf("%d ", __LINE__), printf("dead loop\n"); i++) {
      do {
        // 支持任意的结构嵌套
        c = 5; printf("%d ", __LINE__);
        while ( printf("%d ", __LINE__), a = b = c) {
          // 支持非赋值形式
          c--, b = 1; printf("%d ", __LINE__);
        }
      } while ( printf("%d ", __LINE__), a == b ++ + -c, 0);
       printf("%d ", __LINE__);break;
    }

  }
//}
