#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define MEMORY_MODULE_NUM_MAX 2048
#define CYCLE_COUNT_MAX 1000000
#define TOLERANCE_VALUE 0.02
#define PI_CONSTANT round(M_PI * 1000) / 1000

int cycle_count;
int num_of_processors;
int num_of_memory_module;
char distribution_type;

void initialize_data_structures(int request[], int access_count[], int priority_of_connection[])
{
    // request = -1, access_count = 0, priority_of_connection [0, K-1]
    for(int i=0; i<num_of_processors; i++) {
        request[i] = -1;
        access_count[i] = 0;
        priority_of_connection[i] = i;
    }
    return;
}

void initialize_request(int request[], int size) {
    for(int i=0; i<size; i++)
        request[i] = -1;
}

void initialize_access_count(int access_count[], int size) {
    for(int i=0; i<size; i++)
        access_count[i] = 0;
}

void initialize_priority_of_connection(int priority_of_connection[], int size) {
    for(int i=0; i<size; i++)
        priority_of_connection[i] = i;
}

void initialize_memory_module(int memory_module[], int size) {
    for(int i=0; i<size; i++)
        memory_module[i] = 0;
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("arr[%d]: %d\n", i, arr[i]);
    printf("\n");
}

double generate_unit_interval_uniform_distribution() {
    return rand() / (1.0+RAND_MAX);    // return value is in the range of [0, 1)
}

void setRandomSeed()
{
    // Use the current time as seed for random number generator 
    srand(time(NULL));
}

int generate_int_uniform_distribution() {
    double random_num = generate_unit_interval_uniform_distribution();
    int memory_idx_range = num_of_memory_module; 
    int res = (int) (random_num * memory_idx_range);
    return res;   // return value is in the range of [0, num_of_memory_module) 
}

int generate_int_normal_distribution() {
    double sum=0, aver;
    for(int i=0; i<num_of_processors; i++)
        sum += generate_int_uniform_distribution();
    aver = sum / num_of_processors;
    int standard_deviation = num_of_memory_module / 6;
    int random_variable = generate_int_uniform_distribution();
    double part1 = 1.0/(standard_deviation*(sqrt(2.0*PI_CONSTANT)));
    double part2 = exp((-0.5)*(((random_variable-aver)/standard_deviation)*((random_variable-aver)/standard_deviation)));
    double x = part1 * part2;
    int round_x = round(x * num_of_memory_module);
    return (round_x % num_of_memory_module) + aver;  // return value is in the range of [0, num_of_memory_module) 
}

void generate_requests(int request[], char distribution_type)
{   
    // check distribution type and generate requests if request[i] == -1 
    if(distribution_type == 'u') {
        // generate requests for each processor using a uniform distribution
        for(int i=0; i<num_of_processors; i++)
            if(request[i]==-1)  request[i] = generate_int_uniform_distribution();
        // printf("GENERATED REQUESTS SUCCESS!\n");
    } else if(distribution_type == 'n') {
        // generate requests for each processor using a normal distribution
        for(int i=0; i<num_of_processors; i++)
            if(request[i]==-1)  request[i] = generate_int_normal_distribution();
        // printf("GENERATED REQUESTS SUCCESS!\n");

    } else
        printf("Unexpected character '%c' was entered\n", distribution_type);
}


void process_requests(int request[], int access_count[], int priority_of_connection[], int memory_module[], int who_access_me[], int accessed_processors[])
{

    for(int i=0; i<num_of_memory_module; i++)
        who_access_me[i] = -1;

    for(int i=0; i<num_of_processors; i++)
        accessed_processors[i] = 0;

    for (int i=0; i<num_of_processors; i++) {
        if (memory_module[request[i]] == 1) {
            if (priority_of_connection[i] < priority_of_connection[who_access_me[request[i]]]) {
                access_count[who_access_me[request[i]]]--;
                accessed_processors[who_access_me[request[i]]] = 0;
                who_access_me[request[i]] = i;
                access_count[i]++;
                accessed_processors[i] = 1;
            }
        }
        else {
            memory_module[request[i]] = 1;
            who_access_me[request[i]] = i;
            access_count[i]++;
            accessed_processors[i] = 1;
        }
    }
    //re-label the priority, let the processors who access to memories in this cycle have the lower priority.
    int left_index = 0, right_index = num_of_processors - 1;
    for (int i=0; i<num_of_processors; i++) {
        if (accessed_processors[i] == 1) {
            priority_of_connection[i] = right_index--;
            request[i] = -1;
        }
        else
            priority_of_connection[i] = left_index++;
    }
}

void compute_time_cumulative_average(int cycle_count, int access_count[], double time_cumulative_average[])
{
    for( int i=0; i<num_of_processors; i++) {
        if(access_count[i]==0 || access_count[i]==-1)
            time_cumulative_average[i] = 0;
        else
            time_cumulative_average[i] = (double) cycle_count / (double) access_count[i];
    }
}

double compute_arithmetic_average_of_all(double time_cumulative_average[])
{
    double sum=0;
    for(int i=0;i<num_of_processors;i++)
        sum += time_cumulative_average[i];
    return sum/(double)num_of_processors;
}

void write_result(int memory_module_num, double cur_arithmetic_average)
{
    FILE* fp;
    fp = fopen("output.csv", "a");
    // cur_arithmetic_average = round(cur_arithmetic_average * 10000) / 10000;
    fprintf(fp, "%d", memory_module_num); 
    fprintf(fp, ","); 
    fprintf(fp, "%.4f", cur_arithmetic_average);
    fprintf(fp, "\n");
    fclose(fp);
}

int main(int argc, char * argv[])
{
    if(argc == 3) {
        num_of_processors = atoi(argv[1]);
        distribution_type = argv[2][0];
        int request[num_of_processors];
        int access_count[num_of_processors];
        int priority_of_connection[num_of_processors];
        
        setRandomSeed();

        for(int cur_memory_module_count=1; cur_memory_module_count<=MEMORY_MODULE_NUM_MAX; cur_memory_module_count++) {   // run 2048 times simulation
            cycle_count = 0;
            double cur_arithmetic_average;
            double prev_arithmetic_average;
            num_of_memory_module = cur_memory_module_count;
            int memory_module[num_of_memory_module];

            // Initialize data structures
            initialize_access_count(access_count, num_of_processors);
            initialize_priority_of_connection(priority_of_connection, num_of_processors);
            initialize_request(request, num_of_processors);
            int who_access_me [num_of_memory_module];
            int accessed_processors [num_of_processors];

            while(1) {      // for each simulation
                cycle_count = cycle_count+1;

                // initialize memory module value
                initialize_memory_module(memory_module, num_of_memory_module);
                
                // 0. run a simulation algorithm:
                // - assign each processor a memory module num ( either by 'uniform' or 'normal' distribution )
                generate_requests(request, distribution_type);
                process_requests(request, access_count, priority_of_connection, memory_module, who_access_me, accessed_processors);
                // 1. compute the time-cumulative average of the access-time for each processor
                //     ( The time-cumulative average of a processor’s memory access at cycle c
                //         = the total # of simulated memory cycles c / the total # of granted accesses so far )
                //     time-cumulative average of a processor's memory access at cycle c = c / access_count[ith_idx]
                double time_cumulative_average[num_of_processors];
                compute_time_cumulative_average(cycle_count, access_count, time_cumulative_average);
                    
                // 2. compute the arithmetic average W(Sc(p, m, d)) of all processors’ time-cumulative averages
                cur_arithmetic_average = compute_arithmetic_average_of_all(time_cumulative_average);
                if(cycle_count!=1) {
                    if( cycle_count == CYCLE_COUNT_MAX || fabs(1 - prev_arithmetic_average / cur_arithmetic_average) < TOLERANCE_VALUE )
                        break;
                }
                prev_arithmetic_average = cur_arithmetic_average;
            }
            write_result(cur_memory_module_count, cur_arithmetic_average);
        }
    }
    return 0;
}