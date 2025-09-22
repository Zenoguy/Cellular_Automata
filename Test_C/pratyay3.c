#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_CELLS 8
#define MAX_STATES (1<<MAX_CELLS)

// ===== Rule Database =====
typedef struct { int rule; int class; int is_linear; } RuleInfo;

RuleInfo rule_database[] = {
    {0,1,1},{51,1,1},{204,1,1},{60,1,0},{195,1,0},
    {15,2,0},{30,2,0},{45,2,0},{75,2,0},{90,2,1},
    {5,3,0},{17,3,0},{68,3,0},{80,3,0}
};
int rule_db_size = sizeof(rule_database)/sizeof(RuleInfo);

// ===== Tables =====
int first_rules[3][10]  = {{3,12,0},{5,10,0},{6,9,0}};    // Table (b)
int middle_rules[6][20] = {
    {51,204,60,195,85,90,105,170,102,105,150,153,53,58,83,92,163,172,197,202},
    {15,30,45,60,75,90,105,120,135,165,180,195,210,225,240,0},
    {51,204,15,240,85,105,150,170,90,102,153,105,23,43,77,113,142,178,212,232},
    {60,195,90,165,105,150,0},
    {51,204,85,170,102,153,86,89,101,105,106,149,150,154,165,166,169,0},
    {15,240,105,150,90,165,0}
};
int last_rules[6][10]   = {
    {17,20,65,68,0},{5,20,65,80,0},{5,17,68,80,0},
    {20,65,0},{17,68,0},{5,80,0}
};

// ===== Utility =====
RuleInfo* get_rule_info(int rule) { 
    for(int i=0;i<rule_db_size;i++) if(rule_database[i].rule==rule) return &rule_database[i]; 
    static RuleInfo unknown={0,3,0}; return &unknown; 
}
int get_rule_class(int rule) { return get_rule_info(rule)->class; }
int rule_output(int l,int c,int r,int rule){ return (rule>>(l*4+c*2+r))&1; }
int state_to_int(int *state,int n){ int res=0; for(int i=0;i<n;i++) res=(res<<1)|state[i]; return res; }
void int_to_state(int num,int *state,int n){ for(int i=n-1;i>=0;i--){ state[i]=num&1; num>>=1; } }
void evolve_step(int *cur,int *next,int *rules,int n){ for(int i=0;i<n;i++){ int l=cur[(i-1+n)%n], r=cur[(i+1)%n]; next[i]=rule_output(l,cur[i],r,rules[i]); } }

// ===== Intelligent RCA generation =====
void generate_rca(int input_rule,int n,int *ca_rules){
    int cls = get_rule_class(input_rule);
    int i,j;

    // 1. First cell - pick randomly from Table (b)
    i=0; while(first_rules[cls-1][i]!=0) i++; ca_rules[0] = first_rules[cls-1][rand()%i];

    // 2. Middle cells - coverage + diversity
    int visited[MAX_STATES] = {0};
    int cur[MAX_CELLS]={0}, next[MAX_CELLS]={0};
    cur[0]=0; next[0]=0;

    // Mark first cell rules as applied for diversity
    int rule_count[256]={0}; rule_count[ca_rules[0]]++;

    for(i=1;i<n-1;i++){
        int best_rule=-1, best_score=-1;
        // Count number of candidate rules
        int rule_list[20], rcount=0;
        j=0; while(middle_rules[cls-1][j]!=0){ rule_list[rcount++]=middle_rules[cls-1][j]; j++; }
        // Evaluate each candidate rule
        for(j=0;j<rcount;j++){
            int candidate = rule_list[j];
            ca_rules[i]=candidate;
            // simulate all states with current partial configuration
            int new_coverage=0;
            for(int s=0;s<(1<<n);s++){
                int_to_state(s,cur,n);
                for(int k=0;k<i;k++) next[k]=cur[k];
                for(int k=0;k<=i;k++) evolve_step(cur,next,ca_rules,n);
                int idx = state_to_int(cur,n);
                if(!visited[idx]) new_coverage++;
            }
            int diversity_score = -rule_count[candidate];
            int total_score = new_coverage*10 + diversity_score; // weight coverage higher
            if(total_score>best_score){ best_score=total_score; best_rule=candidate; }
        }
        ca_rules[i]=best_rule;
        rule_count[best_rule]++;
        // Update visited
        for(int s=0;s<(1<<n);s++){
            int_to_state(s,cur,n);
            for(int k=0;k<=i;k++) evolve_step(cur,next,ca_rules,n);
            visited[state_to_int(cur,n)]=1;
        }
    }

    // 3. Last cell - pick randomly from Table (c)
    i=0; while(last_rules[cls-1][i]!=0) i++; ca_rules[n-1] = last_rules[cls-1][rand()%i];
}

// ===== Maximal-length check =====
int is_maximal_length(int *rules,int n){
    int max_states = 1<<n, *visited = calloc(max_states,sizeof(int)), cur[MAX_CELLS], next[MAX_CELLS], max_cycle=0;
    for(int start=0;start<max_states;start++){
        if(visited[start]) continue;
        int path[256]={0}, step=0;
        int_to_state(start,cur,n);
        while(step<256){
            int si=state_to_int(cur,n), cycle_start=-1;
            for(int k=0;k<step;k++) if(path[k]==si){ cycle_start=k; break; }
            if(cycle_start>=0){ int clen=step-cycle_start; if(clen>max_cycle) max_cycle=clen; break; }
            path[step]=si; visited[si]=1;
            evolve_step(cur,next,rules,n);
            for(int k=0;k<n;k++) cur[k]=next[k]; step++;
        }
    }
    free(visited); return max_cycle==(1<<n);
}

// ===== Main =====
int main(){
    srand(time(NULL));
    int n,input_rule,ca_rules[MAX_CELLS]={0};
    printf("Enter input rule (0-255): "); scanf("%d",&input_rule);
    printf("Enter number of cells (3-%d): ",MAX_CELLS); scanf("%d",&n);

    generate_rca(input_rule,n,ca_rules);

    printf("\nGenerated n-cell CA rules:\n[");
    for(int i=0;i<n;i++){ printf("%d",ca_rules[i]); if(i<n-1) printf(", "); } printf("]\n");

    if(is_maximal_length(ca_rules,n)) printf("This CA has maximal-length cycle.\n");
    else printf("This CA does NOT have maximal-length cycle.\n");

    return 0;
}
