// test1_attr.c
struct target_struct {
    int __attribute__((user)) secret_key;
    int public_data;
};

int main() {
    // Local variable with 'user' attribute field
    // Plugin should forcibly initialize this.
    struct target_struct leaked_data;
    return (int)leaked_data.public_data; 
}