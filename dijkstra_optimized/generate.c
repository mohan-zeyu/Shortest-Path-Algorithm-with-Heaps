#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// CTR: 14081816 34292496
// CAL 1890815 4657742

int main(){
    int choice, N;
    printf("0: NY, 1: BAY, 2: COL, 3: FLA, 4: NW, 5: NE, 6: CAL, 7: LKS, 8: E, 9: W, 10: CTR, 11: USA\n");
    printf("How many files do you want to generate?\n");
    int n;
    scanf("%d",&n);
    while(n > 0){
        FILE *f;
        printf("City code name:\n");
        scanf("%d", &choice);
        switch (choice){
            case 0: N = 264346; f=fopen("Queries/NY_Queries.txt","w"); break;
            case 1: N = 321270; f=fopen("Queries/BAY_Queries.txt","w"); break;
            case 2: N = 435666; f=fopen("Queries/COL_Queries.txt","w"); break;
            case 3: N = 1070376; f=fopen("Queries/FLA_Queries.txt","w"); break;
            case 4: N = 1207945; f=fopen("Queries/NW_Queries.txt","w"); break;
            case 5: N = 1524453; f=fopen("Queries/NE_Queries.txt","w"); break;
            case 6: N = 1890815; f=fopen("Queries/CAL_Queries.txt","w"); break;
            case 7: N = 2758119; f=fopen("Queries/LKS_Queries.txt","w"); break;
            case 8: N = 3598623; f=fopen("Queries/E_Queries.txt","w"); break;
            case 9: N = 6262104; f=fopen("Queries/W_Queries.txt","w"); break;
            case 10: N = 14081816; f=fopen("Queries/CTR_Queries.txt","w"); break;
            case 11: N = 23947347; f=fopen("Queries/USA_Queries.txt","w"); break;
            default: printf("Fuck you!"); return 2;
        }
        int Q,s,t; 
        printf("Enter the number of Queries you want to generate for %d:\n", choice);
        scanf("%d", &Q);
        srand(time(0)); 
        fprintf(f,"%d\n",Q);
        for(int i=0;i<Q;i++){ 
            do{
                s=rand()%N+1; t=rand()%N+1;
            }while(s==t); 
            fprintf(f,"%d %d\n",s,t); 
        }
        fclose(f); 
        n--;
    }
    
    return 0;
}
