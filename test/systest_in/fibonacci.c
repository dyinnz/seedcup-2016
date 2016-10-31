//#include <stdio.h>
//int main() {
int n = 6;printf("%d ", __LINE__);
    int result;  printf("%d ", __LINE__);
        int previous_result;  printf("%d ", __LINE__);
	    int previous_pre_result;  printf("%d ", __LINE__);
	        result=1;  printf("%d ", __LINE__);
		    previous_result=1;  printf("%d ", __LINE__);
		        while(printf("%d ", __LINE__), n>2)  
	    {  
		            n--;  printf("%d ", __LINE__);
			            previous_pre_result=previous_result;  printf("%d ", __LINE__);
				            previous_result=result;  printf("%d ", __LINE__);
					            result=previous_result+previous_pre_result; printf("%d ", __LINE__); 
						        }  
    printf("", result);  printf("%d ", __LINE__);
//}
